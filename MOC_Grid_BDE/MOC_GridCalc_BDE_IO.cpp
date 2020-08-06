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
*	Program:			MOC_GridCalc_BDE_IO.cpp
*
*	Start Date:			3/8/02
*
*	Last Modified:		2/23/03
*
*	Author:				Tharen Rice   x3863
*
*	Sponsor:			FG888BXX (NASA GRC PDE Work)
*						FG1CAXXX (URETIS)
*
*	Comments:			This is the part of the class that outputs everything				
* 
*	Last Update:		2.23.03	Added a TDK rao.dat data file output							 
*						
*
****************************************************************************/

#include "MOC_GridCalc_BDE.h"
#include "math.h"
// Tharen Rice May 2020 Change: New call for fstream
//#include "fstream.h"
#include <fstream>
using namespace std;
// END of changes
#include "engineering_constants.hpp"
#include "Resource.h"
#include "MOC_GridDlg.h"
#include "MOCPlotDialog.h"

//****************************************************************************************
//****************************************************************************************
//****************************************************************************************
void MOC_GridCalc::OutputSummaryFile(void)
{
	int i,j,jj;
	double vel1,vel2;
	double mdotTotal, fTotal, da,velAvg,pAvg,rhoUAvg;

	// This will output the all of the summary information
	fstream ofile;
	ofile.open("summary.out", ios::out);
	ofile << "Summary output file for CONGO-2D" << endl
		<< "\nDefinition of terms:" << endl
		<< "Stream Thrust = Mdot*Vel + Pexit*Aexit" << endl
		<< "Gross Thrust = Mdot*Vel + Pexit*Aexit - Pamb*Aexit" << endl
		<< "Stream Thrust = Gross Thrust when Pamb = 0 (vacuum)" << endl
		<< "Cfg = Nozzle Thrust Coefficient (Gross Isp/Ideal Isp)" << endl
		<< "C* = Characteristic Exhaust Velocity" << endl
		<< "CD = Discharge coefficient: Massflow 2D/ Massflow 1D" << endl
		<< "1D = Assumes input are axial" << endl 
		<< "2D = Uses actual flow angles in calculation\n" << endl;
	ofile << "Input parameters" << endl << "R*  = Throat Radius (in)\n\n" << "Nozzle Type: ";
	if ( nozzleType == RAO) ofile << "Minimum Length Nozzle";
	else if ( nozzleType == CONE) ofile << "Cone, with half angle (deg) = " << designValue[1];
	else if ( nozzleType == PERFECT) ofile << "Perfect Nozzle";
	else if ( nozzleType == FIXEDEND) ofile << "Nozzle with a fixed end point, X/R* = "
		<< designValue[0] << "\tR/R* = " << designValue[1];
	ofile << "\n\nNozzle Geometry:\t";
	if ( nozzleGeom == TWOD) ofile << "Two dimensional Nozzle (assumes 1ft reference width)";
	else if ( nozzleGeom == AXI) ofile << "Axisymmetric Nozzle";
	ofile << "\nDesign Parameter:\t";
	if ( designParam == EXITMACH) ofile << "Exit Mach Number = " << designValue[0];
	else if ( designParam == EPS) ofile << "Nozzle Area Ratio = " << designValue[0];
	else if ( designParam == NOZZLELENGTH) ofile << "Nozzle Length = " << designValue[0];
	else if ( designParam == EXITPRESSURE) ofile << "PTotal/PExit = " << designValue[0];
	ofile << "\nThroat Geometry created by two arcs of given radii."
		<< "\nUpstream Radius/R*:\t" << RWTU << "\nDownstream Radius/R*:\t" << RWTD
		<< "\n\nThermodynamic Inputs"
		<< "\nTotal Pressure (psia):\t" << pTotal
		<< "\nTotal Temperature (R):\t" << tTotal
		<< "\nGamma:\t" << gamma_i
		<< "\nMolecular Weight:\t" << molWt
		<< "\nAmbient Pressure (psia):\t" << pAmbient
		<< "\n\nMOC Grid Parameters"
		<< "\nNumber of characteristic lines at the nozzle throat:\t" << nC
		<< "\nMaximum allowable angle between characteristics along wall (deg):\t" << DTLIMIT*DEG_PER_RAD
		<< "\nNumber of RRCs to calculate above KERNEL (50-100):\t" << nRRCAboveBD;
		
	ofile << "\n\nThe Initial Data Line is where the MOC calculation begins.\n"
		<< "Its based on the upstream radius of curvature and gamma.\n"
		<< "It is solved for using a method developed\n"
		<< "by Kliegel and Levine. This method is based on a method\n"
		<< "developed by Hall. Hall used cylindrical coordinates (R)\n"
		<< "This method uses toroid coordinates (R+1). This method\n"
		<< "does better for small upstream radii.\n\n"
		<< "Initial Data Line (Throat)\n\n"
		<< "I\tX/R*\tR/R*\tMach\tFlow Angle(deg)\tPres.(psia)\tTemp.(R)\tDen.(slug/ft3)\t"
		<< "Gamma\tMass Flow(lbm/s)"<< endl;
		
	for ( i = 0; i <= iLast[0]; i++)
	{
		ofile << i << "\t" << x[i][0] << "\t" << r[i][0] << "\t" << mach[i][0] 
			<< "\t" << theta[i][0]*DEG_PER_RAD << "\t" << pres[i][0] << "\t" << temp[i][0]
			<< "\t" << rho[i][0] << "\t" << gamma[i][0] << "\t" << massflow[i][0] << endl;
	}

	ofile << "\nInitial Data Line(Throat Plane) \nFor a throat radius(in) = 1.0. ";
	if ( nozzleGeom == TWOD) ofile << "Values are for half the nozzle with 12in reference width.";
	ofile << "\n2-D Mass Flow (lbm/s):\t" << massflow[0][0] 
		<< "\n2-D Gross Thrust (lbf):\t" << thrust[0][0]
		<< "\n2-D Isp (lbf-s/lbm):\t" << thrust[0][0]/massflow[0][0];

	double cStar, f1D, mdot1D, cStar1D, cF1D;
	dummyStruct PT;

	PT = CalcIsentropicP_T_RHO(gamma_i,1.0); // Throat Pressure
	f1D = (PT.dSx[0]-pAmbient)*(1+gamma_i); // lbf/in2
	mdot1D = GRAV*PT.dSx[0] * sqrt(gamma_i/(GASCON*GRAV/molWt*PT.dSx[1]));

	if ( nozzleGeom == TWOD) 
	{
		ofile << "\nFor a 2-D Nozzle, thrust and massflow are proportional to the throat radius."
			  << "\nTo find these values for a radius other than 1 inch, multiple the values by"
			  << "\nthe new radius. Make sure it also is in inches.";
		cStar = 12 * pTotal*GRAV/massflow[0][0];
		f1D *= 12.0;
		mdot1D *= 12.0;
		cStar1D = 12 * pTotal*GRAV/mdot1D;
	}
	else if ( nozzleGeom == AXI)
	{
		ofile << "\nFor an axisymmeric nozzle ,thrust and massflow are proportional to the throat"
			  << "\nradius squared (R*^2). To find these values for a radius other than 1 inch,"
			  << "\nmultiple the values by the square of the new radius. For example, for a radius"
			  << "\nof 3 inches, thrust and massflow would increase by a factor of 9.";
		cStar = pTotal*PI*GRAV/massflow[0][0];
		f1D *= PI;
		mdot1D *= PI;
		cStar1D = pTotal*PI*GRAV/mdot1D;
		cF1D = f1D / (pTotal*PI);
	}
		
    ofile << "\n2-D C* (ft/s):\t" << cStar << endl;
	
	ofile << "\nBased on the total conditions and assuming Mach = 1 at throat"
		<< "\n1-D Mass Flow (lbm/s):\t" << mdot1D
		<< "\n1-D Gross Thrust (lbf):\t" << f1D
		<< "\n1-D Gross Isp (lbf-s/lbm):\t" << f1D/mdot1D
		<< "\n1-D C* (ft/s):\t" << cStar1D
		<< "\nCD(2-D/1-D):\t" << massflow[0][0]/mdot1D;
	
	ofile << "\n\nInitial Expansion Angle ThetaB(deg):\t" << thetaBAns*DEG_PER_RAD 
		<< "\nMassflow error due to grid at end of Kernel (%):\t" << (1-mdotErrRatio)*100;
	if (nozzleType != CONE)
	{
		ofile << "\n\nLast LRC, DE that effects the nozzle wall contour." 
			<< "\nJ\tX/R*\tR/R*\tMach\tTheta(deg)\tP(psia)\tT(R)\tVelocity(ft/s)\tDensity(slug/ft3)\tGamma\t" 
			<< endl;
		for ( j = 0; j <= jDELast; j++)
			ofile << j << "\t" << xDE[j] << "\t" << rDE[j] << "\t" << mDE[j] << "\t" 
				<< thetaDE[j]*DEG_PER_RAD << "\t" << pDE[j] << "\t" << tDE[j] << "\t" 
				<< rhoDE[j] << "\t" << gDE[j] << endl;
	}
	ofile << "\n\nCalculated wall contour"
		<< "\nJ\tX/R*\tR/R*\tMach\tTheta(deg)\tP(psia)\tT(R)\tDensity(slug/ft3)\tGamma" 
		<< "\tMass Flow (lbm)\t%Dif in Mdot" << endl;
	for (j = 0; j<=lastRRC; j++)
		ofile << j << "\t" << x[0][j] << "\t" << r[0][j] << "\t" << mach[0][j] << "\t" << theta[0][j]*DEG_PER_RAD
			<< "\t" << pres[0][j] << "\t" << temp[0][j] << "\t" << rho[0][j] << "\t" 
			<< gamma[0][j] << "\t" << massflow[0][j] << "\t" 
			<< (massflow[0][j] - massflow[0][0])/massflow[0][0]*100 << endl;

	/********************************************************/

	ofile << "\n\nNozzle Summary"
		<< "\nThroat Radius R* (in):\t1.000"
		<< "\nNozzle Length/R*:\t" << x[0][lastRRC]
		<< "\nExpansion Ratio:\t";
	if (nozzleGeom == TWOD) ofile << r[0][lastRRC];
	else if ( nozzleGeom == AXI) ofile << r[0][lastRRC]*r[0][lastRRC];
	ofile << "\nExit Area (in2):\t";
	if (nozzleGeom == TWOD) ofile << 12*r[0][lastRRC];
	else if ( nozzleGeom == AXI) ofile << PI*r[0][lastRRC]*r[0][lastRRC];
	ofile << "\nSurface Area (in2):\t" << CalcNozzleSurfaceArea(lastRRC,nozzleGeom);

	//	Output the nozzle exit plane data
	ofile << "\n\nNozzle Exit Plane Data"
		<< "\nJ\tX/R*\tR/R*\tMach\tTheta(deg)\tP(psia)\tT(R)\tVelocity(ft/s)\tDensity(slug/ft3)\tGamma\t" 
		<< "Mass Flow (lbm/s)\tStream Thrust(lbf)" << endl;

	jj = 0;
	while (x[iLast[jj]][jj] < x[0][lastRRC] && jj <= lastRRC) jj++;  // Find the first RRC that does not end on
	//	the axis
	if (jj > lastRRC)
	{
		AfxMessageBox("There was an error finding the Nozzle Exit Plane");
		exit(1);
	}
	for (j = jj; j<=lastRRC; j++)
	{
		vel2 = mach[iLast[j]][j] * sqrt(gamma[iLast[j]][j]*GASCON/molWt*GRAV*temp[iLast[j]][j]);
		ofile << j << "\t" << x[iLast[j]][j] << "\t" << r[iLast[j]][j] << "\t" << mach[iLast[j]][j] << "\t" << theta[iLast[j]][j]*DEG_PER_RAD
			<< "\t" << pres[iLast[j]][j] << "\t" << temp[iLast[j]][j] << "\t" 
			<< vel2 << "\t" << rho[iLast[j]][j] << "\t" 
			<< gamma[iLast[j]][j] << "\t" ;
		//	Calculate the nozzle mass flow and stream thrust at the nozzle exit
		//	For the centerline j = jj, the values are 0.0
		
		if (j == jj)
		{
			mdotTotal = 0.0;
			fTotal = 0.0;
		}
		else
		{
			if (nozzleGeom == TWOD) da = 12*(r[iLast[j]][j]-r[iLast[j-1]][j-1]);
			else if ( nozzleGeom == AXI) da = PI*(r[iLast[j]][j]*r[iLast[j]][j]-r[iLast[j-1]][j-1]*r[iLast[j-1]][j-1])/144.; // ft2
			vel1 = mach[iLast[j-1]][j-1] * sqrt(gamma[iLast[j-1]][j-1]*GASCON/molWt*GRAV*temp[iLast[j-1]][j-1]);
			rhoUAvg = 0.5*(rho[iLast[j]][j]*vel2*cos(theta[iLast[j]][j]) + rho[iLast[j-1]][j-1]*vel1*cos(theta[iLast[j-1]][j-1])); //slug/ft2-s
			velAvg = (vel2*cos(theta[iLast[j]][j])+vel1*cos(theta[iLast[j-1]][j-1]))/2.;
			pAvg = (pres[iLast[j]][j] + pres[iLast[j-1]][j-1])/2.; // psi
			mdotTotal += rhoUAvg*da*GRAV; // lbm/s
			fTotal += (rhoUAvg*velAvg+pAvg*144)*da; //lbf
		}
		ofile << mdotTotal << "\t" << fTotal << endl;
	}

	ofile << "\nExit Mach at wall:\t" << mach[0][lastRRC] 
		<<	"\nExit Mach at centerline:\t" << mach[iLast[jj]][jj]
		<< "\nMassflow (lbm/s):\t" << mdotTotal
		<< "\nStream Thrust (lbf):\t" << fTotal
		<< "\nIsp (lbf-s/lbm):\t" << fTotal/mdotTotal
		<< "\nIdeal Isp (lbf-s/lbm:\t" << ispIdeal
		<< "\nCfg:\t" << fTotal/mdotTotal/ispIdeal
		<< "\n% diffence in initial and exit mass flow:\t" 
		<< (mdotTotal - massflow[0][0])/massflow[0][0]*100
		<< "\n\nAll of the MOC data can be found in the file MOC_Grid.out"
		<< "\nThe Streamlines can be found in SL.out"
		<< "\nThe centerline data can be found in center.out";
	OutputPrimaryChars(nozzleType);
	return;
}


