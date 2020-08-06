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
*	Program:			C3D_MOCGrid.cpp
*
*	Start Date:			5/14/02
*
*	Last Modified:		5/14/02
*
*	Author:				Tharen Rice   x3863
*
*	Sponsor:			FG800XXX (NASA GRC Nozzle Work)
*
*	Comments:			This is the 3D_MOC class. It uses the reference plane
*						method to do the solution.  The best write up I have 
*						found on this is
*						WC Armstrong, "A Method of Characteristics Computer
*						Program for Three-Dimensional Supersonic Internal 
*						Flows," AEDC-TR-78-68.			
*
*						Inputs: Nozzle geometry
*						Outputs: 3d nozzle data in TECPLOT format
* 
*	Last Update:		Initial Start	
*						10.10.02 Implemented an equally spaced initial plane
*							Changed _pt from 3 dimensions to 1	
*							Added _nPTS global to track total number of reference plane points			 
*						
*
****************************************************************************/
#include "stdafx.h"
#include <fstream>
#include "engineering_constants.hpp"
#include "3D_MOCGrid.hpp"
#include "UserMessages.h"
using namespace std;


double _a[3][4];
//	Global function used in Newton Raphson routine to solve the body points
void funcv(Vec_I_DP &x, Vec_O_DP &f)
{
	//	_a[i][j] is a global array of doubles
	//	f[0] & f[1] are the Compatibility equations
	//	f[2] is the flow tangency equation
        f[0] = _a[0][0]*x[0] + _a[0][1]*x[1] + _a[0][2]*x[2] - _a[0][3];
        f[1] = _a[1][0]*x[0] + _a[1][1]*x[1] + _a[1][2]*x[2] - _a[1][3];
		f[2] = _a[2][0]*cos(x[2]) + _a[2][1]*tan(x[1]) + _a[2][2]*sin(x[2]);
		for( int i = 0; i <=2; i++) if ( f[i] > -1e-10 && f[i] < 1e-10) f[i] = 0.0;
		return;
}

void funcvDeriv(Vec_I_DP &x, Mat_DP &f)
{
	//	Derivatives of the above function
	f[0][0] = _a[0][0]; //	df[0]/dx[0]
	f[0][1] = _a[0][1]; //	df[0]/dx[1]
	f[0][2] = _a[0][2]; //	df[0]/dx[2]
	f[1][0] = _a[1][0]; //	df[1]/dx[0]
	f[1][1] = _a[1][1]; //	df[1]/dx[1]
	f[1][2] = _a[1][2]; //	df[1]/dx[2]
	f[2][0] = 0.0;		//  df[2]/dx[0]
	f[2][1] = _a[2][1]/(cos(x[1])*cos(x[1]));  // df[2]/dx[1]
	f[2][2] = -_a[2][0]*sin(x[2]) + _a[2][2]*cos(x[2]);// df[2]/dx[2]
	for (int i = 0; i <=2; i++)
		for (int j = 0; j <=2; j++) if ( f[i][j] > -1e-10 && f[i][j] < 1e-10) f[i][j] = 0.0;
	return;

}

/****************************************************************************
*
*	Constructor and Destructors
*
****************************************************************************/

C3D_MOCGrid::C3D_MOCGrid(void)
{
	_printMode = 0;
	_xOutputStep = 1;
	_yOutputStep = 1;
	_zOutputStep = 1;
	_stepStep = 1;
	sFit = "9 Point Spline";
	outfile.open("outfile.out");
	return;
}


C3D_MOCGrid::~C3D_MOCGrid( void)
{
	return;
}

/****************************************************************************
*
*	Public Functions
*
****************************************************************************/
void C3D_MOCGrid::SetPrintMode( int iMode, int xStep, int yStep, int zStep, int stepStep)
{
	_printMode = iMode;
	_xOutputStep = xStep;
	_yOutputStep = yStep;
	_zOutputStep = zStep;
	_stepStep = stepStep;
	return;
}

//**************************************************************************************
//**************************************************************************************
//**************************************************************************************
int C3D_MOCGrid::CalcNozzle( CDialog *p3D_MOCDlg)
{
	//	This is the main function in this class. It directs the class on what to do.
	//	To solve for the nozzle contour, the methodolofy is a follows.
	//	Given: initial plane flow properties and nozzle wall geometry
	//	For a point P1 on the initial plane, located P2 on a new reference plane.
	//		This is done by assuming P2 is on the same streamline as P1.
	int i,k, timer = 0, failCounter = 0;
	double dz; // step size	

	int *stepCounter = new int;
	int *totalCounter = new int;
	int CBPFlag;
	*stepCounter = 0;
	*totalCounter = _nZ;
	p3D_MOCDlg->PostMessage( WM_COUNTER_UPDATE, (WPARAM) stepCounter, (LPARAM) totalCounter);
	//	Step through each axial plane of data. K is the new reference plane, k-1 is
	//	the initial reference plane
	_nNeighbors = 8;
	for ( k = 1; k < _nZ; k++)
	{
		if (sFit == "All Point Spline") AllPointSurfaceFit(k-1);
		else SetNeighborPoints( k-1, k-1);
		
		// On each plane, step through all of the points
		for ( i = 0; i < _nPTS; i++)
		{
			dz = _wallPt[0][k].z - _wallPt[0][k-1].z;
			if (!_bodyPointFlag[i]) // This is a field point calculation
			{
				if (!CalcFieldPoint(i,k-1, dz) && failCounter < 15)
				{
					failCounter ++;
					AddNewNozzlePoint(k);
					i = -1;
				}
			}
			else // This is the body point calculation
			{
				CBPFlag = CalcBodyPoint(i, k-1, dz);
				if (CBPFlag == FAIL_MACH && failCounter < 15)
				{
					//	The calculate body point failed, add a wall point at dz/2 and try again.
					//	do this 15 times, If it does not work, then kill the program.
					failCounter ++;
					AddNewNozzlePoint(k);
					i = -1;
				}
				else if (failCounter >= 15 || CBPFlag == FAIL) 
				{
					AfxMessageBox("Body Point Calculation FAILED");
					return FAIL;
				}
			}
		}
		if (++timer == _stepStep) 
		{
			OutputContour( 0, _nPTS-1, 0, k);
			timer = 0;
		}
		*stepCounter = k;
		p3D_MOCDlg->PostMessage( WM_COUNTER_UPDATE, (WPARAM) stepCounter, (LPARAM) totalCounter);
	} // end if k: axial loop
	OutputContour( 0, _nPTS-1,0, k-1);
	return OK;
}

//**************************************************************************************
//**************************************************************************************
//**************************************************************************************
void C3D_MOCGrid::SetSplineFit( CString x)
{
	sFit = x;
	return;
}
			
//**************************************************************************************
//**************************************************************************************
//**************************************************************************************
int C3D_MOCGrid::SetInitialPropertiesForCircularThroat(double pres, double temp, double mach, 
		double mWt, double gamma, double theta, double psi, int nDiv, CString geomFileName)
{
	//	This functions sets the initial properties at the initial reference plane
	//	Pressure, temperature, velocity, and the rest of the state is set.
	int j,k;
	double r0, x0, y0, angle, z0;
	char header[80];

	//	Check to see if the initial parameters are OK
	if ( pres <= 0.0 || temp <= 0.0 || mWt <= 0.0 || gamma <= 0.0 || mach < 1.0 )
	{
		AfxMessageBox("All input parameters must be > 0.0");
		return 0;
	}

	//	Open the Geometry File, to read in the nozzle geometry
	fstream geomFile;
	geomFile.open(geomFileName, ios::in);
	if (!geomFile.is_open())
	{
		AfxMessageBox("Could not open geometry file");
		return FAIL;
	}

	//	The geometry file contains a header that gives the number of axial locations
	//	followed by the number of Rays that define the wall points
	geomFile >> _nZ >> ws;
	
	if (_nZ <= 0)
	{
		AfxMessageBox("Could not read in correct number of points in geometry file.");
		exit(1);
	}
	geomFile.getline(header,80);
	//	Set some of the private members to the inputs
	_nDiv = nDiv; // number of Divisions to break up the outside wall surface into

	//	Set the private grid members and relative data arrays 
	//	to their correct sizes
	InitializeWallPoints();

	//	Open up a geometry output file
	fstream ofile;
	ofile.open("Initial Wall.plt", ios::out);
	ofile << "VARIABLES = \"X(in)\",\"Y(in)\",\"Z(in)\"" << endl
		<< "TITLE = \"Initial Wall Plot\"" << endl
		<< "text  x=5  y=93  t=\"Initial Wall Plot\"" << endl ;
//		<< "zone t=\"Data\"" << " I =" << _nDiv << " J = 1 K = " << _nZ << endl;
	//	Now that the data members are initialized, read from the data file
	//	There are _nZ data sets to read 
	for ( k = 0; k < _nZ; k++)
	{
		ofile << "zone t=\"K = " << k << "\"" << " I =" << _nDiv+1 << " J = 1 K = 1" << endl;
		//	Read the axial location of the plane, and the circle that defines the 
		//	contour.
		geomFile >> z0 >> r0 >> x0 >> y0 >> ws;
		for ( j = 0; j < _nDiv; j++)
		{
			_wallPt[j][k].z = z0;
			angle = j*2*PI/(_nDiv);
			_wallPt[j][k].x = r0*cos(angle) + x0;
			_wallPt[j][k].y = r0*sin(angle) + y0;
			if (fabs(_wallPt[j][k].x) < 1e-10) _wallPt[j][k].x = 0.0;
			if (fabs(_wallPt[j][k].y) < 1e-10) _wallPt[j][k].y = 0.0;
			if (k == 0 && j == 0) SetInitialReferencePlane(pres, temp, mach, mWt, gamma, theta, psi, z0, r0, _nDiv);
			ofile << _wallPt[j][k].x << "\t" << _wallPt[j][k].y << "\t" << _wallPt[j][k].z
				<< endl;
		}
		//	Output the first point again to close the surface
		ofile << _wallPt[0][k].x << "\t" << _wallPt[0][k].y << "\t" << _wallPt[0][k].z << endl;
		ofile << endl;
	}
	geomFile.close();
	ofile.close();

	//	Use this function to set the initial reference plane, Z = z0;
	return 1; // good return
}

//**************************************************************************************
//**************************************************************************************
//**************************************************************************************
void C3D_MOCGrid::OutputContour( int iStart, int iEnd, int kStart, int kEnd)
{
	//	This Function will output the nozzle contour in TECPLOT format
	int i,k;
	double rad,pt,tt,ratio;
	fstream ofile;
	ofile.open("full_mesh.plt", ios::out);
	ofile << "VARIABLES = \"X(in)\",\"Y(in)\",\"Z(in)\",\"R(in)\",\"P(psia)\","
		<< "\"PT(psia)\",\"T(R)\",\"TT(R)\",\"RHO(lbm/ft3)\",\"Mach\"," 
		<< "\"Velocity(ft/s)\",\"Theta(deg)\",\"Psi(deg)\",\"I\",\"K\",\"BodyFlag\"" << endl
		<< "TITLE = \"Volume Mesh Plot\"" << endl
		<< "text  x=5  y=93  t=\"Characteristic Mesh Plot\" " << endl
		<< "zone t=\"All Data\"" << " I = " 
		<< int ( (iEnd-iStart)/_xOutputStep) +1 << " J = 1 K =" 
		<< int ( (kEnd-kStart)/_zOutputStep) +1 << endl;// << " K = 1" << endl;*/
  
	//	Loop through each ray that defines the wall
	for ( k = kStart; k <= kEnd; k += _zOutputStep)
	{
		for (i = iStart; i <= iEnd; i += _xOutputStep)
		{
			ratio = 1+(_pt[i][k].g-1.)/2.*_pt[i][k].mach*_pt[i][k].mach;
			tt = _pt[i][k].t*ratio;
			pt = _pt[i][k].p*pow(ratio,(_pt[i][k].g)/(_pt[i][k].g-1));
			rad = sqrt(_pt[i][k].x*_pt[i][k].x + _pt[i][k].y*_pt[i][k].y);
				ofile << _pt[i][k].x << "\t" << _pt[i][k].y << "\t" << _pt[i][k].z << "\t" << rad
				<< "\t" << _pt[i][k].p << "\t" << pt << "\t" << _pt[i][k].t 
				<< "\t" << tt << "\t" << _pt[i][k].rho 
				<< "\t" << _pt[i][k].mach  << "\t" << _pt[i][k].q << "\t"
				<< _pt[i][k].theta*DEG_PER_RAD << "\t" << _pt[i][k].psi*DEG_PER_RAD << "\t" 
				<< i << "\t" << k << "\t" << _bodyPointFlag[i] << endl;
		}
	}
	ofile.close();
	OutputStreamlines(iStart,iEnd,kStart,kEnd);
	OutputAxialStations(iStart,iEnd,kStart,kEnd);
	OutputBoundary(iStart, iEnd, kStart,kEnd);
	return;
}


