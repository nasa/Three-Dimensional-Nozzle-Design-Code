/***************************************************************************
*
*					The Johns Hopkins University Applied Physics Laboratory
*					        Research and Technology Development Center
*						    Aeronautical Science and Technology Group
*							                  MS 10A-115
*							             Johns Hopkins Road
*								           Laurel, MD 20723
*								             443-778-3863
*								          443-778-5850 (FAX)
*
*
*	Program:			MOC_Grid.cpp
*
*	Start Date:			7/27/01
*
*	Last Modified:		3/11/02
*
*	Author:				Tharen Rice   x3863
*
*	Sponsor:			FG888BXX (NASA GRC PDE Work)
*
*	Comments:			This class will calculate the MOC grid for a nozzle				
* 
*	Last Update:		Added Cone option						 
*						
*
****************************************************************************/

#include "MOC_GridCalc_BDE.h"
#include "math.h"
#include "afx.h"
// Tharen Rice May 2020 Change: New call for fstream
//#include "fstream.h"
#include <fstream>
using namespace std;
// END OF CHANGES
#include "engineering_constants.hpp"
#include "Resource.h"
#include "MOC_GridDlg.h"
#include "MOCPlotDialog.h"


/****************************************************************************
*
*	Constructor and Destructors
*
****************************************************************************/

MOC_GridCalc::MOC_GridCalc(void)
{
	//Default the number of characteristics to 20
	nC = -99;
	RWTD = 1.0;		//  Downstream radius/Throat Radius
	RWTU = 1.0;		//	Upstream radius/Throat Radius
	DTLIMIT = 0.5*RAD_PER_DEG;
	nozzleGeom = NOGEOM;
	nozzleType = NOTYPE;
	designParam = NOPARAM;
	designValue[0] = 3.0;
	designValue[1] = -99.9;
	conCrit = 1e-10;
	thetaBMin = 0.1*RAD_PER_DEG;
	thetaBMax = 50*RAD_PER_DEG;
	lastRRC = 0;
	maxLRC = 1000; // i
	maxRRC = 999; // j
	printMode = 0;
	mThroat = -99.9;
	return;
}


MOC_GridCalc::~MOC_GridCalc( void)
{
	return;
}

/****************************************************************************
*
*	Public Functions
*
****************************************************************************/
int MOC_GridCalc::SetInitialProperties(double pres, double temp, double mWt, 
								double gamma, double pa, int n, double rwtu, double rwtd, 
								double dTLimit, int nRRCPP, int NSLi, int NSLj,
								double vel, int flag, double idealIsp)
{
	if ( pres <= 0.0 || temp <= 0.0 || mWt <= 0.0 || gamma <= 0.0 )
	{
		AfxMessageBox("All input parameters must be > 0.0");
		return 0;
	}
	//	If n is an even number, increment it by one, so that there are an odd number of 
	//	characteristics
	if ( n % 2 == 0) n++;

	throatFlag = flag;
	molWt = mWt;
	gamma_i = gamma;
	nC = n;
	RWTD = rwtd;
	RWTU = rwtu;
	DTLIMIT = dTLimit*RAD_PER_DEG;
	pAmbient = pa;
	nRRCAboveBD = nRRCPP;
	nSLi = NSLi;
	nSLj = NSLj;
	ispIdeal = idealIsp;
	mThroat = vel/sqrt(gamma*GASCON/mWt*GRAV*temp);

	if (throatFlag == 1) // input conditions are throat static conditions
	{
		tTotal = temp * (1+ (gamma-1)/2*mThroat*mThroat);
		pTotal = pres * pow(tTotal/temp,gamma/(gamma-1));
	}
	else // Total conditions set
	{
		pTotal = pres;
		tTotal = temp;
	}
	return 1; // good return
}

//****************************************************************************************
//****************************************************************************************
//****************************************************************************************
void MOC_GridCalc::SetSolutionParameters( int geom, int type, int param, double value,
										 double t)
{
	nozzleGeom = geom;
	nozzleType = type;
	designParam = param;
	designValue[0] = value;
	thetaBi = t;
//	initialLineAngle = a;
	if ( param == ENDPOINT) nozzleType = FIXEDEND;
	return;
}

//****************************************************************************************
//****************************************************************************************
//****************************************************************************************
void MOC_GridCalc::SetSolutionParameters( int geom, int type, int param, double value1, 
										 double value2, double t)
{
	SetSolutionParameters( geom, type, param, value1, t);
	designValue[1] = value2;
	return;
}
//****************************************************************************************
//****************************************************************************************
//****************************************************************************************
int MOC_GridCalc::CreateMOCGrid(void)
{
	int flag = 0;
	if ( nC < 5) AfxMessageBox("Increase the number of starting characteristics > 5");
	else flag = CalcMOC_Grid(gamma_i, pAmbient, throatFlag, 
		mThroat, nozzleType, designParam, nozzleGeom, nSLi, nSLj);
	return flag;
}

/****************************************************************************
*
*	Private Funcitons
*
****************************************************************************/
//************************************************************************************
//************************************************************************************
//************************************************************************************
int MOC_GridCalc::CalcMOC_Grid( double g, double pAmb, int tFlag, double mT, int nType, 
							   int dParam, int geom, int nni, int nnj)
{
	//	pT = total pressure; psia
	//	tT = total Temperature; R
	//	g = gamma;
	//	mWt = Molecular Weight
	//	pAmb = ambient pressure
	//	tFlag = throat flag
	//	mT = throat mach number
	int goodFlag = 0; // flag set to 1 if the CalcNozzle function are OK
	//	Make sure all of the inputs are set
	if (nType == NOTYPE) 
	{
		AfxMessageBox("Nozzle type not set");
		return SEC_FAIL;
	}
	if (dParam == NOPARAM)
	{
		AfxMessageBox("Nozzle parameter not set");
		return SEC_FAIL;
	}
	if (geom == NOGEOM)
	{
		AfxMessageBox("Nozzle parameter not set");
		return SEC_FAIL;
	}

	//	First, initialize the arrays that will contain the characteristic data. As a default, the array size is 100
	InitializeDataMembers();
	
	//	Set the initial data line based on the corrected HALL method
	iLast[0] = nC-1; //	For j = 0, the ivalues go from 0 to nC-1;
	if (CalcInitialThroatLine(RWTU,iLast[0],g, pAmb, geom, tFlag, mT) == SEC_FAIL) return SEC_FAIL;

	//	The program is set up to solve for the CONE, minimum length or perfect nozzle contours
	//	given a user defined value.  This value can be either Mach number, Area ratio, or 
	//	nozzle wall end point.	Given the choses, the code goes off and does different things

	//	As of 3.29.02, the code worked very well for a perfect nozzle cases, but there was
	//	some trouble solving for high expansion ratio minimum length nozzles. To 
	//	counter act this, a plan has been devised.  First, for every min-length case,
	//	solve for the perfect nozzle.  It is known that the XD for the perfect nozzle
	//	has to be greater then the XD of the min length.  This will stop the code
	//	from choosing excessively large values of the THETAB, which can BOMB the code.

	if ( designParam == ENDPOINT)
	{
		double eps[2];
		if ( nozzleGeom == TWOD) eps[0] = designValue[1];
		else if ( nozzleGeom == AXI) eps[0] = designValue[1]*designValue[1];
	}

	if (nozzleType == CONE) goodFlag = CalcConeNozzle( dParam, designValue, pAmb, geom, nni, nnj);
	else goodFlag = CalcContouredNozzle( dParam, designValue, g, pAmb, geom, nRRCAboveBD,
		nozzleType, nni, nnj);
	
	if (printMode)
	{
		OutputCenterlineData("center.out");
		OutputMOC_Grid();
	}
	if (goodFlag) OutputSummaryFile();

	return goodFlag;
}

//**************************************************************************************
//**************************************************************************************
//**************************************************************************************
int MOC_GridCalc::CalcContouredNozzle( int paramType, double *paramMatch, 
								double g, double pAmb, int geom, int nRRCPlus, int nType,
								int nni, int nnj)
{
	//	Inputs:
	//	paramType (MACH, EPS, NOZZLELENGTH, ENDPOINT)
	//	paramMatch: value of the paramType. ENDPOINT has two values (x/R* and r/R*)
	//	g : gamma
	//	pAmb: ambient pressure (psia)
	//	geom: TWOD or AXI
	//	nRRCPlus: number of additional RRC above line BD (TWOD only)
	//	nType: type of nozzle to find (RAO, CONE, PERFECT, FIXEND) ENDPOINT = FIXEDEND
	//	Note: The Initial throat line TT' has alrady been computed.
	//	This function will compute the minimum length nozzle contour for a given design
	//	parameter, paramMatch
	//	It uses the method described in RAO's 1958 paper. 
	//	In this method, a few iteration loops are used.  The first loop is the determination
	//	of an initial expansion angle thetaB. The initial guess for THETAB will be
	//	1/2 the Prandlt-Meyer function for an exit Mach number of 3.0 = 25 deg
	//	for Pa = 0.0.
	//	For each thetaB, Calculate the MOC grid within TT'BF. 
	//	B is the wall point at thetaB
	//	F is the axial point created by the RRC from B.
	//	Here is the 2nd iteration loop.
	//	Chose a point D' along RRC BF.
	//	Calculate the mass flow along BD' by integrating the flow properties along the RRC
	//	Create a LRC at D'.  Find the end point (E') where the mass flow from D'E' equals the 
	//	mass flow from BD'.
	//	At point E, solve eq. 14 of RAO, and see if it checks out.  If it doesn't, iterate on
	//	point D' until it does.  This solution gives the optimum nozzle of length XE for a 
	//	given area ratio (RE/R*)^2 and corresponding Mach number ME.  
	//	Depending on the type of solution, Given ME, eps, XE, RE, vary thetaB until this
	//	condition is met.
	
	//	thetaB[3]: guesses for thetaB
				// dS.dSx[0] =	xD
					// dS.dSx[1] =	rD
					//		2	=	machD
					//		3	=	thetaD
					//		4	=	presD
					//		5	=	tempD
					//		6	=	RhoD
					//		7	=	xE
					//		8	=	rE
					//		9	=	machE
					//		10	=	thetaE
					//		11	=	presE
					//		12	=	tempE
					//		13	=	rhoE
					//		14	=	gammaE
					//		15	=	mdot
	
	//	Create an instance of the Dialog box to graph the nozzle
	double thetaB[3],xArcMax, paramExit=0.0, paramErr[3];
	int i,j, lastKernelJ,k;
	dummyStruct dS;

	ofstream ofile;
	ofile.open("ThetaB.out");
	ofile << "I\tThetaB\txArcMax\tLastKernelJ\tparamErr\n";
	lastKernelJ = 0;
	thetaB[0] = thetaBi*RAD_PER_DEG; // set to the initial Guess 
	for ( i = 0; i <= 1; i++)
	{
		//	for the various design parameters, vary the second guess of thetaB based on
		//	the first guess and the resultant error.
		//	if the calculated area ratio is to small, increase thetaB
		if ( i == 1)
		{
			if( fabs(paramErr[0]) >= 1.0)
			{
				// To get a better intial guess, makes sure the first guess is close
				i = 0;
				thetaB[0] *= 0.8;
			}
			else thetaB[1] = SetThetaB(paramType, paramErr[0], thetaB[0]);
		}
				
		//	Convert the thetaB parameter, into a maximum X value along an predescribed arc
		xArcMax = sin(thetaB[i])*RWTD;
		//	The initial data lines ( j =0 ) has already been calculated
		j = 0;	//	Step through the RRCs, if they have already been calculated, then use them.
		while ( x[0][j+1] < xArcMax && j < lastKernelJ) j++;
		if ( j > 7) j-=5;
		else j = 0;
		lastKernelJ = CalcRRCsAlongArc(j, RWTD, thetaB[i],DTLIMIT, pAmb, geom );
		if ( lastKernelJ == -999019) return SEC_FAIL;
	
		k = 0;
		while (lastKernelJ < 0 && k++ < 20)
		{
			//	-lastKernelJ is the max thetaB.
			lastKernelJ = -lastKernelJ;
			thetaBMax = asin(x[0][lastKernelJ]/RWTD);
			if (i == 0) thetaB[0] = __min(0.95*thetaBMax, 0.95*thetaB[0]);
			else if (i == 1) thetaB[1] = 0.5*(thetaB[0] + thetaB[1]);
			xArcMax = sin(thetaB[i])*RWTD;
			j = 0;	
			while ( x[0][j+1] < xArcMax && j < lastKernelJ) j++;
			if ( j > 7) j-=5;
			else j = 0;
			lastKernelJ = CalcRRCsAlongArc(j, RWTD, thetaB[i],DTLIMIT, pAmb, geom );
			if ( lastKernelJ == -999019) return SEC_FAIL;
		}
		if (k == 20)
		{
			AfxMessageBox("Could not initialize THETAB calculation.\nCalcContouredNozzle");
			return SEC_FAIL;
		}			
		//	At this point numerous RRCs have been calculated. Along the last RRC, j, construct
		//	a LRC from a point D on j to some point E.  The following criteria have to be met.
		//	Mdot from (B(wall) to D along j equals Mdot for D to E along LRC.
		//	At E. sin(2*theta) = 2*(p-Pa)cot(mu)/(rho*W*W).  Within this iteration scheme
		//	The Mach number at E is needed.  This and a bunch of other variable describing
		//	points D and E are also returned.						// r end point
		
		dS = CalcLRCDE(lastKernelJ, iLast[lastKernelJ], pAmb, geom, nRRCPlus, nType, paramMatch[1],0);

		//	If CalcLRCDE fails low, then thetaB should be raised)
		if (dS.dSi[2] == SEC_FAIL_LOW)
		{
			k = 0;
			while (dS.dSi[2] == SEC_FAIL_LOW && k++ < 10 )
			{
				thetaBMin = thetaB[i];
				thetaB[i] = 0.5*(thetaBMin + thetaBMax);
				xArcMax = sin(thetaB[i])*RWTD;
				j = 0;
				while ( x[0][j+1] < xArcMax && j < lastKernelJ) j++;
				if ( j > 7) j-=5;
				else j = 0;
				lastKernelJ = CalcRRCsAlongArc(j, RWTD, thetaB[i],DTLIMIT, pAmb, geom );
				if ( lastKernelJ == -999019) return SEC_FAIL;
				while (lastKernelJ < 0)
				{
					//	-lastKernelJ is the max thetaB.
					lastKernelJ = -lastKernelJ;
					thetaBMax = asin(x[0][lastKernelJ]/RWTD);
					thetaB[i] = thetaBMax;
					j = 0;	
					while ( x[0][j+1] < xArcMax && j < lastKernelJ) j++;
					if ( j > 3) j-=2;
					else j = 0;
					lastKernelJ = CalcRRCsAlongArc(j, RWTD, thetaB[i],DTLIMIT, pAmb, geom );
					if ( lastKernelJ == -999019) return SEC_FAIL;
				}
				dS = CalcLRCDE(lastKernelJ, iLast[lastKernelJ], pAmb, geom, nRRCPlus, nType, paramMatch[1],0);
			}
			if ( k > 10)
			{
				CString str;
				if (thetaB[2] == thetaBMax || thetaB[2] == thetaBMin) 
				{
					str = "The RAO Method could not find the for this high\n";
					str += "area ratio nozzle. Try the 'Set End Point' type\n";
					str += "or try a different Initial THETAB guess.\n";
				}
				else
				{
					str = "Could not converge on THETAB for given Area Ratio";
				}
				AfxMessageBox(str);
				OutputJ(lastKernelJ,"LastKernel.out");
				return SEC_FAIL;
			}
		}
		else if ( dS.dSi[2] == SEC_FAIL_HIGH)
		{
			k = 0;
			while (dS.dSi[2] == SEC_FAIL_HIGH && thetaBMin < thetaBMax && k++ < 10)
			{
				thetaBMax = thetaB[i];
				if (i == 1 && paramErr[0] < 0.0) thetaB[i] = 0.5*(thetaB[0]+thetaBMax);
				else thetaB[i] = (thetaBMax + thetaBMin)/2.0;
				xArcMax = sin(thetaB[i])*RWTD;
				j = 0;
				while ( x[0][j+1] < xArcMax && j < lastKernelJ) j++;
				if ( j > 7) j-=5;
				else j = 0;
				lastKernelJ = CalcRRCsAlongArc(j, RWTD, thetaB[i],DTLIMIT, pAmb, geom );
				if ( lastKernelJ == -999019) return SEC_FAIL;
				dS = CalcLRCDE(lastKernelJ, iLast[lastKernelJ], pAmb, geom, nRRCPlus, nType, paramMatch[1],0);
			}
			if ( k > 10)
			{
				CString str;
				str = "Could not converge on THETAB for given Area Ratio";
				AfxMessageBox(str);
				OutputJ(lastKernelJ,"LastKernel.out");
				return SEC_FAIL;
			}
		}
		else if (dS.dSi[2] == SEC_FAIL)
		{
			AfxMessageBox("Failure is determining minimum expansion angle.\nCalcContouredNozzle");
			return SEC_FAIL;
		}

		if (thetaBMin >= thetaBMax)
		{
			AfxMessageBox("Failure is determining minimum expansion angle.\nCalcContouredNozzle");
			return SEC_FAIL;
		}
			
		//	Based on the calculation of the LRC DE, a point [i][j] was found. The value of
		//	i+j = the total number of j's to calculate. This value is returned in dS.
		lastRRC = dS.dSi[1];
				
		//	Depending on what the design parameter is, Mach, Eps of length, the Error
		//	has to be calculated differently.
		if ( paramType == EXITMACH) paramExit = dS.dSx[9];
		else if ( paramType == NOZZLELENGTH || paramType == ENDPOINT) paramExit = dS.dSx[7];
		else if ( paramType == EXITPRESSURE) paramExit = pTotal/dS.dSx[11];
		else if ( paramType == EPS)
		{
			//	Calculate the difference in the calculated area ratio and the desired EPS
			if (geom == TWOD) paramExit = dS.dSx[8];
			else if (geom == AXI) paramExit = dS.dSx[8]*dS.dSx[8];
		}
		paramErr[i] = (paramExit - paramMatch[0])/paramMatch[0];	
		if (printMode && i == 1) OutputInitialKernel(lastKernelJ);
		ofile << i << "\t" << thetaB[i]*DEG_PER_RAD << "\t" << xArcMax << "\t" << lastKernelJ << "\t" << paramErr[i] << endl;
	}	// End of for i loop

	//	Set the min theta if both initial guesses are too high.
	if (paramErr[0] < 0.0 && paramErr[1] < 0.0)
	{
		if (paramType != ENDPOINT) thetaBMin = __max(thetaB[0],thetaB[1]);
		else thetaBMax = __min(thetaB[0], thetaB[1]);
	}
	i = 0;
	paramErr[2] = 9.9;
	//	Continue to iterate on thetaB until the MACHE values are equal
	//	So that there is no mistakes in the calculation of Reset the MOC grid above J
	//	and along I
	while( fabs(paramErr[2]) > 1e-8 && i++ < 20)
	{
		if ( paramErr[0] != paramErr[1])
			thetaB[2] = thetaB[1] - paramErr[1] * (thetaB[1] - thetaB[0]) / (paramErr[1] - paramErr[0]);
		else i = 20;

		if (thetaB[2] < thetaBMin) thetaB[2] = thetaBMin;
		else if (thetaB[2] > thetaBMax) thetaB[2] = thetaBMax;

		xArcMax = sin(thetaB[2])*RWTD;	
		j = 0;	//	Step through the RRCs, if they have already been calculated, then use them.
		while ( x[0][j+1] < xArcMax && j < lastKernelJ) j++;
		if ( j > 7) j-=5;
		else j = 0;
		lastKernelJ = CalcRRCsAlongArc(j, RWTD, thetaB[2],DTLIMIT, pAmb, geom );
		if ( lastKernelJ == -999019) return SEC_FAIL;
		while (lastKernelJ < 0)
		{
			//	-lastKernelJ is the max thetaB.
			lastKernelJ = -lastKernelJ;
			thetaBMax = asin(x[0][lastKernelJ]/RWTD);
			thetaB[2] = thetaBMax;
			j = 0;	
			while ( x[0][j+1] < xArcMax && j < lastKernelJ) j++;
			if ( j > 3) j-=2;
			else j = 0;
			lastKernelJ = CalcRRCsAlongArc(j, RWTD, thetaB[2],DTLIMIT, pAmb, geom );
			if ( lastKernelJ == -999019) return SEC_FAIL;
		}

		dS = CalcLRCDE(lastKernelJ, iLast[lastKernelJ], pAmb, geom, nRRCPlus, nType,paramMatch[1],0);
		
		while (dS.dSi[2] == SEC_FAIL_LOW || dS.dSi[2] == SEC_FAIL_HIGH && i++ < 20)
		{
			// This means that the last point along THETAB did not meet the 
			//	requirements. 
			if (dS.dSi[2] == SEC_FAIL_LOW ) thetaBMin = thetaB[2];
			else thetaBMax = thetaB[2];
				
			thetaB[2] = (thetaBMin+thetaBMax)/2.;
			xArcMax = sin(thetaB[2])*RWTD;
 			j = 0;
			while ( x[0][j+1] < xArcMax && j < lastKernelJ) j++;
			if ( j > 7) j-=5;
			else j = 0;
			lastKernelJ = CalcRRCsAlongArc(j, RWTD, thetaB[2],DTLIMIT, pAmb, geom );
			if ( lastKernelJ == -999019) return SEC_FAIL;
			while (lastKernelJ < 0)
			{
				//	-lastKernelJ is the max thetaB.
				lastKernelJ = -lastKernelJ;
				thetaBMax = asin(x[0][lastKernelJ]/RWTD);
				thetaB[2] = thetaBMax;
				j = 0;	
				while ( x[0][j+1] < xArcMax && j < lastKernelJ) j++;
				if ( j > 3) j-=2;
				else j = 0;
				lastKernelJ = CalcRRCsAlongArc(j, RWTD, thetaB[2],DTLIMIT, pAmb, geom );
				if ( lastKernelJ == -999019) return SEC_FAIL;
			}
			dS = CalcLRCDE(lastKernelJ, iLast[lastKernelJ], pAmb, geom, nRRCPlus, nType,  paramMatch[1],0);
		}
		
		//	Depending on what the design parameter is, Mach, Eps of length, the Error
		//	has to be calculated differently.
		if ( paramType == EXITMACH) paramExit = dS.dSx[9];
		else if ( paramType == NOZZLELENGTH || paramType == ENDPOINT) paramExit = dS.dSx[7];
		else if ( paramType == EXITPRESSURE) paramExit = pTotal/dS.dSx[11];
		else if ( paramType == EPS)
		{
			//	Calculate the difference in the calculated area ratio and the desired EPS
			if (geom == TWOD) paramExit = dS.dSx[8];
			else if (geom == AXI) paramExit = dS.dSx[8]*dS.dSx[8];
		}
		else
		{
			paramExit = 0.0;
			AfxMessageBox("Exit Parameter not set in CalcContouredNozzle.");
			return SEC_FAIL;
		}

		paramErr[2] = (paramExit - paramMatch[0])/paramMatch[0];	
		
		thetaB[0] = thetaB[1];
		thetaB[1] = thetaB[2];
		paramErr[0] = paramErr[1];
		paramErr[1] = paramErr[2];
		ofile << i << "\t" << thetaB[2]*DEG_PER_RAD  << "\t" << xArcMax << "\t" << lastKernelJ << "\t" << paramErr[2] << endl;
		if (paramErr[2] < 0.0)
		{
			if(paramType != ENDPOINT ) thetaBMin = thetaB[2]; 
			else thetaBMax = thetaB[2];
		}
		else if (paramErr[2] > 0.0)
		{
			if(paramType != ENDPOINT ) thetaBMax = thetaB[2]; 
			else thetaBMin = thetaB[2];
		}
	}
	if ( i == 21) 
	{
		if (fabs(paramErr[2]) > 1e-5)
		{
			CString str;
			if (thetaB[2] == thetaBMax || thetaB[2] == thetaBMin) 
			{
				str = "The RAO Method could not solve for this high\n";
				str += "area ratio nozzle. Try the 'Set End Point' type\n";
				str += "If you have tried this, then this program cannot\n";
				str += "find a solution. Sorry.";
			}
			else
			{
				str = "Could not converge on THETAB for given Area Ratio";
			}
			AfxMessageBox(str);
			OutputJ(lastKernelJ,"LastKernelFAIL.out");
			return SEC_FAIL;
		}
	}
	ofile.close();
	//	Reset all points above lastKernel J
	ResetGrid(0,maxLRC-1,lastKernelJ+1, maxRRC-1);
	if (printMode) OutputInitialKernel(lastKernelJ);
	//	Use these functions to set the point DE for the final time.
	dS = CalcLRCDE(lastKernelJ, iLast[lastKernelJ], pAmb, geom, nRRCPlus, nType,  paramMatch[1],1);
		
	if(printMode)OutputJ(lastKernelJ,"LastKernel.out");
	thetaBAns = thetaB[2];
	//	To make the following easier set j = lastKernelJ
	j = lastKernelJ;
	if ( nType == RAO) thetaBMax = thetaB[2];
	else if (nType == PERFECT) thetaBMin = thetaB[2];
	
	//	At this point the correct Initial expansion angle has been found. Also, Point D 
	//	has been found, and nRRCPlus points along DE (including E) has been found. 
	//	The RRC's defined by the arc have already been computed. 
	//	Since this was an iteration, a new point [i][j] (=D) along BD was not computed
	//	so that the solution would not messed up.  Now that the iteration is complete
	//	set point [i][j] equal to point D. i = dS.dSi[0];

	int ii = dS.dSi[0];
	//	Add point D to the current j by shifting down all of the other points
	//	Only do this if the point D is different then ii
	if (x[ii][j] != dS.dSx[0] && r[ii][j] != dS.dSx[1])
	{
		for ( i = iLast[j]; i >= ii; i--)
		{
			x[i+1][j] = x[i][j];
			r[i+1][j] = r[i][j];
			mach[i+1][j] = mach[i][j];
			theta[i+1][j] = theta[i][j];
			pres[i+1][j] = pres[i][j];
			temp[i+1][j] = temp[i][j];
			rho[i+1][j] = rho[i][j];
			massflow[i+1][j] = massflow[i][j];
			gamma[i+1][j] = gamma[i][j];
		}
		iLast[j] += 1;
		//	set point ii equal to D
		x[ii][j]	=	dS.dSx[0];
		r[ii][j]	=	dS.dSx[1];
		mach[ii][j] =	dS.dSx[2];
		theta[ii][j]=	dS.dSx[3];
		pres[ii][j] =	dS.dSx[4];
		temp[ii][j] =	dS.dSx[5];
		rho[ii][j] =	dS.dSx[6];
		massflow[ii][j] = dS.dSx[15];
		gamma[ii][j] = g;
		CalcMassFlowAndThrustAlongMesh(j,j,pAmb,geom);
	}

	//	Set the i & j of the line BD
	iBD = i+1;
	jBD = j;
	
	//	Set the wall point E. These are alos equal to [ii][lastRRC]
	lastRRC = dS.dSi[1]; 
	x[0][lastRRC]	=	dS.dSx[7];
	r[0][lastRRC]	=	dS.dSx[8];
	mach[0][lastRRC] =	dS.dSx[9];
	theta[0][lastRRC]=	dS.dSx[10];
	pres[0][lastRRC] =	dS.dSx[11];
	temp[0][lastRRC] =	dS.dSx[12];
	rho[0][lastRRC] =	dS.dSx[13];
	massflow[0][lastRRC] = massflow[0][0];
	gamma[0][lastRRC] = dS.dSx[14];
	iLast[lastRRC] = 0;
		
	//	Now compute the LRCs along j from the line DE to i = 0. This is region BDE.
	//	The same logic for creating the internal points is going to be used, but
	//	backwards. The line BD (j) is already solved for, so start with j+1 and go to
	//	LASTRRC. For a point [i][j], [i+1][j] and [i][j-1] is known where [i+1][j] is influenced by the other
	//	two points.
	//	Inputs 
	CalcBDERegion(ii,j,lastRRC,geom);

	//	This is the region ADE from DE to the nozzle exit plane
	CalcRemainingMesh(ii,j, lastRRC, geom);
	//	Use this function to determine the nozzle contour based on BDE region.
	CalcWallContour(ii, j+1, lastRRC, geom);

	//	Now that the wall has been found, calculate the massflow at each point
	//	of the mesh. This will be used later in the streamline determination
	//	At this point, the entire MOC mesh is done. Now calculate the mass flow of each point
 	CalcMassFlowAndThrustAlongMesh(j+1, lastRRC, pAmb, geom);

	//	At this point all of the points of the MOC are created.  Some of the points are beyond 
	//	nozzle exit plane.  Crop the RRCs at the nozzle plane.  Also, create a point along
	//	the axis that is at the nozzle exit plane.
	if (printMode) OutputUncroppedKernel(lastRRC);
	CropNozzleToLength(lastRRC);

	jDELast = CalcDE(iBD, jBD, lastRRC, nType, geom);

	//	Now, calculate the streamlines, based on mass flow. For each point along
	//	the initial data line, find the points on the proceeding streamlines, where
	//	the mass is equal
	if (printMode) 
	{
		OutputStreamlines(iLast[0], lastRRC, nni, nnj,geom);
		OutputFinalKernel(ii,j, lastRRC);
	}
	OutputTDKRAODataFile(j, lastRRC);

	//	Set up the chart
	MOCPlotDialog plot;
	plot.FormatMOCChart(lastRRC);
	plot.PlotMOCChartPoints( x[0], r[0], lastRRC, x[0][lastRRC]+.1, r[0][lastRRC]+.1);
	plot.DoModal();

	return SEC_OK;
}