//****************************************************************************************
//****************************************************************************************
//****************************************************************************************
void MOC_GridCalc::OutputCenterlineData(CString fileName )
{
	// This will output the Centerline data in a file. 
	fstream ofile;
	ofile.open(fileName,ios::out);
	ofile << "Centerline Data file" << endl;
	ofile << "J\tX/R*\tR/R*\tMach\tPres\tTemp\tRho\tTheta\tGamma\tMassFlow" << endl;
	int i,j;
	for (j = 0; j <= lastRRC; j++)
	{
		i = iLast[j];	
		if ( r[i][j] == 0.0)
			ofile << j << "\t" << x[i][j] << "\t" << r[i][j] << "\t" << mach[i][j] 
				<< "\t" << pres[i][j] << "\t" << temp[i][j] << "\t" << rho[i][j] << "\t" 
				<< theta[i][j]*DEG_PER_RAD << "\t"	<< gamma[i][j] << "\t" 
				<< massflow[i][j] << endl;
	}
	return;
}

//****************************************************************************************
//****************************************************************************************
//****************************************************************************************
void MOC_GridCalc::OutputMOC_Grid(void )
{
	// This will output the entire grid. 
	int i,j;
	fstream ofile;

	ofile.open("MOC_Grid.plt",ios::out);
	ofile << "VARIABLES = \"X/R\",\"R/R\",\"Mach\",\"Theta\",\"Massflow\",\"I\"," << endl
		<< "TITLE = \"RRC Contours\"" << endl
		<< "text  x=5  y=93  t=\"MOC Grid Data\"" << endl;
	
	//	Right running characteristic file
	for (j = 0; j <= lastRRC; j++)
	{
		ofile << "zone t=\"J = " << j << "\""  << endl;
		for ( i = 0; i <= iLast[j]; i++)
			ofile << x[i][j] << "\t" << r[i][j] << "\t" << 
				mach[i][j] << "\t" << theta[i][j]*DEG_PER_RAD << "\t" 
				<< massflow[i][j] << "\t" << i << endl;
	}
	ofile.close();
/*
	//	Output one TECPLOT output file
	ofile.open("Tecplot_out.plt",ios::out);
	ofile << "VARIABLES = \"X/R\",\"R/R\",\"Mach\",\"Pressure\","
		<< "\"Temperature\",\"Density\",\"Gamma\",\"Theta\""
		<< "\"Velocity\",\"RHO*V\",\"Massflow\"" << endl
		<< "TITLE = \"RRC Contours\"" << endl
		<< "text  x=5  y=93  t=\"MOC Grid Data\"" << endl;

	iMax = 0;
	for (j = 0; j <= lastRRC; j++) iMax = __max(iMax, iLast[j]);
	ofile << "zone t=\"All data\"" << " I = " << iMax +1 << " J =" << lastRRC
		<< " K = 1" << endl;
	//	Right running characteristic file
	for (j = 0; j < lastRRC; j++)
	{
		for ( i = 0; i <= iMax; i++)
		{
			ii = i;
			if ( i > iLast[j]) ii = iLast[j];
			vel = mach[ii][j] * sqrt(gamma[ii][j]*GASCON/molWt*GRAV*temp[ii][j]);
			ofile << x[ii][j] << "\t" << r[ii][j] << "\t" << 
				mach[ii][j] << "\t" << pres[ii][j] << "\t" << temp[ii][j] << "\t"
				<< rho[ii][j] << "\t" << gamma[ii][j] << "\t"
				<< theta[ii][j]*DEG_PER_RAD << "\t" << vel << "\t" << rho[ii][j]*vel
				<< "\t" << massflow[ii][j] <<  endl;
		}
	}
	ofile.close();
*/

	return;
}