/****************************************************************************
*
*	Private Functions
*
****************************************************************************/
int C3D_MOCGrid::CalcFieldPoint(int i, int k, double dz)
{
	//	This functions finds the point P2 (_pt[i][k+1]) which lies on proceeding axial plane
	//	based on knowing the propeties at point P1 (_pt[i][k]) and the distance between
	//	the planes.
	//	k is the reference plane. P1 lies on k. P2 lies on k+1
	//	This done in the following way.
	//	Given P1 and dz (the axial distance from P1 to P2), a streamline from P1
	//	is used to determine the location of P2.
	//	From P2, 4 bicharacteristic lines are drawn back to plane 1 to give the location
	//	of 4 base points that are near P1.
	//	A surface fit of plane 1 is calculated given the 8 neighboring around P1. This
	//	surface fit is used to determine the properties at the 4 Base points.
	//	The compatability equation for each of the 4 base Points related to P2 are solved
	//	to give the flow properties at P2. 
	//	A new streamline from P1 to P2 is created based on the average properties of P1 and
	//	P2. The process of the creating the base points and solving to the compatability
	//	equations is repeated until the location of P2 does not change (within some tolerance).

	//	Initialize the variables that will be used
	double theta, psi, beta, pErr, thetaErr, psiErr,del,tTotal,pTotal,g;
	double dxdN, dydN, dtdx, dtdy, dpdx, dpdy, dpdz, dtdz, basePtRad;
	double psi12,theta12, newPres2, newPsi2, newTheta2, deltaI[4];
	dummyStruct Deriv, PTPsi;
	int n, iBase,ii;
	
	XYZPoint P2; // declaration of the point to be found
	const XYZPoint P1 = _pt[i][k]; // declaration of the known point to make coding easier
	XYZPoint *basePt = new XYZPoint[4]; // 4 base points that will be created
	//	Parameters used in the compatability equation solutions of the base points
	double *dtdN = new double[4]; // dTheta/dN 
	double *dpdN = new double[4]; // dPsi/dN
	double *dzdN = new double[4]; // dz/dM
	//	Average values of P2 and the 4 base points
	double *thetaAvg = new double[4];
	double *deltaAvg = new double[4];
	double *betaAvg = new double[4];
	
	//	Initially set all of the base points to P1. Also set P2 equal to P1
	for ( ii = 0; ii <= 3; ii++) basePt[ii] = P1;
	P2 = P1;

	if ( k == 129 && i == 0)
	{
		i = i;
	}
	//	Calculate the total conditions at P1. These are assumed to be the total
	//	conditions at P2.
	g = P1.g;
	tTotal = P1.t*(1+ (g-1)/2*P1.mach*P1.mach);
	pTotal = P1.p*pow( 1 + (g-1)/2*P1.mach*P1.mach, g/(g-1));

	//	Set the axial location (z) for P2 to the correct value.  Of all of the values
	//	this remains constant during the iteration
	P2.z = P1.z + dz;

	//	Calculate the surface fit coeffiecient around the point i,k. The global array
	//	__cFit contains the information
	if (sFit == "9 Point Spline") SurfaceFit(i,k);

	//	Find the 4 base points P3-P6 created by 4 bicharacteristics that extend
	//	from P2. This is an iterative process.  The points are located at delta
	//	angles of 0, pi/2, pi and 3pi/2. Delta is defined as the angle normal to q and
	//	measured from the x-q plane.
	//	deltaI will be used to store the new parametric angle
	basePt[0].delta = 0.0;
	if (P1.x != 0.0 || P1.y != 0.0) basePt[0].delta = atan2(P1.x,P1.y);
	if (basePt[0].delta < 0.0) basePt[0].delta += 2*PI;
	deltaI[0] = basePt[0].delta;
	for (ii = 1; ii <=3; ii++)
	{
		basePt[ii].delta = basePt[ii-1].delta + PI/2;
		if (basePt[ii].delta >= 2*PI) basePt[ii].delta -= 2*PI;
		deltaI[ii] = basePt[ii].delta;
	}
	
	//	For the initial iterations, P1 is used as the starting quantities. The iterations
	//	continue until pressure, theta and psi converge
	pErr = 9.9;
	thetaErr = 9.9;
	psiErr = 9.9;
	n = 0;
	while ( (fabs(pErr) > 1e-5 || fabs(thetaErr) > 1e-5 || fabs(psiErr) > 1e-5) && n++<50)
	{
		//	Average the angles of P1 and P2
		theta12 = (P1.theta + P2.theta)/2;
		psi12 = (P1.psi + P2.psi)/2;

		// Solve for P2 location based on that it sits on the streamline from P1
		//	eq 27 and 28
		P2.x = P1.x + tan(theta12)*dz/cos(psi12);
		P2.y = P1.y + tan(psi12)*dz;
		if (fabs(P2.x) < 1e-5) P2.x = 0.0;
		if (fabs(P2.y) < 1e-5) P2.y = 0.0;

		//	Step through each of the bicharacteristic points
		for ( iBase = 0; iBase < 4; iBase++)
		{
			//	The delta have already been calculated for each point
			//	Set some angle to easier to read values
			beta = (CalcMu(basePt[iBase].mach)+CalcMu(P2.mach))/2.;
			psi = (basePt[iBase].psi + P2.psi) / 2. ;
			theta = (basePt[iBase].theta + P2.theta)/2.;
			del = (basePt[iBase].delta + deltaI[iBase])/2;
					
			//	solve the bicharacteristic equations eq 30-33
			basePt[iBase].L = dz / (cos(beta)*cos(theta)*cos(psi) - 
				sin(beta)*(sin(theta)*cos(psi)*cos(del) + sin(psi)*sin(del)));
			basePt[iBase].x = P2.x - basePt[iBase].L*(cos(beta)*sin(theta) + 
				sin(beta)*cos(theta)*cos(del));
			basePt[iBase].y = P2.y - basePt[iBase].L*(cos(beta)*cos(theta)*sin(psi) - 
				sin(beta)*(sin(theta)*sin(psi)*cos(del) - cos(psi)*sin(del)));
			basePt[iBase].z = P1.z;
			
			// Make a check to see of the base point is within the current nozzle geometry
			basePtRad = sqrt(basePt[iBase].x*basePt[iBase].x + basePt[iBase].y*basePt[iBase].y);
			if (basePtRad > _wallPt[0][k].x )
			{
				// If this happens then the dz is set too big. return a FAIL and try again.
				return FAIL;
				//AfxMessageBox("BasePt outside current nozzle geometry.\nFieldPoint");
			}

			//	Return the variables dtdx, dtdy, dpsidx, and dpsidy based on the 
			//	surface fit for basePt[iBase]. This function alos updates the 
			//	thermo properties of the base point.
			Deriv = CalcSurfacePointProperties(&basePt[iBase],i,k);
			dtdx = Deriv.dSx[3];//	dtdx: deriv[3]
			dtdy = Deriv.dSx[6];//	dtdy: deriv[6]
			dpdx = Deriv.dSx[4];//	dpdx: deriv[4]
			dpdy = Deriv.dSx[8];//	dpdy: deriv[8]
			
			//	Calculate new deltaI for the base Point
			deltaI[iBase] = CalcParametricAngle( beta, theta, psi, basePt[iBase].delta);
			if (fabs(deltaI[iBase]) <= 1e-8) deltaI[iBase] = 0.0;
				
			//	Average the P2 and current base Pt angles again
			beta = (CalcMu(basePt[iBase].mach)+CalcMu(P2.mach))/2.;
			psi = (basePt[iBase].psi + P2.psi) / 2. ;
			theta = (basePt[iBase].theta + P2.theta)/2.;
			del = (basePt[iBase].delta + deltaI[iBase])/2;
  

			//	dxdN, dydN and dzdN are taken directly from eqs 23-25 of reference
			dxdN = -cos(theta)*sin(del);
			dydN = sin(theta)*sin(psi)*sin(del) + cos(psi)*cos(del);
			dzdN[iBase] = sin(theta)*cos(psi)*sin(del) - sin(psi)*cos(del);
			dtdz = (dtdx*(basePt[iBase].x - P2.x) + dtdy*(basePt[iBase].y - P2.y))/dz;
			dpdz = (dpdx*(basePt[iBase].x - P2.x) + dpdy*(basePt[iBase].y - P2.y))/dz;
		
			dtdN[iBase] = dtdx*dxdN + dtdy*dydN + dtdz*dzdN[iBase];
			dpdN[iBase] = dpdx*dxdN + dpdy*dydN + dpdz*dzdN[iBase];

			betaAvg[iBase] = beta;
			thetaAvg[iBase] = theta;
			deltaAvg[iBase] = del;

		} // end of For

		//  Use the compatability equations to solve for  P2, THETA2 and PSI2.
		//	The three unknows can be found using 3 of the 4 base points. To get a better
		//	answer 4 sets (loops) of 3 points will be used and the values averaged.
		//	Loop 1. Points 0,1,2
		//	Loop 2. Points 1,2,3
		//	Loop 3. Points 2,3,0
		//	Loop 4. Points 3,0,1

		PTPsi = CompatabilityEquationSolverForFieldPoint(basePt, dtdN, 
			dpdN, dzdN, thetaAvg, betaAvg, deltaAvg, dz);

		newPres2 = PTPsi.dSx[0];
		newTheta2 = PTPsi.dSx[1];
		newPsi2 = PTPsi.dSx[2];
		
		pErr = ( newPres2 - P2.p) / P2.p;
		if ( fabs(P2.theta) > 2e-2 ) 
			thetaErr = ( newTheta2 - P2.theta) / P2.theta;
		else thetaErr = newTheta2 - P2.theta;
		if ( fabs(P2.psi) > 2e-2 ) 
			psiErr = ( newPsi2 - P2.psi) / P2.psi;
		else psiErr = newPsi2 - P2.psi;

		//	Set all of the properties of P2
		P2.p = newPres2;
		P2.theta = newTheta2;
		P2.psi = newPsi2;
		P2.g = g;
		P2.molWt = P1.molWt;
		P2.t = tTotal*pow(P2.p/pTotal, (g-1)/g);
		P2.rho = (P2.p*144)/(GASCON/P2.molWt*P2.t);
		P2.q = sqrt(2*g/(g-1)*GASCON/P2.molWt*GRAV*(tTotal-P2.t));
		P2.mach = P2.q/(sqrt(g*GASCON/P2.molWt*P2.t*GRAV));
		if (P2.mach < 1.0)
		{
			AfxMessageBox("Field Point Mach number < 1.0. WHOOPS in CalcFieldPoint.");
			exit(1);
		}
		
	}	// End of finding P2 loop
	if ( n == 51)
	{
		AfxMessageBox("Could not converge on field Point");
		exit(1);
	}
	if (fabs(newTheta2) < 1e-5) newTheta2 = 0.0;
	if (fabs(newPsi2)   < 1e-5) newPsi2 = 0.0;

	_pt[i][k+1] = P2;
	delete [] dzdN;
	delete [] thetaAvg;
	delete [] deltaAvg;
	delete [] betaAvg;
	delete [] basePt;
	delete [] dtdN;
	delete [] dpdN;
	return OK;
}