//**************************************************************************************
//**************************************************************************************
//**************************************************************************************
int MOC_GridCalc::CalcConeNozzle( int paramType, double *paramMatch, 
								  double pAmb, int geom, int nni, int nnj)
{
	//	Inputs:
	//	paramType {NOPARAM,EXITMACH, EPS, NOZZLELENGTH, EXITPRESSURE};
	//	paramMatch: 1. value of the paramType. 2. Cone Angle in degrees
	//	g : gamma
	//	pAmb: ambient pressure (psia)
	//	geom: TWOD or AXI
	//	nRRCPlus: number of additional RRC above line BD (TWOD only)
	
	//	Note: The Initial throat line TT' has alrady been computed.
	//	This routine is much simpler than the contoured nozzle because the 
	//	wall has already been set. The only thing left to do it to calculate the
	//	MOC grid.
	
	double paramExit, paramErr,ratio = 0;
	int j;

	paramExit = paramMatch[0];
	//	First Find the wall contour up to the given angle
	//	The initial data line ( j =0 ) has already been calculated
	j = CalcRRCsAlongArc(0, RWTD, paramMatch[1]*RAD_PER_DEG,DTLIMIT, pAmb, geom );
	if ( j == -999019) return SEC_FAIL;
	paramErr = -1.0;
	while ( paramErr < 0.0)
	{
		j++;
		//	The first step is to calculate the wall point [0][j]. 
		CalcConeWallPoint(j, paramMatch[1]*RAD_PER_DEG, geom);
		//	Depending on the paramType, determine is the current j RRC is beyond 
		//	what is required
		if ( paramType == EXITMACH) paramExit = mach[0][j];
		else if ( paramType == NOZZLELENGTH ) paramExit = x[0][j];
		else if ( paramType == EXITPRESSURE) paramExit = pTotal/pres[0][j];
		else if ( paramType == EPS)
		{
			//	Calculate the difference in the calculated area ratio and the desired EPS
			if (geom == TWOD) paramExit = r[0][j];
			else if (geom == AXI) paramExit = r[0][j]*r[0][j];
		}
		
		paramErr = (paramExit - paramMatch[0]);
		if (paramErr < 0.0)
		{
			//	Calculate the interior and axial points along this j
			iLast[j] = iLast[j-1];
			//	iEnd = where the centerline is along j
			if (CalcInteriorMeshPoints( j, 1, iLast[j], 0, geom) == SEC_FAIL) return SEC_FAIL;
			CalcAxialMeshPoint(j, iLast[j]);
			//	Calculate the mass flow at each point along the calculated j
			CalcMassFlowAndThrustAlongMesh(j, j, pAmb, geom);
			
		}
	}

	//	At this point the j line is beyond the constraint and the j-1 RRC is
	//	below the constraint. Interpolate on the exit parameter to find the nozzle wall
	//	end point.  Set that point = to j;

	if ( paramType == EXITMACH) ratio = (mach[0][j] - paramMatch[0])/(mach[0][j] - mach[0][j-1]);
	else if ( paramType == NOZZLELENGTH ) ratio = (x[0][j] - paramMatch[0])/( x[0][j] - x[0][j-1]);
	else if ( paramType == EXITPRESSURE) ratio = (pTotal/pres[0][j] - paramMatch[0])/( pTotal/pres[0][j] - pTotal/pres[0][j-1]);
	else if ( paramType == EPS)
	{
		//	Calculate the difference in the calculated area ratio and the desired EPS
		if (geom == TWOD) ratio = (r[0][j] - paramMatch[0])/ (r[0][j]-r[0][j-1]);
		else if (geom == AXI) ratio = (r[0][j]*r[0][j] - paramMatch[0])/( r[0][j]*r[0][j] - r[0][j-1]*r[0][j-1]);
	}

	x[0][j] = x[0][j] - ratio*(x[0][j] - x[0][j-1]);
	r[0][j] = r[0][j] - ratio*(r[0][j] - r[0][j-1]);
	mach[0][j] = mach[0][j] - ratio*(mach[0][j] - mach[0][j-1]);
	pres[0][j] = pres[0][j] - ratio*(pres[0][j] - pres[0][j-1]);
	temp[0][j] = temp[0][j] - ratio*(temp[0][j] - temp[0][j-1]);
	theta[0][j] = theta[0][j] - ratio*(theta[0][j] - theta[0][j-1]);
	rho[0][j] = rho[0][j] - ratio*(rho[0][j] - rho[0][j-1]);
	gamma[0][j] = gamma[0][j] - ratio*(gamma[0][j] - gamma[0][j-1]);

	thetaBAns=paramMatch[1]*RAD_PER_DEG;
	//	Calculate the interior and axial points along this j
	iLast[j] = iLast[j-1];
	//	iEnd = where the centerline is along j
	if(CalcInteriorMeshPoints( j, 1, iLast[j], 0, geom) == SEC_FAIL) return SEC_FAIL;
	CalcAxialMeshPoint(j, iLast[j]);
	//	Calculate the mass flow at each point along the calculated j
	CalcMassFlowAndThrustAlongMesh(j, j, pAmb, geom);
		
	lastRRC = j;	
		
	CropNozzleToLength(lastRRC);

		//	Now, calculate the streamlines, based on mass flow. For each point along
	//	the initial data line, find the points on the proceeding streamlines, where
	//	the mass is equal
	if (printMode) 
	{
		OutputStreamlines(iLast[0], lastRRC, nni, nnj,geom);
	}
	OutputTDKRAODataFile(0, lastRRC);

	//	Set up the chart
	MOCPlotDialog plot;
	plot.FormatMOCChart(lastRRC);
	plot.PlotMOCChartPoints( x[0], r[0], lastRRC, x[0][lastRRC]+.1, r[0][lastRRC]+.1);
	plot.DoModal();

	return SEC_OK;
}
//*********************************************************************************
//*********************************************************************************
//**************************************************************************************
int MOC_GridCalc::CheckRRCForNegativePoints(int j)
{
	int i;
	for (i = 0; i <= iLast[j]; i++) if (r[i][j] < 0.0) return TRUE;
	return FALSE;
}

//*********************************************************************************
//*********************************************************************************
//**************************************************************************************
int MOC_GridCalc::CalcArcWallPoint( int j, double rad, double betaMax, double dBLimit, int geom)
{
	//	j: RRC number
	//	rad: downstream radius of curvature
	//	betaMax: maximum angle along downstream arc
	//	dBLimit: maximum delta beta allowed between wall points
	//	The x,r,mach,pres,temp, rho,gamma and theta are all global variables defining the 
	//	flow properties at a given point.
	//	This routine is only used when defining the wall point along a predescribed ARC
	//	up to and including the betaMax Limit

	int k, flagSpecial;
	double slrc[4], A[4], B[4], R[4],T1, slrc13;
	double xErr, rErr, MErr, TErr, x3Old, r3Old, m3Old, theta3Old;

	flagSpecial = 0; // No special point have been added

	//	Set constants at point 1 that will be used later
	slrc[1] = lDyDx(theta[1][j-1],CalcMu(mach[1][j-1])); // slope of LRC from point 1
	A[1] = CalcA(mach[1][j-1], gamma[1][j-1]);
	B[1] = CalcB(mach[1][j-1], theta[1][j-1], r[1][j-1]);
	R[1] = CalcR(mach[1][j-1], theta[1][j-1], r[1][j-1]);

	//	For the start of iteration, set point 3 position equal to the previous wall point
	//	and all of the other properties to the point 1 values (point of influence)
	r[0][j] = r[0][j-1];
	x[0][j] = x[1][j-1];
	gamma[0][j] = gamma[1][j-1];
	slrc[3] = slrc[1];
	A[3] = A[1];
	B[3] = B[1];
	R[3] = R[1];

	//	Iterate until X amd R stop moving
	xErr= 9.9;
	rErr= 9.9;
	MErr = 9.9;
	TErr = 9.9;
	x3Old = 9.9;
	r3Old = 9.9;
	m3Old = 9.9;
	theta3Old = 9.9;
	k = 0;
	while ( (fabs(xErr) > conCrit || fabs(rErr) > conCrit || fabs(MErr) > conCrit ||
		fabs(TErr) > conCrit) && k++ < 50)
	{
		//	The starting x and r values for each iteration are x[0][j] and r[0][j]
		//	In each iteration, new values, x3 and r3, are determined.
		//	when x3 = x[0][j] & r3 = r[0][j] then you are done

		//	Slope of line characteristic from 1 to 3 is the Tan Average of the slopes
		slrc13 = TanAvg(slrc[1], slrc[3]);
		//	Know everything else, calculate the x position of point 3
		x[0][j] = (r[0][j] - r[1][j-1])/slrc13 + x[1][j-1];

		//	Calculate a new r and theta for 3. 
		r[0][j] = 1 + rad - sqrt(rad*rad - x[0][j]*x[0][j]);
		theta[0][j] = asin(x[0][j]/rad);

		//	Calculate M3 from compatability equations
		if ( geom == AXI)
		{
			if ( B[1] <= R[1]) T1 = (x[0][j] - x[1][j-1])*(B[3] + B[1]);
			else T1 = (r[0][j] - r[1][j-1])*(R[3] + R[1]);
		}
		else T1 = 0.0;

		//	Calculate new mach at 3 and the related characteristic values.
		mach[0][j] = mach[1][j-1] + (theta[0][j] - theta[1][j-1] + 0.5*T1)/(0.5*(A[1] + A[3]));

		slrc[3] = lDyDx(theta[0][j], CalcMu(mach[0][j]));
		A[3] = CalcA(mach[0][j], gamma[0][j]);
		B[3] = CalcB(mach[0][j], theta[0][j], r[0][j]);
		R[3] = CalcR(mach[0][j], theta[0][j], r[0][j]);

		//	Compute the error of the old x,r,mach and theta vs the new values
		rErr = (r[0][j] - r3Old)/ r3Old;
		xErr = (x[0][j] - x3Old)/ x3Old;
		MErr = (mach[0][j] - m3Old)/ m3Old;
		TErr = (theta[0][j] - theta3Old)/ theta3Old;

		//	Set the old values equal to the new values
		x3Old= x[0][j];
		r3Old = r[0][j];
		m3Old = mach[0][j];
		theta3Old = theta[0][j];
	}
	if ( k == 51)
	{
		AfxMessageBox("Could not converge on Arc wall point");
		exit(1);
	}
	
	//	At the wall the slope of the wall equal theta
	//	If the difference in the current and past wall angle is greater
	//	than dBLimit, then a special point needs to be added where theta[0][j] equals
	//	half the dBLimit.  This make future	calcuations easier. Or if theta[0][j] > the max arc angle, add a point
	//	at theta[0][j] = betaMax.
	if ( (theta[0][j] - theta[0][j-1]) > dBLimit || theta[0][j] > betaMax)
	{
		//	Calculate a new wall point based on the minimum difference between
		//	the BetaMax parameters and the dBLimit
		double alpha;
		alpha = min(betaMax,theta[0][j-1] + 0.5*dBLimit);
		flagSpecial = 1;
		CalcSpecialWallPoint(j, rad, alpha); 
	}

	//	Set the remaining properties
	// use the isentropic realtions to obtain the new properties at the point [i][j]
	//	Anderson MCF p.87
	CalcIsentropicP_T_RHO(0, j, gamma[0][j], mach[0][j]);
	return flagSpecial;
}

