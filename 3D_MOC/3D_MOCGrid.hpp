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
*	Program:			3D_MOCGrid.hpp
*
*	Start Date:			5/14/02
*
*	Last Modified:		5/14/02
*
*	Author:				Tharen Rice   x3863
*
*	Sponsor:			FG800XXX (NASA GRC Nozzle Work)
*
*	Comments:			This is the 3D_MOC class			
* 
*	Last Update:		Initial Start					 
*						
*
****************************************************************************/

#ifndef C3D_MOCGrid_CLASS
#define C3D_MOCGrid_CLASS
#include "afxwin.h"
#include "dummyStruct.h"
#include "point.hpp"
#include "nr.h"
#include <limits>
#include <cmath>
#include "float.h"


enum bodyType{CONSTANT_X, CONSTANT_Y, LINE, CIRCLE}; 
enum secantMethod{ FAIL, FAIL_MACH, FAIL_HIGH, OK};
enum failflag {NO, YES};

//	Global functions used in the Newton-Rhaphson solution for Body point determination
void funcv(Vec_I_DP &x, Vec_O_DP &f);
void funcvDeriv(Vec_I_DP &x, Mat_DP &f);

class C3D_MOCGrid
{
	public:
		C3D_MOCGrid( void);
		~C3D_MOCGrid(void);
		/************************************************************
		*															*
		*					PUBLIC FUNCTIONS						*
		*															*
		************************************************************/
		int SetInitialPropertiesForCircularThroat(const double pres, const double temp, 
			const double mach, const double mWt, const double gamma, const double theta, 
			const double psi, const int nDiv, const CString geomFile);
		void SetPrintMode(int i) {_printMode = i;}
		void SetPrintMode(int i, int xStep, int yStep, int zStep, int stepStep);
		void SetSplineFit(CString x);
		int CalcNozzle( CDialog*);
		void OutputContour( int iStart, int iEnd, int kStart, int kEnd);
	
	private:
		/************************************************************
		*															*
		*					PRIVATE MEMBERS 						*
		*															*
		************************************************************/
		//	The following is a list of variables that will be kept track of
		//	Mach, Temperature, Pressure, Density.  The memory will be allocated 
		//	dynamically, becasue the grid size is variable.
		XYZPoint **_pt; //[i]:points along the ray [j]: ray [k]: axial plane
		XYZPoint **_neighbor; // Neighboring point array; Reset for each k
		bool *_bodyPointFlag; // if TRUE, then the point is a body point
		//	Wall coordinates
		XYZPoint **_wallPt;
		double _cFit[5000][7];
		CString sFit;
		int _printMode;
		int _nRadii, _nDiv, _nZ;
		int _xOutputStep, _yOutputStep, _zOutputStep, _stepStep;
		int _nPTS; // Total number of points in the initial plane
		int _nNeighbors; // number of neighboring point to find for surface fit interpolation

		/************************************************************
		*															*
		*					PRIVATE FUNCTIONS						*
		*															*
		************************************************************/
		
		//	Functions used to initialize and delete private member arrays
		void InitializeWallPoints( void);
		void InitializeDataMembers( void);
		void DeleteDataMembers( void);

		//	Functions used to solve for the nozzle
		int SetInitialReferencePlane(const double pres, const double temp, const double mach, 
			const double mWt, const double gamma, const double theta, const double psi, 
			const double z0, const double r0, const int nDivs);
		int CalcFieldPoint(const int i, const int k, const double z);
		int CalcBodyPoint(const int i, const int k, const double z);
		dummyStruct CalcSurfacePointProperties(XYZPoint *basePt, int i, int k);
		dummyStruct CompatabilityEquationSolverForFieldPoint( const XYZPoint *basePt, 
			const double *dtdN, const double *dpdN, const double *dzdN, const double *theta, 
			const double *beta, const double *delta, const double dz );
		
		dummyStruct CompEqu( const double beta, const XYZPoint basePt,
		const double delta, const double dzdN, const double dz, const double theta,
			const double dtdN, const double dpdN);
		dummyStruct CompatabilityEquationSolverForBodyPoint( 
			const XYZPoint *basePt, const double *dtdN, const double *dpdN, const double *dzdN,
			const double *theta, const double *beta, const double *delta, const double dz,
			const XYZPoint P2, const dummyStruct unitNormal);
		dummyStruct CalcUnitNormalToBodySurface( const XYZPoint P);
//		dummyStruct UnitNormalVector(const RTZPoint P3, const RTZPoint P1, const RTZPoint P4);
		dummyStruct BodyFit( int j, int k);
		dummyStruct BodyFit(XYZPoint X);
		dummyStruct FindClosestBodyPoint( XYZPoint X);
		dummyStruct SolveForBodyPointPosition( dummyStruct CC, dummyStruct bFit,
			const double X, const double Y);
		dummyStruct CheckNewPointPosition( XYZPoint P2, int j, int k);
		double CalcParametricAngle( const double beta, const double theta, const double psi,
				const double delta);
		double DeltaF( void);
		void SetNeighborPoints( int kSet, int kNow);
		void SurfaceFit( int i, int k);
		void AllPointSurfaceFit( int k);
		void AddNewNozzlePoint(int k);
	/*	dummyStruct NewtonRap(double theta2, double psi2, const double a1,
			const double b1, const double c1, const double n1, const double n2,
			const double n3);*/
		void OutputAxialStations(int iStart, int iEnd, int kStart, int kEnd);
		void OutputBoundary(int iStart, int iEnd, int kStart, int kEnd);
		void OutputStreamlines( int iStart, int iEnd, int kStart, int kEnd);
	
		//****************************************************************
		//	Function that do some little things
		double CalcMu(const double mach);
		double lDyDx( const double theta, const double mu);
		double rDyDx( const double theta, const double mu);
		double TanAvg( const double x, const double y);
		double MM(const double mach);

		ofstream outfile;
};

#endif