//**************************************************************************************
//**************************************************************************************
//**************************************************************************************
int C3D_MOCGrid::CalcBodyPoint(int i, int k, double dz)
{
	//	This routine finds the body point P2 (_pt[i][k+1]). given P1 _pt[i,k] 
	//	The new body point P2 is located at the intersection of the streamline through
	//	P1 and a surface normal to the body at Z = k+1
	//	k is the reference plane. P1 lies on k. P2 lies on k+1
	//	The body points are stored in the array	_pt array where _bodyPointFlag[i] = 1

	double theta, psi, beta, pErr, thetaErr, psiErr,del, basePtRad;
	double dxdN, dydN, dtdx, dtdy, dpdx, dpdy, dpdz, dtdz;
	double psi12,theta12, newPres2, newPsi2, newTheta2, deltaI[3];
	double N1,N2,N3,tTotal, pTotal,g,cosDelta;
//	double denom. radCheck, delStep, radCheckOld;
	dummyStruct Deriv, PTPsi, bFit1, bFit2, unitNormal1, unitNormal2,CC,newP2XY,newJK;
	int n, iBase;
	//	For this routine, 3 base points are needed. These base points are on the plane
	//	k and surround P1. Declare base Points
	XYZPoint *basePt = new XYZPoint[3];
	//	These are parmeated used in the compatability equation solutions
	double *dtdN = new double[3];
	double *dpdN = new double[3];
	double *dzdN = new double[3];

	//	These are average values between P2 and the 3 Base points
	double *thetaAvg = new double[3];
	double *deltaAvg = new double[3];
	double *betaAvg = new double[3];

	//	Set P1 and P2 to make the coding easier
	XYZPoint P2;
	const XYZPoint P1 = _pt[i][k];

	//	Initially, set the Base point equal to the P1 values
	for (n = 0; n < 3; n++) basePt[n] = P1;

	//	Initially, set P2 and the base point equal to the values at P1. Also set the
	//	point to be found equal to the P1. It is known that the Z position of P2 has
	//	been incremented dz from P1
	P2 = P1;
	//	Calculate the other properties at _pt[i][j][k+1]. Assume isentropic properties
	//	along the streamline
	g = P1.g;
	tTotal = P1.t*(1+ (g-1)/2*P1.mach*P1.mach);
	pTotal = P1.p*pow( 1+ (g-1)/2*P1.mach*P1.mach, g/(g-1));
	
	P2.z += dz;
	//	Calculate the surface fit coeffiecient around the point i,k. The global array
	//	__cFit contains the information
	if (sFit == "9 Point Spline") SurfaceFit(i,k);
	
	//	Calculate the body fit coefficients of the body Point closest to P1 and P2 
	//	dSi[0] =  CONSTANT_X, X = C
	//				CONSTANT_Y, Y = C
	//				LINE: AX + BY = C
	//				CIRCLE: (X-A)^2 + (Y-B)^2 = C	
	//	dSx[0,1,2] = A,B,C	
	bFit1 = BodyFit(P1);
	bFit2 = BodyFit(P2);

	//	Calculate the unitNormal coefficients at P1 and P2
	if (k == 196 && i== 77)
	{
		k= k;
	}
	unitNormal1 = CalcUnitNormalToBodySurface(P1);
	unitNormal2 = CalcUnitNormalToBodySurface(P2);
	if (unitNormal1.dSx[0] < 0.0 || unitNormal2.dSx[0] < 0.0)
	{
		//	The code is calculating a unit normal vector that is going upstream
		//	This may be bad.
//		AfxMessageBox("Nozzle contour is converging\nCalcBodyPoint");
//		return FAIL_MACH;
	}

	//	Set N1, N2 and N3 so that it is easier to implement
	N1 = unitNormal1.dSx[0]; // Z
	N2 = unitNormal1.dSx[1]; // Y
	N3 = unitNormal1.dSx[2]; // X
	
	//	Set the parametric angle delta based on P2 to start with.
	//	The calculated deltas should put the basePt within the current
	//	known geometry.
	cosDelta = -N1*sin(P2.theta)*cos(P2.psi) + N2*cos(P2.theta) 
			- N3*sin(P2.theta)*sin(P2.psi);
	/*denom = sqrt(N2*N2 + N3*N3);
	cosDelta = N2*cos(P2.theta)/denom - N3*sin(P2.theta)*sin(P2.psi)/denom;*/
	if ( cosDelta > 1.0)
	{
	//	AfxMessageBox("Calculated Parametric angle, delta, is being set to 1.0");
		cosDelta = 1.0;
	}
	else if ( cosDelta < -1.0)
	{
	//	AfxMessageBox("Calculated Parametric angle, delta, is being set to -1.0");
		cosDelta = -1.0;
	}
	basePt[0].delta = 	acos(cosDelta);
 	if (N3 < 0.0) basePt[0].delta = 2*PI - basePt[0].delta;
	basePt[1].delta = basePt[0].delta - PI/3.;
	if (basePt[1].delta < 0.0) basePt[1].delta += 2*PI;
	basePt[2].delta = basePt[0].delta + PI/3.;
	if (basePt[2].delta >= 2*PI) basePt[2].delta -= 2*PI;
	for ( iBase = 0; iBase <= 2; iBase++) deltaI[iBase] = basePt[iBase].delta;
	
	//	Set up an iterative loop to converge on the P2, Theta2 and Psi2
	pErr = 9.9;
	thetaErr = 9.9;
	psiErr = 9.9;
	n = 0;
	while ( (fabs(pErr) > 1e-5 || fabs(thetaErr) > 1e-5 || fabs(psiErr) > 1e-5) && n++<50)
	{ 
		//	Average the angles of P1 and P2
		theta12 = (P1.theta + P2.theta)/2;
		psi12 = (P1.psi + P2.psi)/2;

		N1 = (unitNormal1.dSx[0] + unitNormal2.dSx[0])/2;
		N2 = (unitNormal1.dSx[1] + unitNormal2.dSx[1])/2;
		N3 = (unitNormal1.dSx[2] + unitNormal2.dSx[2])/2;
		
		//	Calculate the coefficient to eq 40
		CC.dSx[0] = N3*cos(theta12)*cos(psi12) - N1*cos(theta12)*sin(psi12);
		CC.dSx[1] = N1*sin(theta12) - N2*cos(theta12)*cos(psi12);
		CC.dSx[2] = dz*(N3*sin(theta12) - N2*cos(theta12)*sin(psi12)) + 
			CC.dSx[0]*P1.x + CC.dSx[1]*P1.y;

		//	Solve for new position of P2 given the BodyFit and the eq 40 coefficients
		//	The values are stored in new P2XY
		newP2XY = SolveForBodyPointPosition( CC, bFit2, P2.x, P2.y);
		P2.x = newP2XY.dSx[0];
		P2.y = newP2XY.dSx[1];
		if (fabs(P2.x) < 1e-5) P2.x = 0.0;
		if (fabs(P2.y) < 1e-5) P2.y = 0.0;
		
		//	All of the above seems to work OK.

		//	Check to make sure that the closest point to the new point still the same
		//	The closest point was stored in unitNormal2
		newJK = CheckNewPointPosition( P2, unitNormal2.dSi[0], unitNormal2.dSi[1]);
		if (newJK.dSi[0] != unitNormal2.dSi[0] && newJK.dSi[1] != unitNormal2.dSi[1])
		{
			AfxMessageBox("Error in finding new point");
			bFit2 = BodyFit(P2);
		}
				
		//	Find the unit normal vector at the new P2 position. This is line A in ref
		unitNormal2 = CalcUnitNormalToBodySurface(P2);
		if ( unitNormal2.dSx[0] < 0.0)
		{
			//	The code is calculating a unit normal vector that is going upstream
			//	This is bad.
			AfxMessageBox("Nozzle contour is converging\nCalcBodyPoint");
		//	exit(1);
		}
		N1 = unitNormal2.dSx[0]; // Z
		N2 = unitNormal2.dSx[1]; // X
		N3 = unitNormal2.dSx[2]; // Y

		//	Set the parametric angle delta based on P2
		cosDelta = -N1*sin(P2.theta)*cos(P2.psi) + N2*cos(P2.theta) 
         				- N3*sin(P2.theta)*sin(P2.psi);
		//	For now, I am going to base this in the normal projection in the XY plane
	/*	denom = sqrt(N2*N2 + N3*N3);
		cosDelta = N2*cos(P2.theta)/denom - N3*sin(P2.theta)*sin(P2.psi)/denom;*/
		if ( cosDelta > 1.0)
		{
			AfxMessageBox("Calculated Parametric angle, delta, is being set to 1.0");
			cosDelta = 1.0;
		}
		else if ( cosDelta < -1.0)
		{
	//		AfxMessageBox("Calculated Parametric angle, delta, is being set to -1.0");
			cosDelta = -1.0;
		}
		//Tharen changed PI/2 to PI/3. This should help keep the base points in the 
		//	current nozzle geometry
		basePt[0].delta = 	acos(cosDelta);
		if (N3 < 0.0) basePt[0].delta = 2*PI - basePt[0].delta;
		basePt[1].delta = basePt[0].delta - PI/3.;
		if (basePt[1].delta < 0.0) basePt[1].delta += 2*PI;
		basePt[2].delta = basePt[0].delta + PI/3.;
		if (basePt[2].delta >= 2*PI) basePt[2].delta -= 2*PI;

		//	Step through each of the 3 bicharacteristic points (aka base points)
		for ( iBase = 0; iBase <= 2; iBase++)
		{
			//	Along each bicharacteristic, assume that the values are the
			//	average of P2 and bicharacteristic point
			beta = (CalcMu(basePt[iBase].mach)+CalcMu(P2.mach))/2.;
			psi = (basePt[iBase].psi + P2.psi) / 2. ;
			theta = (basePt[iBase].theta + P2.theta)/2.;
			del = (basePt[iBase].delta + deltaI[iBase])/2;
			del -= PI;
			if (del < 0.0) del += 2*PI;
		
			//	Set the position of the bicharacteristic point
			basePt[iBase].L = dz / (cos(beta)*cos(theta)*cos(psi) - 
				sin(beta)*(sin(theta)*cos(psi)*cos(del) + sin(psi)*sin(del)));
			basePt[iBase].x = P2.x - basePt[iBase].L*(cos(beta)*sin(theta) + 
				sin(beta)*cos(theta)*cos(del));
			basePt[iBase].y = P2.y - basePt[iBase].L*(cos(beta)*cos(theta)*sin(psi) - 
				sin(beta)*(sin(theta)*sin(psi)*cos(del) - cos(psi)*sin(del)));
			basePt[iBase].z = P1.z;

			// Make a check to see of the base point is within the current nozzle geometry
			basePtRad = sqrt(basePt[iBase].x*basePt[iBase].x + basePt[iBase].y*basePt[iBase].y);
			if (basePtRad > sqrt(bFit2.dSx[2]))
			{
				// Commented out by H. Reising on 5/28/20 to suppress pop-up messages
		 	    //AfxMessageBox("BasePt outside current nozzle geometry\nBodyPoint");	
			}
		
			//	At this point the P2 location has been found

			//	Return the variables dtdx, dtdy, dpsidx, and dpsidy based on the 
			//	surface fit for basePt[iBase]. This function also updates the 
			//	thermo properties and angles of the base point.
			Deriv = CalcSurfacePointProperties(&basePt[iBase],i,k);
			//	dtdx: deriv[3]
			//	dtdy: deriv[6]
			//	dpdx: deriv[4]
			//	dpdy: deriv[8]
			dtdx = Deriv.dSx[3];
			dtdy = Deriv.dSx[6];
			dpdx = Deriv.dSx[4];
			dpdy = Deriv.dSx[8];
			
			//	Calculate new deltaI for the base Point
			deltaI[iBase] = CalcParametricAngle( beta, theta, psi, basePt[iBase].delta);
			if (fabs(deltaI[iBase]) <= 1e-8) deltaI[iBase] = 0.0;

			//	Average the P2 and current base Pt angles again
			beta = (CalcMu(basePt[iBase].mach) + CalcMu(P2.mach))/2.;
			psi = (basePt[iBase].psi + P2.psi) / 2. ;
			theta = (basePt[iBase].theta + P2.theta)/2.;
			del = (basePt[iBase].delta + deltaI[iBase])/2;
			del -= PI;
			if (del < 0.0) del += 2*PI;
	
			dxdN = -cos(theta)*sin(del);
			dydN = sin(theta)*sin(psi)*sin(del) + cos(psi)*cos(del);
			dzdN[iBase] = sin(theta)*cos(psi)*sin(del) - sin(psi)*cos(del);
			dtdz = (dtdx*(basePt[iBase].x - P2.x) + dtdy*(basePt[iBase].y - P2.y))/dz;
			dpdz = (dpdx*(basePt[iBase].x - P2.x) + dpdy*(basePt[iBase].y - P2.y))/dz;

			dtdN[iBase] = dtdx*dxdN + dtdy*dydN + dtdz*dzdN[iBase];
			dpdN[iBase] = dpdx*dxdN + dpdy*dydN + dpdz*dzdN[iBase];

			betaAvg[iBase] = beta;
			thetaAvg[iBase] = theta;
			deltaAvg[iBase] = del;
		} // end of For

		//	Now the compatability equations has to be solved for using the 3 base points
		//	that were calculated. This compatibility equation is the same for the 
		//	field point, but the application is a bit different. See function for a 
		//	better explanation
		PTPsi = CompatabilityEquationSolverForBodyPoint( basePt, dtdN, 
			dpdN, dzdN, thetaAvg, betaAvg, deltaAvg, dz, P2, unitNormal2);

		newPres2 = PTPsi.dSx[0]; 
		newTheta2 = PTPsi.dSx[1];
		newPsi2 = PTPsi.dSx[2];
	
		//	Calculate the error in the new guesses and the current values for P,theta and psi
		pErr = ( newPres2 - P2.p) / P2.p;
		if ( fabs(P2.theta) > 2e-2 ) 
			thetaErr = ( newTheta2 - P2.theta) / P2.theta;
		else thetaErr = newTheta2 - P2.theta;
		if ( fabs(P2.psi) > 2e-2 ) 
			psiErr = ( newPsi2 - P2.psi) / P2.psi;
		else psiErr = newPsi2 - P2.psi;

		P2.p = newPres2;
		P2.theta = newTheta2;
		P2.psi = newPsi2;
		P2.g = P1.g;
		P2.molWt = P1.molWt;
		P2.t = tTotal*pow(P2.p/pTotal, (g-1)/g);
		P2.rho = (P2.p*144)/(GASCON/P2.molWt*P2.t);
		P2.q = sqrt(2*g/(g-1)*GASCON/P2.molWt*GRAV*(tTotal-P2.t));
		P2.mach = P2.q/(sqrt(g*GASCON/P2.molWt*P2.t*GRAV));
		
		//	This is a check to make sure the calculation is correct. If the Mach number decreases then 
		//	one of the unit normals must be < 0.0.
		
		if (P2.mach < 1.0)
		{
			AfxMessageBox("Body Mach number < 1.0. WHOOPS in CalcBodyPoint.\nTry increasing initial Mach or decreasing THETA or PSI");
			return FAIL_MACH;
		}
/*		if (P1.theta == 0 && P2.theta !=0)
		{
			P2.p = newPres2;
		}
		if (P1.psi == 0 && P2.psi !=0)
		{
			P2.p = newPres2;
		}*/
		
	}
	if ( n == 51 && fabs(pErr) > 1e-3 && fabs(thetaErr) > 1e-3 && fabs(psiErr) > 1e-3)
	{
		
		// This may be a problem return a FAIL and try to decrease the DZ step size
		//	if the error is small then proceed
	
		//AfxMessageBox("Could not converge on body Point");
		return FAIL;
	}
/*(P2.mach < P1.mach && unitNormal1.dSx[0] > 0.0 && unitNormal2.dSx[0] > 0.0)
	{
			AfxMessageBox("Mach is decreasing through an expanding duct");
			exit(1);
	}
	else if (P2.mach > P1.mach && unitNormal1.dSx[0] < 0.0 && unitNormal2.dSx[0] < 0.0)
	{
			AfxMessageBox("Mach is increasing through a converging duct");
			exit(1);
	}*/
	
	if (fabs(P2.theta) < 1e-4)
	{
		P2.theta = 0.0;
		P2.x = P1.x;
	}
	if (fabs(P2.psi) < 1e-4)
	{
		P2.psi = 0.0;
		P2.y = P1.y;
	}
	
/*	if (k > 0 && P1.theta == 0 && P2.theta !=0)
	{
		P2.p = newPres2;
	}
	if (k > 0 && P1.psi == 0 && P2.psi !=0)
	{
		P2.p = newPres2;
	}*/
	
	_pt[i][k+1] = P2;
	delete [] dtdN;
	delete [] basePt;
	delete [] dpdN;
	delete [] dzdN;
	delete [] thetaAvg;
	delete [] deltaAvg;
	delete [] betaAvg;

	outfile << k << "\t" << i << "\t" << N1 << "\t" << N2 << "\t" << N3 << endl;


	return OK;
}
	