//*********************************************************************************
//*********************************************************************************
//**************************************************************************************
void MOC_GridCalc::CalcConeWallPoint( int j, double cA, int geom)
{
	//	j: RRC number
	//	dr: increase in r over r[0][j-1]
	//	cA: cone Angle (rad)
	//	This routine is only used when defining the wall point along a cone at a 
	//	certain angle cA.
	//	There are two points of influence. Point 1 is [1][j-1]. Point 2 is the previous
	//	wall point
	int k;
	double slrc[4], A[4], B[4], R[4],T1, slrc13, MErr, m3Old;

	//	Set constants at point 1 that will be used later
	slrc[1] = lDyDx(theta[1][j-1],CalcMu(mach[1][j-1])); // slope of LRC from point 1
	A[1] = CalcA(mach[1][j-1], gamma[1][j-1]);
	B[1] = CalcB(mach[1][j-1], theta[1][j-1], r[1][j-1]);
	R[1] = CalcR(mach[1][j-1], theta[1][j-1], r[1][j-1]);

	x[0][j] = (r[1][j-1] - r[0][j-1] - slrc[1]*x[1][j-1] + tan(cA)*x[0][j-1])/ (tan(cA) - slrc[1]);
	r[0][j] = r[1][j-1] + slrc[1]*(x[0][j] - x[1][j-1]);
	theta[0][j] = cA;
	gamma[0][j] = gamma[1][j-1];
	
	A[3] = A[1];
	B[3] = B[1];
	R[3] = R[1];
	slrc[3] = lDyDx(theta[0][j],CalcMu(mach[1][j-1]));

	//	Iterate until Mach stops moving
	MErr = 9.9;
	m3Old = 9.9;
	k = 0;
	while ( fabs(MErr) > 1e-8 && k++ < 50)
	{
		//	The starting x and r values for each iteration are x[0][j] and r[0][j]
		//	In each iteration, new values, x3 and r3, are determined.
		//	when x3 = x[0][j] & r3 = r[0][j] then you are done

		//	Slope of line characteristic from 1 to 3 is the Tan Average of the slopes
		slrc13 = TanAvg(slrc[1], slrc[3]);
		
		//	Calculate M3 from compatability equations
		if ( geom == AXI)
		{
			if ( B[1] <= R[1]) T1 = (x[0][j] - x[1][j-1])*(B[3] + B[1]);
			else T1 = (r[0][j] - r[1][j-1])*(R[3] + R[1]);
		}
		else T1 = 0.0;

		//	Calculate new mach at 3 and the related characteristic values.
		mach[0][j] = mach[1][j-1] + (theta[0][j] - theta[1][j-1] + 0.5*T1)/(0.5*(A[1] + A[3]));

		slrc[3] = lDyDx(theta[0][j], CalcMu(mach[0][j]));
		A[3] = CalcA(mach[0][j], gamma[0][j]);
		B[3] = CalcB(mach[0][j], theta[0][j], r[0][j]);
		R[3] = CalcR(mach[0][j], theta[0][j], r[0][j]);

		MErr = (mach[0][j] - m3Old)/ m3Old;
		m3Old = mach[0][j];
	}
	if ( k == 51)
	{
		AfxMessageBox("Could not converge on Cone wall point");
		exit(1);
	}

	//	Set the remaining properties
	// use the isentropic realtions to obtain the new properties at the point [i][j]
	//	Anderson MCF p.87
	CalcIsentropicP_T_RHO(0, j, gamma[0][j], mach[0][j]);

	return ;
}

//**************************************************************************************
//**************************************************************************************
//**************************************************************************************
int MOC_GridCalc::CalcRRCsAlongArc(int j, double rad, double alphaMax, double dALimit, 
									 double pAmb, int geom )
{
	//	j: number of the RRC
	//	rad: downstream radius of curavture of throat arc
	//	alphaMax: max arc angle allowed
	//	dALimit: angular limit between wall point along arc
	//	geom: Geometry type ( TWOD or AXI)
	//	nc: number of characteristic points. Used to define where to stop calculation
	
	//	This function will calculate the RRC [j] for the upper wall to the lower wall including
	//	the Axis points. For the most part, this function is just a control for other
	//	function.
	int specialFlag, k, minusRFlag;
	double xArcMax,mdotErr;

	//	The is going to be a while loop set up that check to see if the calculated 
	//	mass flow along j is equal to the mass flow if the initial data line.
	//	If it isn't, then the DALimit paramter will be cut in half until it does
	
	//	This is the maximum X value to be calculated
	xArcMax = sin(alphaMax)*rad;
	minusRFlag = TRUE;

	while (fabs(x[0][j] - xArcMax) > 1e-6)
	{
		j++;
		massflow[0][j] = 10*massflow[0][0];
		mdotErr = (massflow[0][j] - massflow[0][0])/massflow[0][0];
		specialFlag = 0;
		//	The first step is to calculate the wall point [0][j]. If a special wall point is added
		//	the functions will return a 1. If not then it will return a 0
		specialFlag = CalcArcWallPoint(j, rad, alphaMax, dALimit, geom);
	
		if (r[0][j] - r[0][j-1] <= 0.0)
		{
			AfxMessageBox("Failure in determining Contour wall point\nCalcRRCsAlongArc");
			return -999019;
		}
		iLast[j] = iLast[j-1] + specialFlag;
			//	iEnd = where the centerline is along j
		if(CalcInteriorMeshPoints( j, 1, iLast[j], specialFlag, geom) == SEC_FAIL) return SEC_FAIL;
		CalcAxialMeshPoint(j, iLast[j]);
	
		//	If the calculated axial Mach number is above 50, then assume that
		//	thetaB is too high
		if (mach[iLast[j]][j] > 50.0) return -(j-1);
		//	Calculate the mass flow at each point along the calculated j
		//	massflow[iLast][j] = 0.0. 
		CalcMassFlowAndThrustAlongMesh(j, j, pAmb, geom);
		mdotErr = (massflow[0][j] - massflow[0][0])/massflow[0][0];
	
		//	Due to slight errors in each point, the massflow at the wall, may not be the 
		//	same as the initial massflow.  If the error is less that 2% then move on.
		//	NOTE: That the thrust measurement is still using the old massflow,
		if ( fabs(mdotErr) > .02)
		{
			AfxMessageBox("The calculated wall massflow along the initial expansion region is > 102% of the throat massflow.\nTry increasing # of starting characteristics.\nCalcRRCsAlongArc");
			return -999019;
		}
				
		//	For the MOC Grid, the first RRCs calculated, have points below the axis.
		//	These are valid, however it is best to skip over them when determining
		//	the overall grid a streamlines. Step through each point for each j. For the
		//	first J where there isn't a point below 0.0, set that j to j = 1.
		//*********************
		// Check for negative points, but do not do anything to the initial data line
		if (CheckRRCForNegativePoints(j) == TRUE && j != 1)
		{
			for ( k = 0; k <= iLast[j]; k++)
			{
				x[k][j-1] = x[k][j];
				r[k][j-1] = r[k][j];
				mach[k][j-1] = mach[k][j];
				theta[k][j-1] = theta[k][j];
				pres[k][j-1] = pres[k][j];
				temp[k][j-1] = temp[k][j];
				rho[k][j-1] = rho[k][j];
				gamma[k][j-1] = gamma[k][j];
				iLast[j-1] = iLast[j];
			}//
			j--;
		}
		//*************************
	}
	return j;
}


//**************************************************************************************
//**************************************************************************************
//**************************************************************************************
void MOC_GridCalc::CalcRemainingMesh( int iD, int jD, int jEnd, int geom)
{
	//	Starting from the line DE, calculate the remaining mesh to the centerline.
	//	Use the same routines that were used previously
	//	The point iD,jD is point D
	int j,i,ii;
	
	//	Start at the LRC above BD
	for ( j = jD+1; j <= jEnd; j++)
	{
		iLast[j] = iLast[j-1]+1;
		CalcInteriorMeshPoints(j,iD+1,iLast[j],1,geom);
		CalcAxialMeshPoint(j,iLast[j]);
		
		//	Check to see if any of the r points is less than 0.0. If they are, then delete 
		for ( i = 0; i <= iLast[j]; i++)
		{
			if ( r[i][j] < 0.0)
			{
				iLast[j] -= 1;
				for ( ii = i; ii <= iLast[j]; ii++)
				{
					x[ii][j] = x[ii+1][j];
					r[ii][j] = r[ii+1][j];
					mach[ii][j] = mach[ii+1][j];
					theta[ii][j] = theta[ii+1][j];
					gamma[ii][j] = gamma[ii+1][j];
					pres[ii][j] = pres[ii+1][j];
					temp[ii][j] = temp[ii+1][j];
					rho[ii][j] = rho[ii+1][j];
					massflow[ii][j] = massflow[ii+1][j];
				}
				
			}
		}
	}
	


	return;
}
			
//**************************************************************************************
//**************************************************************************************
//**************************************************************************************
void MOC_GridCalc::CalcWallContour(int iD, int jStart, int jEnd, int geom)
{
	//	At this point the MOC mesh above the region BDE has already been found.
	//	BDE was found by constructing LRC from DE back to the wall. The massflow along the 
	//	DE has been calculated. These values have been stored in massflow[iD][j] from 
	//	j = jStart to jEnd.  The difference in these mass flow is mdotMatch.
	//	Using mdotMatch, the function goes along j until the massflow along j equals
	//	mdotMatch
	//	The point BD is [iD][jStart-1], 

	double mdotMatch, a0,a1,a2,gamma0,gamma1,gamma2,theta0,theta1,theta2,rho0,rho1;
	double rho2,u0,u1,u2,v0,v1,v2,r0,r1,r2,x0,x1,x2,mach0,mach1,mach2,dxdr;
	double rhoUAvg,da,mdot,mdot0,xGuess[3],mdotErr[3], ratio,temp0,temp1,temp2;
	int i,j,k,iTranslate,iNew;

	//	Loop through all of the RRCs
	for ( j = jStart; j < jEnd; j++)
	{
		//	Calculate the massflow alond BD from the wall to the current point[iD]
		mdotMatch = (massflow[0][jStart-1] - massflow[iD][jStart-1]) - massflow[iD][j];

		// Calculate the mass flow from x[iD][j] along j until the mass flow
		// is greater than mdotMatch. At that point, use the secant method to determine 
		//	wall contour.
		temp0 = temp[iD][j];
		gamma0 = gamma[iD][j];
		mach0 = mach[iD][j];
		a0 = sqrt(gamma0*GASCON/molWt*GRAV*temp0);
		theta0 = theta[iD][j];
		rho0 = rho[iD][j];
		u0 = mach0*cos(theta0) * a0;
		v0 = mach0*sin(theta0) * a0;
		r0 = r[iD][j];
		x0 = x[iD][j];
		mdot = 0;
		mdot0 = 0;
		for ( i = iD-1; i >= 0; i--)
		{
			temp1 = temp[i][j];
			gamma1 = gamma[i][j];
			a1 = sqrt(gamma1*GASCON/molWt*GRAV*temp1);
			mach1 = mach[i][j];
			theta1 = theta[i][j];
			rho1 = rho[i][j];
			u1 = mach1*cos(theta1) * a1;
			v1 = mach1*sin(theta1) * a1;
			r1 = r[i][j];
			x1 = x[i][j];

			dxdr = (x1 - x0)/(r1 - r0);
			rhoUAvg = 0.5*GRAV*(rho0*u0 + rho1*u1 - dxdr*(rho0*v0 + rho1*v1));
			if (geom == TWOD) da = (r1 - r0)/12;
			else da = PI*(r1*r1 - r0*r0)/144.;
			mdot += rhoUAvg*da;

			if ( mdot < mdotMatch)
			{
				temp0 = temp1;
				a0 = a1;
				gamma0 = gamma1;
				mach0 = mach1;
				theta0 = theta1;
				rho0 = rho1;
				u0 = u1;
				v0 = v1;
				r0 = r1;
				x0 = x1;
				mdot0 = mdot;
			}
			else break;
		} // End of for j loop
		//	At this point the wall point is between j-1 and j


		xGuess[0] = x0;
		xGuess[1] = x1;
		mdotErr[0] = (mdot0 - mdotMatch)/mdotMatch;
		mdotErr[1] = (mdot - mdotMatch)/mdotMatch;
		mdotErr[2] = 9.9;
		k = 0;
		while ( fabs(mdotErr[2]) > conCrit && k++ < 50)
		{
			if ( mdotErr[0] != mdotErr[1])
				xGuess[2] = xGuess[1] - mdotErr[1] * (xGuess[1] - xGuess[0]) / (mdotErr[1] - mdotErr[0]);
			else k = 50;
					
			//	Interpolate based on the distance between points to find the remaining properties
			//	Set X2 to xGuess for ease of the calculation
			x2 = xGuess[2];
			//	Find r2 assuming a straight line between x1 and x0
			ratio = (x2 - x0) / (x1 - x0);
			r2 = r0 + ratio*(r1 - r0);

			//	This is the ratio set on distance
			ratio = sqrt(pow(x2 - x0,2)+pow(r2 - r0,2))/sqrt(pow(x1-x0,2)+pow(r1-r0,2));
			//	Calculate the massflow from the axis to the point

		
			temp2 = temp0 + ratio*(temp1 - temp0);
			gamma2 = gamma0 + ratio*(gamma1 - gamma0);
			mach2 = mach0 + ratio*(mach1 - mach0);
			theta2 = theta0 + ratio*(theta1 - theta0);
			a2 = sqrt(gamma2*GASCON/molWt*GRAV*temp2);
			rho2 = rho0 + ratio*(rho1 - rho0);
			r2 = r0 + ratio*(r1 - r0);
			u2 = mach2*cos(theta2) * a2;
			v2 = mach2*sin(theta2) * a2;

			dxdr = (x2 - x0)/(r2 - r0);
			rhoUAvg = 0.5*GRAV*(rho0*u0 + rho2*u2 - dxdr*(rho0*v0 + rho2*v2));
			if (geom == TWOD) da = (r2 - r0)/12;
			else da = PI*(r2*r2 - r0*r0)/144.;
			mdot = mdot0 + rhoUAvg*da;
		
			mdotErr[2] = (mdot - mdotMatch)/mdotMatch;
			xGuess[0] = xGuess[1];
			xGuess[1] = xGuess[2];
			mdotErr[0] = mdotErr[1];
			mdotErr[1] = mdotErr[2];
		}	// end of while loop
		if ( k == 51)
		{
			AfxMessageBox("Could not converge on massflow\nCalcWallContour");
			exit(1);
		}

		//	Set the calculate wall point equal to [0][j] and all of other points in line 
		iTranslate = i;
		x[0][j] = x2;
		r[0][j] = r2;
		theta[0][j] = theta2;
		CalcIsentropicP_T_RHO(0,j,gamma2,mach2);
			
		for ( i = iTranslate+1; i <= iLast[j]; i++)
		{
			iNew = i-iTranslate;
			x[iNew][j] = x[i][j];
			r[iNew][j] = r[i][j];
			theta[iNew][j] = theta[i][j];
			CalcIsentropicP_T_RHO(iNew,j,gamma[i][j],mach[i][j]);
		}
		iLast[j] = iNew;	
	}	// end of for j loop

	//	The jEnd LRC has to be calculated. It is know that iD has to be translated to 0
	iTranslate = iD;
	for ( i = iTranslate; i <= iLast[j]; i++)
	{
		iNew = i-iTranslate;
		x[iNew][j] = x[i][j];
		r[iNew][j] = r[i][j];
		theta[iNew][j] = theta[i][j];
		CalcIsentropicP_T_RHO(iNew,j,gamma[i][j],mach[i][j]);
	}
	iLast[j] = iNew;

	//	Output the wall contour
	if (printMode)
	{
		fstream wallfile;
		wallfile.open("wall.out", ios::out);
		wallfile << "i\tj\tX/R*\tR/R*\tmach\ttheta(deg)\tPressure, psia" << endl;
		for ( j = 0; j <= jEnd; j++)
			wallfile << 0 << "\t" << j << "\t" << x[0][j] << "\t" << r[0][j] << "\t"
				<< mach[0][j] << "\t" << theta[0][j]*DEG_PER_RAD << "\t" << pres[0][j] << endl;
		wallfile.close();
	}

	return ;
}

//**************************************************************************************
//**************************************************************************************
//**************************************************************************************
void MOC_GridCalc::CropNozzleToLength( int jEnd)
{
	double ratio, dxErr;
	int i,j,newLast;

	iLast[jEnd] = 0;
	//	2. Crop the RRCs at x = x[0][jEnd] {the nozzle exit plane).
	//	3. For the last RRC that crosses the centerline r = 0, Add a point at the nozzle exit plane.
	for (j = 1; j < jEnd; j ++)
	{
		// If the last axis point [iLast[j]][j] is greater than the x[iLast][jEnd], crop
		//	the new RRC.
		if ( x[iLast[j]][j] >= x[0][jEnd])
		{
			//	This is the centerline case
			if ( r[iLast[j-1]][j-1] == 0.0)
			{
				newLast = iLast[j-1] + 1;
				r[newLast][j-1] = 0.0;
				ratio = (x[0][jEnd] - x[iLast[j-1]][j-1]) / 
					(x[iLast[j]][j] - x[iLast[j-1]][j-1]);
				x[newLast][j-1] = x[iLast[j-1]][j-1] + 
					ratio*(x[iLast[j]][j] - x[iLast[j-1]][j-1]);
				mach[newLast][j-1] = mach[iLast[j-1]][j-1] + 
					ratio*(mach[iLast[j]][j] - mach[iLast[j-1]][j-1]);
				temp[newLast][j-1] = temp[iLast[j-1]][j-1] + 
					ratio*(temp[iLast[j]][j] - temp[iLast[j-1]][j-1]);
				pres[newLast][j-1] = pres[iLast[j-1]][j-1] + 
					ratio*(pres[iLast[j]][j] - pres[iLast[j-1]][j-1]);
				theta[newLast][j-1] = theta[iLast[j-1]][j-1] + 
					ratio*(theta[iLast[j]][j] - theta[iLast[j-1]][j-1]);
				rho[newLast][j-1] = rho[iLast[j-1]][j-1] + 
					ratio*(rho[iLast[j]][j] - rho[iLast[j-1]][j-1]);
				gamma[newLast][j-1] = gamma[iLast[j-1]][j-1] + 
					ratio*(gamma[iLast[j]][j] - gamma[iLast[j-1]][j-1]);
				massflow[newLast][j-1] = 0.0;
				thrust[newLast][j-1] = 0.0;
				Sthrust[newLast][j-1] = 0.0;
				iLast[j-1] = newLast;
			}
			
			i = 0;
			//	Step through each point along each j until you get to one that is close to
			//	or beyond the end of the nozzle
			while( x[i][j] < x[0][jEnd] && i <= iLast[j]) i++; 
			//	The point x[i][j] is beyond the end of the nozzle
			//	if i = 0, then there is nothing to do
			if ( i == 0)
			{
				//set the point [0][j] equal to [0][j-1]
				x[i][j] = x[i][j-1];
				mach[i][j] = mach[i][j-1];
				theta[i][j] = theta[i][j-1];
				gamma[i][j] = gamma[i][j-1];
				CalcIsentropicP_T_RHO(i,j, gamma[i][j], mach[i][j-1]);
				rho[i][j] = rho[i][j-1];
				r[i][j] = r[i][j-1];
				massflow[i][j] = massflow[i][j-1];
				iLast[j] = 0;
				thrust[i][j] = thrust[i][j-1];
				Sthrust[i][j] = Sthrust[i][j-1];
			}
			else
			{
				//	Check the previous point to see if it is close to the end of the nozzle.
				//	if it is, then stop there
				dxErr = fabs(x[i-1][j] - x[0][jEnd])/x[0][jEnd];
				if ( dxErr < 1e-8) iLast[j] = i-1;
				else
				{
					if ( x[i][j] == x[i-1][j]) ratio = 0.0;
					else ratio = (x[0][jEnd] - x[i-1][j]) / (x[i][j] - x[i-1][j]);
					x[i][j] = x[0][jEnd];
					mach[i][j] = mach[i-1][j] + ratio*(mach[i][j] - mach[i-1][j]);
					theta[i][j] = theta[i-1][j] + ratio*(theta[i][j] - theta[i-1][j]);
					CalcIsentropicP_T_RHO(i,j, gamma[i-1][j], mach[i][j]);
					rho[i][j] = rho[i-1][j] + ratio*(rho[i][j] - rho[i-1][j]);
					r[i][j] = r[i-1][j] + ratio*(r[i][j] - r[i-1][j]);
					gamma[i][j] = gamma[i-1][j] + ratio*(gamma[i][j] - gamma[i-1][j]);
					massflow[i][j] = massflow[i-1][j] + ratio*(massflow[i][j] - massflow[i-1][j]);
					iLast[j] = i;
					thrust[i][j] = thrust[i-1][j] + ratio*(thrust[i][j] - thrust[i-1][j]);
					Sthrust[i][j] = Sthrust[i-1][j] + ratio*(Sthrust[i][j] - Sthrust[i-1][j]);
				}
			}
		}
	}
	return;
}