//****************************************************************************
//****************************************************************************
//****************************************************************************
void MOC_GridCalc::OutputJ(int j, CString fileName)
{
	fstream outfile;
	outfile.open(fileName, ios::out);
	outfile << "i\tj\tx\tr\tmach\ttheta\tpress\ttemp\tMassflow\n";
	for (int i = 0; i <= iLast[j]; i++)
		outfile << i <<"\t" << j << "\t" << x[i][j] << "\t" << r[i][j] << "\t"
			<< mach[i][j] << "\t" << theta[i][j]*DEG_PER_RAD << "\t" << pres[i][j] << "\t"
			<< temp[i][j] << "\t" << massflow[i][j] << endl;
	
}

//****************************************************************************
//****************************************************************************
//****************************************************************************
void MOC_GridCalc::OutputInitialKernel(int jEnd)
{
	//	This function is called to output the initial wall and axis points based
	//	up to the initial thetaB guess
	int j;
	fstream wallFile,axisFile;
	wallFile.open("wall_i.out", ios::out);
	axisFile.open("axis_i.out", ios::out);
	

	wallFile << "i\tj\tx\tr\tmach\ttheta\tpress\ttemp\tmassflow\n";
	axisFile << "i\tj\tx\tr\tmach\ttheta\tpress\ttemp\tmassflow\n";
	
	for ( j = 0; j <= jEnd; j++)
	{
		wallFile << "0\t" << j << "\t" << x[0][j] << "\t" << r[0][j] << "\t"
			<< mach[0][j] << "\t" << theta[0][j]*DEG_PER_RAD << "\t" << pres[0][j] << "\t"
			<< temp[0][j] << "\t" << massflow[0][j] << endl;
		axisFile << iLast[j] <<"\t" << j << "\t" << x[iLast[j]][j] << "\t" << r[iLast[j]][j] << "\t"
			<< mach[iLast[j]][j] << "\t" << theta[iLast[j]][j]*DEG_PER_RAD << "\t" << pres[iLast[j]][j] << "\t"
			<< temp[iLast[j]][j] << "\t" << massflow[iLast[j]][j] << endl;
	}
}		