//**************************************************************************************
//**************************************************************************************
//**************************************************************************************	
dummyStruct C3D_MOCGrid::CalcUnitNormalToBodySurface(const XYZPoint P)
{
	//	In this new function, given the Point P at k, the function will find the 
	//	surface fit of the surface defined 9 points in the k-1, k and k+1 planes
	//	the points are
		
	//	From there take the neighboring wall points and calculate the surface fit.
	//	This is the same and the surface fit solution used in the plane calculation
	//	The spline surface is fit by the following equation
	//	a0 + a1X + a2Y + SUM[i:0-8](bi * f(X,Y)) = W(X,Y)
	//	f(X,Y) = r^2* ln(r^2)
	//	r^2 = (X-xi)^2 + (Y-yi)^2
		//	the neighboring point _neighbor[i][0] = _pt[i][k]
	//	The remaining 3 equations are SUM(bi) = SUM(xibi) = SUM(yibi) = 0.0
	//	Output _cFit[i][j] array
	//	[i] = 0 - 11: coefficient number

	//	Assume a linear interpolation between the points and find the normal vector
	//	along each of these segments.  Then average the two together.
	
	//	Find the closest Body Point to P
	//	J is in dSi[0];
	//	K is in dSi[1];
/*	JK = FindClosestBodyPoint( P);

	//	There may be times k is the first or last plane of data, and therefore k-1 and/or
	//	k+1 is not there. If that is the case move the k plane.
	if (JK.dSi[1] == 0) JK.dSi[1] = 1;
	if (JK.dSi[0] == _nDiv -1) JK.dSi[0] = _nDiv-2;

	const int j = JK.dSi[0];
	const int k = JK.dSi[1];

	//	The 9 points are
	//	[j-1][k-1]	[j-1][k] [j-1][k+1]
	//	[j][k-1]	[j][k]		[j][k+1]
	//	[j+1][k-1]	[j+1][k]	[j+1][k+1]
	
	// To make the calcation go smoother through the use of loops, set up an array that 
	//	defines the indicees
	int jj[12] = {j-1,j-1,j-1,j,j,j,j+1,j+1,j+1};
	int kk[12] = {k-1,k,k+1,k-1,k,k+1,k-1,k,k+1};


		
	int iRow, kPt;
	//	Declare 2 matrices that will be used to solve for the 12 equations and 
	//	12 unknowns simultaneously
	Mat_DP a(12,12),aTmp(12,12);
	double r;

	for ( iRow = 0; iRow <= 8; iRow++) // Step through the first 9 equations
	{
		a[iRow][0] = 1.0;
		//	Each solution will go into a separate column starting a iCol = 3	
		for ( kPt = 0; kPt <= 8; kPt++) // Step through each column
		{
			r = pow(_wallPt[jj[iRow]][kk[iRow]].x - _wallPt[jj[iRow]][kk[kPt]].x, 2.) + 
				pow(_wallPt[jj[iRow]][kk[iRow]].y - _wallPt[jj[iRow]][kk[kPt]].y, 2.);
			if ( r != 0.0) a[iRow][kPt+3] = r*log(r);
			else a[iRow][kPt+3] = 0.0;
		}
		a[iRow][1] = _wallPt[jj[iRow]][kk[iRow]].x;
		a[iRow][2] = _wallPt[jj[iRow]][kk[iRow]].y;
		//	RHS of the equation
		_cFit[iRow][0] = 0.0;
	}	// End of Row loop
	//	The above takes care the first 9 equations.

	//	Start the last 3 rows, 	
	for ( iRow = _nNeighbors+1; iRow <= _nNeighbors+3; iRow++)
	{
		for (kPt = 0; kPt <=2; kPt++) a[iRow][kPt] = 0.0;
		//	All of the right hand side = 0.0
		_cFit[iRow][0] = 0.0;
		_cFit[iRow][1] = 0.0;
		_cFit[iRow][2] = 0.0;
		_cFit[iRow][3] = 0.0;
		_cFit[iRow][4] = 0.0;
		_cFit[iRow][5] = 0.0;
		_cFit[iRow][6] = 0.0;
	}

	//	Finish the last 3 equations
	for (kPt = 0; kPt <= _nNeighbors; kPt++)
	{
		a[_nNeighbors+3][kPt+3] = 1.0; // bi
		a[_nNeighbors+1][kPt+3] = _neighbor[i][kPt].y; 
		a[_nNeighbors+2][kPt+3] = _neighbor[i][kPt].x;
	}

	//	At this point there is a set of simultaneous equations
	//	Solve for them, one at a time
	//	Initialize vectors that are needed.
	DP d;
	Vec_INT indx(_nNeighbors+4);
	Vec_DP b(_nNeighbors+4);
    
	for ( kk = 0; kk <= 6; kk++)
	{
		for ( iRow = 0; iRow <= _nNeighbors+3; iRow++)
		{
			b[iRow] = _cFit[iRow][kk];
			for ( kPt = 0; kPt <= _nNeighbors+3; kPt++) aTmp[iRow][kPt] = a[iRow][kPt];
		}
		// Perform the LU decomposition
		NR::ludcmp(aTmp,indx,d);
		// Perform the Back Substitution
		NR::lubksb(aTmp,indx,b);
		for ( iRow = 0; iRow <= _nNeighbors+3; iRow++) _cFit[iRow][kk] = b[iRow];
	}

	//	Output Pressure coefficients
/*	ofile << endl << endl;
	for ( kk = 0; kk <= 6; kk++)
	{
		for ( iRow = 0; iRow <= _nNeighbors+3; iRow++) ofile << _cFit[iRow][kk] << "\t" ;
		ofile << endl;
	}
	ofile.close();*/

	//	The array _cFit contains the surface fit coefficients
/*	return;
}

*/


	//	Assume a linear interpolation between the points and find the normal vector
	//	along each of these segments.  Then average the two together.
	
/*	//	Find the closest Body Point to P
	//	J is in dSi[0];
	//	K is in dSi[1];
	JK = FindClosestBodyPoint( P);

	//	There may be times k is the first or last plane of data, and therefore k-1 and/or
	//	k+1 is not there. If that is the case move the k plane.
	if (JK.dSi[1] == 0) JK.dSi[1] = 1;
	if (JK.dSi[0] == _nDiv -1) JK.dSi[0] = _nDiv-2;

	const int j = JK.dSi[0];
	const int k = JK.dSi[1];

	dx = _wallPt[j][k].x - wallPt[j][k-1].x;
	dy = _wallPt[j][k].y - wallPt[j][k-1].y; 
	dz = _wallPt[j][k].z - wallPt[j][k-1].z;

*/


	
	//	This function finds the unit normals to the body surface given the Point P,
	//	n1: Z direction: positive means a diverging nozzle
	//	n2: X direction
	//	n3: Y direction
	dummyStruct bFit1, bFit2, bFit3, JK, n;
	double a1,a2,a3,b1,b2,b3,c1,c2,c3,fac[5], f[4][6], fa[6], fb[6], fc[6];
	double ax,by,n1,n2,n3,d,dz21,dz31,dz32,z12;
	int i;

	//	Find the closest Body Point to P
	//	J is in dSi[0];
	//	K is in dSi[1];
	JK = FindClosestBodyPoint( P);

	//	The function is going to find the body fits of 3 axial splices, k, k-1, and k+1.
	//	The following is a check to make sure that these are within the bounds of the 
	//	given wall data.
	
	//	There may be times k is the first or last plane of data, and therefore k-1 and/or
	//	k+1 is not there. If that is the case move the k plane.
	if (JK.dSi[1] == 0) JK.dSi[1] = 1;

	if (JK.dSi[0] == _nDiv -1) JK.dSi[0] = _nDiv-2;
	
	const int j = JK.dSi[0];
	const int k = JK.dSi[1];
	n.dSi[0] = j;
	n.dSi[1] = k;
	//	Find the body fit coordinates of the points of three points around wall points around P
	//	BodyFit return for a Cicle A,B,C in the equation (X-A)^2+(Y-B)^2 = C
	bFit1 = BodyFit(j, k-1);
	a1 = bFit1.dSx[0];
	b1 = bFit1.dSx[1];
	c1 = bFit1.dSx[2];
	bFit2 = BodyFit(j, k); 
	a2 = bFit2.dSx[0];
	b2 = bFit2.dSx[1];
	c2 = bFit2.dSx[2];
	//	If it the past plane of data then there is no k+1 term
	if (JK.dSi[1] == _nZ-1) bFit3 = bFit2;
	else bFit3 = BodyFit(j, k+1);
	a3 = bFit3.dSx[0];
	b3 = bFit3.dSx[1];
	c3 = bFit3.dSx[2];

	if ( bFit2.dSi[0] == CIRCLE) goto continue23;
	if ( bFit1.dSi[0] <= bFit2.dSi[0] && bFit3.dSi[0] <= bFit2.dSi[0]) goto continue24;
	goto continue27;

continue23:
	if ( bFit1.dSi[0] != bFit2.dSi[0] || bFit3.dSi[0] != bFit2.dSi[0]) goto continue27;

continue24:
	fac[1] = 1./(_wallPt[j][k].z - _wallPt[j][k-1].z);
	fac[2] = 1./(_wallPt[j][k+1].z - _wallPt[j][k-1].z);
	fac[3] = 1./(_wallPt[j][k+1].z - _wallPt[j][k].z);
	fac[4] = _wallPt[j][k-1].z + _wallPt[j][k].z;
	a2 = (a2-a1)*fac[1];
	b2 = (b2-b1)*fac[1];
	c2 = (c2-c1)*fac[1];
	a3 = (a3-a1)*fac[2];
	b3 = (b3-b1)*fac[2];
	c3 = (c3-c1)*fac[2];
	a3 = (a3-a2)*fac[3];
	b3 = (b3-b2)*fac[3];
	c3 = (c3-c2)*fac[3];
	a2 = a2-a3*fac[4];
	b2 = b2-b3*fac[4];
	c2 = c2-c3*fac[4];
	a1 -= (a2 + a3*_wallPt[j][k-1].z)*_wallPt[j][k-1].z;
	b1 -= (b2 + b3*_wallPt[j][k-1].z)*_wallPt[j][k-1].z;
	c1 -= (c2 + c3*_wallPt[j][k-1].z)*_wallPt[j][k-1].z;
	ax = a1 + (a2 + a3*P.z)*P.z;
	by = b1 + (b2 + b3*P.z)*P.z;
	if (bFit2.dSi[0] != CIRCLE) goto continue25;

	n2 = -2.*(P.x - a1 - (a2 + a3*P.z)*P.z);
	if ( P.x - ax < 0) n2 = fabs(n2);
	else n2 = -fabs(n2);
	n3 = -2.*(P.y - b1 - (b2 + b3*P.z)*P.z);
	if ( P.y - by < 0) n3 = fabs(n3);
	else n3 = -fabs(n3);
	n1 = -n2*(a2 + 2.*a3*P.z) - n3*(b2 + 2*b3*P.z) + c2 + 2.*c3*P.z;
	// Tharen added this to stop a negative unit normal occuring at z = 0.
	if (P.z == 0.0) n1 = fabs(n1);
	goto continue26;

continue25:
	if ( P.x - ax < 0) n2 = fabs(ax);
	else n2 = -fabs(ax);
	if ( P.y - by < 0) n3 = fabs(by);
	else n3 = -fabs(by);
	n1 = -(a2 + 2.*a3*P.z)*P.x - (b2 + 2.*b3*P.z)*P.y + (c2 +2.*c3*P.z);

continue26:
	d = sqrt(n1*n1 + n2*n2 + n3*n3);
	n.dSx[0] = n1 / d;
	n.dSx[1] = n2 / d;
	n.dSx[2] = n3 / d;
	return n;

continue27:
	if (bFit1.dSi[0] == CIRCLE) goto continue29;
	f[1][1] = 0.0;
	f[1][2] = a1;
	f[1][3] = 0.0;
	f[1][4] = b1;
	f[1][5] = c1;
	goto continue30;

continue29:
	f[1][1] = 1.0;
	f[1][2] = -2*a1;
	f[1][3] = 1.0;
	f[1][4] = -2*b1;
	f[1][5] = c1 - a1*a1 - b1*b1;

continue30:
	if (bFit2.dSi[0] == CIRCLE) goto continue31;
	f[2][1] = 0.0;
	f[2][2] = a2;
	f[2][3] = 0.0;
	f[2][4] = b2;
	f[2][5] = c2;
	goto continue32;

continue31:
	f[2][1] = 1.0;
	f[2][2] = -2*a2;
	f[2][3] = 1.0;
	f[2][4] = -2*b2;
	f[2][5] = c2 - a2*a2 - b2*b2;

continue32:
	if (bFit3.dSi[0] == CIRCLE) goto continue33;
	f[3][1] = 0.0;
	f[3][2] = a3;
	f[3][3] = 0.0;
	f[3][4] = b3;
	f[3][5] = c3;
	goto continue34;

continue33:
	f[3][1] = 1.0;
	f[3][2] = -2*a3;
	f[3][3] = 1.0;
	f[3][4] = -2*b3;
	f[3][5] = c3 - a3*a3 - b3*b3;

continue34:
	dz21 = _wallPt[0][k].z - _wallPt[0][k-1].z;
	dz31 = _wallPt[0][k+1].z - _wallPt[0][k-1].z;
	dz32 = _wallPt[0][k+1].z - _wallPt[0][k].z;
	z12 =  _wallPt[0][k-1].z + _wallPt[0][k].z;

	for (i = 1; i<= 5; i++)
	{
		fb[i] = (f[2][i] - f[1][i])/dz21;
		fc[i] = ((f[3][i] - f[1][i])/dz31 - fb[i])/dz32;
		fb[i] += -fc[i]*z12;
		fa[i] = f[1][i] - (fb[i] + fc[i]*_wallPt[0][k-1].z)*_wallPt[0][k-1].z;
	}
	n2 = 2.*(fa[1] + (fb[1] + fc[1]*P.z)*P.z)*P.x + fa[2] + (fb[2] + fc[2]*P.z)*P.z;
	n3 = 2.*(fa[3] + (fb[3] + fc[3]*P.z)*P.z)*P.y + fa[4] + (fb[4] + fc[4]*P.z)*P.z;
	n1 = -(fb[1] + 2.*fc[1]*P.z)*P.x*P.x - (fb[2] + 2.*fc[2]*P.z)*P.x
		 -(fb[3] + 2.*fc[3]*P.z)*P.y*P.y - (fb[4] + 2.*fc[4]*P.z)*P.y
		 +(fb[5] + 2.*fc[5]*P.z);

	ax = fa[1] + (fb[1] + fc[1]*P.z)*P.z;
	if (ax == 0.0) ax = -2.0;
	ax = -2*(fa[2] + (fb[2] + fc[2]*P.z*P.z))/ax;
	by = fa[3] + (fb[3] + fc[3]*P.z)*P.z;
	if (by == 0.0) by = -2;
	by = -2*(fa[4] + (fb[4] + fc[3]*P.z)*P.z) / by;
	
	if ( P.x - ax < 0) n2 = fabs(n2);
	else n2 = -fabs(n2);
	if ( P.y - by < 0) n3 = fabs(n3);
	else n3 = -fabs(n3);
	goto continue26;

}
	
//**************************************************************************************
//**************************************************************************************
//**************************************************************************************
dummyStruct C3D_MOCGrid::CompatabilityEquationSolverForFieldPoint( const XYZPoint *basePt, 
					const double *dtdN, const double *dpdN, const double *dzdN, const double *theta,
					const double *beta, const double *delta, const double dz)
{
	//	This function uses the LU Decomposition algorithm to find P2, THETA2 and PSI2.
	//	given the three base points whose indicies are pt[0], pt[1], and pt[2] and
	//	the average theta, beta and delta quanities
	//	The LU decomposition routine is from the Numberical recipes in C++ book.
	//	This solution finds the solution to A.x = b
	
	//	Declare a Matrix A 3x3 rows,colums
	int PTN[4][3] = {0,1,2,1,2,3,2,3,0,3,0,1};
	const int n = 3;
	int i,j,k;
	DP d;
	Vec_INT indx(n);
	Vec_DP b(n);
    Mat_DP a(n,n),x(n,n);
	dummyStruct CU,X; // X is the pressure, theta and psi results

	for ( i = 0; i <= 2; i++) X.dSx[i] = 0.0;

	// Debug file output
  /*   fstream ofile;
	ofile.open("Field Coeff.out", ios::out);
	ofile << "I\tPT\tX\tY\tZ\tPres\ta0\ta1\ta2\tRHS\tdtdN\tdpdN\tdzdN\ttheta\tbeta\tdelta\t"
		"Psi(psi)\tRHO(lbm/ft3)\tVel(ft/s)\tDL\tdz" << endl;  */

	//	Step through each set of points
	for ( j = 0; j < 4; j++)
	{
		// For each point (i) in the set of points (j)
		//	Put the coefficients into the a matrix, each row is a different point
		for ( i = 0; i < n ;i++)
		{
			k = PTN[j][i];
			CU = CompEqu( beta[k], basePt[k], delta[k], dzdN[k], dz, theta[k], dtdN[k], dpdN[k]);
			//	a[i][0] is to in unit of (in2/lbf) to work correctly
			a[i][0] = CU.dSx[0]; 
			a[i][1] = CU.dSx[1];
			a[i][2] = CU.dSx[2];
			b[i] = CU.dSx[3];
	/*		ofile << i << "\t" << pt[i] << "\t" << basePt[k].x << "\t" << basePt[k].y << "\t" 
				<< basePt[k].z << "\t" << basePt[k].p << "\t" << a[i][0] << "\t" << a[i][1] << "\t" 
				<< a[i][2] << "\t" << b[i] << "\t" << dtdN[k] << "\t" << dpdN[k] << "\t"
				<< dzdN[k] << "\t" << theta[k] << "\t" << beta[k] << "\t" << delta[k] << "\t" 
				<< basePt[k].psi << "\t" << basePt[k].rho << "\t" << basePt[k].q << "\t"
				<< basePt[k].L << "\t" << dz <<  endl;*/
		}

		// Perform the LU decomposition
		NR::ludcmp(a,indx,d);
		// Perform the Back Substitution
		NR::lubksb(a,indx,b);
    
		//	The matrix B contains the answer. Set the be vector to X 
   			
		//	ofile << "\n\nP2\tTHETA2\tPSI2" << endl;
		for ( i = 0; i <=2; i++)
		{
			X.dSx[i] += b[i];
			//ofile << b[i] << "\t";
		}
		//	ofile.close();
		if (b[0] <= 0.) AfxMessageBox("Field Compatibility Equation yielded a negative pressure");
	} // end of j loop
	for ( i = 0; i <=2; i++) X.dSx[i] /= 4.0;
	return X;
}