//**************************************************************************************
//**************************************************************************************
//**************************************************************************************
double MOC_GridCalc::CalcMdotBD( int j, double xD)
{
	//	This function calculates the mass flow from the wall to a point XD along j
	//	The massflow at each point along j has already been calculated from the axis
	//	to wall.  

	double mdot, massflowD, ratio, rD;
	int ii;
		
	//	Find the point where XD is between and calculate the properties
	ii = 0;
	while ( xD > x[++ii][j]);	// XD is between ii-1 and ii

	//	Find the ratio of the point XD to [ii-1] and ii based on distance
	//	first find rD assuming a straight line between the two points
	ratio = (xD - x[ii-1][j]) / (x[ii][j] - x[ii-1][j]);
	rD = r[ii-1][j] + ratio*(r[ii][j] - r[ii-1][j]);

	ratio = sqrt(pow(xD - x[ii-1][j],2)+pow(rD - r[ii-1][j],2))/sqrt(pow(x[ii][j]-x[ii-1][j],2)+pow(r[ii][j]-r[ii-1][j],2));
	//	Calculate the massflow from the axis to the point
	massflowD = massflow[ii-1][j] + ratio*(massflow[ii][j] - massflow[ii-1][j]);
	//	This is the mass flow from the wall to the point
	mdot = massflow[0][j] - massflowD;
	
/*	ofstream fs;
	fs.open("Mdot.out");
	for (ii = 0; ii <= iLast[j]; ii++)
	{
		fs << x[ii][j] << "\t" << massflow[ii][j] << endl;
	}*/
	return mdot;
}

//**************************************************************************************
//**************************************************************************************
//**************************************************************************************
dummyStruct MOC_GridCalc::CalcLRCDE(int j, int iEnd, double pAmb, int geom, int nRRCPlus,
									  int nType, double rMatch, int pointFlag)
{
	//	This function is used to determine the LRC DE along the RRC (j) where the mass
	//	flow from [0][j](point B) to point D along j is equal to the mass flow from D to 
	//	E along the LRC. 
	//	Depending on the nType and pType parameters, some other constraints have to be met.
	//	For instarnace, if a minimum length nozzle is to be found eq. 14 from RAO is met.
	//	j: designation of RRC
	//	iEnd: designation of centerline point along RRC[iEnd][j]. As a note [0][j] corresponds
	//	to the wall point.
	//	nType: nozzle Type (NONE , MIN Length, CONE, PERFECT, ENDPOINT)
	//	rMatch is used when the end point has been fixed
	double xD[3], param_err[3], mdotBD, thetaCalc, xDMin, xDMax, w0, xDLast, xDMaxBisect;
	double xDMinBisect;
	int i;
	dummyStruct dS;		
					// dS.dSx[0] =	xD
					// dS.dSx[1] =	rD
					//		2	=	machD
					//		3	=	thetaD
					//		4	=	presD
					//		5	=	tempD
					//		6	=	RhoD
					//		7	=	xE
					//		8	=	rE
					//		9	=	machE
					//		10	=	thetaE
					//		11	=	presE
					//		12	=	tempE
					//		13	=	rhoE
					//		14	=	gammaE
					//		15	=	Mdot
					//		16	=   wE ft/s
					//	dS.dSi[0] = i;
					//	dS.dSi[1] = lastRRC
					//	dS.dSi[2] = OK or FAIL

	//	If the nozzle type is PERFECT, then it is known that the point x(iEnd][j]) is the 
	//	point that has to be chosen. If not, 
	//	The secant method will be used to iterate on the axial location (x) along
	//	BD. 

	if (nType == PERFECT)
	{
		mdotBD = CalcMdotBD(j, x[iEnd][j]);
		dS = FindPointE(j, x[iEnd][j], mdotBD, geom, nType, nRRCPlus,pointFlag);
	}
	else 
	{
		//	First Calculate the mass flow along BD, where D is at xD[i]
		//	Now D is known. Create a LRC from D to a point E where the mass flow
		//	of DE = mdotBD. Return dS that contains information on Points D and E.
		//	Once point E has been found, nRRCPlus points will be created along 
		//	DE, equally spaced in Mach number, so that the rest of the MOC grid
		//	can be calculated
		//	Set the first guess equal to the wall point on BD.  This is obviuosly
		//	not the right point, but it is a good start
		xDMax = __min(9e9,x[iEnd][j]);
		xD[0] = x[0][j];
		xDMin = x[0][j];
		//	Velocity
		w0 = mach[0][j] * sqrt(gamma[0][j]*GASCON/molWt*GRAV*temp[0][j]);
		if ( nType == RAO)
		{
			//	For there to be a correct RAO solution, theta[0][j] (the flow angle at the 
			//	nozzle wall exit must be equal to the equation below (thetaCalc). Compare
			//	these numbers and set the error to param_err
			
			thetaCalc = 0.5 * asin(2*(pres[0][j] - pAmb)*144 / 
				( rho[0][j]*w0*w0 * tan(CalcMu(mach[0][j]))));
			param_err[0] = (theta[0][j] - thetaCalc) / thetaCalc;
			if (param_err[0] < 0.)
			{
				//	The chosen theta is too low
				dS.dSi[2] = SEC_FAIL_LOW;
				return dS;
			}
				
			if (thetaCalc < 0.0)
			{
				AfxMessageBox("(1)Ambient pressure > nozzle pressure. This is bad. Low p Ambient");
				dS.dSi[2] = SEC_FAIL;
				return dS;
			}
		}
		else if ( nType == FIXEDEND)
		{	
			//	If the param_err > 0.0, then the chosen thetaB is too high
			param_err[0] = (r[0][j] - rMatch);
			if ( param_err[0] > 0.0)
			{
				dS.dSi[2] = SEC_FAIL_HIGH;
				return dS;
			}
		}
		
//		ofstream fs;
//		fs.open("thetaCalc.out", ios::app);
//		fs << "i\tXD\tThetaCalc\tThetaE\txE\trE\tErr" << endl;
		
		//	Step through each point along j to find where the param_err changes sign < 0.0;
		//	This is done differently for a Rao and a Fixend nozzle

		i = 1;
		

		if (nType == RAO)
		{
			param_err[1] = 9e9;
			while(i < iEnd && thetaCalc >= 0.0 && param_err[1] > 0.0 )
			{
				xD[1] = x[i][j];
				mdotBD = CalcMdotBD(j, xD[1]);
				dS = FindPointE(j, xD[1], mdotBD, geom, nType, nRRCPlus,pointFlag);
				if ( nType == RAO)
				{
					thetaCalc = 0.5 * asin(2*(dS.dSx[11] - pAmb)*144 / 
						( dS.dSx[13]*dS.dSx[16]*dS.dSx[16] * tan(CalcMu(dS.dSx[9]))));
					param_err[1] = (dS.dSx[10] - thetaCalc) / fabs(thetaCalc);
				}
				
		//			fs << i << "\t" << xD[1] << "\t" << thetaCalc*DEG_PER_RAD << "\t" << dS.dSx[10]*DEG_PER_RAD
		//				<< "\t" << dS.dSx[0] << "\t" <<  dS.dSx[1] << "\t" << param_err[1] << endl;
				i++;
			}	// end of while
		}
		else if ( nType == FIXEDEND)
		{
			param_err[1] = param_err[0];
			while(i < iEnd && param_err[1] < 0.0 )
			{
				xD[1] = x[i][j];
				mdotBD = CalcMdotBD(j, xD[1]);
				dS = FindPointE(j, xD[1], mdotBD, geom, nType, nRRCPlus,pointFlag);
				param_err[1] = (dS.dSx[8] - rMatch);
				
				//			fs << i << "\t" << xD[1] << "\t" << thetaCalc*DEG_PER_RAD << "\t" << dS.dSx[10]*DEG_PER_RAD
				//				<< "\t" << dS.dSx[0] << "\t" <<  dS.dSx[1] << "\t" << param_err[1] << endl;
				i++;
			}	// end of while
		}
//		fs.close();
		if (thetaCalc < 0.0 && nType == RAO)
		{
			dS.dSi[2] = SEC_FAIL_HIGH;
			return dS;
		}
		else if (i == iEnd)
		{
			//	The code stepped through all of the points along BD but was unable to find a point where the 
			//	param_err < 0.0. Therefore the THETAB guess is too high.
			dS.dSi[2] = SEC_FAIL_HIGH;
			return dS;
		}
		
		//	If the code makes it to here, then the param_err[1] > 0.0 and all is well. Iterate between
		//	XD[1] and XD[0] which will be the point in front of it
		xD[0] = x[i-2][j];
		mdotBD = CalcMdotBD(j, xD[0]);
		dS = FindPointE(j, xD[0], mdotBD, geom, nType, nRRCPlus,pointFlag);
		if ( nType == RAO)
		{
			thetaCalc = 0.5 * asin(2*(dS.dSx[11] - pAmb)*144 / 
				( dS.dSx[13]*dS.dSx[16]*dS.dSx[16] * tan(CalcMu(dS.dSx[9]))));
			param_err[0] = (dS.dSx[10] - thetaCalc) / thetaCalc;
						
			if (thetaCalc < 0.0)
			{
				AfxMessageBox("(2)Ambient pressure > nozzle pressure. This is bad. Low p Ambient");
				dS.dSi[2] = SEC_FAIL;
				return dS;
			}
		}
		else if ( nType == FIXEDEND) param_err[0] = (dS.dSx[8] - rMatch);

		

		i = 0;
		param_err[2] = 9.9;
		//	Iterate until the calculated thetaE is equal to the returned thetaE
		while( fabs(param_err[2]) > 1e-7 && i++ <50)
		{
			if ( param_err[0] != param_err[1])
				xD[2] = xD[1] - param_err[1] * (xD[1] - xD[0]) / (param_err[1] - param_err[0]);
			else i = 50;
			
			if (xD[2] > xDMax) xD[2] = xDMax;
			else if (xD[2] < xDMin)
			{
				xD[2] = xDMin;
				xDLast = xD[1];
			}

			if (xD[2] <= x[0][j]) 
			{
				xD[2] = x[0][j];
				if ( nType == RAO)
				{
					thetaCalc = 0.5 * asin(2*(pres[0][j] - pAmb)*144 / 
					( rho[0][j]*w0*w0 * tan(CalcMu(mach[0][j]))));
					param_err[2] = (theta[0][j] - thetaCalc) / thetaCalc;
					
					if (thetaCalc < 0.0)
					{
						AfxMessageBox("Calculated Theta < 0.0");
						dS.dSi[2] = SEC_FAIL;
						return dS;
					}
				}
				else if ( nType == FIXEDEND) param_err[2] = (r[0][j] - rMatch);			
			}
			else
			{
				if (xD[2] >= x[iEnd][j]) 
				{	
					//	This is a perfect nozzle case
					xD[2] = x[iEnd][j];										
					mdotBD = CalcMdotBD(j, xD[2]);
					dS = FindPointE(j, xD[2], mdotBD, geom,PERFECT, nRRCPlus,pointFlag);
				}
				else if (xD[2] < xDMin)
				{
					xD[2] = xDMin;
					mdotBD = CalcMdotBD(j, xD[2]);
					dS = FindPointE(j, xD[2], mdotBD, geom,nType, nRRCPlus,pointFlag);
				}
				else
				{
					mdotBD = CalcMdotBD(j, xD[2]);
					dS = FindPointE(j, xD[2], mdotBD, geom,nType, nRRCPlus,pointFlag);
				}
				if (dS.dSi[2] == SEC_FAIL)
				{
					//	FindPointE could not Find a LRC from xD[2]. Try increasing xD[2]
					xDMin = xD[2];
					xD[2] = 0.5*(xDMin + xDMax);
					mdotBD = CalcMdotBD(j, xD[2]);
					dS = FindPointE(j, xD[2], mdotBD, geom,nType, nRRCPlus,pointFlag);						
				}

				if ( nType == RAO)
				{
					//	For a min length nozzle
					//	From RAO eq. 14, THETAE must equal the equation below.  Calculate the THETAE
					//	from the equation (thetaCalc) and compare it to the thetaE returned in dS.
					//	For a perfect nozzle THETAE has to equal 0.0
					thetaCalc = 0.5 * asin(2*(dS.dSx[11] - pAmb)*144 / 
						( dS.dSx[13]*dS.dSx[16]*dS.dSx[16] * tan(CalcMu(dS.dSx[9]))));
					param_err[2] = (dS.dSx[10] - thetaCalc) / thetaCalc;
					
					if (thetaCalc < 0.0)
					{
						AfxMessageBox("Calculated Theta < 0.0");
						dS.dSi[2] = SEC_FAIL;
						return dS;
					}
				}
				else if ( nType == FIXEDEND) param_err[2] = (dS.dSx[8] - rMatch);
			}
				
			//	If the convergence criteria is not met, Reset the DE LRC
			if (fabs(param_err[2]) > 1e-7) ResetGrid(dS.dSi[0],dS.dSi[0],j+1, j+nRRCPlus);
			
			xD[0] = xD[1];
			xD[1] = xD[2];
			param_err[0] = param_err[1];
			param_err[1] = param_err[2];
			//	There are some cases that fail in the secant method, but do
			//	have a solution
			if ( param_err[2] < 0.0) xDMaxBisect = xD[2];
			else if (param_err[2] > 0.0) xDMinBisect = xD[2];
		} // End of While
		if ( i == 51) 
		{
			if ( xD[2] == xDMax) dS.dSi[2] = SEC_FAIL_LOW;
			else dS.dSi[2] = SEC_FAIL_HIGH;
			return dS;
		} // end of if i=51
	}	// end of else

	//	Return in dS the properties at the points D and E.
	dS.dSx[15] = mdotBD;
	dS.dSi[2] = SEC_OK;
	return dS;
}