//****************************************************************************
//****************************************************************************
//****************************************************************************
void MOC_GridCalc::OutputPrimaryChars(int nType)
{
	//	This file contains the PLT file for the major lines in the grid
	//	These include, the wall, BD and DE
	int i,j;
	fstream ofile;
	ofile.open("Summary.plt",ios::out);
	ofile << "VARIABLES = \"X/R\",\"R/R\",\"Mach\",\"Pres\",\"Temp\",\"Rho\",\"Theta\", \"Gamma\"" << endl
		<< "TITLE = \"Primary MOC Contour Data file\"" << endl
		<< "text  x=5  y=92  t=\"MOC Grid Development Contours\"" << endl;
	
	//	Output put wall data
	ofile << "zone t=\"Wall contour\"" << endl;
	for (j = 0; j<=lastRRC; j++)
		ofile << x[0][j] << "\t" << r[0][j] << "\t" << mach[0][j] 
			<< "\t" << pres[0][j] << "\t" << temp[0][j] << "\t" << rho[0][j] << "\t" 
			<< theta[0][j]*DEG_PER_RAD << "\t" << gamma[0][j] << endl;

	if ( nType != CONE)
	{
		//	RRC BD
		ofile << "zone t=\"RRC BD\"" << endl;
		for ( i = 0; i <= iLast[jBD]; i++)
			ofile << x[i][jBD] << "\t" << r[i][jBD] << "\t" << mach[i][jBD] 
				<< "\t" << pres[i][jBD] << "\t" << temp[i][jBD] << "\t" << rho[i][jBD] << "\t" 
				<< theta[i][jBD]*DEG_PER_RAD << "\t" << gamma[i][jBD] << endl;

		//	DE data
		ofile << "zone t=\"LRC DE\"" << endl;
		for ( j = 0; j <= jDELast; j++)
			ofile << xDE[j] << "\t" << rDE[j] << "\t" << mDE[j] << "\t" 
				 << pDE[j] << "\t" << tDE[j] << "\t" << rhoDE[j] << "\t" 
				 << thetaDE[j]*DEG_PER_RAD << "\t" << gDE[j] << endl;
		ofile.close();
	}
}