//**************************************************************************************
//**************************************************************************************
//**************************************************************************************
dummyStruct C3D_MOCGrid::CompatabilityEquationSolverForBodyPoint( const XYZPoint *basePt, 
			const double *dtdN, const double *dpdN, const double *dzdN, const double *theta, 
			const double *beta, const double *delta, const double dz, const XYZPoint P2, 
			const dummyStruct unitNormal)
{
	//	The compatability equation along two bicharacteristics are solved simultaneously
	//	with the tangency flow conditions, to get the 3 unknowns, P2, THETA2 and PSI2.
	//	As declared below, x(n) are the 3 three variables to be found
	//	fvec(n) contains the functions values at x for the 2 characteristics and tangency
	//	function
	
	//	Inputs: 3 base points are sent as well as the related parameters
	//	unitNormal is the unit Normal vector at P2
	//	unitNormal2 contains the unit normals of the body at P2.
	//	P2 contains the current data at P2 (the eventual point to be found)
	dummyStruct X,CU;
	//	As a note, I tried varying all of the points and the results are the same.
	//	I even did all 2 points and the results are the same.
	const int n=2, pt[3][2] = {0,1,0,2,1,2};
//	double al[3][3],cr[3],am[3][3],n1,n2,n3,a1,b1,c1;
    int i,j,k;
	bool check;
    Vec_DP x(3),f(3); // the equations, and 3 unknowns
	//	The other way to do this is the following
	//	Using 2 Base points at a time, solve for the compatability equations
	//	Debug output file
/*	fstream ofile;
	ofile.open("Body Coeff.out", ios::out);
	// Debug file output
	ofile << "P2.X\t" << P2.x << "\nP2.Y\t" << P2.y << "\nP2.Z\t" << P2.z;*/
	for ( i = 0; i <=2; i++) X.dSx[i] = 0.0;
	_a[2][0] = unitNormal.dSx[0];
	_a[2][1] = unitNormal.dSx[1];
	_a[2][2] = unitNormal.dSx[2];
	for ( j = 0; j < n; j++)
	{
	/*	ofile << "\n\n\nJ\tI\tPT\tX\tY\tZ\tPres\tMach\tTheta\tPsi\tRho\tVel\tdL\tDelta\t"
			<< "a0\ta1\ta2\tRHS\tdtdN\tdpdN\tdzdN\tThetaAvg\tBetaAvg\tdeltaAvg\tdz" << endl; */
		x[0] = P2.p;
		x[1] = P2.theta;
		x[2] = P2.psi;
		for ( i = 0; i <= 1; i++)
		{
			k = pt[j][i];
			CU = CompEqu( beta[k], basePt[k], delta[k], dzdN[k], dz, theta[k], dtdN[k], dpdN[k]);
			_a[i][0] = CU.dSx[0]; 
			_a[i][1] = CU.dSx[1]; 
			_a[i][2] = CU.dSx[2]; 
			_a[i][3] = CU.dSx[3]; // RHS of the compatability equation
			//	Output stuff
		/*	ofile << j	<< "\t"<< i << "\t" << k << "\t" << basePt[k].x << "\t" << basePt[k].y 
				<< "\t" << basePt[k].z << "\t" << basePt[k].p << "\t" << basePt[k].mach 
				<< "\t" << basePt[k].theta << "\t" << basePt[k].psi << "\t" << basePt[k].rho 
				<< "\t" << basePt[k].q << "\t" << basePt[k].L << "\t" << basePt[k].delta
				<< "\t" << _a[i][0] << "\t" << _a[i][1]
				<< "\t" << _a[i][2] << "\t" << _a[i][3] << "\t" << dtdN[k] << "\t" << dpdN[k] 
				<< "\t" << dzdN[k] << "\t" << theta[k] << "\t" << beta[k] << "\t" << delta[k]
				<< "\t" << dz <<  endl;*/
		}
		
		//	funcV is a global function call that puts the Compatability equation and 
		//	the flow tangency equation into an array. That array is solved for
		//	using the Newton-Rhapson Method. It should be noted that Pressure is not 
		//	in the flow tangency equation.
		NR::newt(x,check,funcv);
		if (check) 
		{	
			AfxMessageBox("Could not converge on Body Point Compatibility Equation");
			exit(1);
		}

		for ( i = 0; i <= 2; i++) X.dSx[i] += x[i];
	/*	ofile << "\nNormal Coefficients\nN1\tN2\tN3\n" << _a[2][0] << "\t" << _a[2][1]
			<< "\t" << _a[2][2] << endl;
	*/	
		//	More output
	/*	ofile << "\nP2\tTheta2\tPSI2\n";
		for ( i = 0; i<=2; i++) ofile << x[i] << "\t"; 
		ofile << "\n\nF1\tF2\tF3\tTangency <= should all equal 0.0\n";
		funcv(x,f);
		for ( i = 0; i<=2; i++) ofile << f[i] << "\t"; 
		f[0] = _a[2][0]*cos(x[2])+_a[2][1]*tan(x[1]) + _a[2][2]*sin(x[2]);
		ofile << f[0];*/
		//	End of more output

	}	// end of j loop
	
	//	Average the 3 points
	for ( i = 0; i <=2; i++) X.dSx[i] /= double(n);

	//	This is a check of the average
/*	x[0] = X.dSx[0];
	x[1] = X.dSx[1];
	x[2] = X.dSx[2];

//	More output for average solutions
	ofile << "\n\n\nNormal Coefficients\nN1\tN2\tN3\n" << _a[2][0] << "\t" << _a[2][1]
			<< "\t" << _a[2][2] << endl;
	ofile << "\nAveraged values\nP2\tTheta2\tPSI2\n";
	for ( i = 0; i<=2; i++) ofile << X.dSx[i] << "\t"; 
	ofile << "\n\nSolutions for avergaged values\nF1\tF2\tF3\tTangency\t <= should all equal 0.0\n";
	funcv(x,f);
	for ( i = 0; i<=2; i++) ofile << f[i] << "\t"; 
	//	solve for tangency relation
	f[0] = _a[2][0]*cos(x[2])+_a[2][1]*tan(x[1]) + _a[2][2]*sin(x[2]);
	ofile << f[0];	
	ofile.close();*/

	return X;
}

//*************************************************************************************
//**************************************************************************************
//**************************************************************************************
int C3D_MOCGrid::SetInitialReferencePlane(const double pres, const double temp, 
		const double mach, const double mWt, const double gamma, const double theta, 
		const double psi, const double z0, const double r0, const int nDiv)
{
	//	This function will set the initial plane.
	//	pres, tenp, mach, mWt, gamma, theta, psi are all state values
	//	z0 is the axial location of the plane
	//	r0 is radius of the circle that defines the entrance plane with center x=y=0
	//	nDiv, is the number of radial divisions along the wall contour
	//	For now, use the following steps to determine the field points
	//	There will always be a point at 0,0
	//	The average distance between the wall points is: dis = 2*PI*r0/(nDiv-1)
	//	The number of the radii is r0/dis + 2
	//	For each radii set the X,Y,Z and thermo properties.
	double dis, rad, radStep;
	int i,j,k, nPt;
	

	dis = 2*PI*r0/nDiv; // Calculates the average distance between wall points

	_nRadii = int (r0/dis) + 2; // This is how many radial positions there will be. They
	//	will range from 0 to _nRadii-1
	//	Calculate the total number of points to be initialized
	radStep = r0/_nRadii;
	k = 0;
	for ( j = 0; j <= _nRadii; j++)
	{
		rad = j*radStep;
		nPt = int(2*PI*rad/radStep);
		if (nPt < 7 && rad > 0.0) nPt = 7;
		for ( i = 0; i <= nPt; i++) k++;
	}
	_nPTS = k;
	InitializeDataMembers();

	//	To be used in TECPLOT correctly, the THETA = 0 ray has to be output at the end
	fstream ofile;
	ofile.open("z=0.out", ios::out);
	ofile << "X(in)\tY(in)\tZ(in)\tRadius(in)\tP(psia)\tT(R)\tRHO(lbm/ft3)\tMach\t" 
		<< "Velocity(ft/s)\tTheta(deg)\tPsi(deg)\tRadii#\tPt#\tTotal Pt#\tBodyFlag" << endl;
	
	//	Step through each _nRadii until the entire nozzle is done
	radStep = r0/_nRadii;
	k = 0;
	for ( j = 0; j <= _nRadii; j++)
	{
		rad = j*radStep;
		nPt = int(2*PI*rad/radStep);
		if (nPt < 7 && rad > 0.0) nPt = 7;
		for ( i = 0; i <= nPt; i++)
		{
			_pt[k][0].x = rad*cos(2*PI*i/(nPt+1));
			_pt[k][0].y = rad*sin(2*PI*i/(nPt+1));
			_pt[k][0].z = z0;
			_pt[k][0].p = pres;
			_pt[k][0].g = gamma;
			_pt[k][0].mach = mach;
			_pt[k][0].molWt = mWt;
			_pt[k][0].t = temp;
			_pt[k][0].rho = pres*144/(GASCON/mWt*temp); // (lbm/ft3)
			_pt[k][0].theta = theta*RAD_PER_DEG;	// dx/dz
			_pt[k][0].psi = psi*RAD_PER_DEG;		//dy/dz
			_pt[k][0].q = mach*sqrt(gamma*GASCON/mWt*temp*GRAV); // velocity
			if ( j == _nRadii) _bodyPointFlag[k] = TRUE;
			else _bodyPointFlag[k] = FALSE;
			ofile << _pt[k][0].x << "\t" << _pt[k][0].y << "\t" << _pt[k][0].z << "\t" << rad
			 << "\t" << _pt[k][0].p << "\t" << _pt[k][0].t << "\t" << _pt[k][0].rho
			 << "\t" <<_pt[k][0].mach << "\t" << _pt[k][0].q 
			 << "\t" << _pt[k][0].theta*DEG_PER_RAD << "\t" << _pt[k][0].psi*DEG_PER_RAD
			 << "\t" << j << "\t" << i << "\t" << k << "\t" << _bodyPointFlag[k] << endl;
			k++;
		}
	}
	ofile.close();
	return 1;	
}

//**************************************************************************************
//**************************************************************************************
//**************************************************************************************
dummyStruct C3D_MOCGrid::CalcSurfacePointProperties(XYZPoint* basePt, int i, int k)
{
	//	This function is used to determine the properties at basePt. The X,Y,and Z location
	//	of the base point has already been set. Use the Surface fit coefficents to 
	//	solve for the properties. It also calculates the derivative of the surface at the basePt;
	//	dtdx: deriv[3]
	//	dtdy: deriv[6]
	//	dpdx: deriv[4]
	//	dpdy: deriv[8]
	//	Given:
	//		a known point basePt
	//		a reference plane point _pt[i][k]
	//		8 neighboring points of pt[i][k] _neighbor[i][0-7]
	//		coefficients of the surface spline  _cFit[0-11][D],
	//	[i] = 0 - 11: coefficient number
	//	[D] = parameter
	//		[0] = pressure
	//		[1] = density
	//		[2] = velocity
	//		[3] = theta
	//		[4] = psi
	//		[5] = gamma
	//		[6] = molWtD specifies the parameter
	//	The Dx is in the form of a1 + 2*SUM(Bi*(1+ln(R^2)*(x-xi)
	//	The Dy is in the form of a2 + 2*SUM(Bi*(1+ln(R^2)*(y-yi)

	dummyStruct deriv;
	int ival,jj;
	double dx,dy,rad,fac,val[7];

	if ( k == 129 && i == 0)
	{
		i = i;
	}

	//	Step through each of the values to be calculated. using the _cFit coefficients.
	//	First set the higher order terms
	for ( ival = 0; ival <= 6; ival++)
	{
		val[ival] = _cFit[0][ival] + _cFit[1][ival]*basePt->x + _cFit[2][ival]*basePt->y;
	}
	
	//	There are 2 ways to calculate this based on the spline fit used
	if (sFit == "9 Point Spline")
	{
		//	Set the 2nd order terms here. The initial fit was set based on actual plane points.
		//	This new calculation assumes a basePt. There may be a chance the the BasePt equals
		//	a 'neighbor' point yielding a r = 0.0. If this is the case, then skip the point
		//	Step through the 9 neighbor of _pt[i][k]. _neighbor[0] is point [i][k]
		for ( jj = 0; jj <= _nNeighbors; jj++)
		{
			dx = basePt->x - _neighbor[i][jj].x;
			dy = basePt->y - _neighbor[i][jj].y;
			rad = dx*dx + dy*dy;
			if ( rad > 0.0 && _finite(rad))
			{
				for (ival = 0; ival <= 6; ival++) val[ival] += _cFit[jj+3][ival]*rad*log(rad);
			}
		}

		// Now, set the base point properties
		basePt->p = val[0];
		basePt->rho = val[1];
		basePt->q = val[2];
		basePt->theta = val[3];
		basePt->psi = val[4];
		basePt->g = val[5];
		basePt->molWt = val[6];
		if (fabs(basePt->theta) < 1e-8) basePt->theta = 0.0;
		if (fabs(basePt->psi) < 1e-8) basePt->psi = 0.0;

		//	Solve for the other properties
		basePt->t = 144*basePt->p/(basePt->rho*GASCON/basePt->molWt);
		basePt->mach = basePt->q/sqrt((GRAV*basePt->g*GASCON/basePt->molWt*basePt->t));

		
		//	Now calculate the derivatives of the basePt properties using the _cFit
		//	coefficients. 
		//	Start with the 1st order terms
		deriv.dSx[3] = _cFit[1][3];	// dtdx
		deriv.dSx[4] = _cFit[1][4]; // dpdx
		deriv.dSx[6] = _cFit[2][3]; // dtdy
		deriv.dSx[8] = _cFit[2][4]; // dpdy

		// Now step thru all of the neighbor points
		for ( jj = 0; jj <= _nNeighbors; jj++)
		{
			dx = basePt->x - _neighbor[i][jj].x;
			dy = basePt->y - _neighbor[i][jj].y;
			rad = dx*dx + dy*dy;
			if ( rad > 0.0 && _finite(rad))
			{
				fac = 2*(1+log(rad));
				deriv.dSx[3] += _cFit[jj+3][3]*fac*dx; // dtdx
				deriv.dSx[6] += _cFit[jj+3][3]*fac*dy; // dtdy
				deriv.dSx[4] += _cFit[jj+3][4]*fac*dx; // dpdx
				deriv.dSx[8] += _cFit[jj+3][4]*fac*dy; // dpdy	
			}
		}
	}
	else	// All Point fit
	{
		for ( jj = 0; jj < _nPTS; jj++)
		{
			dx = basePt->x - _pt[jj][k].x;
			dy = basePt->y - _pt[jj][k].y;
			rad = dx*dx + dy*dy;
			if ( rad > 0.0 && _finite(rad))
			{
				for (ival = 0; ival <= 6; ival++) val[ival] += _cFit[jj+3][ival]*rad*log(rad);
			}
		}
		// Now, set the base point properties
		basePt->p = val[0];
		basePt->rho = val[1];
		basePt->q = val[2];
		basePt->theta = val[3];
		basePt->psi = val[4];
		basePt->g = val[5];
		basePt->molWt = val[6];
		if (fabs(basePt->theta) < 1e-8) basePt->theta = 0.0;
		if (fabs(basePt->psi) < 1e-8) basePt->psi = 0.0;

		//	Solve for the other properties
		basePt->t = 144*basePt->p/(basePt->rho*GASCON/basePt->molWt);
		basePt->mach = basePt->q/sqrt((GRAV*basePt->g*GASCON/basePt->molWt*basePt->t));

		
		//	Now calculate the derivatives of the basePt properties using the _cFit
		//	coefficients. 
		//	Start with the 1st order terms
		deriv.dSx[3] = _cFit[1][3];	// dtdx
		deriv.dSx[4] = _cFit[1][4]; // dpdx
		deriv.dSx[6] = _cFit[2][3]; // dtdy
		deriv.dSx[8] = _cFit[2][4]; // dpdy

		// Now step thru all of the points
		for ( jj = 0; jj < _nPTS; jj++)
		{
			dx = basePt->x - _pt[jj][k].x;
			dy = basePt->y - _pt[jj][k].y;
			rad = dx*dx + dy*dy;
			if ( rad > 0.0 && _finite(rad))
			{
				fac = 2*(1+log(rad));
				deriv.dSx[3] += _cFit[jj+3][3]*fac*dx; // dtdx
				deriv.dSx[6] += _cFit[jj+3][3]*fac*dy; // dtdy
				deriv.dSx[4] += _cFit[jj+3][4]*fac*dx; // dpdx
				deriv.dSx[8] += _cFit[jj+3][4]*fac*dy; // dpdy	
			}
		}
	}


	return deriv;
}

//**************************************************************************************
//**************************************************************************************
//**************************************************************************************
void C3D_MOCGrid::InitializeWallPoints( void )
{	
	//	This function initializes the wallPt array
	//	Initialize Wall point
	//  _nI is the square of the maximum number of wall points
	int i;
	_wallPt = new XYZPoint*[_nDiv];
	for (i = 0; i < _nDiv; i++) _wallPt[i] = new XYZPoint[_nZ*5];
	return;
}

void C3D_MOCGrid::InitializeDataMembers(void)
{
	//	Initialize the neighboring point array. For each point nI, there are 8 neighboring
	//	points that will be found.  Initialize 9 points so that the initial point can
	//	also be included in the array. Also initialize the field point array
	int i;
	_neighbor = new XYZPoint*[_nPTS];
	_pt = new XYZPoint*[_nPTS];
	_bodyPointFlag = new bool[_nPTS];
	for (i = 0; i < _nPTS; i++)
	{
		_neighbor[i] = new XYZPoint[9];
		_pt[i] = new XYZPoint[_nZ*5];
	}
	
	return;
}