//**************************************************************************************
//**************************************************************************************
//**************************************************************************************
dummyStruct MOC_GridCalc::FindPointE( int jStart, double xD, double mdotMatch, int geom, 
									   int nType, int nRRCPlus, int pointFlag)
{

	//	This function constructs a LRC from point D to some point E, where the mass
	//	flow from DE = mdotMatch.  Remember that mdotMatch is a massflux
	//	By definition of a LRC, RAO eq 12 & 13
	//	L2 = - W*cos(theta-mu)/cos(mu)
	//	L3 = -r*RHO*W^2*sin^2(theta)*tan(mu);
	//	L2 and L3 are constant Lagrange multipliers.
	//	The integral for mass flow is S(DE) RHO*W*sin(mu)/sin(theta+mu)*r*dr;
	//	Let it be noted that this function will only return the properties at
	//	point E so that the interation in CalcLRCDE can be solved. Along the way
	//	the points along DE are also found if POINTFLAG = 1. 
	//	dummyStruct Used to return multiple values
	//				dS.dSx[0]	=	xD
					// dS.dSx[1] =	rD
					//		2	=	machD
					//		3	=	thetaD
					//		4	=	presD
					//		5	=	tempD
					//		6	=	RhoD
					//		7	=	xE
					//		8	=	rE
					//		9	=	machE
					//		10	=	thetaE
					//		11	=	presE
					//		12	=	tempE
					//		13	=	rhoE
					//		14	=	gammaE
					//		15	=	mdot
					//		16  =	wE
					//	dS.dSi[0] = i;
	
	double ratio, machD,tempD,thetaD,rhoD,gammaD,presD,muD,wD,rD;
	double machE,tempE,thetaE,rhoE,gammaE,presE,muE,rE, xE;
	double mdotTotalE, dr, mdotErr[3], rGuess[3];
	dummyStruct dS,dPTR;
	int i,jj,k;
	//	Step through each point along the RRC to find where xD falls
	i = 0;
	while( xD > x[++i][jStart]); // xD is between point i and i-1
	//	Set the average properties of the point xD based on a liner interpolation
	//	of the points along the RRC that bound it. This is done by a distance weighted average
	ratio = (xD - x[i-1][jStart]) / (x[i][jStart] - x[i-1][jStart]);
	//	Find rD Assuming a straight line between the two points
	rD = r[i-1][jStart] + ratio*(r[i][jStart] - r[i-1][jStart]);

	// Here is the distance weighted ratio
	ratio = sqrt(pow(xD - x[i-1][jStart],2)+pow(rD - r[i-1][jStart],2))/sqrt(pow(x[i][jStart]-x[i-1][jStart],2)+
		pow(r[i][jStart]-r[i-1][jStart],2));
	//	Calculate the massflow from the axis to the point
	
	machD = mach[i-1][jStart] + ratio*(mach[i][jStart] - mach[i-1][jStart]);
	gammaD = gamma[i-1][jStart] + ratio*(gamma[i][jStart] - gamma[i-1][jStart]);
	

	
	if (rD == 0.0 && nType != PERFECT)
	{
		AfxMessageBox("The solution will not converge on Point E");
		exit(1);
	}
	thetaD = theta[i-1][jStart] + ratio*(theta[i][jStart] - theta[i-1][jStart]);
	//	Calc Isentropic properties
	dPTR = CalcIsentropicP_T_RHO(gammaD, machD);
	presD = dPTR.dSx[0];
	tempD = dPTR.dSx[1];
	rhoD  = dPTR.dSx[2];
	
	muD = CalcMu(machD);
	wD = machD * sqrt(gammaD*(GASCON/molWt)*tempD*GRAV);
	
	//	Set the properties at D 
	dS.dSx[0] = xD;
	dS.dSx[1] = rD;
	dS.dSx[2] = machD;
	dS.dSx[3] = thetaD;
	dS.dSx[4] = presD;
	dS.dSx[5] = tempD;
	dS.dSx[6] = rhoD;
	dS.dSi[0] = i;
	//	Set the respective DE properities
	xDE[jStart] = xD;
	rDE[jStart] = rD;
	mDE[jStart] = machD;
	thetaDE[jStart] = thetaD;
	pDE[jStart] = presD;
	tDE[jStart] = tempD;
	rhoDE[jStart] = rhoD;
	gDE[jStart] = gammaD;

	if ( mdotMatch <= 0.0)
	{
		dS.dSx[7] = xD;
		dS.dSx[8] = rD;
		dS.dSx[9] = machD;
		dS.dSx[10] = thetaD;
		dS.dSx[11] = presD;
		dS.dSx[12] = tempD;
		dS.dSx[13] = rhoD;
		dS.dSx[14] = gammaD;
		dS.dSx[16] = wD;
		dS.dSi[1] = jStart;
	}
	else // mass flow > 0.0
	{
		//  If the nozzle geometry is 2D, then the mass flow calculation is fairly straight forward.
		//	Along DE all of the state properties (Mach, P, T etc.) are constant.
		//	Therefore rE can be directly calculated.
		if (geom == TWOD)
		{
			// mdotMatch is in lbm/ft-s so multiply by 12 for get it in inches
			rE = rD + (mdotMatch*12*sin(thetaD+muD))/(rhoD*wD*sin(muD)*GRAV);
			xE = xD + (rE-rD)/lDyDx(thetaD,muD);
			// Set the properties at E equal to the properties at D
			dS.dSx[7] = xE;
			dS.dSx[8] = rE;
			dS.dSx[9] = machD;
			dS.dSx[10] = thetaD;
			dS.dSx[11] = presD;
			dS.dSx[12] = tempD;
			dS.dSx[13] = rhoD;
			dS.dSx[14] = gammaD;
			dS.dSx[16] = wD;

			//	From here, Create nRRCPlus points along DE, equally spaced in R. This will be used
			//	calculate other LRCs. Start with the point above D[j+1] and continue of to E
			if (pointFlag)
			{
				dr = (rE - rD) / nRRCPlus;
				thrust[i][jStart] = 0.0;
				Sthrust[i][jStart] = 0.0;
				for ( jj = 1; jj <= nRRCPlus; jj++)
				{
					thrust[i][jStart+jj] = -99.9;
					Sthrust[i][jStart+jj] = -99.9;
					r[i][jStart+jj]		= rD + jj*dr;
					x[i][jStart+jj]		= xD + (r[i][jStart+jj] - rD)/ tan(thetaD+muD);
					mach[i][jStart+jj]	= machD;
					theta[i][jStart+jj]	= thetaD;
					pres[i][jStart+jj]	= presD;
					temp[i][jStart+jj]	= tempD;
					rho[i][jStart+jj]	= rhoD;
					gamma[i][jStart+jj]	= gammaD;
					massflow[i][jStart+jj] = mdotMatch*(r[i][jStart+jj] - rD)/(rE-rD);
					//	Set the respective DE properities
					xDE[jStart+jj] = x[i][jStart+jj];
					rDE[jStart+jj] = r[i][jStart+jj];
					mDE[jStart+jj] = machD;
					thetaDE[jStart+jj] = thetaD;
					pDE[jStart+jj] = presD;
					tDE[jStart+jj] = tempD;
					rhoDE[jStart+jj] = rhoD;
					gDE[jStart+jj] = gammaD;
					massDE[jStart+jj] = massflow[i][jStart+jj];
				}
			}
				dS.dSi[1] = jStart + nRRCPlus;  // Last RRC number
		}	
		else if ( geom == AXI)
		{
			double u0;
			//	There are 2 cases to consider, a PERFECT Nozzle, one that is NOT and
			
			if ( nType == PERFECT)
			{
				//	For a perfect nozzle rD = 0.0, therefore L3 = 0.
				//	In the solution for massflow, v = 0 therefore rE can be solved
				//	for directly. xE can be solved for because ED is a characteristic,
				//	the slope for the characteristic is muD).
				u0 = machD * sqrt(gammaD*GASCON/molWt*GRAV*tempD);
				rE = sqrt(mdotMatch*144/(GRAV*rhoD*u0*PI));
				xE = xD + (rE - rD) * lDyDx(thetaD, CalcMu(machD));

				// Set the properties at E equal to the properties at D
				dS.dSx[7] = xE;
				dS.dSx[8] = rE;
				dS.dSx[9] = machD;
				dS.dSx[10] = thetaD;
				dS.dSx[11] = presD;
				dS.dSx[12] = tempD;
				dS.dSx[13] = rhoD;
				dS.dSx[14] = gammaD;
				dS.dSx[16] = wD;

				//	From here, Create nRRCPlus points along DE, equally spaced in R. This will be used
				//	calculate other LRCs. Start with the point above D[j+1] and continue of to E
				if (pointFlag)
				{
					dr = rE / nRRCPlus;
					thrust[i][jStart] = 0.0;
					Sthrust[i][jStart] = 0.0;
					for ( jj = 1; jj <= nRRCPlus; jj++)
					{	
						thrust[i][jStart+jj] = -99.9;
						Sthrust[i][jStart+jj] = -99.9;
						r[i][jStart+jj]		= rD + jj*dr;
						x[i][jStart+jj]		= xD + (r[i][jStart+jj] - rD)/ lDyDx(thetaD,muD);
						mach[i][jStart+jj]	= machD;
						theta[i][jStart+jj]	= thetaD;
						pres[i][jStart+jj]	= presD;
						temp[i][jStart+jj]	= tempD;
						rho[i][jStart+jj]	= rhoD;
						gamma[i][jStart+jj]	= gammaD;
						massflow[i][jStart+jj] = mdotMatch*(r[i][jStart+jj]*r[i][jStart+jj])/(rE*rE);
						//	Set the respective DE properities
						xDE[jStart+jj] = x[i][jStart+jj];
						rDE[jStart+jj] = r[i][jStart+jj];
						mDE[jStart+jj] = machD;
						thetaDE[jStart+jj] = thetaD;
						pDE[jStart+jj] = presD;
						tDE[jStart+jj] = tempD;
						rhoDE[jStart+jj] = rhoD;
						gDE[jStart+jj] = gammaD;
						massDE[jStart+jj] = massflow[i][jStart+jj];
					}
				}
				dS.dSi[1] = jStart + nRRCPlus;  // Last RRC number
			}	// End of PERFECT AXI
			else //	NOT PERFECT AXI CASE
			{
				double mach0,gamma0, mdotTotal0, r0, rhoUAvg, x0, vel0, mu0, des;
				double a0,v0,aE,uE,vE, wE, temp0, theta0, rho0,dxdr,da, mdot;
				double dMErr, dTErr, dxErr,drErr,dMdot,mdotLevel;
//				int kk;
				dummyStruct MXT; // 0: mach, 1:x, 2:theta
			
		
				//	Let point D by defined by [i][j]
				//	For a given new mach number [i][j+1] along DE. theta[i][j+1] can be computed from L2. 
				//	Then r[i][j+1] can be found from L3.
				//	x[r][j+1] can be found knowing that dr=tan(theta+mu)dx along the LRC.
				//	From here the other properties can be found, and a mass flow calculated.
				//	Keep this up until the mass flow = mdotMatch.
				//	Because this is part of another iteration, you should take care not to change 
				//	any values on the [j] line.
					
				mdotTotalE = 0;
				mdotTotal0 = 0;
				//	Set temporary properties 0 equal to the Point D.  These points will be set 
				//	to the previous values for each iteration
				temp0 = tempD;
				gamma0 = gammaD;
				a0 = sqrt(gamma0*GASCON/molWt*GRAV*temp0);
				mach0 = machD;
				theta0 = thetaD;
				rho0 = rhoD;
				vel0 = mach0*a0;
				u0 = vel0*cos(theta0);
				v0 = vel0*sin(theta0);
				mu0 = CalcMu(mach0);
				r0 = rD;
				x0 = xD;
				
				dMdot = mdotMatch/nRRCPlus;
				mdotLevel = dMdot;

				jj = 0;
				while ( mdotTotalE < mdotMatch)// && mach0 < 20.0)
				{
					//	For each point, use
					//	Use 4th order Runge Kutta to intergrate along the line DE
					//	The RK function will return a new Mach, X and Theta. From there
					//	the rest of the properties can be found.
			//		mdot = dMdot;
					des  = 0.05;
			//		kk = 0;
			//		while (mdot >= dMdot && kk++<50)
			//		{
				//		des *= 0.5;
						dMErr = 9.9;
						k = 0;
						while ((fabs(dMErr) > 1e-6 || fabs(dxErr) > 1e-6 || 
							fabs(dTErr) > 1e-6 || fabs(drErr) > 1e-12) && k++ < 50)
						{
							dr = des*sqrt(mach0)*sin(theta0+mu0);
							//MXT = RungeKutta(dr,r0,x0,mach0,theta0,gamma0);
							MXT = RungeKuttaFehlberg(dr,r0,x0,mach0,theta0,gamma0);
							machE = MXT.dSx[0];
							xE = MXT.dSx[1];
							thetaE = MXT.dSx[2];
							rE = MXT.dSx[3];
							dMErr = MXT.dSx[5];
							dxErr = MXT.dSx[6];
							dTErr = MXT.dSx[7];
							drErr = MXT.dSx[8];
							des *= .5;
						}
						if ( k == 51)
						{
							AfxMessageBox("Could not converge on DE integration criteria(Inner loop)");
							exit(1);
						}
						
						muE = CalcMu(machE);
						gammaE = gamma0;
						//	Calculate the rest of the parameters using isentropic relations
						dPTR = CalcIsentropicP_T_RHO(gammaE, machE);
						presE = dPTR.dSx[0];
						tempE = dPTR.dSx[1];
						rhoE  = dPTR.dSx[2];
						aE = sqrt(gammaE*GASCON/molWt*GRAV*tempE);
						wE = machE * aE;
						uE = wE*cos(thetaE);
						vE = wE*sin(thetaE);
						dxdr = (xE - x0)/dr;
						rhoUAvg = 0.5*GRAV*(rho0*u0 + rhoE*uE - dxdr*(rho0*v0 + rhoE*vE));
						da = PI*(rE*rE - r0*r0)/144.;
						mdot = rhoUAvg*da;
			/*		}
					if ( kk == 51)
					{
						AfxMessageBox("Could not converge on DE integration criteria(Outer loop)");
						exit(1);
					}
		*/
					
					mdotTotalE += mdot;
				 
					if ( mdotTotalE < mdotMatch)
					{
						jj++;
						if (pointFlag)
						{
							mach[i][jStart+jj] = machE;
							gamma[i][jStart+jj] = gammaE;
							massflow[i][jStart+jj] = mdotTotalE;
							theta[i][jStart+jj] = thetaE;
							x[i][jStart+jj] = xE;
							r[i][jStart+jj] = rE;
							CalcIsentropicP_T_RHO( i, jStart+jj, gammaE, machE);
							//	Set the respective DE properities
							xDE[jStart+jj] = xE;
							rDE[jStart+jj] = rE;
							mDE[jStart+jj] = machE;
							thetaDE[jStart+jj] = thetaE;
							gDE[jStart+jj] = gammaE;
							massDE[jStart+jj] = mdotTotalE;
							dPTR = CalcIsentropicP_T_RHO( gammaE, machE);
							pDE[jStart+jj] = dPTR.dSx[0];
							tDE[jStart+jj] = dPTR.dSx[1];
							rhoDE[jStart+jj] = dPTR.dSx[2];
						}
											
						//	Set up values for iterations
						gamma0 = gammaE;
						mach0 = machE;
						mdotTotal0 = mdotTotalE;
						r0 = rE;
						x0 = xE;
						theta0 = thetaE;
						rho0 = rhoE;
						u0 = uE;
						v0 = vE;
						mu0 = muE;
					}	// end of if
				}	// end of while loop
		
				//	At this point the mass flow at point E is greater than mdotMatch. Iterate in between
				//	points 0 and E to find the new point E that matches the required mass flow.
				//	Use the secant method to iterate on r
				double minR, maxR;
				minR = r0;
				maxR = rE;
				rGuess[0] = r0;
				rGuess[1] = rE;
				mdotErr[0] = (mdotTotal0 - mdotMatch)/mdotMatch;
				mdotErr[1] = (mdotTotalE - mdotMatch)/mdotMatch;
				mdotErr[2] = 9.9;
				k = 0;
				
				while ( fabs(mdotErr[2]) > 1e-8 && k++ < 50)
				{
					if ( mdotErr[0] != mdotErr[1])
						rGuess[2] = rGuess[1] - mdotErr[1] * (rGuess[1] - rGuess[0]) / (mdotErr[1] - mdotErr[0]);
					else k = 50;
					if (rGuess[2] < minR) rGuess[2] = minR;
					else if (rGuess[2] > maxR) rGuess[2] = maxR;
					
					dr = rGuess[2] - r0;
					MXT = RungeKutta(dr,r0,x0,mach0,theta0,gamma0);
					
					machE = MXT.dSx[0];
					xE = MXT.dSx[1];
					thetaE = MXT.dSx[2];
					rE = r0 + dr;
					gammaE = gamma0;
					muE = CalcMu(machE);
					//	Calculate the rest of the parameters using isentropic relations
					dPTR = CalcIsentropicP_T_RHO(gammaE, machE);
					presE = dPTR.dSx[0];
					tempE = dPTR.dSx[1];
					rhoE  = dPTR.dSx[2];
					aE = sqrt(gammaE*GASCON/molWt*GRAV*tempE);
					wE = machE * aE;
					
					dxdr = (xE - x0)/dr;
					rhoUAvg = 0.5*GRAV*(rho0*u0 + rhoE*uE - dxdr*(rho0*v0 + rhoE*vE));
					da = PI*(rE*rE - r0*r0)/144.;
					mdot = rhoUAvg*da;

					mdotTotalE = mdotTotal0 + mdot;
					mdotErr[2] = (mdotTotalE - mdotMatch)/mdotMatch;
				//	ofile << jStart << "\t" << k << "\t" << rGuess[2] << "\t" << mdotTotalE << "\t" << mdotErr[2] << endl;
					rGuess[0] = rGuess[1];
					rGuess[1] = rGuess[2];
					mdotErr[0] = mdotErr[1];
					mdotErr[1] = mdotErr[2];
				}
				if ( k == 51)
				{
					// This may not converge at very small mass flows. If that is the case, then check to see if the
					//	difference is small. If that is the case, then it is OK
					mdotErr[2] = mdotTotalE-mdotMatch;
			//		ofile << "Solution did not converge" << endl;
			//		ofile << jStart << "\t" << k << "\t" << rGuess[2] << "\t" << mdotTotalE << "\t" << mdotErr[2] << endl;
			//		if (fabs(mdotErr[2]) > 1e-13)
			//		{
						AfxMessageBox("Could not converge on massflow at point E");
						exit(1);
			//		}
				}
				
				//	Set the values of E into the returned structure
				dS.dSx[7] = xE;
				dS.dSx[8] = rE;
				dS.dSx[9] = machE;
				dS.dSx[10] = thetaE;
				dS.dSx[11] = presE;
				dS.dSx[12] = tempE;
				dS.dSx[13] = rhoE;
				dS.dSx[14] = gammaE;
				dS.dSx[16] = wE;
				//	Set the values of [i][j+jj] also equal to E;
				jj++;
				if (pointFlag)
				{
					mach[i][jStart+jj] = machE;
					gamma[i][jStart+jj] = gammaE;
					massflow[i][jStart+jj] = mdotTotalE;
					theta[i][jStart+jj] = thetaE;
					x[i][jStart+jj] = xE;
					r[i][jStart+jj] = rE;
					CalcIsentropicP_T_RHO( i, jStart+jj, gammaE, machE);
					//	Set the respective DE properities
					xDE[jStart+jj] = xE;
					rDE[jStart+jj] = rE;
					mDE[jStart+jj] = machE;
					thetaDE[jStart+jj] = thetaE;
					gDE[jStart+jj] = gammaE;
					massDE[jStart+jj] = mdotTotalE;
					dPTR = CalcIsentropicP_T_RHO( gammaE, machE);
					pDE[jStart+jj] = dPTR.dSx[0];
					tDE[jStart+jj] = dPTR.dSx[1];
					rhoDE[jStart+jj] = dPTR.dSx[2];
				}
				//	This is the return value for the lastRRC parameter
				dS.dSi[1] = jStart+jj;
				}
		}	// End of Axi case
	}	
	
	return dS;
}

//**************************************************************************************
//**************************************************************************************
//**************************************************************************************
void MOC_GridCalc::ResetGrid(int iStart, int iEnd, int jStart, int jEnd)
{
	// Reset Grid while leaving the initial data line intact
	if (iEnd > maxLRC) iEnd = maxLRC;
	if (jEnd > maxRRC) jEnd = maxRRC;

	for ( int i = iStart; i <= iEnd; i++)
	{
		for ( int j = jStart; j <= jEnd; j++)
		{
			mach[i][j] = 0.0;
			pres[i][j] = 0.0;
			temp[i][j] = 0.0;
			rho[i][j] = 0.0;
			gamma[i][j] = 0.0;
			x[i][j] = 0.0;
			r[i][j] = 0.0;
			theta[i][j] = 0.0;
			massflow[i][j] = 0.0;
			thrust[i][j] = 0.0;
			Sthrust[i][j] = 0.0;
		}
	}
	return;
}	

//**************************************************************************************
//**************************************************************************************
//**************************************************************************************
void MOC_GridCalc::CalcAxialMeshPoint(int j, int iEnd)
{
	//	Along the Axial point, r = 0 and theta = 0;  The point of influence is [i-1][j] 
	//	This point is on the same RRC as [iEnd][j]
	//	Set the initial values for point 2 and then set point 3 equal to those values
	int k;
	double s2,A2,b2,s3,A3,MErr, xErr, slope23, m3Old, x3Old;

	r[iEnd][j] = 0.0;
	theta[iEnd][j] = 0.0; // 0.0 is the right answer
	gamma[iEnd][j] = gamma[iEnd-1][j];

	s2 = rDyDx(theta[iEnd-1][j],CalcMu(mach[iEnd-1][j]));
	A2 = CalcA(mach[iEnd-1][j],gamma[iEnd-1][j]);
	b2 = Calcb(mach[iEnd-1][j], theta[iEnd-1][j], r[iEnd-1][j]);
	s3 = s2;
	A3 = A2;
	
	x3Old = 9.9;
	m3Old = 9.9;
	MErr = 9.9; 
	xErr = 9.9;
	k = 0;
	while ((fabs(MErr) > conCrit || fabs(xErr) > conCrit) && k++ < 500)
	{
		slope23 = TanAvg(s2,s3);
		x[iEnd][j] = x[iEnd-1][j] - r[iEnd-1][j]/slope23;
		mach[iEnd][j] = mach[iEnd-1][j] + 2*(theta[iEnd-1][j] + 
			b2*(x[iEnd][j] - x[iEnd-1][j]))/(A2+A3);
		
		s3 = rDyDx(theta[iEnd][j], CalcMu(mach[iEnd][j]));
		A3 = CalcA(mach[iEnd][j], gamma[iEnd][j]);

		MErr = (mach[iEnd][j] - m3Old)/ m3Old;
		xErr = (x[iEnd][j] - x3Old) / x3Old;

		x3Old = x[iEnd][j];
		m3Old = mach[iEnd][j];
	}
	if ( k == 501)
	{
		AfxMessageBox("Could not find an axial mesh point\nCalcAxialMeshPoint");
		exit(1);
	}

	//	Set the properties of this point using the isentropic relations.
	CalcIsentropicP_T_RHO( iEnd, j, gamma[iEnd][j], mach[iEnd][j]);
	massflow[iEnd][j] = 0.0;
	thrust[iEnd][j] = 0.0;
	Sthrust[iEnd][j] = 0.0;
	return;
}

//**************************************************************************************
//**************************************************************************************
//**************************************************************************************
void MOC_GridCalc::CalcContourWallPoint(int j, int iBottom)
{
	//	This function calculates the wall points beyond the downstream radius
	//	For the wall point [0][j], 2 points influence it, the wall point [0][j-1] and
	//	the point [1][j-1].  The properties at point [0][j] equal the properties at
	//	point [1][j-1]. The new flow angle at [0][j] is the average of flow angles of the 
	//	2 points of influence.
	// TODO: is this right
	double slope1, slope2, mu2;

	mach[0][j] = mach[1][j-1];
	pres[0][j] = pres[1][j-1];
	temp[0][j] = temp[1][j-1];
	gamma[0][j] = gamma[1][j-1];
	rho[0][j] = rho[1][j-1];
	theta[0][j] = theta[1][j-1];
	
	//	The location of the point is found by finding the intersection of the line 
	//	from [1][j-1] and a line from point[0][j-1] with the slope of theta[0][j]
	slope1 = tan(0.5 * (theta[0][j]+theta[0][j-1]));
	mu2 = asin(1/mach[1][j-1]);
	slope2 = tan(theta[1][j-1] + mu2);  // LRC

	x[0][j] = (r[0][j-1] - r[1][j-1] - x[0][j-1]*slope1 + x[1][j-1]*slope2)/(slope2-slope1);
	r[0][j] = slope1*(x[0][j] - x[0][j-1]) + r[0][j-1];

	//	Set the bottom side equal to the top side
	r[iBottom][j] = -r[0][j];
	x[iBottom][j] = x[0][j];
	mach[iBottom][j] = mach[0][j];
	pres[iBottom][j] = pres[0][j];
	temp[iBottom][j] = temp[0][j];
	rho[iBottom][j] = rho[0][j];
	gamma[iBottom][j] = gamma[0][j];
	theta[iBottom][j] = -theta[0][j];

	return;
}