//****************************************************************************************
//****************************************************************************************
//****************************************************************************************
void MOC_GridCalc::OutputStreamlines(int iEnd, int jEnd, int nni, int nnj, int geom)
{
	//	This functions calculate iEnd+1 streamlines from the nozzle throat to the nozzle
	//	exit plane.  For each streamline, x,r,mach,p,t,rho, and gamma are output
	//	For each STREAMLINE, define it by the massflow.
	//	The MOC Grid is defined by a J (axial) coordinate. the number of steps for each streamline
	//	will be defined by j/nnj approximately
	//	This function will also calculate for each streamline point where the 
	//	reflective wave end on the streamline
	//	The output will be in 3D where each streamline is calculate every 5 degrees around a circle
	int i,j,im,k, jStep, count;
	double ratio, mdot, T, A,D,R,G,P,F,M, X, Y,Z, mdotStep, mdotMatch;

	fstream ofile;
	ofile.open("MOC_SL.plt",ios::out);
	ofile << "VARIABLES = \"X/R\",\"Y/R\",\"Z/R\",\"R/R\",\"Mach\",\"Pres\",\"Temp\",\"Rho\",\"Theta\", \"Gamma\", \"Massflow\"" 
		<< "\"J\"" << endl
		<< "TITLE = \"Streamline Data file\"" << endl
		<< "text  x=5  y=93   t=\"Streamline Data\"" << endl;
	
	//	jStep is used to tell how many SLs to output in the axial direction
	jStep = int(jEnd/nnj);
	if (jStep ==0) jStep = 1;
	//	Warn the user if the axial streamline spacing is less than 2 
	if (x[0][jEnd]/nnj >= 2)
	{
		CString str;
		char buffer[10];
		int source = int(x[0][jEnd]/2.+1);
		_gcvt( source, 7, buffer );
		

		str = "The Streamlines will be output with a DX/R* > 2.\n";
		str += "To fix this, set the axial values = ";
		str += buffer;
		AfxMessageBox(str);
	}
	//	*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
	//	*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
	//	Output the centerline streamline
	//	There is no reflective wave along this streamline so set the values to -1;
	j = -jStep;
	count = 1;
	while(j < jEnd-jStep)
	{
		j += jStep;
		i = iLast[j];	
		if ( r[i][j] != 0.0)
		{
			while (r[iLast[j]][j] != 0.0) j--;
			break;
		}
		count++;
	}


	ofile << "zone t=\"MassFlow % = 0\" I = " << count << " J = 1 K = 1" <<  endl;
	j = -jStep;
	while(j < jEnd-jStep)
	{
		j += jStep;
		i = iLast[j];	
		if ( r[i][j] == 0.0)
			ofile << x[i][j] << "\t0.0\t0.0\t" << r[i][j] << "\t" 
				<< mach[i][j] << "\t" << pres[i][j] << "\t" << temp[i][j] << "\t" 
				<< rho[i][j] << "\t" << theta[i][j]*DEG_PER_RAD << "\t"	<< gamma[i][j] 
				<< "\t" << massflow[i][j] << "\t" << j << endl;
		else
		{
			//	The point r[i][j] is beyond the centerline
			//	Step from j back by 1 until r[i][j] = 0.0;
			while (r[iLast[j]][j] != 0.0) j--;
			i = iLast[j];
			ofile << x[i][j] << "\t0.0\t0.0\t" << r[i][j] << "\t" 
				<< mach[i][j] << "\t" << pres[i][j] << "\t" << temp[i][j] << "\t" 
				<< rho[i][j] << "\t" << theta[i][j]*DEG_PER_RAD << "\t"	<< gamma[i][j] 
				<< "\t" << massflow[i][j] << "\t" << j << endl;
			break;
		}
	}

	//	*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
	//	*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
	//	the input nni is used to tell how many SL to be output. It start and the 
	//	centerline and works to the wall. The streamlines will be split equally in massflow
	mdotMatch = 0.0;
	mdotStep = massflow[0][0]/nni;
	int throatStep = int(iLast[0]/nni);
	
	//	Step through each massflow along the initial data line. Each massflow will be a zone
	for ( i = 1; i < nni; i++)
	{
		// For each zone, this is the massflow to match
		//mdotMatch = massflow[iLast[0]-i*throatStep][0];
		mdotMatch += mdotStep;
		//  Since the number points along a streamline can vary, it first has to be determined so that
		//	it can be input into the TECPLOT datafile
		//	Set up an initial loop to determined the number of points to be output. jStep is the approximate
		//	amount of points in the axial direction
		j = -jStep;
		count = 0;
		while ( j < jEnd)
		{
			j += jStep;
			if (j > jEnd) j = jEnd;
			//	search along each j until the massflow is greater than massflow[i][0]
			//	Start at the nozzle exit plane and work backwards.  
			im = iLast[j];
			while ( massflow[im][j] <= mdotMatch && im > 0) im--;
			if ( im == iLast[j] && massflow[im][j] > mdotMatch)
			{	
				// The mass flow at the nozzle exit for this j is greater than the
				//	needed mass flow. 
				//	Step back in J until the mass flow at the exit is lower than the
				//	required massflow.
				while(massflow[iLast[j]][j] > mdotMatch) j--;
				j++;
				j = jEnd+1;
			}
			count ++;
		}

		ofile << "zone t=\"MassFlow % = " << mdotMatch/massflow[0][0]*100 << "\"" 
		<< " I = " << count << " J = 1 K = 37 " <<  endl;
		
		for ( k = 0; k <= 36; k++)
		{
			j = -jStep;
			while ( j < jEnd)
			{
				j += jStep;
				if (j > jEnd) j = jEnd;
				//	search along each j until the massflow is greater than massflow[i][0]
				//	Start at the nozzle exit plane and work backwards.  
				im = iLast[j];
				while ( massflow[im][j] <= mdotMatch && im > 0) im--;
				if ( im == iLast[j] && massflow[im][j] > mdotMatch)
				{	
					// The mass flow at the nozzle exit for this j is greater than the
					//	needed mass flow. 
					//	Step back in J until the mass flow at the exit is lower than the
					//	required massflow.
					while(massflow[iLast[j]][j] > mdotMatch) j--;
					j++;
					//	on the exit plane that is equal to this massflow.
					ratio = (mdotMatch - massflow[iLast[j-1]][j-1])/
						(massflow[iLast[j]][j] - massflow[iLast[j-1]][j-1]);
					mdot  = massflow[iLast[j-1]][j-1] + ratio*(massflow[iLast[j]][j] - massflow[iLast[j-1]][j-1]);
					M  = mach[iLast[j-1]][j-1] + ratio*(mach[iLast[j]][j] - mach[iLast[j-1]][j-1]);
					T  = temp[iLast[j-1]][j-1] + ratio*(temp[iLast[j]][j] - temp[iLast[j-1]][j-1]);
					P  = pres[iLast[j-1]][j-1] + ratio*(pres[iLast[j]][j] - pres[iLast[j-1]][j-1]);
					A  = theta[iLast[j-1]][j-1] + ratio*(theta[iLast[j]][j] - theta[iLast[j-1]][j-1]);
					D  = rho[iLast[j-1]][j-1] + ratio*(rho[iLast[j]][j] - rho[iLast[j-1]][j-1]);
					R  = r[iLast[j-1]][j-1] + ratio*(r[iLast[j]][j] - r[iLast[j-1]][j-1]);
					X  = x[iLast[j-1]][j-1] + ratio*(x[iLast[j]][j] - x[iLast[j-1]][j-1]);
					G  = gamma[iLast[j-1]][j-1] + ratio*(gamma[iLast[j]][j] - gamma[iLast[j-1]][j-1]);
					F  = thrust[iLast[j-1]][j-1] + ratio*(thrust[iLast[j]][j] - thrust[iLast[j-1]][j-1]);
					
					//step over the remaining js
					j = jEnd+1;
				}
				else if ( im == iLast[j] && massflow[im][j] < mdotMatch)
				{
					//	There could be cases near the wall exit, where the calculation in massflow is off
					//	The code would be to here when the calculated massflow at the wall for
					//	the j location is less than the desired massflow.  
					M  =  mach[im][j];
					T  = temp[im][j];
					P  = pres[im][j];
					A  = theta[im][j];
					D  = rho[im][j];
					R  = r[im][j];
					X  = x[im][j];
					G  = gamma[im][j];
					F  = thrust[im][j];
					mdot  = massflow[im][j];
					
				}
				else 
				{
					// normal operation
					if  ( massflow[im][j] < mdotMatch && im == 0) ratio = 0.0;
					else ratio = (mdotMatch - massflow[im+1][j]) / 
						(massflow[im][j] - massflow[im+1][j]);
					mdot  = massflow[im+1][j] + ratio*(massflow[im][j] - massflow[im+1][j]);
					M  =   mach[im+1][j] + ratio*(mach[im][j] - mach[im+1][j]);
					T  = temp[im+1][j] + ratio*(temp[im][j] - temp[im+1][j]);
					P  = pres[im+1][j] + ratio*(pres[im][j] - pres[im+1][j]);
					A  = theta[im+1][j] + ratio*(theta[im][j] - theta[im+1][j]);
					D  = rho[im+1][j] + ratio*(rho[im][j] - rho[im+1][j]);
					R  = r[im+1][j] + ratio*(r[im][j] - r[im+1][j]);
					X  = x[im+1][j] + ratio*(x[im][j] - x[im+1][j]);
					G  = gamma[im+1][j] + ratio*(gamma[im][j] - gamma[im+1][j]);
					F  = thrust[im+1][j] + ratio*(thrust[im][j] - thrust[im+1][j]);
				}
				if (geom == AXI)
				{
					Y  = R*sin(k*PI/18.);
					Z  = R*cos(k*PI/18.);
				}
				else
				{
					Y = R;
					Z = k/3.;
				}
				ofile << X  << "\t" << Y  << "\t" << Z  << "\t" << R  << "\t" << M  << "\t" 
					<< P  << "\t" << T  << "\t" << D  << "\t" 
					<< A *DEG_PER_RAD << "\t" << G  << "\t" << mdot  << "\t" << j << endl;
			}	// End of while j loop
		}	// End of k loop

		//	At this point the points along the SL are calculated. For each point
	}	//	End of while i loop

	//*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
	//*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
	//	Output the wall contour as the final streamline
	i = 0;
	count = 0;

	j = -jStep;
	while(j < jEnd)
	{
		j += jStep;
		if ( j > jEnd) j = jEnd;
		count ++;
	}
	ofile << "zone t=\"MassFlow % = 100\" I = " << count << " J = 1 K = 37" <<  endl;
	
	for ( k = 0; k <= 36; k++)
	{
		j = -jStep;
		while(j < jEnd)
		{
			j += jStep;
			if ( j > jEnd) j = jEnd;
			R = r[i][j];
			if (geom == AXI)
				{
					Y  = R*sin(k*PI/18.);
					Z  = R*cos(k*PI/18.);
				}
				else
				{
					Y = R;
					Z = k/3.;
				}
			ofile << x[i][j] << "\t" << Y << "\t" << Z << "\t" << r[i][j] << "\t" 
				<< mach[i][j] << "\t" << pres[i][j] << "\t" << temp[i][j] << "\t" 
				<< rho[i][j] << "\t" << theta[i][j]*DEG_PER_RAD << "\t"	<< gamma[i][j] 
				<< "\t" << massflow[i][j] << "\t" << j << endl;
		}
	}
	
	return;
}