//**************************************************************************************
//**************************************************************************************
//**************************************************************************************
void C3D_MOCGrid::DeleteDataMembers( void)
{
	//	This function initializes the arrays for the data members
	int i;

	for ( i = 0; i < _nDiv; i++) delete _wallPt[i];
	for ( i = 0; i < _nPTS; i++)
	{
		delete _pt[i];
		delete _neighbor[i];
	}

	delete []_bodyPointFlag;
	delete []_pt;
	delete []_neighbor;
	delete []_wallPt;
	
	return;
}

//****************************************************************************************
//****************************************************************************************
//****************************************************************************************
double C3D_MOCGrid::CalcMu(const double mach)
{
	//	Mach angle
	if ( mach < 1.0) 
	{
		if ( mach < .999)
		{
			AfxMessageBox("Could not calculate the Mach Angle for Mach < 1.0");
			exit(1);
		}
		else return PI/2.;
	}
	return asin(1./mach);
}

//****************************************************************************************
//****************************************************************************************
//****************************************************************************************
double C3D_MOCGrid::lDyDx( const double theta, const double mu)
{
	//	characteristic angle for LRC
	return tan(theta+mu);
}

//****************************************************************************************
//****************************************************************************************
//****************************************************************************************
double C3D_MOCGrid::rDyDx( const double theta, const double mu)
{
	//	characteristic angle for RRC
	return tan(theta-mu);
}

//****************************************************************************************
//****************************************************************************************
//****************************************************************************************
double C3D_MOCGrid::TanAvg( const double x, const double y)
{
	//	Tanget averaging function.  x and y are tangents of two angles
	return tan(0.5 * (atan(x)+atan(y)));
}

//****************************************************************************************
//****************************************************************************************
//****************************************************************************************
double C3D_MOCGrid::MM(const double mach)
{
	return sqrt(mach*mach-1);
}

/*
//****************************************************************************************
//****************************************************************************************
//****************************************************************************************
dummyStruct C3D_MOCGrid::UnitNormalVector(const RTZPoint P3, const RTZPoint P1, const RTZPoint P4)
{
	//	Calculate the unit normal vector w/rt the 3 points.  The vectors created are
	//	P3P1 and P4P1;

	XYZPoint P31, P41;
	double mag;
	dummyStruct unitNormal;

	P31.x = P3.r*sin(P3.Theta) - P1.r*sin(P1.Theta);
	P31.y = P3.r*cos(P3.Theta) - P1.r*cos(P1.Theta);
	P31.z = P3.z - P1.z;

	P41.x = P4.r*sin(P4.Theta) - P1.r*sin(P1.Theta);
	P41.y = P4.r*cos(P4.Theta) - P1.r*cos(P1.Theta);
	P41.z = P4.z - P1.z;

	// Take the cross product
	unitNormal.dSx[0] = P31.y*P41.z - P31.z*P41.y;
	unitNormal.dSx[1] = -(P31.x*P41.z - P31.z*P41.x);
	unitNormal.dSx[2] = P31.x*P41.y - P31.y*P41.x;

	mag = sqrt(unitNormal.dSx[0]*unitNormal.dSx[0] + unitNormal.dSx[1]*unitNormal.dSx[1]+
		unitNormal.dSx[2]*unitNormal.dSx[2]);

	//	Normalize the vector
	for ( int i = 0; i <=2; i++) unitNormal.dSx[i] *= mag;

	return unitNormal;
}
*/
//****************************************************************************************
//****************************************************************************************
//****************************************************************************************
dummyStruct C3D_MOCGrid::BodyFit( XYZPoint X)
{
	//	Given the point X, find the body Fit coeffiecients using a body point that is closest
	//	to point X
	dummyStruct bFit, JK;
	//	Find the closest body point to point X.
	JK = FindClosestBodyPoint(X);
	bFit = BodyFit(JK.dSi[0], JK.dSi[1]);
	return bFit;
}
	
//****************************************************************************************
//****************************************************************************************
//****************************************************************************************
dummyStruct C3D_MOCGrid::BodyFit( int j, int k)
{
	//	Find the best equation fit along the body, starting a _wallPt[j][k]	
	//	This functions returns coefficients defining the body based on 
	//	several fits
	//	typeFlag =  CONSTANT_X, X = C
	//				CONSTANT_Y, Y = C
	//				LINE: AX + BY = C
	//				CIRCLE: (X-A)^2 + (Y-B)^2 = C
	//	the point to solve for is between j-1 and j
	//	use three points j-1,j and j+1 to determine what the contour looks like
	//	k is the plane to work in
	dummyStruct DS;
	double a,b,c, x[3], y[3], rad[3], dx10, dy10, dx21, dy21, h;
	int jM1, jP1;

	jM1 = j-1;
	jP1 = j+1;
	//	The closest point to X is defined by J
	if ( jM1 == -1) jM1 = _nDiv - 1;
	if ( jP1 == _nDiv) jP1 = 1;	
	
	x[0] = _wallPt[jM1][k].x;
	x[1] = _wallPt[j][k].x;
	x[2] = _wallPt[jP1][k].x;
	y[0] = _wallPt[jM1][k].y;
	y[1] = _wallPt[j][k].y;
	y[2] = _wallPt[jP1][k].y;

	if ( x[0] == x[1] && x[0] == x[2])
	{
		//	Constant X
		DS.dSx[0] = 1.0;
		DS.dSx[1] = 0.0;
		DS.dSx[2] = x[1];
		DS.dSi[0] = CONSTANT_X;
	}
	else if ( y[0] == y[1] && y[0] == y[2])
	{
		//	Constant Y
		DS.dSx[0] = 0.0;
		DS.dSx[1] = 1.0;
		DS.dSx[2] = y[1];
		DS.dSi[0] = CONSTANT_Y;
	}
	else
	{
		//	Either a Line or a circle
		rad[0] = x[0]*x[0] + y[0]*y[0];
		rad[1] = x[1]*x[1] + y[1]*y[1];
		rad[2] = x[2]*x[2] + y[2]*y[2];
		dx10 = 2.*(x[1] - x[0]);
		dy10 = 2.*(y[1] - y[0]);
		dx21 = 2.*(x[2] - x[1]);
		dy21 = 2.*(y[2] - y[1]);

		if ( dx10 == 0.0)
		{
			dx10 = 2.*(x[2] - x[0]);
			dx21 = -dx21;
			dy10 = 2.*(y[2] - y[0]);
			dy21 = -dy21;
			h = rad[1];
			rad[1] = rad[2];
			rad[2] = h;
		}	
		
		if (dx21 != 0.0 && dy10/dx10 == dy21/dx21)
		{
			// LINE
			a = -dy10/dx10;
			DS.dSx[0] = a;
			DS.dSx[1] = 1.0;
			DS.dSx[2] = y[0] + x[0]*a;
			DS.dSi[0] = LINE;
		}
		else
		{
			a = (rad[1] - rad[0])/dx10;
			b = rad[2] - rad[1];
			c = dy10/dx10;
			h = dy21;
			if ( dx21 != 0.0)
			{
				b = b/dx21 - a;
				h = h/dx21 - c;
			}
			
			b /= h;
			a -= b*c;
			c = rad[0] - 2*(a*x[0]-b*y[0]);
			c += b*b + a*a;
			DS.dSx[0] = a;
			DS.dSx[1] = b;
			DS.dSx[2] = c;
			DS.dSi[0] = CIRCLE;
		}
	}

	if (fabs(DS.dSx[0]) < 1e-10) DS.dSx[0] = 0.0;
	if (fabs(DS.dSx[1]) < 1e-10) DS.dSx[1] = 0.0;
	return DS;
}		

//****************************************************************************************
//****************************************************************************************
//****************************************************************************************
dummyStruct C3D_MOCGrid::FindClosestBodyPoint( XYZPoint X)
{
	//	This function will find the closest 	
	int j, jMin,k;
	double minD, dMin;
	dummyStruct DS;

	k = 0;
	minD = 9e9;
	//	All of the wall points are at the same Z location for a given plane.
	//	Step through each Z location until one is found that is the closest to X
	while  (k < _nZ && _wallPt[0][k].z < X.z) k++; 
	//	At this point, k is >= X.z.  k-1 is less then X.z. Find the closest
	if ( _wallPt[0][k].z - X.z > X.z - _wallPt[0][k-1].z && k > 0) k--; 

	for ( j = 0; j < _nDiv; j++)
	{
		dMin = pow(X.x - _wallPt[j][k].x,2.) + pow(X.y - _wallPt[j][k].y,2.);
		if (dMin < minD)
		{
			jMin = j;
			minD = dMin;
		}
	}

	DS.dSi[0] = jMin;
	DS.dSi[1] = k;
	return DS;
}


//****************************************************************************************
//****************************************************************************************
//****************************************************************************************
dummyStruct C3D_MOCGrid::SolveForBodyPointPosition( dummyStruct CC, dummyStruct bFit,
												   const double P2x, const double P2y)
{
	//	This functions takes the body fit coefficients (bFit) at P2x,P2y and the
	//	coefficients from eq 40 (CC) and solves for X,Y of P2.

	double x,y,d1,d2,d3,rad;
	dummyStruct DS;

	const double a = CC.dSx[0];
	const double b = CC.dSx[1];
	const double c = CC.dSx[2];
	const double a1 = bFit.dSx[0];
	const double b1 = bFit.dSx[1];
	const double c1 = bFit.dSx[2];

	//	There are various solutions depending on the type of body fit

	switch(bFit.dSi[0])
	{
		case CONSTANT_X:
		{
			x = c1;
			y = (c - a*x)/b;
			break;
		}
		case CONSTANT_Y:
		{
			y = c1;
			x = (c - b*y)/a;
			break;
		}
		case LINE:
		{
			y = (a*c1 - c*a1)/(a*b1 - b*a1);
			x = (c - b*y)/a;
			break;
		}
		case CIRCLE:
		{
			if ( fabs(b) <= fabs(a))
			{
				d1 = 1 + (b/a)*(b/a);
				d2 = c/a - a1;
				d3 = (d2*d2 + b1*b1 - c1)/d1;
				d2 = (b*d2/a + b1)/d1;
				rad = d2*d2 - d3;
				if ( rad < 0.0)
				{
					AfxMessageBox("Error in SolveForBodyPointPosition(1)");
					exit(1);
				}
				else
				{
					rad = sqrt(rad);
					y = d2 + rad;
					if(fabs(P2y-y) >= fabs(P2y - d2 + rad)) y = d2 - rad;
					x = (c - b*y)/a;
				}
			}
			else
			{
				d1 = 1 + (a/b)*(a/b);
				d2 = c/b - b1;
				d3 = (d2*d2 + a1*a1 - c1)/d1;
				d2 = (a*d2/b + a1)/d1;
				rad = d2*d2 - d3;
				if (rad < 0.0)
				{
					AfxMessageBox("Error in SolveForBodyPointPosition(2)");
					exit(1);
				}
				else
				{
					rad = sqrt(rad);
					x = d2 + rad;
					if (fabs(P2x - x) > fabs(P2x - d2 + rad)) x = d2 - rad;
					y = (c - a*x)/b;
				}
			}
			break;
		}
	}	// End of switch

	if ( x > -1e-5 && x < 1e-5) x = 0.0;
    if ( y > -1e-5 && y < 1e-5) y = 0.0;
	DS.dSx[0] = x;
	DS.dSx[1] = y;

	return DS;
}

//****************************************************************************************
//****************************************************************************************
//****************************************************************************************
dummyStruct C3D_MOCGrid::CheckNewPointPosition( const XYZPoint P, const int jPoint, const int k)
{
	//	This function will check to see if the wall point _wallPt[j][k] is still the 
	//	closest point to the point P. This is called after the P2 has been updated, 
	//	and therefore it needs to be checked. If it is not the closest point, the 
	//	the closest point has to be found.

	dummyStruct JK;
	double dMin,dis;
	int j;

	j = jPoint;
	dMin = pow(P.x - _wallPt[j][k].x, 2.) + 
		   pow(P.y - _wallPt[j][k].y, 2.);
	
	j = jPoint-1;
	if( j == -1) j = _nDiv-2;
	dis = pow(P.x - _wallPt[j][k].x, 2.) + 
		  pow(P.y - _wallPt[j][k].y, 2.);
	
	if( dis < dMin) 
	{
		JK = FindClosestBodyPoint(P);
	}
	else
	{
		j = jPoint + 1;
		if( j == _nDiv) j = 1;
		dis = pow(P.x - _wallPt[j][k].x, 2.) + 
		      pow(P.y - _wallPt[j][k].y, 2.);
		if( dis < dMin) 
		{
			JK = FindClosestBodyPoint(P);
		}
		else
		{
			JK.dSi[0] = jPoint;
			JK.dSi[1] = k;
		}
	}
	return JK;
}

//****************************************************************************************
//****************************************************************************************
//****************************************************************************************
double C3D_MOCGrid::CalcParametricAngle( const double beta, const double theta, const double psi,
										const double delta)
{
	//	This functions calculates the parametric angle delta, for the given point
	//	and the old Delta angle
	double a,b,c,sind,cosd,asbs,rad,sini,sini1,sini2,cosi,cosi1,cosi2,deltaI;

	a = sin(theta)*cos(psi)*sin(beta);
	b = sin(psi)*sin(beta);
	c = -0.5*cos(theta)*cos(psi)*(sin(beta) + cos(beta));

	sind = sin(delta);
	cosd = cos(delta);
	deltaI = delta;
	asbs = a*a + b*b;
	rad = asbs - c*c;

	if (rad >= 0.0)
	{
		rad = sqrt(rad)/asbs;
		sini = b*c/asbs;
		sini1 = sini+a*rad;
		sini2 = sini - a*rad;
		if ( fabs(sind - sini2) < fabs(sind - sini1)) goto x21;
		sini = sini2;
		if( fabs(sini) <= 1.0) goto x22;
x21:	sini = sini2;
		if ( fabs(sini) > 1.0) sini = sini1;
		if ( fabs(sini) > 1.0) return deltaI;
x22:    cosi = a*c/asbs;
		cosi1 = cosi + b*rad;
		cosi2 = cosi - b*rad;
		
		if ( fabs(cosd - cosi2) < fabs(cosd - cosi1)) goto x23;
		cosi = cosi1;
		if(fabs(cosi) <= 1.0) goto x24;
x23:	cosi = cosi2;
		if( fabs(cosi) > 1.0) cosi = cosi1;
		if( fabs(cosi) > 1.0) return deltaI;
x24: 	deltaI = atan2(sini,cosi);
		if (deltaI < 0.0) deltaI += 2*PI;
	}
	return deltaI;
}

//****************************************************************************************
//****************************************************************************************
//****************************************************************************************
dummyStruct C3D_MOCGrid::CompEqu( const double beta, const XYZPoint basePt,
		const double delta, const double dzdN, const double dz, const double theta,
		const double dtdN, const double dpdN)
{
	//	This will calculate and return the compatibility equation coefficients
	dummyStruct a;
		
	//	a[0] has to be in units of (in2/lbf) to work correctly
	//	rho: lbm/ft3
	//	q: ft/s
	a.dSx[0] = 144./(tan(beta)*basePt.rho*basePt.q*basePt.q/GRAV);
	a.dSx[1] = cos(delta) - sin(beta)*sin(delta)*dzdN*basePt.L/dz;
	a.dSx[2] = cos(theta)*(sin(delta) + (sin(beta)*cos(delta)*dzdN*basePt.L)/dz);
	a.dSx[3] =	sin(beta)*basePt.L*(sin(delta)*dtdN - cos(theta)*cos(delta)*dpdN) + 
		a.dSx[0]*basePt.p + a.dSx[1]*basePt.theta + a.dSx[2]*basePt.psi;

 	return a;
}