//**************************************************************************************
//**************************************************************************************
//**************************************************************************************
void MOC_GridCalc::CalcSpecialWallPoint(int j, double rad, double alpha2)
{
	//	j: RRC increment
	//	rad: downstream radius of curvature
	//	alpha2: wall angle at the wall point to be found
	//	This function is called when the spacing between adjacent wall point is too high
	//	or the last wall point along the prescribed arc has to be found.  
	//Tharen Rice May 2020 changes.  Initialized s1rc to [5]; it was [4]
	double slrc[5];
	double A[5],B[5],R[5], srrc[4],x4, M4, T4, r4, AErr, BErr, KErr,RErr;
	double K3new, A3new, B3new, R3new, s4rrc, slope34, ratio, theta4;
	int k;
	
	//	In this function, the properties at point [0][j] have to be found. Now, set the 
	//	known properties at [0][j]
	theta[0][j] = alpha2;  //	Wall angle
	x[0][j] = rad*sin(alpha2); 
	r[0][j] = 1 + rad * (1 - cos(alpha2));
	gamma[0][j] = gamma[1][j-1];

	//	Set other constants that will be used in the iteration
	slrc[1] = lDyDx(theta[1][j-1], CalcMu(mach[1][j-1]));
	slrc[2] = lDyDx(theta[0][j-1], CalcMu(mach[0][j-1]));
	A[1] = CalcA(mach[1][j-1], gamma[1][j-1]);
	B[1] = CalcB(mach[1][j-1], theta[1][j-1], r[1][j-1]);
	R[1] = CalcR(mach[1][j-1], theta[1][j-1], r[1][j-1]);
	srrc[1] = rDyDx(theta[1][j-1],CalcMu(mach[1][j-1]));
	srrc[2] = rDyDx(theta[0][j-1],CalcMu(mach[0][j-1]));
	A[2] = CalcA(mach[0][j-1], gamma[0][j-1]);
	B[2] = CalcB(mach[0][j-1], theta[0][j-1], r[0][j-1]);
	R[2] = CalcR(mach[0][j-1], theta[0][j-1], r[0][j-1]);

	//	Set some initial values at point 3 and 4
	slrc[3] = slrc[1];
	slrc[4] = slrc[2];
	A[3] = A[1];
	B[3] = B[1];
	R[3] = R[1];	
	
	//	Unlike other calculations, the positions of [0][j] is known.  There exists a point 4
	//	between the previous wall point 2 and the point of influence 1 and must be found
	//	to determine the remaining properties at 4. Points 1,2 and 4 are all on the RRC [j-1].
	//	The slope of the RRC at 4 is the tangetn Average of slopes of 1 and 2.
	s4rrc = TanAvg(srrc[1], srrc[2]);

	AErr = 9.9;
	BErr = 9.9;
	RErr = 9.9;
	KErr = 9.9;
	k = 0;
	//	Continue the iteration until A3 B3, R3 and K3 remain unchanged
	while ((fabs(AErr) > conCrit || fabs(BErr) > conCrit || fabs(RErr) > conCrit || 
		fabs(KErr) > conCrit) && k++ < 50)
	{
		slope34 = TanAvg(slrc[3], slrc[4]);
		if ( fabs(slope34) < 10000) 
			x4 = (r[0][j] - r[0][j-1] + s4rrc*x[0][j-1] - slope34*x[0][j])/(s4rrc - slope34);
		else x4 = x[0][j];

		ratio = (x4 - x[1][j-1])/(x[0][j-1] - x[1][j-1]);
		A[4] = A[1] + ratio*(A[2] - A[1]);
		theta4 = theta[1][j-1] + ratio*(theta[0][j-1] - theta[1][j-1]);
		slrc[4] = slrc[1] + ratio*(slrc[2] - slrc[1]);
		M4 = mach[1][j-1] + ratio*(mach[0][j-1] - mach[1][j-1]);

		if (fabs(B[2]) <= fabs(R[2]))
		{
			B[4] = B[1] + ratio*(B[2] - B[1]);
			T4 = (x[0][j] - x4)*(B[3] + B[4]);
		}
		else
		{
			R[4] = R[1] + ratio*(R[2] - R[1]);
			r4 = r[1][j-1] + ratio*(r[0][j-1] - r[1][j-1]);
			T4 = (r[0][j] - r4)*(R[3]+R[4]);
		}

		mach[0][j] = M4 + (theta[0][j] - theta4 + 0.5*T4)/(0.5*(A[4]+A[3]));

		K3new = lDyDx(theta[0][j],CalcMu(mach[0][j]));
		A3new = CalcA(mach[0][j], gamma[0][j]);
		B3new = CalcB(mach[0][j], theta[0][j], r[0][j]);
		R3new = CalcR(mach[0][j], theta[0][j], r[0][j]);

		KErr = (K3new - slrc[3]) / slrc[3];
		AErr = (A3new - A[3]) / A[3];
		BErr = (B3new - B[3])/ B[3];
		RErr = (R3new - R[3]) / R[3];

		slrc[3] = K3new;
		A[3] = A3new;
		B[3] = B3new;
		R[3] = R3new;
	}

	//	Set the remaining properties
	// use the isentropic realtions to obtain the new properties at the point [i][j]
	//	Anderson MCF p.87
	CalcIsentropicP_T_RHO( 0, j, gamma[0][j], mach[0][j]);
	return;
}


//**************************************************************************************
//**************************************************************************************
//**************************************************************************************
int MOC_GridCalc::CalcInteriorMeshPoints( int j, int iStart, int iEnd, int flag, int geom)
{
	//	This calculate all of the interior points for the MOC grid. For all points above the 
	//	centerline. For every point 3 [i][j].
	//	Into point 3, there is a LRC from point 1 [i][j-1] and an RRC from point 2 [i-1][j]
 	//	The centerline is defined by i = iEnd. 
	//	The solution for Theta3 and Mach 3 are the taken from RAO eq. 15.  It is the average of
	//	point 3 and point 1 times the difference in these values.  The RAO manual is wrong in their
	//	equations (PTIN).  I have corrected them in my copy of the book.

	//	To solve for point 3, the method a described in the RAO book.
	//	For the initial guess, set lDyDx3 = lDyDx1 and rDyDx3 = rDyDx2.
	//	The slopes of the lines two point 3 is a tangent averaged slope of the individual points.
	//	from there r[i][j] and x[i][j] can be calculated.
	//	Then mach[i][j] and theta[i][j] can be calcuated.
	//	from mach[i][j], mu[i][j] can be calculated.
	//	Check to see if lDyDx3 (new) = lDyDx3 that you started with.
	//	and				rDyDx3 (new) = rDyDx3 that you started with.
	//	If they are not, then 
	double xErr, rErr, MErr, s3rrc,s3lrc,slope13,slope23 ,G3;
	double minMErr, minXErr, minRErr, minErrMach, minErrX, minErrR, minErrTheta;
	double A[4], B[4], b[4],R[4], RS[4], T[4], TH[4] ,M[4], s[3];
	double x3Old, r3Old, m3Old,rDif;
	int i,k,ii;


	for ( i = iStart; i < iEnd; i++) 
	{
		// Step through each interior point. Do not include the axis point. Also, if the 
		//	r[i-1][j] point is less than 0.001, than that is the last point
		minMErr = 9.9;
		xErr= 9.9;
		rErr= 9.9;
		MErr = 9.9;
		x3Old = 9.9;
		r3Old = 9.9;
		m3Old = 9.9;
		M[3] = 9.9;
		k = 0;
		if (flag) ii = i; // Special point added
		else ii = i+1;
		
		if (ii > iLast[j-1]) ii = iLast[j-1];
		//	Set some constants for Point 1. These are used in later calculations
		//	There are two sets of solutions that needs to be found.  If the point is not an axial point then
		//	the normal calculations are ok
		s[1] = lDyDx(theta[ii][j-1],CalcMu(mach[ii][j-1]));	// slope of characteristic at 1
		A[1] = CalcA(mach[ii][j-1],gamma[ii][j-1]);
		M[1] = mach[ii][j-1];
		TH[1] = theta[ii][j-1];
		if (r[ii][j-1] != 0.0) // Not an axial point
		{
			B[1] = CalcB(mach[ii][j-1], theta[ii][j-1], r[ii][j-1]);
			R[1] = CalcR(mach[ii][j-1], theta[ii][j-1], r[ii][j-1]);
		}
		else // axial point used
		{
			if (geom == TWOD)
			{
				R[1] = 0.0;
				B[1] = 0.0;
			}
			else
			{
				// Use the first point off of the centerline
				R[1] = CalcR(mach[ii-1][j-1], theta[ii-1][j-1], r[ii-1][j-1]);
				B[1] = CalcB(mach[ii-1][j-1], theta[ii-1][j-1], r[ii-1][j-1]);	
			}
		}
	
		//	Set some constants for Point 2 
		s[2] = rDyDx(theta[i-1][j],CalcMu(mach[i-1][j])); // slope of characteristic at 2
		A[2] = CalcA(mach[i-1][j], gamma[i-1][j]);
		M[2] = mach[i-1][j];
		TH[2] = theta[i-1][j];
		B[2] = CalcB(mach[i-1][j], theta[i-1][j], r[i-1][j]);
		b[2] = Calcb(mach[i-1][j], theta[i-1][j], r[i-1][j]);
		R[2] = CalcR(mach[i-1][j], theta[i-1][j], r[i-1][j]);
		RS[2] = CalcRStar(mach[i-1][j], theta[i-1][j], r[i-1][j]);
		
		//	Set point 3 values equal to some starting values
		//	The starting LRC slope should be less than the slope from point 1 to 2
		s3lrc = s[1];
		s3rrc = s[2]; // slope of rrc at 3
		b[3] = b[2];
		B[3] = B[1];
		R[3] = R[1];
		RS[3] = RS[2];
		A[3] = (A[1] + A[2]) / 2.;
		G3 = 0.5 * ( gamma[ii][j-1] + gamma[i-1][j]);
		TH[3] = TH[1];
		
		//	The iteration will continue until the initial guesses for the characteristic
		//	slope for the LRC and RRC are equal to the final values. In doing so, all of the 
		//	conditions at point 3 are correct. I have found that there are two possible solutions.
		//	The correct solution is when x[i][j] is > than x[ii][j-1] and x[i-1][j]. Another solution
		//	can arise where the x[i][j] is > x[ii][j-1] but less than x[i-1][j]. This is not good. I am currently
		//	working on how NOT 2 get the later solution.
		while ( (fabs(xErr) > conCrit || fabs(rErr) > conCrit || fabs(MErr) > conCrit) && k++<1000 && M[3] >= 1.0) 
		{
			slope13 = TanAvg(s[1], s3lrc);	// avg slope between 1 and 3
			slope23 = TanAvg(s[2], s3rrc);	// avg slope between 2 and 3

			if (slope13 > 10000.) x[i][j] = x[i-1][j];
			else if (slope23 > 10000.) x[i][j] = x[ii][j-1];
			else x[i][j] = (r[ii][j-1] - r[i-1][j] - slope13*x[ii][j-1] + slope23*x[i-1][j]) / 
					(slope23 - slope13);
			
						
			//	The next is put in so that smallest step away for point 1 or 2 is taken
			if ( fabs(s[2]) <= fabs(s[1]))	r[i][j] = r[i-1][j] + slope23*(x[i][j] - x[i-1][j]) ;
			else							r[i][j] = r[ii][j-1] + slope13*(x[i][j] - x[ii][j-1]);
	    
			if ( geom == TWOD)
			{
				T[1] = 0.0;
				T[2] = 0.0;
			}
			else if ( geom == AXI)
			{
				if( fabs(b[2]) <= fabs(RS[2])) T[2] = (x[i][j] - x[i-1][j])*(b[2] + b[3]);
				else T[2] = (r[i][j] - r[i-1][j])*(RS[3] + RS[2]);

				if( fabs(B[1]) <= fabs(R[1])) T[1] = (x[i][j] - x[ii][j-1])*(B[1] + B[3]);
				else T[1] = (r[i][j] - r[ii][j-1])*(R[3] + R[1]);
			}
			
			M[3] = (2*(TH[2]-TH[1]) + M[2]*(A[2]+A[3]) + M[1]*(A[1]+A[3]) + T[1] + T[2]) / 
				(A[1] + A[2] + 2*A[3]);

			A[3] = CalcA(M[3], G3);

			TH[3] = (TH[1] + TH[2]) / 2. + 0.25*(M[2]*(A[3]+A[2]) - M[1]*(A[1]+A[3]) - 
				M[3]*(A[2]-A[1]) + T[2] - T[1]);

			if ( TH[3] < 0.0) TH[3] = 0.0;//__max(TH[3], 1.e-5);

			//	Set starting slope equal to new slope
			s3lrc = lDyDx(TH[3], CalcMu(M[3]));
			s3rrc = rDyDx(TH[3], CalcMu(M[3]));
			B[3] = CalcB(M[3], TH[3], r[i][j]);
			b[3] = Calcb(M[3], TH[3], r[i][j]);
			R[3] = CalcR(M[3], TH[3], r[i][j]);
			RS[3] = CalcRStar(M[3], TH[3], r[i][j]);

			//	This calculates the error between guesses. 
			xErr = (x[i][j] - x3Old)/ x3Old;
			//	There exists times were r[i][j] is very small. This would make the 
			//	a small difference seem larger.  In these cases, base the error
			//	only on the difference
			rDif = r[i][j] - r3Old;
			if(fabs(rDif) < 1e-5 && fabs(r[i][j]) < 1e-4) rErr = 1e-3*rDif;
			else rErr = (r[i][j] - r3Old)/ r3Old;
			MErr = (M[3] - m3Old) / m3Old;
	
			if (fabs(MErr)< minMErr)
			{
				minMErr = fabs(MErr);
				minErrMach = M[3];
				minErrTheta = TH[3];
				minErrX = x[i][j];
				minErrR = r[i][j];
				minXErr = xErr;
				minRErr = rErr;
			}
		
			x3Old = x[i][j];
			r3Old = r[i][j];
			m3Old = M[3];
		}	// End of While loop
		
		//	The new point is in front of the old point
		mach[i][j] = M[3];
		theta[i][j] = TH[3];
		gamma[i][j] = G3;
		
		//	1001 Iterations is good, do not change
		if ( k == 1001 || M[3] < 1.0)
		{
			if ( minMErr <= 5e-4)
			{
				// If the solution does not converge to full criteria, but is close,
				//	use the point.
				x[i][j] = minErrX;
				r[i][j] = minErrR;
				mach[i][j] = minErrMach;
				theta[i][j] = minErrTheta;
		//		AfxMessageBox("Could not converge on an interior mesh point. Continuing");
			}
			else
			{
				AfxMessageBox("Could not converge on an interior mesh point.");
				return SEC_FAIL;
			}
		}
		
		if (r[i][j] < 0.0)  //&& iEnd-i <= 2
		{
			//	The calculated point is less than 0.0. For these cases, ignore this point 
			//	and solve for the axial point.
			iLast[j] = i;
			break;
		}

		//	If the new point is behind the old point (the characteristics cross, flag it
	/*	if (x[i][j] <= x[ii][j-1])
		{
			AfxMessageBox("Current RRC Point is behind Last RRC point.\nCalcInteriorMeshPoints");
		}*/
			
		if ( theta[i-1][j] != 0.0 ) theta[i][j] = __max(theta[i][j], 0.0);
//		else if ( theta[i][j] < 0.0) theta[i][j] = 0.0;
	
		//	Use the isentropic relations to calculate the properties of point [i][j]
		CalcIsentropicP_T_RHO( i, j, gamma[i][j], mach[i][j]);


	}	//	End of for i step

	return TRUE;
}

//**************************************************************************************
//**************************************************************************************
//**************************************************************************************
void MOC_GridCalc::InitializeDataMembers( void)
{
	//	This function initializes the arrays for the data members
	mach = new double*[maxLRC];
	pres = new double*[maxLRC];
	temp = new double*[maxLRC];
	rho = new double*[maxLRC];
	gamma = new double*[maxLRC];
	x = new double*[maxLRC];
	r = new double*[maxLRC];
	massflow = new double*[maxLRC];
	thrust = new double*[maxLRC];
	Sthrust = new double*[maxLRC];
	theta = new double*[maxLRC];
	iLast = new int[maxLRC];
	mDE = new double[maxLRC];
	pDE = new double[maxLRC];
	tDE = new double[maxLRC];
	rhoDE = new double[maxLRC];
	xDE = new double[maxLRC];
	rDE = new double[maxLRC];
	gDE = new double[maxLRC];
	thetaDE = new double[maxLRC];
	massDE = new double[maxLRC];
	
	//	For new, it will be assumed that nRRCPlus points will be used in the calculation. If this is exceeded, then
	//	the code will have to change
	for (int i = 0; i < maxLRC; i++)
	{
		mach[i] = new double[maxRRC];
		pres[i] = new double[maxRRC];
		temp[i] = new double[maxRRC];
		rho[i] = new double[maxRRC];
		x[i] = new double[maxRRC];
		r[i] = new double[maxRRC];
		theta[i] = new double[maxRRC];
		gamma[i] = new double[maxRRC];
		massflow[i] = new double[maxRRC];
		thrust[i] = new double[maxRRC];
		Sthrust[i] = new double[maxRRC];
	}

	return;
}


//**************************************************************************************
//**************************************************************************************
//**************************************************************************************
void MOC_GridCalc::DeleteDataMembers( void)
{
	//	This function initializes the arrays for the data members
	for (int i = 0; i < maxLRC; i++)
	{
		delete mach[i];
		delete pres[i];
		delete temp[i];
		delete rho[i];
		delete x[i];
		delete r[i];
		delete theta[i];
		delete gamma[i];
		delete massflow[i];
		delete thrust[i];
		delete Sthrust[i];
	}
	delete []mach; 
	delete []pres; 
	delete []temp;
	delete []rho; 
	delete []gamma; 
	delete []x; 
	delete []r; 
	delete []theta; 
	delete []massflow;
	delete []thrust;
	delete []Sthrust;
	delete []iLast;
	return;
}

//************************************************************************************
//************************************************************************************
//************************************************************************************
void MOC_GridCalc::CalcIsentropicP_T_RHO( int i, int j, double g, double m)
{
	// This function calculate the isentropic relations at a given point i,j
	dummyStruct dS;
	dS = CalcIsentropicP_T_RHO( g, m);
	gamma[i][j] = g;
	mach[i][j] = m;
	pres[i][j] = dS.dSx[0];	// psia
	temp[i][j] = dS.dSx[1];	//R
	rho[i][j] =  dS.dSx[2]; // slug/ft3
	return;
}

//************************************************************************************
//************************************************************************************
//************************************************************************************
dummyStruct MOC_GridCalc::CalcIsentropicP_T_RHO( double g, double m)
{
	// This function calculate the isentropic relations at a given point i,j
	dummyStruct dS;
	double ratio = 1 + (g-1)/2*m*m;
	dS.dSx[0] = pTotal/pow(ratio, g/(g-1));  // pressure
	dS.dSx[1] = tTotal/ratio;
	dS.dSx[2] = (dS.dSx[0]*144)/(GASCON/molWt*dS.dSx[1]*GRAV); // slug/ft3
	return dS;
}

//************************************************************************************
//************************************************************************************
//************************************************************************************
int MOC_GridCalc::CalcInitialThroatLine( double rUp, int n, double g, 
										   double pAmb, int geom, int tFlag, double mT)
{
	//	rUp: upstream throat radius
	//	nType: nozzle type
	//	n: number of points to be calculated nC
	//	g:	gamma
	//	pAmb: ambient pressure
	//	geom: TWOD or AXI nozzle
	//	tFlag: throatFlag
	//	mT: throat Mach number
	//	The initial throat Line can be found two ways. If the total conditions are given
	//	then the Sauer method is used to find the throat conditions.  If the throat
	//	conditions are input, then they are directly input into the variables.

	//	The calculation is different if the nozzle is 2D (nType = 0) or Axi (nType =1 )
	
	//	This functions calls CalcHallLine which actually does the work. that function
	//	returns the dummyStruct where dSx[0] = mach and dSx[1] = theta.  these number
	//	need to be put into mach[i][0] and theta[i][0].  The theta is then used to calculate
	//	the x coordinate for [i+1][0].  This is done until the axial station is reached.
	//	At that point the bottom nozzle is set to the top-side nozzle.  The iterations begin 
	//	at the top nozzle wall.

	int i;
	double R,drdx;
	x[0][0] = 0.0;
		
	for ( i = 0; i <= n; i++) // from the wall to the centerline
	{
		//	Based on the way it is done in RAO, the r[i][0] is based on a sinusoidal
		//	x is assumed to be on the RRC
		gamma[i][0] = g;
		r[i][0] = pow(sin(1.5707963*(n-i)/n),1.5);
		if ( i != 0) 
		{
			// This calculates a new X assumed it falls on the RRC
			drdx = rDyDx(theta[i-1][0],CalcMu(mach[i-1][0]));
			
		}
		
		//	Within SAUER the other properties are calculated
//		if (!tFlag) Sauer(i,geom,rUp); 
		mach[i][0] = 2.0;
		if (!tFlag)
		{
			if (i != 0)
			{
				while (mach[i][0] > 1.5)
				{
					if (i > 0) x[i][0] = x[i-1][0] + (r[i][0] - r[i-1][0])/drdx;
					if(KLThroat(i,geom,rUp) == SEC_FAIL) return SEC_FAIL;
					//	Increase drdx so the slope is half of what is was
					//	This will only make a difference if mach[i][0] > 1,5
					drdx *=2.0;
				}
			}
			//	Don't do anything for i = 0
			else if(KLThroat(i,geom,rUp) == SEC_FAIL) return SEC_FAIL;
			
		}
		else	
		{
			R = GASCON/molWt;
			theta[i][0] = 0.0;
			mach[i][0] = mT;
			if (i > 0) x[i][0] = x[i-1][0] + (r[i][0] - r[i-1][0])/drdx;
			if (mach[i][0] < 1.0)
			{
				AfxMessageBox("Calculated Throat Mach number < 1.0\nCalcInitialThroatLine");
				return SEC_FAIL;
			}
			CalcIsentropicP_T_RHO(i,0,gamma[i][0],mach[i][0]);
		}
		
	}	// End of for

	
	iLast[0] = n;
	//	Now that the initial data line has been calculated. Step from n to 0 and calculate the 
	//	mass flow and thrust.
	//	First, set the mass of [n][0] = 0.0;
	CalcMassFlowAndThrustAlongMesh(0,0, pAmb, geom);
	fstream ofile;
	ofile.open("TT'.out", ios::out);
	ofile << "i\tX\tR\tMACH\tTHETA\tmassflow" << endl;
	
	for ( i = 0; i <= n; i++) // from the wall to the centerline
		ofile << i << "\t" << x[i][0] << "\t" << r[i][0] << "\t" << mach[i][0] << "\t" << theta[i][0]*DEG_PER_RAD 
			<< "\t" << massflow[i][0] << endl;
	
	ofile.close();
	
	return SEC_OK;
	
}