void MOC_GridCalc::OutputTDKRAODataFile(int jStart, int jEnd)
{
	//	This function will output a file called rao.dat.  This file can be used by TDK99 as
	//	the inviscid nozzle contour file.  If you were used RAO, this file would be 
	//	called WALL_TBL.50.
	//	The file contains 3 columns of data R/R*, X/R* and Theta (degrees)
	//	The data starts at THETAB and continues to the nozzle exit

	ofstream ofile;
	ofile.open("rao.dat");
	int j;
	for (j = jStart; j <= jEnd; j++)
		ofile << r[0][j] << "\t" << x[0][j] << "\t" << theta[0][j]*DEG_PER_RAD << endl;

	ofile.close();
}

//****************************************************************************
//****************************************************************************
//****************************************************************************
void MOC_GridCalc::OutputFinalKernel(int iD, int jD, int jEnd)
{
	//	iD is the i of point D
	//	jD is the j of point D
	//	jEnd is the last RRC

	//	This function is called to output two regions (kernels)
	//	The first kernel is below the LastRRC, the second is above the last RRC
	int i,j;
	fstream kFile;
	kFile.open("BFE_Kernel.out", ios::out);
	kFile << "i\tj\tx\tr\tmach\ttheta\tpress\ttemp\n";
	for ( j = jD+1; j <= jEnd; j++)
	{
		for ( i = 0; i <= iLast[j];i++)
			kFile << i <<"\t" << j << "\t" << x[i][j] << "\t" << r[i][j] << "\t"
			<< mach[i][j] << "\t" << theta[i][j]*DEG_PER_RAD << "\t" << pres[i][j] << "\t"
			<< temp[i][j] << endl;
		kFile << endl;
	}
	kFile.close();

	kFile.open("TT'BF_Kernel.out", ios::out);
	kFile << "i\tj\tx\tr\tmach\ttheta\tpress\ttemp\n";
	
	for ( j = 0; j <= jD; j++)
	{
		for ( i = 0; i <= iLast[j];i++)
			kFile << i <<"\t" << j << "\t" << x[i][j] << "\t" << r[i][j] << "\t"
			<< mach[i][j] << "\t" << theta[i][j]*DEG_PER_RAD << "\t" << pres[i][j] << "\t"
			<< temp[i][j] << endl;
		kFile << endl;
	}
	kFile.close();


}

//****************************************************************************
//****************************************************************************
//****************************************************************************
void MOC_GridCalc::OutputUncroppedKernel(int jEnd)
{
	//	iD is the i of point D
	//	jD is the j of point D
	//	jEnd is the last RRC

	//	This function is called to output the initial wall and axis points based
	//	up to the initial thetaB guess
	int i,j;
	fstream kFile;
	kFile.open("UncroppedKernel.out", ios::out);

	kFile << "i\tj\tx,in\tr,in\tmach\ttheta\tpres,psi\ttemp,R\tDensity,slug/ft3\tmassflow\n";

	for ( j = 0; j <= jEnd; j++)
	{
		for ( i = 0; i <= iLast[j];i++)
			kFile << i <<"\t" << j << "\t" << x[i][j] << "\t" << r[i][j] << "\t"
			<< mach[i][j] << "\t" << theta[i][j]*DEG_PER_RAD << "\t" << pres[i][j] << "\t"
			<< temp[i][j] << "\t" << rho[i][j] << "\t" << massflow[i][j] << endl;
		kFile << endl;
	}

}		

		