//****************************************************************************************
//****************************************************************************************
//****************************************************************************************
void C3D_MOCGrid::SetNeighborPoints( int kSet, int kNow)
{
	//	This function will go through the reference plane (kSet) and find the 8 neighboring point
	//	for a given point. The eight points will be the closest 8 points
	//	to the given point based on distance. To ease later calcuations _neighbor[i][0]
	//	will be the point _pt[i][kSet].
	
	int i,j,kPt;
	
	Vec_IO_DP arr(_nPTS), indc(_nPTS);
	
/*	fstream ofile;
	if ( kSet == 0)
	{
		ofile.open("neighbor kSet = 0.out", ios::out);
		ofile << "I\tX\tY\tPT#" << endl;
	}*/

	//  Step through all of the points to calculate the distance between each point and
	//	every other point in the mesh
	for ( i = 0; i < _nPTS; i++)
	{
		//	For each point i, find the closest 8 points by stepping through each point in the
		//  mesh and calculating the distance
		for ( j = 0; j < _nPTS; j++)
		{
			//	arr is the distance array
			arr[j] = pow(_pt[i][kSet].x - _pt[j][kSet].x,2.) + pow(_pt[i][kSet].y - _pt[j][kSet].y,2.);
			//	The indicie is also calculated so that the real point can be related backSet
			indc[j] = j;
		}
		
		//	At this point an array of distances have been found. Sort them
		//	from lowest to highest.
		NR::sort2(arr,indc);
		
		//	for later use set the first point equal to the point that 
		//	being worked on. Set the remaining points equal to the 
		//	8 closest points
		for ( kPt = 0; kPt <= _nNeighbors; kPt++)
		{
			_neighbor[i][kPt] = _pt[int(indc[kPt])][kNow];
		}
		//	Output the neighboring points to a file
	/*	if ( kSet == 0)
		{
			for ( kPt = 0; kPt <= _nNeighbors; kPt++)
				ofile << i << "\t" << _neighbor[i][kPt].x << "\t" << _neighbor[i][kPt].y 
				<< "\t" << indc[kPt] << endl;
			ofile << endl;
		}*/
	}		
	
//	ofile.close();
	// At this point the nearest 8 points for each point has been found.
	return;
}

//****************************************************************************************
//****************************************************************************************
//****************************************************************************************
void C3D_MOCGrid::SurfaceFit( int i, int k)
{
	//	For the given point i,k find the spline fit of the surrounding surface
	//	using _pt[i][k] and 8 (N-1) neighboring points.
	//	The spline surface is fit by the following equation
	//	a0 + a1X + a2Y + SUM[i:0-8](bi * f(X,Y)) = W(X,Y)
	//	f(X,Y) = r^2* ln(r^2)
	//	r^2 = (X-xi)^2 + (Y-yi)^2
	//	For each equations X,Y is a fixed points ( 1 of the 9)
	//	xi,yi are all of the 9 (N) points
	//	W is either P, THETA, etc. for that point.
	//	This is 12 (N+3) unknowns
	//	Use the 9 (N) knowns points for the first 9 (N) equations. As was set up when calculating
	//	the neighboring point _neighbor[i][0] = _pt[i][k]
	//	The remaining 3 equations are SUM(bi) = SUM(xibi) = SUM(yibi) = 0.0
	//	Output _cFit[i][j] array
	//	[i] = 0 - 11: coefficient number
	//	[j] = parameter
	//		[0] = pressure
	//		[1] = density
	//		[2] = velocity
	//		[3] = theta
	//		[4] = psi
	//		[5] = gamma
	//		[6] = molWt

	
	int iRow, kPt, kk;
	//	Declare 2 matrices that will be used to solve for the 12 equations and 
	//	12 unknowns simultaneously
	Mat_DP a(_nNeighbors+4,_nNeighbors+4),aTmp(_nNeighbors+4,_nNeighbors+4);
	double r;

	for ( iRow = 0; iRow <= _nNeighbors; iRow++) // Step through the first 9 equations
	{
		a[iRow][0] = 1.0;
		//	The current point for X,Y _neighbor[i][iRow];

		//	Each solution will go into a separate column starting a iCol = 3	
		for ( kPt = 0; kPt <= _nNeighbors; kPt++) // Step through each column
		{
			r = pow(_neighbor[i][iRow].x - _neighbor[i][kPt].x, 2.) + 
				pow(_neighbor[i][iRow].y - _neighbor[i][kPt].y, 2.);
			if ( r != 0.0) a[iRow][kPt+3] = r*log(r);
			else a[iRow][kPt+3] = 0.0;
		}
		a[iRow][1] = _neighbor[i][iRow].x;
		a[iRow][2] = _neighbor[i][iRow].y;
		//	RHS of the equation
		_cFit[iRow][0] = _neighbor[i][iRow].p;
		_cFit[iRow][1] = _neighbor[i][iRow].rho;
		_cFit[iRow][2] = _neighbor[i][iRow].q;
		_cFit[iRow][3] = _neighbor[i][iRow].theta;
		_cFit[iRow][4] = _neighbor[i][iRow].psi;
		_cFit[iRow][5] = _neighbor[i][iRow].g;
		_cFit[iRow][6] = _neighbor[i][iRow].molWt;
	}	// End of Row loop
	//	The above takes care the first 9 equations.

	//	Start the last 3 rows, 	
	for ( iRow = _nNeighbors+1; iRow <= _nNeighbors+3; iRow++)
	{
		for (kPt = 0; kPt <=2; kPt++) a[iRow][kPt] = 0.0;
		//	All of the right hand side = 0.0
		_cFit[iRow][0] = 0.0;
		_cFit[iRow][1] = 0.0;
		_cFit[iRow][2] = 0.0;
		_cFit[iRow][3] = 0.0;
		_cFit[iRow][4] = 0.0;
		_cFit[iRow][5] = 0.0;
		_cFit[iRow][6] = 0.0;
	}

	//	Finish the last 3 equations
	for (kPt = 0; kPt <= _nNeighbors; kPt++)
	{
		a[_nNeighbors+3][kPt+3] = 1.0; // bi
		a[_nNeighbors+1][kPt+3] = _neighbor[i][kPt].y; 
		a[_nNeighbors+2][kPt+3] = _neighbor[i][kPt].x;
	}

	//	At this point there is a set of simultaneous equations
	//	Solve for them, one at a time
	//	Initialize vectors that are needed.
	DP d;
	Vec_INT indx(_nNeighbors+4);
	Vec_DP b(_nNeighbors+4);
    
	for ( kk = 0; kk <= 6; kk++)
	{
		for ( iRow = 0; iRow <= _nNeighbors+3; iRow++)
		{
			b[iRow] = _cFit[iRow][kk];
			for ( kPt = 0; kPt <= _nNeighbors+3; kPt++) aTmp[iRow][kPt] = a[iRow][kPt];
		}
		// Perform the LU decomposition
		NR::ludcmp(aTmp,indx,d);
		// Perform the Back Substitution
		NR::lubksb(aTmp,indx,b);
		for ( iRow = 0; iRow <= _nNeighbors+3; iRow++) _cFit[iRow][kk] = b[iRow];
	}

	//	Output Pressure coefficients
/*	ofile << endl << endl;
	for ( kk = 0; kk <= 6; kk++)
	{
		for ( iRow = 0; iRow <= _nNeighbors+3; iRow++) ofile << _cFit[iRow][kk] << "\t" ;
		ofile << endl;
	}
	ofile.close();*/

	//	The array _cFit contains the surface fit coefficients
	return;
}

//****************************************************************************************
//****************************************************************************************
//****************************************************************************************
void C3D_MOCGrid::AllPointSurfaceFit(int k)
{
	//	For the given plane, k, find the spline fit of the surrounding surface
	//	using _pt[0][k] (The center point) and all of the other points
	//	The spline surface is fit by the following equation
	//	a0 + a1X + a2Y + SUM[i:0 - (_nPTs-1)](bi * f(X,Y)) = W(X,Y)
	//	f(X,Y) = r^2* ln(r^2)
	//	r^2 = (X-xi)^2 + (Y-yi)^2
	//	For each equations X,Y is a fixed points ( 1 of all of the points)
	//	xi,yi are all of the (N) points
	//	W is either P, THETA, etc. for that point.
	//	This are (N+3) unknowns
	//	Use the N knowns points for the first N equations. As was set up when calculating
	//	The remaining 3 equations are SUM(bi) = SUM(xibi) = SUM(yibi) = 0.0
	//	Output _cFit[i][j] array
	//	[i] = 0 - (N-1): coefficient number
	//	[j] = parameter
	//		[0] = pressure
	//		[1] = density
	//		[2] = velocity
	//		[3] = theta
	//		[4] = psi
	//		[5] = gamma
	//		[6] = molWt

	double r;
	int iRow, kPt, kk;
	//	Declare 2 matrices that will be used to solve for the N+3 equations and 
	//	12 unknowns simultaneously
	Mat_DP a(_nPTS+3,_nPTS+3),aTmp(_nPTS+3,_nPTS+3);

	DP d;
	Vec_DP b(_nPTS+3);
	Vec_INT indx(_nPTS+3);
	

	//	iRow: corresponds to (x,y) in equation 9 
	for ( iRow = 0; iRow < _nPTS; iRow++) // Step through the first N equations
	{
		a[iRow][0] = 1.0;
		a[iRow][1] = _pt[iRow][k].x;
		a[iRow][2] = _pt[iRow][k].y;
			//	Each solution will go into a separate column starting a iCol = 3	
		for ( kPt = 0; kPt < _nPTS; kPt++) // Step through each column
		{
			r = pow(_pt[iRow][k].x - _pt[kPt][k].x, 2.) + pow(_pt[iRow][k].y - _pt[kPt][k].y, 2.);
			if ( r != 0.0) a[iRow][kPt+3] = r*log(r);
			else a[iRow][kPt+3] = 0.0;
		}
		
		//	RHS of the equation
		_cFit[iRow][0] = _pt[iRow][k].p;
		_cFit[iRow][1] = _pt[iRow][k].rho;
		_cFit[iRow][2] = _pt[iRow][k].q;
		_cFit[iRow][3] = _pt[iRow][k].theta;
		_cFit[iRow][4] = _pt[iRow][k].psi;
		_cFit[iRow][5] = _pt[iRow][k].g;
		_cFit[iRow][6] = _pt[iRow][k].molWt;
	}	// End of Row loop
	//	The above takes care the first N equations.

	//	Start the last 3 rows, 	
	for ( iRow = _nPTS; iRow < _nPTS+3; iRow++)
	{
		for (kPt = 0; kPt <=2; kPt++) a[iRow][kPt] = 0.0;
		//	All of the right hand side = 0.0
		_cFit[iRow][0] = 0.0;
		_cFit[iRow][1] = 0.0;
		_cFit[iRow][2] = 0.0;
		_cFit[iRow][3] = 0.0;
		_cFit[iRow][4] = 0.0;
		_cFit[iRow][5] = 0.0;
		_cFit[iRow][6] = 0.0;
	}

	//	Finish the last 3 equations
	for (kPt = 0; kPt < _nPTS; kPt++)
	{
		a[_nPTS][kPt+3] = _pt[kPt][k].y; 
		a[_nPTS+1][kPt+3] = _pt[kPt][k].x;
		a[_nPTS+2][kPt+3] = 1.0; // bi	
	}

	//	At this point there is a set of simultaneous equations
	//	Solve for them, one at a time
	
    
	for ( kk = 0; kk <= 6; kk++)
	{
		for ( iRow = 0; iRow < _nPTS+3; iRow++)
		{
			b[iRow] = _cFit[iRow][kk];
			for ( kPt = 0; kPt < _nPTS+3; kPt++) aTmp[iRow][kPt] = a[iRow][kPt];
		}
		// Perform the LU decomposition
		NR::ludcmp(aTmp,indx,d);
		// Perform the Back Substitution
		NR::lubksb(aTmp,indx,b);
		for ( iRow = 0; iRow < _nPTS+3; iRow++) _cFit[iRow][kk] = b[iRow];
	}

	//	Output Pressure coefficients
/*	fstream ofile;
	ofile.open("cfit.out", ios::out);
//	for ( kk = 0; kk <= 6; kk++)
//	{
		for ( iRow = 0; iRow < _nPTS+3; iRow++) ofile << _cFit[iRow][0] << "\t" ;
		ofile << endl;
//	}
	ofile.close();*/

	//	The array _cFit contains the surface fit coefficients
	return;
}

//**************************************************************************************
//**************************************************************************************
//**************************************************************************************
void C3D_MOCGrid::OutputAxialStations( int iStart, int iEnd, int kStart, int kEnd)
{
	//	This Function will output the nozzle contour in TECPLOT format
	int i,k;
	double rad,pt,tt,ratio;
	fstream ofile;
	ofile.open("axialStations.plt", ios::out);
	ofile << "VARIABLES = \"X(in)\",\"Y(in)\",\"Z(in)\",\"R(in)\",\"P(psia)\","
		<< "\"PT(psia)\",\"T(R)\",\"TT(R)\",\"RHO(lbm/ft3)\",\"Mach\"," 
		<< "\"Velocity(ft/s)\",\"Theta(deg)\",\"Psi(deg)\",\"I\",\"K\",\"BodyFlag\"" << endl
		<< "TITLE = \"Volume Mesh Plot\"" << endl
		<< "text  x=5  y=93  t=\"Characteristic Mesh Plot\" " << endl;

	//	Loop through each ray that defines the wall
	for ( k = kStart; k <= kEnd; k += _zOutputStep)
	{
		ofile << "zone t=\"Z = " << k << "\"" << " I = 1 J = 1 K = " 
		<< int ( (iEnd-iStart)/_xOutputStep) +1 << endl;

		for (i = iStart; i <= iEnd; i += _xOutputStep)
		{
			ratio = 1+(_pt[i][k].g-1.)/2.*_pt[i][k].mach*_pt[i][k].mach;
			tt = _pt[i][k].t*ratio;
			pt = _pt[i][k].p*pow(ratio,(_pt[i][k].g)/(_pt[i][k].g-1));
			rad = sqrt(_pt[i][k].x*_pt[i][k].x + _pt[i][k].y*_pt[i][k].y);
				ofile << _pt[i][k].x << "\t" << _pt[i][k].y << "\t" << _pt[i][k].z << "\t" << rad
				<< "\t" << _pt[i][k].p << "\t" << pt << "\t" << _pt[i][k].t 
				<< "\t" << tt << "\t" << _pt[i][k].rho 
				<< "\t" << _pt[i][k].mach  << "\t" << _pt[i][k].q << "\t"
				<< _pt[i][k].theta*DEG_PER_RAD << "\t" << _pt[i][k].psi*DEG_PER_RAD << "\t" 
				<< i << "\t" << k << "\t" << _bodyPointFlag[i] << endl;
		}
	}

	//	If the last plane of data was not printed, print it.
	if  (k > kEnd && k - _zOutputStep != kEnd)
	{
		k = kEnd;
		ofile << "zone t=\"Z = " << k << "\"" << " I = 1 J = 1 K = " 
		<< int ( (iEnd-iStart)/_xOutputStep) +1 << endl;

		for (i = iStart; i <= iEnd; i += _xOutputStep)
		{
			ratio = 1+(_pt[i][k].g-1.)/2.*_pt[i][k].mach*_pt[i][k].mach;
			tt = _pt[i][k].t*ratio;
			pt = _pt[i][k].p*pow(ratio,(_pt[i][k].g)/(_pt[i][k].g-1));
			rad = sqrt(_pt[i][k].x*_pt[i][k].x + _pt[i][k].y*_pt[i][k].y);
				ofile << _pt[i][k].x << "\t" << _pt[i][k].y << "\t" << _pt[i][k].z << "\t" << rad
				<< "\t" << _pt[i][k].p << "\t" << pt << "\t" << _pt[i][k].t 
				<< "\t" << tt << "\t" << _pt[i][k].rho 
				<< "\t" << _pt[i][k].mach  << "\t" << _pt[i][k].q << "\t"
				<< _pt[i][k].theta*DEG_PER_RAD << "\t" << _pt[i][k].psi*DEG_PER_RAD << "\t" 
				<< i << "\t" << k << "\t" << _bodyPointFlag[i] << endl;
		}
	}
	ofile.close();
	return;
}