//****************************************************************************************
//****************************************************************************************
//****************************************************************************************
dummyStruct MOC_GridCalc::CalcHallLine(double rUp, double x, double r, double g, int type)
{
	//	rUp = upstream radius of curvature
	//	x = axial distance
	//	r = radial location normalized by R*
	//	g = gamma
	//	type = nozzle type, 2D (0) or axi(1);
	
	double u1,u2,u3,v1,v2,v3,z,u,v,q,mach,theta,CD;
	
	dummyStruct dS;

	//	This is all of the modified HALL stuff
	z = x*sqrt((g+1)/((1+type)*rUp));
	u1 = 0.5*r*r - 0.25 + z;
	v1 = 0.25*r*r*r - 0.25*r + r*z;
	u2 = (2*g+9)/24*r*r*r*r - (4*g+15)/24*r*r + (10*g+57)/288 + z*(r*r - 5/8) - (2*g-3)/6*z*z;
	v2 = (g+3)/9*r*r*r*r*r - (20*g+63)/96*r*r*r + (28*g+93)/288*r + 
		z*((2*g+9)/6*r*r*r - (4*g+15)/12*r) + r*z*z;
	u3 = (556*g*g+1737*g+3069)/10368*r*r*r*r*r*r - (388*g*g+1161*g+1881)/2304*r*r*r*r +
		(304*g*g+831*g+1242)/1728*r*r - (2708*g*g+7839*g+14211)/82944 + 
		z*((52*g*g+51*g+327)/384*r*r*r*r - (52*g*g+75*g+279)/192*r*r +(92*g*g+180*g+639)/1152) + 
		z*z*(-(7*g-3)/8*r*r + (13*g-27)/48) + (4*g*g-57*g+27)/144*z*z*z;
	v3 = (6836*g*g+23031*g+30627)/82944*r*r*r*r*r*r*r - 
		(3380*g*g+11391*g+15291)/13824*r*r*r*r*r + (3424*g*g+11271*g+15228)/13824*r*r*r - 
		(7100*g*g+22311*g+30249)/82944*r + 
		z*((556*g*g+1737*g+3069)/1728*r*r*r*r*r - (388*g*g+1161*g+1181)/576*r*r*r + 
		(304*g*g+831*g+1242)/864*r) + 
		z*z*((52*g*g+51*g+327)/192*r*r*r - (52*g*g+75*g+279)/192*r) - z*z*z*((7*g-3)/12*r);

	double rp1 = rUp + 1;
	u = 1 + u1/rp1 + (u2+u1)/(rp1*rp1) + (u3+2*u2+u1)/(rp1*rp1*rp1);
	v = sqrt((g+1)/((1+type)*rp1)) * 
		(v1/rp1 + (v2 + 1.5*v1)/(rp1*rp1) + (v3+2.5*v2 + 15/8*v1)/(rp1*rp1*rp1));

	q = sqrt(u*u+v*v);

	mach = q*sqrt((g+1)/2 - (g-1)/2*q*q);
	theta = atan(v/u);
	if (fabs(theta) < 1e-6) theta = 0.0;
	
	CD = 1 - (g+1)/(rp1*rp1)*(1/96-(8*g-27)/(2304*rp1) + (754*g*g-757*g+3633)/(276480*rp1*rp1));

	dS.dSx[0] = mach;
	dS.dSx[1] = theta;
	dS.dSx[2] = CD;
	return dS;
}

//****************************************************************************************
//****************************************************************************************
//****************************************************************************************
double MOC_GridCalc::CalcMu(double mach)
{
	//	Mach angle
	return asin(1./mach);
}

//****************************************************************************************
//****************************************************************************************
//****************************************************************************************
double MOC_GridCalc::CalcA(double mach, double g)
{
	//	First term in the dtheta eq. Rao eq. 15
	return MM(mach)/(mach*(1.+(g-1.)/2.*mach*mach));
}

//****************************************************************************************
//****************************************************************************************
//****************************************************************************************
double MOC_GridCalc::CalcB(double mach, double theta, double r)
{
	//	Second term of dtheta Equation for LRC as a function of dz
	//theta = __max(theta, 1e-5);
	if ( r!= 0.0) return 1./(r*(MM(mach)/tan(theta)-1.));
	else return 0.0;
}

//****************************************************************************************
//****************************************************************************************
//****************************************************************************************
double MOC_GridCalc::Calcb(double mach, double theta, double r)
{
	//	Second term of dtheta Equation for RRC as a function of dz
	//theta = __max(theta, 1e-5);
	if ( r!= 0.0) return 1./(r*(MM(mach)/tan(theta)+1.));
	else return 0.0;
}

//****************************************************************************************
//****************************************************************************************
//****************************************************************************************
double MOC_GridCalc::CalcR(double mach, double theta, double r)
{
	//	Second term of dtheta Equation for LRC as a function of dz
	//theta = __max(theta, 1e-5);
	if ( r!= 0.0) return 1./(r*(MM(mach) + 1./tan(theta)));
	else return 0.0;
}

//****************************************************************************************
//****************************************************************************************
//****************************************************************************************
double MOC_GridCalc::CalcRStar(double mach, double theta, double r)
{
	//	Second term of dtheta Equation for LRC as a function of dz
	//theta = __max(theta, 1e-5);
	if ( r!= 0.0) return 1./(r*(MM(mach) - 1./tan(theta)));
	else return 0.0;
}

//****************************************************************************************
//****************************************************************************************
//****************************************************************************************
double MOC_GridCalc::lDyDx( double theta, double mu)
{
	//	characteristic angle for LRC
	return tan(theta+mu);
}

//****************************************************************************************
//****************************************************************************************
//****************************************************************************************
double MOC_GridCalc::rDyDx( double theta, double mu)
{
	//	characteristic angle for RRC
	return tan(theta-mu);
}

//****************************************************************************************
//****************************************************************************************
//****************************************************************************************
double MOC_GridCalc::TanAvg( double x, double y)
{
	//	Tanget averaging function.  x and y are tangents of two angles
	return tan(0.5 * (atan(x)+atan(y)));
}

//****************************************************************************************
//****************************************************************************************
//****************************************************************************************
double MOC_GridCalc::MM(double mach)
{
	return sqrt(mach*mach-1);
}

//****************************************************************************************
//****************************************************************************************
//****************************************************************************************
void MOC_GridCalc::Sauer(int i, int geom, double RS)
{
	//	This is taken from the TDK program. This is how they calculate the 
	//	Initial data Line
	//	Modified Sauer Transonic for Axisymmetric or Planar Flow. (GRN,1/95)
	double G1,RRA,Z,B1,B0,UP,VP,U,V,R,X,G,QB;

	double DELTA =1.0;
  
	R = r[i][0];
	X = x[i][0];
	G = gamma[i][0];
   
	G1 = G + 1.;            
	RRA = RS + DELTA*G/4.0 ;   
	Z = X* sqrt(RS/RRA);

	if ( geom == AXI)
	{
		B1 = sqrt(2./(G1*RRA));
        B0 = -1./(4.*RRA);
        UP = (G1*pow(B1*R,2))/4.+ B0 + B1*Z;
        VP = (G1*G1*pow(B1*R,3))/16. + G1*B1*B0*R/2. + G1*B1*B1*R*X/2.;
	}
	else if (geom == TWOD)
	{
		B1 = sqrt(1./(G1*RRA));
        B0 = -1./(6.*RRA);
        UP = (G1*pow(B1*R,2))/2.+ B0 + B1*Z;
        VP = (G1*G1*pow(B1*R,3))/6. + G1*B1*B0*R + G1*B1*B1*R*X;
	}
     
	//CALCULATE U,V,P/P*,THETA,Qbar,XMACH

	U = 1.0 + UP;
    V = VP;
    theta[i][0] = atan2(V,U);
	if (theta[i][0] < 0.0) theta[i][0] = 0.0;
    QB = sqrt(U*U+V*V);
    mach[i][0] = QB/sqrt ((G1-(G-1.)*QB*QB)/2.);
	CalcIsentropicP_T_RHO(i,0,gamma[i][0],mach[i][0]);

	//	Here is the Modif
    return;
}

//****************************************************************************************
//****************************************************************************************
//****************************************************************************************
int MOC_GridCalc::KLThroat(int i, int geom, double RS)
{
	//	This is taken from 'Transonic Flow in Small Throat Radius Curvature Nozzles',
	//	by Kliegel and Levine. In this they take the HALL method and modify the Axi
	//	calculation for a toroid coordinate system.  The 2D version is the one 
	//	developed by HALL.

	double u[4], v[4],G,y,z, RSP,U,V,Q;

  
	//	This is the radial distance of a point on the starting plane
	y = r[i][0];
	G = gamma[i][0];

	if ( geom == AXI)
	{
		// Uses the modified Hall Method in toroid coordinates
		z = x[i][0]*sqrt(2*RS/(G+1)); // Eq 12 in Hall paper
		RSP = RS + 1;
		u[1] = y*y/2 - 0.25 + z;
		v[1] = y*y*y/4 - y/4 + y*z;
		u[2] = (2*G + 9)*y*y*y*y/24 - (4*G + 15)*y*y/24 + (10*G + 57)/288 + z*(y*y - 5/8) - (2*G - 3)*z*z/6;
		v[2] = (G + 3)*y*y*y*y*y/9 - (20*G + 63)*y*y*y/96 + (28*G + 93)*y/288 + 
			z*((2*G + 9)*y*y*y/6 - (4*G + 15)*y/12) + y*z*z;
		u[3] = (556*G*G + 1737*G + 3069)*y*y*y*y*y*y/10368 - (388*G*G + 1161*G + 1881)*y*y*y*y/2304 + 
			(304*G*G + 831*G + 1242)*y*y/1728 - (2708*G*G + 7839*G + 14211)/82944 +
			z*((52*G*G + 51*G + 327)*y*y*y*y/34 - (52*G*G + 75*G + 279)*y*y/192 + (92*G*G + 180*G + 639)/1152) + 
			z*z*(-(7*G - 3)*y*y/8 + (13*G - 27)/48) + (4*G*G - 57*G + 27)*z*z*z/144;
		v[3] = (6836*G*G + 23031*G + 30627)*y*y*y*y*y*y*y/82944 - (3380*G*G + 11391*G + 15291)*y*y*y*y*y/13824 +
			(3424*G*G + 11271*G + 15228)*y*y*y/13824 - (7100*G*G + 22311*G + 30249)*y/82944 +
			z*((556*G*G + 1737*G + 3069)*y*y*y*y*y/1728 * (388*G*G + 1161*G + 1181)*y*y/576 +
			(304*G*G + 831*G + 1242)*y/864) + z*z*((52*G*G + 51*G + 327)*y*y*y/192 - (52*G*G + 75*G + 279)*y/192) -
			z*z*z*(7*G - 3)*y/12;


		U = 1 + u[1]/RSP + (u[1] + u[2])/(RSP*RSP) + (u[1] + 2*u[2] + u[3])/(RSP*RSP*RSP);
		V = sqrt((G+1)/(2*RSP))*(v[1]/RSP + (1.5*v[1] + v[2])/(RSP*RSP) + (15./8.*v[1] + 2.5*v[2] + v[3])/(RSP*RSP*RSP));
	}
	else if (geom == TWOD)
	{	
		//	Calculate the z to be used in the velocity equations Eq 12
			z = x[i][0]*sqrt(RS/(G+1));

		u[1] = 0.5*y*y - 1/6 + z;
		v[1] = y*y*y/6 - y/6 + y*z;
		u[2] = (y+6)*y*y*y*y/18 - (2*G+9)*y*y/18 + (G+30)/270 + z*(y*y-0.5) - (2*G-3)*z*z/6;
		v[2] = (22*G+75)*y*y*y*y*y/360 - (5*G+21)*y*y*y/54 + (34*G+195)*y/1080 + z/9*((2*G+12)*y*y*y - (2*G+9)*y) + y*z*z;
		u[3] = (362*G*G+1449*G+3177)*y*y*y*y*y*y/12960 - (194*G*G + 837*G + 1665)*y*y*y*y/2592 + 
			(854*G*G + 3687*G + 6759)*y*y/12960 - (782*G*G + 5523 + 2*G*2887)/272160 + 
			z*((26*G*G + 27*G + 237)*y*y*y*y/288 - (26*G*G + 51*G + 189)*y*y/144 +
			(134*G*G + 429*G + 1743)/4320) + z*z*(-5*G*y*y/4 + (7*G - 18)/36) + 
			z*z*z*(2*G*G - 33*G + 9)/72;
		v[3] = (6574*G*G + 26481*G + 40059)*y*y*y*y*y*y*y/181440 - (2254*G*G + 10113*G + 16479)*y*y*y*y*y/25920 + 
			(5026*G*G + 25551*G + 46377)*y*y*y/77760 - (7570*G*G + 45927*G + 98757)*y/544320 + 
			z*((362*G*G + 1449*G + 3177)*y*y*y*y*y/2160 * (194*G*G + 837*G + 1665)*y*y*y/648 + 
			(854*G*G + 3687*G + 6759)*y/6480) + z*z*((26*G*G + 27*G + 237)*y*y*y/144 - (26*G*G + 51*G + 189)/144) + 
			z*z*z*(-5*G*y/6);
		
		U = 1 + u[1]/RS + u[2]/RS/RS + u[3]/RS/RS/RS;
		V = sqrt((G+1)/RS)*(v[1]/RS + v[2]/RS/RS + v[3]/RS/RS/RS);
	}
	if (fabs(V) < 1e-5) V = 0.0;
	theta[i][0] = atan2(V,U);
	if (fabs(theta[i][0]) < 1e-5) theta[i][0] = 0.0;
	Q = sqrt(U*U+V*V);
	mach[i][0] = Q;
	CalcIsentropicP_T_RHO(i,0,gamma[i][0],mach[i][0]);
	
	if (mach[i][0] < 1.0)
	{
		AfxMessageBox("Intial Data Line is subsonic, try increasing Initial Line Angle");
		return SEC_FAIL;
	}

    return SEC_OK;
}

//****************************************************************************************
//****************************************************************************************
//****************************************************************************************
void MOC_GridCalc::CalcMassFlowAndThrustAlongMesh(int jStart, int jEnd, double pAmb,int geom)
{
	//	This function will calculate the mass flow along each RRC, starting
	//	between jStart and jEnd
	int i,j,nAxis;
	double a1,u1,v1,a2,u2,v2,da,dxdr,f_a,Sf_a,mdot_a, mdot_2D;
		ofstream mdotfile;
		
	for ( j = jStart; j <= jEnd; j++)
	{
/*		if (j==89)
	{
	
		mdotfile.open("Mass flow.out");
		mdotfile << "i\tx\tr\tu1\tu2\tv1\tv2\trho1\tdxdr\tmdot_a\tda\tmassflow" << endl;
	}*/
		nAxis = iLast[j];
		//	Start at the axis i = iLast[j] and go to the wall
		massflow[nAxis][j] = 0.0;
		thrust[nAxis][j] = 0.0;
		Sthrust[nAxis][j] = 0.0;
		mdot_2D = 0.0;
		
		for ( i = nAxis-1; i >= 0; i--)
		{
			//	Set the average properties of the point rP between i and i+1 
			a1 = sqrt(gamma[i][j]*GASCON/molWt*GRAV*temp[i][j]);
			u1 = mach[i][j]*cos(theta[i][j]) * a1;
			v1 = mach[i][j]*sin(theta[i][j]) * a1;

			a2 = sqrt(gamma[i+1][j]*GASCON/molWt*GRAV*temp[i+1][j]);
			u2 = mach[i+1][j]*cos(theta[i+1][j]) * a2;
			v2 = mach[i+1][j]*sin(theta[i+1][j]) * a2;

			dxdr = (x[i+1][j] - x[i][j])/(r[i][j] - r[i+1][j]);
			mdot_a = fabs(0.5*GRAV*(rho[i][j]*u1 + rho[i+1][j]*u2 + dxdr*(rho[i][j]*v1 + rho[i+1][j]*v2)));
			f_a = fabs((pres[i][j] - pAmb)*144 + 0.5*(rho[i][j]*u1*u1 + rho[i+1][j]*u2*u2 
				+ dxdr * (rho[i][j]*u1*v1 + rho[i+1][j]*u2*v2)));
			Sf_a = fabs(pres[i][j]*144 + 0.5*(rho[i][j]*u1*u1 + rho[i+1][j]*u2*u2 
				+ dxdr * (rho[i][j]*u1*v1 + rho[i+1][j]*u2*v2)));
			
			if ( geom == TWOD) da = fabs((r[i][j] - r[i+1][j])/12); // Mass flow/R* for 1 ft width
			else if ( geom == AXI) da = fabs(PI*(r[i][j]*r[i][j] - r[i+1][j]*r[i+1][j])/144.);
			massflow[i][j] = massflow[i+1][j] + mdot_a*da;
			thrust[i][j] = thrust[i+1][j] + f_a*da;
			Sthrust[i][j] = Sthrust[i+1][j] + Sf_a*da;
			mdot_2D += mdot_a*da;
	/*		if (j == 89)
				mdotfile << i << "\t" << x[i][j] << "\t" << r[i][j] << "\t" 
				<< u1 << "\t" << u2 << "\t" << v1 << "\t" << v2 << "\t" 
				<< rho[i][j] << "\t" << dxdr << "\t" 				<< mdot_a << "\t"
				<< da << "\t" << massflow[i][j] << endl;*/
		}
	}
	return;
	//	For Axi case, this the total massflow
	//	For the TWOD case it is for the top half of the nozzle
}

//****************************************************************************
//****************************************************************************
//****************************************************************************
double MOC_GridCalc::CalcPMFunction( double m, double g)
{
	//	This is the Prandtl-Meyer Function as found in Anderson (Fund. of Aero., p368).
	//m = mach;
	// g = gamma
	double nu;
	nu = sqrt((g+1)/(g-1)) * atan(sqrt((g-1)/(g+1) * (m*m - 1))) - atan(sqrt(m*m - 1));
	return nu;
}