//**************************************************************************************
//**************************************************************************************
//**************************************************************************************
void C3D_MOCGrid::OutputStreamlines( int iStart, int iEnd, int kStart, int kEnd)
{
	//	This Function will output the nozzle contour in TECPLOT format
	int i,k,n;
	double rad,pt,tt,ratio;
	fstream ofile;
	ofile.open("Streamlines.plt", ios::out);
	ofile << "VARIABLES = \"X(in)\",\"Y(in)\",\"Z(in)\",\"R(in)\",\"P(psia)\","
		<< "\"PT(psia)\",\"T(R)\",\"TT(R)\",\"RHO(lbm/ft3)\",\"Mach\"," 
		<< "\"Velocity(ft/s)\",\"Theta(deg)\",\"Psi(deg)\",\"I\",\"K\",\"BodyFlag\"" << endl
		<< "TITLE = \"Streamline Plot\"" << endl
		<< "text  x=5  y=93  t=\"Streamline Plot\" " << endl; 
		
  
	//	Loop through each streamline 
	for (i = iStart; i <= iEnd; i += _xOutputStep)
	{
		ofile << "zone t=\"I = " << i << "\"" << " I =" << (kEnd-kStart)/_zOutputStep+1 << " J = 1 K = 1" << endl;
		//	For each streamline, step through the axial steps
		for ( k = kStart; k <= kEnd; k += _zOutputStep)
		{
			ratio = 1+(_pt[i][k].g-1.)/2.*_pt[i][k].mach*_pt[i][k].mach;
				tt = _pt[i][k].t*ratio;
				pt = _pt[i][k].p*pow(ratio,(_pt[i][k].g)/(_pt[i][k].g-1));
			rad = sqrt(_pt[i][k].x*_pt[i][k].x + _pt[i][k].y*_pt[i][k].y);
				ofile << _pt[i][k].x << "\t" << _pt[i][k].y << "\t" << _pt[i][k].z << "\t" << rad
				<< "\t" << _pt[i][k].p << "\t" << pt << "\t" << _pt[i][k].t 
				<< "\t" << tt << "\t" << _pt[i][k].rho 
				<< "\t" << _pt[i][k].mach  << "\t" << _pt[i][k].q << "\t"
				<< _pt[i][k].theta*DEG_PER_RAD << "\t" << _pt[i][k].psi*DEG_PER_RAD << "\t" 
				<< i << "\t" << k << "\t" << _bodyPointFlag[i] << endl;
		}
		if  (k > kEnd && k - _zOutputStep != kEnd)
		{
			k = kEnd;
			ratio = 1+(_pt[i][k].g-1.)/2.*_pt[i][k].mach*_pt[i][k].mach;
				tt = _pt[i][k].t*ratio;
				pt = _pt[i][k].p*pow(ratio,(_pt[i][k].g)/(_pt[i][k].g-1));
			rad = sqrt(_pt[i][k].x*_pt[i][k].x + _pt[i][k].y*_pt[i][k].y);
				ofile << _pt[i][k].x << "\t" << _pt[i][k].y << "\t" << _pt[i][k].z << "\t" << rad
				<< "\t" << _pt[i][k].p << "\t" << pt << "\t" << _pt[i][k].t 
				<< "\t" << tt << "\t" << _pt[i][k].rho 
				<< "\t" << _pt[i][k].mach  << "\t" << _pt[i][k].q << "\t"
				<< _pt[i][k].theta*DEG_PER_RAD << "\t" << _pt[i][k].psi*DEG_PER_RAD << "\t" 
				<< i << "\t" << k << "\t" << _bodyPointFlag[i] << endl;
		}

	}
	ofile.close();

	//	This function will out at specific streamtube that is defined by a user defined set of streamlines.
	//	The streamlines are obtained from a file called SL.inp. In the file, the first row is the number
	//	of streamlines. the remaining numbers are integers defined the Streamline number. This corresponds
	//	to the initial streamline points defined in the z=0.plt file
	ifstream ifile;
	ifile.open("SL.inp");
	if (ifile.is_open())
	{
		ifile >> n >>ws;
		int j;
		int *pti = new int[n];
		for (i = 0; i < n; i++)  ifile >> pti[i] >>ws;
		//Use a similar output as the OuputBoundary function to output the streamtube
		ofile.open("streamtube.plt", ios::out);
		ofile << "VARIABLES = \"X(in)\",\"Y(in)\",\"Z(in)\",\"R(in)\",\"P(psia)\","
			<< "\"PT(psia)\",\"T(R)\",\"TT(R)\",\"RHO(lbm/ft3)\",\"Mach\"," 
			<< "\"Velocity(ft/s)\",\"Theta(deg)\",\"Psi(deg)\",\"I\",\"K\",\"BodyFlag\"" << endl
			<< "TITLE = \"Volume Mesh Plot\"" << endl
			<< "text  x=5  y=93  t=\"Characteristic Mesh Plot\" " << endl
			<< "zone t=\"All data\"" << " I = " << int ( (kEnd-kStart)/_zOutputStep) +1 
			<< " J = 1 K = " << n+1 << endl; 

		//	Loop through each point in the SL.inp file
		for ( j = 0; j < n; j++)
		{
			// Set i to the current streamline
			i = pti[j];
			for ( k = kStart; k <= kEnd; k += _zOutputStep)
			{
				ratio = 1+(_pt[i][k].g-1.)/2.*_pt[i][k].mach*_pt[i][k].mach;
				tt = _pt[i][k].t*ratio;
				pt = _pt[i][k].p*pow(ratio,(_pt[i][k].g)/(_pt[i][k].g-1));
				rad = sqrt(_pt[i][k].x*_pt[i][k].x + _pt[i][k].y*_pt[i][k].y);
				ofile << _pt[i][k].x << "\t" << _pt[i][k].y << "\t" << _pt[i][k].z << "\t" << rad
					<< "\t" << _pt[i][k].p << "\t" << pt << "\t" << _pt[i][k].t 
					<< "\t" << tt << "\t" << _pt[i][k].rho 
					<< "\t" << _pt[i][k].mach  << "\t" << _pt[i][k].q << "\t"
					<< _pt[i][k].theta*DEG_PER_RAD << "\t" << _pt[i][k].psi*DEG_PER_RAD << "\t" 
					<< i << "\t" << k << "\t" << _bodyPointFlag[i] << endl;
			}
		}	// Go to the next streamline

		//	Step thru again, this time only outputting the first line, This will make a completely
		//	enclosed region
		i = pti[0];
		for ( k = kStart; k <= kEnd; k += _zOutputStep)
		{
			ratio = 1+(_pt[i][k].g-1.)/2.*_pt[i][k].mach*_pt[i][k].mach;
			tt = _pt[i][k].t*ratio;
			pt = _pt[i][k].p*pow(ratio,(_pt[i][k].g)/(_pt[i][k].g-1));
			rad = sqrt(_pt[i][k].x*_pt[i][k].x + _pt[i][k].y*_pt[i][k].y);
			ofile << _pt[i][k].x << "\t" << _pt[i][k].y << "\t" << _pt[i][k].z << "\t" << rad
				<< "\t" << _pt[i][k].p << "\t" << pt << "\t" << _pt[i][k].t 
				<< "\t" << tt << "\t" << _pt[i][k].rho 
				<< "\t" << _pt[i][k].mach  << "\t" << _pt[i][k].q << "\t"
				<< _pt[i][k].theta*DEG_PER_RAD << "\t" << _pt[i][k].psi*DEG_PER_RAD << "\t" 
				<< i << "\t" << k << "\t" << _bodyPointFlag[i] << endl;
		}
	}	// End of if file is open

	return;
}

//**************************************************************************************
//**************************************************************************************
//**************************************************************************************
void C3D_MOCGrid::OutputBoundary( int iStart, int iEnd, int kStart, int kEnd)
{
	//	This Function will output the nozzle contour in TECPLOT format
	int i,k,nPts;
	double rad,pt,tt,ratio;

	//	First, the code has to find the number of wall points that were calculated for the 
	//	first data plane. The same number of points is then used from then on
	nPts = 0;
	for ( i = iStart; i <=iEnd; i++) if (_bodyPointFlag[i]) nPts++;

	fstream ofile;
	ofile.open("Wall.plt", ios::out);
	ofile << "VARIABLES = \"X(in)\",\"Y(in)\",\"Z(in)\",\"R(in)\",\"P(psia)\","
		<< "\"PT(psia)\",\"T(R)\",\"TT(R)\",\"RHO(lbm/ft3)\",\"Mach\"," 
		<< "\"Velocity(ft/s)\",\"Theta(deg)\",\"Psi(deg)\",\"I\",\"K\",\"BodyFlag\"" << endl
		<< "TITLE = \"Volume Mesh Plot\"" << endl
		<< "text  x=5  y=93  t=\"Characteristic Mesh Plot\" " << endl
		<< "zone t=\"All data\"" << " I = " << int ( (kEnd-kStart)/_zOutputStep) +2 
		<< " J = 1 K = " << nPts+1 << endl; 

	//	Loop through each ray that defines the wall
	for (i = iStart; i <= iEnd; i ++)
	{
		for ( k = kStart; k <= kEnd; k += _zOutputStep)
		{
			if (_bodyPointFlag[i])
			{
				ratio = 1+(_pt[i][k].g-1.)/2.*_pt[i][k].mach*_pt[i][k].mach;
				tt = _pt[i][k].t*ratio;
				pt = _pt[i][k].p*pow(ratio,(_pt[i][k].g)/(_pt[i][k].g-1));
				rad = sqrt(_pt[i][k].x*_pt[i][k].x + _pt[i][k].y*_pt[i][k].y);
				ofile << _pt[i][k].x << "\t" << _pt[i][k].y << "\t" << _pt[i][k].z << "\t" << rad
					<< "\t" << _pt[i][k].p << "\t" << pt << "\t" << _pt[i][k].t 
					<< "\t" << tt << "\t" << _pt[i][k].rho 
					<< "\t" << _pt[i][k].mach  << "\t" << _pt[i][k].q << "\t"
					<< _pt[i][k].theta*DEG_PER_RAD << "\t" << _pt[i][k].psi*DEG_PER_RAD << "\t" 
					<< i << "\t" << k << "\t" << _bodyPointFlag[i] << endl;
			}
		}
		k = kEnd;
		if (_bodyPointFlag[i])
		{
			ratio = 1+(_pt[i][k].g-1.)/2.*_pt[i][k].mach*_pt[i][k].mach;
			tt = _pt[i][k].t*ratio;
			pt = _pt[i][k].p*pow(ratio,(_pt[i][k].g)/(_pt[i][k].g-1));
			rad = sqrt(_pt[i][k].x*_pt[i][k].x + _pt[i][k].y*_pt[i][k].y);
			ofile << _pt[i][k].x << "\t" << _pt[i][k].y << "\t" << _pt[i][k].z << "\t" << rad
				<< "\t" << _pt[i][k].p << "\t" << pt << "\t" << _pt[i][k].t 
				<< "\t" << tt << "\t" << _pt[i][k].rho 
				<< "\t" << _pt[i][k].mach  << "\t" << _pt[i][k].q << "\t"
				<< _pt[i][k].theta*DEG_PER_RAD << "\t" << _pt[i][k].psi*DEG_PER_RAD << "\t" 
				<< i << "\t" << k << "\t" << _bodyPointFlag[i] << endl;
		}
	}

	//	Step thru again, this time only outputting the first line, This will make a completely
	//	enclosed region
	for (i = iStart; i <= iEnd; i ++)
	{
		for ( k = kStart; k <= kEnd; k += _zOutputStep)
		{
			if (_bodyPointFlag[i])
			{
				ratio = 1+(_pt[i][k].g-1.)/2.*_pt[i][k].mach*_pt[i][k].mach;
				tt = _pt[i][k].t*ratio;
				pt = _pt[i][k].p*pow(ratio,(_pt[i][k].g)/(_pt[i][k].g-1));
				rad = sqrt(_pt[i][k].x*_pt[i][k].x + _pt[i][k].y*_pt[i][k].y);
				ofile << _pt[i][k].x << "\t" << _pt[i][k].y << "\t" << _pt[i][k].z << "\t" << rad
					<< "\t" << _pt[i][k].p << "\t" << pt << "\t" << _pt[i][k].t 
					<< "\t" << tt << "\t" << _pt[i][k].rho 
					<< "\t" << _pt[i][k].mach  << "\t" << _pt[i][k].q << "\t"
					<< _pt[i][k].theta*DEG_PER_RAD << "\t" << _pt[i][k].psi*DEG_PER_RAD << "\t" 
					<< i << "\t" << k << "\t" << _bodyPointFlag[i] << endl;
			}
		}
		k = kEnd;
		if (_bodyPointFlag[i])
		{
			ratio = 1+(_pt[i][k].g-1.)/2.*_pt[i][k].mach*_pt[i][k].mach;
			tt = _pt[i][k].t*ratio;
			pt = _pt[i][k].p*pow(ratio,(_pt[i][k].g)/(_pt[i][k].g-1));
			rad = sqrt(_pt[i][k].x*_pt[i][k].x + _pt[i][k].y*_pt[i][k].y);
			ofile << _pt[i][k].x << "\t" << _pt[i][k].y << "\t" << _pt[i][k].z << "\t" << rad
				<< "\t" << _pt[i][k].p << "\t" << pt << "\t" << _pt[i][k].t 
				<< "\t" << tt << "\t" << _pt[i][k].rho 
				<< "\t" << _pt[i][k].mach  << "\t" << _pt[i][k].q << "\t"
				<< _pt[i][k].theta*DEG_PER_RAD << "\t" << _pt[i][k].psi*DEG_PER_RAD << "\t" 
				<< i << "\t" << k << "\t" << _bodyPointFlag[i] << endl;
		}

		if (_bodyPointFlag[i]) break;
	}

	ofile.close();
	return;
}


//**************************************************************************************
//**************************************************************************************
//**************************************************************************************
void C3D_MOCGrid::AddNewNozzlePoint(int k)
{
	// This program will add a new point along the nozzle contour at k.
	int i,j;

	// Move all of the current nozzle points up one k starting at k
	//	first increment the number of Z by one
	_nZ++;

	for ( i = _nZ; i > k; i--)
	{
		for ( j = 0; j < _nDiv; j++)
		{
			_wallPt[j][i].x = _wallPt[j][i-1].x;
			_wallPt[j][i].y = _wallPt[j][i-1].y;
			_wallPt[j][i].z = _wallPt[j][i-1].z;
		}
	}

	// now set k
	for ( j = 0; j < _nDiv; j++)
	{
		_wallPt[j][k].x = (_wallPt[j][k-1].x + _wallPt[j][k+1].x)/2. ;
		_wallPt[j][k].y = (_wallPt[j][k-1].y + _wallPt[j][k+1].y)/2. ;
		_wallPt[j][k].z = (_wallPt[j][k-1].z + _wallPt[j][k+1].z)/2. ;
	}
	return;
}