//****************************************************************************
//****************************************************************************
//****************************************************************************
void MOC_GridCalc::CalcBDERegion(int iD, int jD, int jEnd, int geom)
{
	//	This function will compute the remaining MOC grid from the LRC DE back to the 
	//	i = 0. It uses the same as function 
	//	CalcInteriorMeshPoints, except that point 2 is downstream of point 3
	//	This calculate all of the interior points for the MOC grid. For all points above the 
	//	centerline. For every point 3 [i][j].
	//	Into point 3, there is a LRC from point 1 [i][j-1] and an RRC from point 2 [i+1][j]
	//	The centerline is defined by i = iEnd. 
	//	The solution for Theta3 and Mach 3 are the taken from RAO eq. 15.  It is the average of
	//	point 3 and point 1 times the difference in these values.  The RAO manual is wrong in their
	//	equations (PTIN).  I have corrected them in my copy of the book.

	//	To solve for point 3, the method a described in the RAO book.
	//	For the initial guess, set lDyDx3 = lDyDx1 and rDyDx3 = rDyDx2.
	//	The slopes of the lines two point 3 is a tangent averaged slope of the individual points.
	//	from there r[i][j] and x[i][j] can be calculated.
	//	Then mach[i][j] and theta[i][j] can be calcuated.
	//	from mach[i][j], mu[i][j] can be calculated.
	//	Check to see if lDyDx3 (new) = lDyDx3 that you started with.
	//	and				rDyDx3 (new) = rDyDx3 that you started with.
	//	If they are not, then 
	double xErr, rErr, MErr, TErr, s3rrc,s3lrc,slope13,slope23 ,G3;
	double A[4], B[4], b[4],R[4], RS[4], T[4], TH[4] ,M[4], s[3];
	double x3Old, r3Old, m3Old, theta3Old;
	int i,k,j;
	
	
	for ( j = jD+1; j <= jEnd; j++)
	{
		//	This is the calculation from DE back to the wall i = 0;
		// Step through each interior point. Do not include the axis point
		for ( i = iD-1; i >=  0; i--) 
		{
			xErr= 9.9;
			rErr= 9.9;
			MErr = 9.9;
			TErr = 9.9;
			x3Old = 9.9;
			r3Old = 9.9;
			m3Old = 9.9;
			theta3Old = 9.9;
			k = 0;
					
			//	Set some constants for Point 1. These are used in later calculations
			s[1] = lDyDx(theta[i][j-1],CalcMu(mach[i][j-1]));	// slope of characteristic at 1
			A[1] = CalcA(mach[i][j-1],gamma[i][j-1]);
			M[1] = mach[i][j-1];
			TH[1] = theta[i][j-1];
			B[1] = CalcB(mach[i][j-1], theta[i][j-1], r[i][j-1]);
			R[1] = CalcR(mach[i][j-1], theta[i][j-1], r[i][j-1]);
			RS[1] = CalcRStar(mach[i][j-1], theta[i][j-1], r[i][j-1]);
		
			//	Set some constants for Point 2 
			s[2] = rDyDx(theta[i+1][j],CalcMu(mach[i+1][j])); // slope of characteristic at 2
			A[2] = CalcA(mach[i+1][j], gamma[i+1][j]);
			M[2] = mach[i+1][j];
			TH[2] = theta[i+1][j];
			B[2] = CalcB(mach[i+1][j], theta[i+1][j], r[i+1][j]);
			b[2] = Calcb(mach[i+1][j], theta[i+1][j], r[i+1][j]);
			R[2] = CalcR(mach[i+1][j], theta[i+1][j], r[i+1][j]);
			RS[2] = CalcRStar(mach[i+1][j], theta[i+1][j], r[i+1][j]);
			
			//	Set point 3 values equal to some starting values
			s3lrc = s[1]; // slope of lrc at 3
			s3rrc = s[2]; // slope of rrc at 3
			b[3] = b[2];
			B[3] = B[1];
			R[3] = R[1];
			RS[3] = RS[1];
			A[3] = (A[1] + A[2]) / 2.;
			G3 = 0.5 * ( gamma[i][j-1] + gamma[i+1][j]);

			//	The iteration will continue until the initial guesses for the characteristic
			//	slope for the LRC and RRC are equal to the final values. I doning so, all of the 
			//	conditions at point 3 are correct.
			while ( (fabs(xErr) > conCrit || fabs(rErr) > conCrit || fabs(MErr) > conCrit ||
				fabs(TErr) > conCrit) && k++<50)
			{
				slope13 = TanAvg(s[1], s3lrc);	// avg slope between 1 and 3
				slope23 = TanAvg(s[2], s3rrc);	// avg slope between 2 and 3

				x[i][j] = (r[i][j-1] - r[i+1][j] - slope13*x[i][j-1] + slope23*x[i+1][j]) / 
					(slope23 - slope13);
							
				//	The next is put in so that smallest step away for point 1 or 2 is taken
				if ( fabs(s[2]) <= fabs(s[1]))	r[i][j] = r[i+1][j] + slope23*(x[i][j] - x[i+1][j]) ;
				else							r[i][j] = r[i][j-1] + slope13*(x[i][j] - x[i][j-1]);
			
				if ( geom == TWOD)
				{
					T[1] = 0.0;
					T[2] = 0.0;
				}
				else if ( geom == AXI)
				{
					if( fabs(b[2]) <= fabs(RS[2])) T[2] = (x[i][j] - x[i+1][j])*(b[2] + b[3]);
					else T[2] = (r[i][j] - r[i+1][j])*(RS[3] + RS[2]);

					if( fabs(B[1]) <= fabs(R[1])) T[1] = (x[i][j] - x[i][j-1])*(B[1] + B[3]);
					else T[1] = (r[i][j] - r[i][j-1])*(R[3] + R[1]);
				}
				
				M[3] = (2*(TH[2]-TH[1]) + M[2]*(A[2]+A[3]) + M[1]*(A[1]+A[3]) + T[1] + T[2]) / 
					(A[1] + A[2] + 2*A[3]);

				A[3] = CalcA(M[3], G3);

				TH[3] = (TH[1] + TH[2]) / 2. + 0.25*(M[2]*(A[3]+A[2]) - M[1]*(A[1]+A[3]) - 
					M[3]*(A[2]-A[1]) + T[2] - T[1]);

				//	TODO: 1e-5
				if ( TH[3] != 0.0) TH[3] = __max(TH[3], 0.0);

				//	Set starting slope equal to new slope
				s3lrc = lDyDx(TH[3], CalcMu(M[3]));
				s3rrc = rDyDx(TH[3], CalcMu(M[3]));
				B[3] = CalcB(M[3], TH[3], r[i][j]);
				b[3] = Calcb(M[3], TH[3], r[i][j]);
				R[3] = CalcR(M[3], TH[3], r[i][j]);
				RS[3] = CalcRStar(M[3], TH[3], r[i][j]);

				xErr = (x[i][j] - x3Old)/ x3Old;
				rErr = (r[i][j] - r3Old)/ r3Old;
				MErr = (M[3] - m3Old) / m3Old;
				TErr = (TH[3] - theta3Old) / theta3Old;

				x3Old = x[i][j];
				r3Old = r[i][j];
				m3Old = M[3];
				theta3Old = TH[3];

			}	// End of While loop
			if ( k == 51)
			{
				AfxMessageBox("Could Not converge on Back Calculated LRC");
				exit(1);
			}
			
			mach[i][j] = M[3];
			theta[i][j] = TH[3];
			gamma[i][j] = G3;

			//	Use the isentropic relations to calculate the properties of point [i][j]
			CalcIsentropicP_T_RHO(i, j, gamma[i][j], mach[i][j]);
			
		}	//	End of for i step
		
	}	//	End of J loop

	return;
}

//****************************************************************************
//****************************************************************************
//****************************************************************************
dummyStruct MOC_GridCalc::RungeKutta(double h, double r0, double x0, double mach0, 
									 double theta0, double gamma0)
{
	//	This function uses the Runge-Kutta method for integration of
	//	dM/dr, dx/dr and dtheta/dr for the DE integration
	//	h: is the step size for the integration, dr
	//	dpdr[0] = dM/dr = f(r,M,theta)
	//	dpdr[1] = dx/dr = f(r,theta)
	//	dpdr[2] = dtheta/dr = f(r,M,theta)
	dummyStruct MXT; // r, Mach, Theta
	double k1[4], k2[4], k3[4], k4[4], p[4], ip[4];
	int i;

	//	p is the array of independent variables
	ip[0] = mach0;
	ip[1] = x0;
	ip[2] = theta0;
	ip[3] = r0;

	//	Note p[1], or x is not needed in the Derivative calculation, so it is not passed
	//	i while loop through all of the derivatives
	// k1 calculation
	for ( i = 0; i <= 3; i++) k1[i] = h*Deriv(i, r0, ip[0], ip[2], gamma0);
	// k2 calculation
	r0 += h/2; 
	for ( i = 0; i <= 3; i++) p[i] = ip[i] + k1[i]/2;
	for ( i = 0; i <= 3; i++) k2[i] = h*Deriv(i, r0, p[0], p[2], gamma0);
	// k3 calculation
	for ( i = 0; i <= 3; i++) p[i] = ip[i] + k2[i]/2;
	for ( i = 0; i <= 3; i++) k3[i] = h*Deriv(i, r0, p[0], p[2], gamma0);
	// k4 Calculation
	r0 += h/2;
	for ( i = 0; i <= 3; i++) p[i] = ip[i] + k3[i];
	for ( i = 0; i <= 3; i++)
	{
		k4[i] = h*Deriv(i, r0, p[0], p[2], gamma0);
		MXT.dSx[i] = ip[i] + (k1[i] + 2*k2[i] + 2*k3[i] + k4[i])/6.;
	}
	return MXT;
}

//****************************************************************************
//****************************************************************************
//****************************************************************************
dummyStruct MOC_GridCalc::RungeKuttaFehlberg(double h, double r0, double x0, double mach0, 
									 double theta0, double gamma0)
{
	//	This function uses the Runge-Kutta method for integration of
	//	dM/dr, dx/dr and dtheta/dr for the DE integration
	//	h: is the step size for the integration, dr
	//	dpdr[0] = dM/dr = f(r,M,theta)
	//	dpdr[1] = dx/dr = f(r,theta)
	//	dpdr[2] = dtheta/dr = f(r,M,theta)
	dummyStruct MXT; // r, Mach, Theta
	double k1[4], k2[4], k3[4], k4[4], k5[4], k6[4], p[4], ip[4],rG;
	int i;

	//	p is the array of independent variables
	ip[0] = mach0;
	ip[1] = x0;
	ip[2] = theta0;
	ip[3] = r0;
	
	//	Note p[1], or x is not needed in the Derivative calculation, so it is not passed
	//	i while loop through all of the derivatives
	// k1 calculation
	for ( i = 0; i <= 3; i++) k1[i] = h*Deriv(i, r0, ip[0], ip[2], gamma0);
	// k2 calculation
	rG = r0 + h/4; 
	for ( i = 0; i <= 3; i++) p[i] = ip[i] + k1[i]/4;
	for ( i = 0; i <= 3; i++) k2[i] = h*Deriv(i, rG, p[0], p[2], gamma0);
	// k3 calculation
	rG = r0 + 3*h/8;
	for ( i = 0; i <= 3; i++) p[i] = ip[i] + 3*k1[i]/32 + 9*k2[i]/32;
	for ( i = 0; i <= 3; i++) k3[i] = h*Deriv(i, rG, p[0], p[2], gamma0);
	// k4 Calculation
	rG = r0 + 12*h/13;
	for ( i = 0; i <= 3; i++) p[i] = ip[i] + 1932*k1[i]/2197 - 7200*k2[i]/2197 + 7296*k3[i]/2197;
	for ( i = 0; i <= 3; i++) k4[i] = h*Deriv(i, rG, p[0], p[2], gamma0);
	// k5 Calculation
	rG = r0 + h;
	for ( i = 0; i <= 3; i++) p[i] = ip[i] + 439*k1[i]/216 - 8*k2[i] + 3680*k3[i]/513 - 845*k4[i]/4104;
	for ( i = 0; i <= 3; i++) k5[i] = h*Deriv(i, rG, p[0], p[2], gamma0);
	// k6 Calculation
	rG = r0 + h/2;
	for ( i = 0; i <= 3; i++) p[i] = ip[i] -8*k1[i]/27 + 2*k2[i] - 3544*k3[i]/2565 + 1859*k4[i]/4104 - 11*k5[i]/40;
	for ( i = 0; i <= 3; i++) k6[i] = h*Deriv(i, rG, p[0], p[2], gamma0);

	// Value and error calculation
	for ( i = 0; i <= 3; i++)
	{
		MXT.dSx[i] = ip[i] + 16*k1[i]/135 + 6656*k3[i]/12825 + 28561*k4[i]/56430 -9*k5[i]/50 + 2*k6[i]/55;
		MXT.dSx[i+5] = k1[i]/360 - 128*k2[i]/4275 - 2197*k4[i]/75240 + k5[i]/50 + 2*k6[i]/55;
	}
	return MXT;
}

//****************************************************************************
//****************************************************************************
//****************************************************************************
double MOC_GridCalc::Deriv(int i, double r0, double mach0, double theta0, double gamma0)
{
	//	i = 0; return dM/dr
	//	i = 1; return dx/dr
	//	i = 2; return dTheta/dr
	//  i = 3; return dr/dr = 1
	double fx, mu0,m32,tt0,D,a,b,c,tanMax,tanTheta;

	mu0 = CalcMu(mach0);
	if (theta0 < 5e-6)
	{
		if ( i == 2) fx = 1/tan(mu0);
		else fx = 0.0;
		return fx;
	}
	if ( i == 1) fx = 1/tan(theta0+mu0);
	else if ( i == 3) fx = 1;
	else 
	{
		tanTheta = tan(theta0);
		m32 = pow(mach0*mach0-1,1.5);
		tt0 = 1+(gamma0-1)/2*mach0*mach0;
		tanMax = m32/(((gamma0+1)*mach0*mach0/2.-1.)*mach0*mach0+1.);
		if( fabs(tanTheta - tanMax) < 1e-8)
		{
			AfxMessageBox("Could not converge of DE derivative. Code still proceeding");
		}
		a = r0*sin(theta0+mu0);
		b = 2*m32/tanTheta;
		c = (((gamma0+1)/2*mach0*mach0-2)*mach0*mach0 +2);
		D = a*( b - c);
		if ( i == 0) 
		{	//	dM/dr
			fx = -sin(theta0-mu0)*mach0*(mach0*mach0-1)*tt0/D;
		}
		else if ( i == 2)
		{
			// dTheta/dr
			fx = (sin(theta0)*((gamma0-1)/2.*mach0*mach0*mach0*mach0+1) - 
				m32*cos(theta0))/(mach0*D);
		}	
	}

	return fx;
}

//****************************************************************************
//****************************************************************************
//****************************************************************************
double MOC_GridCalc::CalcNozzleSurfaceArea(int jLast,int geom)
{
	//	This will calculate the nozzle surface area based on the wall contour
	//	from j = 0 to j = jLast. For a TWOD nozzle, 12 in nozzle width is assumed
	//	If its AXI, the throat radius is 1"
	double sa, length, rAvg;
	int j;

	sa = 0;

	for ( j = 1; j <= jLast; j++)
	{
		rAvg = 0.5*(r[0][j] + r[0][j-1]);
		length = sqrt(pow(r[0][j]-r[0][j-1],2) + pow(x[0][j]-x[0][j-1],2));
		if (geom == TWOD) sa += length*12;
		else sa += length*2*PI*rAvg;
	}
//	Simpson's Rule
/*	double saOld, sum, fx, saErr,h,xNew;
	int n,i;

	sa = 0;
	n = 50;
	saErr = 9.9;
	while (fabs(saErr) > 1e-6 && n <= 200000)
	{
		n *= 2;
		h = (x[0][jLast] - x[0][0])/n;
		sum = 0;
		for ( i = 1; i <= n/2; i++)
		{
			xNew = x[0][0] - h + 2*h*i;
			fx = FindSAFx(xNew, geom);
			sum += 4*fx;
			if ( i != n/2)
			{
				fx = FindSAFx(xNew+h, geom);
				sum += 2*fx;
			}
		}
		fx = FindSAFx(x[0][0], geom);
		sum += fx;
		fx = FindSAFx(x[0][jLast],geom);
		sum += fx;
		sa = sum*h/3;
		saErr = (sa - saOld)/saOld;
		saOld = sa;
	}*/

	return sa;
}

//****************************************************************************
//****************************************************************************
//****************************************************************************
double MOC_GridCalc::FindSAFx( double xNew, int geom)
{
	//	Find the R along the wall that corresponds to xNew
	int j;
	double rNew;
	j = 0;

	while (x[0][j] < xNew) j++;
	if ( j == 0) rNew = r[0][0];
	else rNew = r[0][j-1] + (r[0][j] - r[0][j-1])*(x[0][j] - xNew)/(x[0][j] - x[0][j-1]);
	
	if (geom == TWOD) rNew *= 12;
	else rNew *= 2*PI;

	return rNew;
}

//****************************************************************************
//****************************************************************************
//****************************************************************************
int MOC_GridCalc::CalcDE(int iD, int jD, int jEnd, int nType, int geom)
{
	//	This function will resolve the complete line DE for later use
	//	iD, jD is the point D on line BD
	//	jEnd is the last DE registered
	//	nType: PERFECT or something else
	//	geom: AXI or TWOD

	int jT, j,i, ii;
	double ratio;

	if ( nType == PERFECT)
	{
		// Do nothing
	}
	else
	{
		//	The line DE is computed from the line BD to the nozzle exit plane.
		//	A calculation has to be done where DE line is calculated from BD back 
		//	to the centerline. This calculation is the same as was done to 
		//	calculate the DE line in the first place. 
		if (geom == TWOD)
		{
			//	Only one point has to be found. That is the point at the centerline
			//	Because the nozzle is TWOD, along DE , Mach, and theta are constant
			
			rDE[jD-1] = 0.0;
			xDE[jD-1] = xDE[jD] - (rDE[jD]-rDE[jD-1])/lDyDx(thetaDE[jD],CalcMu(mDE[jD]));
			mDE[jD-1] = mDE[jD];
			tDE[jD-1] = tDE[jD];
			pDE[jD-1] = pDE[jD];
			thetaDE[jD-1] = thetaDE[jD];
			rhoDE[jD-1] = rhoDE[jD];
			gDE[jD-1] = gDE[jD];
			jD--;
		}
		else
		{
			//	Axisymmetric, not perfect nozzle
			//	Find out the ratio based on r of point D along J
			ratio = (rDE[jD] - r[iD+1][jD])/(r[iD-1][jD] - r[iD+1][jD]);
			ii = iD-1;
			for ( j = jD-1; j >= 0; j--)
			{
				i = 0;
				if (r[ii+1][j+1] == 0) break;
				while (x[i][j] < x[ii][j+1] && i < iLast[j]) i++; // I is beyond the current point
				if ( i == iLast[j]) break;
				xDE[j] = x[i][j] + ratio*(x[i-1][j] - x[i][j]);
				rDE[j] = r[i][j] + ratio*(r[i-1][j] - r[i][j]);
				mDE[j] = mach[i][j] + ratio*(mach[i-1][j] - mach[i][j]);
				tDE[j] = temp[i][j] + ratio*(temp[i-1][j] - temp[i][j]);
				pDE[j] = pres[i][j] + ratio*(pres[i-1][j] - pres[i][j]);
				rhoDE[j] = rho[i][j] + ratio*(rho[i-1][j] - rho[i][j]);
				gDE[j] = gamma[i][j] + ratio*(gamma[i-1][j] - gamma[i][j]);
				thetaDE[j] = theta[i][j] + ratio*(theta[i-1][j] - theta[i][j]);
				ii = i-1;
			}
			jD = j+1;
		}	
	}
		//	Translate the DE points so that they start with 0
		jT = jD;
		for ( j = jD; j <= jEnd; j++)
		{
			xDE[j-jT] = xDE[j];
			rDE[j-jT] = rDE[j];
			tDE[j-jT] = tDE[j];
			pDE[j-jT] = pDE[j];
			rhoDE[j-jT] = rhoDE[j];
			gDE[j-jT] = gDE[j];
			thetaDE[j-jT] = thetaDE[j];
			mDE[j-jT] = mDE[j];
			massDE[j-jT] = massDE[j];
		}
	return jEnd-jD;
}
//****************************************************************************
//****************************************************************************
//****************************************************************************
double MOC_GridCalc::SetThetaB(int pType, double err, double thetaB0)
{
	//	Depending on the parameterType, and the other given values
	//	set the new value for the thetaB1
	//	for a RAO nozzle, as THETAB increases 

	double thetaB1;

	if ( pType != ENDPOINT)
	{
		if ( err <= 0.0) 
		{
			thetaBMin = thetaB0;
			thetaB1 = 1.2 * thetaB0;
		}
		else 
		{
			thetaBMax = thetaB0;
			thetaB1 = 0.8 * thetaB0;
		}
	}
	else
	{
		if ( err >= 0.0) 
		{
			thetaBMin = thetaB0;
			thetaB1 = 1.2 * thetaB0;
		}
		else 
		{
			thetaBMax = thetaB0;
			thetaB1 = 0.8 * thetaB0;
		}
	}
	return thetaB1;
}

