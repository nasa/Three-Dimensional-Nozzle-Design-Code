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
*	Program:			MOC_GridCalc_BDE.h
*
*	Start Date:			7/27/01
*
*	Last Modified:		7/27/01
*
*	Author:				Tharen Rice   x3863
*
*	Sponsor:			FG888BXX (NASA GRC PDE Work)
*
*	Comments:			This class will calculate the MOC grid for a nozzle				
* 
*	Last Update:		Initial StartUp							 
*						
*
****************************************************************************/

#ifndef MOC_GridCalc_CLASS
#define MOC_GridCalc_CLASS
//#include "afx.h"
#include "dummyStruct.h"
#include "Chart.h"

enum nozzleGeom{NOGEOM,TWOD, AXI};
enum nozzleType{NOTYPE,RAO, CONE, PERFECT, FIXEDEND};
enum param {NOPARAM,EXITMACH, EPS, NOZZLELENGTH, ENDPOINT, EXITPRESSURE}; 
enum secantMethod{ SEC_FAIL, SEC_FAIL_LOW, SEC_FAIL_HIGH, SEC_OK};
enum failflag {NO, YES};

class MOC_GridCalc
{
	public:
		MOC_GridCalc( void);
		~MOC_GridCalc(void);
		/************************************************************
		*															*
		*					PUBLIC FUNCTIONS						*
		*															*
		************************************************************/
		int SetInitialProperties(double pTotal, double temp, double mWt, 
								double gamma, double pAmb, int n, double rwtu, double rwtd, 
								double dtLimit, int nRRCAboveBD, int nSLi, int nSLj,
								double vel, int throatFlag, double idealIsp);
		void SetSolutionParameters( int nozzleGeom, int nozzleType, int designParam, 
			double designParamValue, double thetaBi);
		void SetSolutionParameters( int nozzleGeom, int nozzleType, int designParam, 
			double designParamValue1, double designParamValue2, double thetaBi);
		void SetPrintMode(int i) {printMode = i;}	
		int CreateMOCGrid(void); // This is the main control function

	private:
		/************************************************************
		*															*
		*					PRIVATE MEMBERS 						*
		*															*
		************************************************************/
		int nC; // number of characteristic to start (Default 101)
		//	max values are used to initialize the data 
		int maxLRC; // maximum number of LRCs
		int maxRRC; // maximum number of RRCs
		int *iLast; // The i coordinate of the last point for each j
		int nozzleType;
		int nozzleGeom;
		int designParam;
		int lastRRC; // Number of the last RRC
		int nRRCAboveBD;
		int nSLi;	//	Number of Streamlines to be output in the radial direction
		int nSLj;	//	Number of Streamlines to be output in the axial direction
		int throatFlag; // 1: throat conditions   0: total conditions
		int printMode; // 0: normal // 1; full
		int jDELast;
		int iBD, jBD; // i and j of the point BD
				//	The following is a list of variables that will be kept track of
		//	Mach, Temperature, Pressure, Density.  The memory will be allocated 
		//	dynamically, becasue the grid size is variable.
		double **mach, **pres, **temp, **rho, **x, **r, **gamma, **theta, **massflow;
		double *mDE, *pDE, *tDE, *rhoDE, *xDE, *rDE, *gDE, *thetaDE, *massDE;
		double **thrust, **Sthrust;
		double pTotal, tTotal, molWt, gamma_i, pAmbient, mThroat, ispIdeal;
		double mdotErrRatio;
		double designValue[2], RWTD, RWTU, DTLIMIT, thetaBi,thetaBMin, thetaBMax;
		double conCrit,thetaBAns; // Converge criteria
		double initialLineAngle;

		/************************************************************
		*															*
		*					PRIVATE FUNCTIONS						*
		*															*
		************************************************************/
		
		//	Functions used to initialize and delete private member arrays
		void InitializeDataMembers( void);
		void DeleteDataMembers( void);

		//	These are the controlling functions that determines what type of nozzle to
		//	solve for
		int CalcMOC_Grid( double g, double pAmb, int tFlag, double mThroat, int nType, 
			int dparam, int geom, int nSLi, int nSLj);
		int CalcContouredNozzle( int paramType, double* designParamValue,
			double g, double pAmb, 	int nozzleGeom, int nRRCPlus, int nozzleType, 
			int nSLi, int nSLj);
		int CalcConeNozzle( int paramType, double* designParamValue, 
			double pAmb, int nozzleGeom, int nSLi, int nSLj);
		
		//	Functions used to create the initial data line
		dummyStruct CalcHallLine(double rUp, double x, double r, double g, int type);
		int CalcInitialThroatLine( double rUp, int n, double g, double pAmb, 
			int geom, int tFlag, double Mach_Throat);
		void Sauer(int i, int geom, double RS);
		int KLThroat(int i, int geom, double RS);

		// Functions used to calculate the individual MOC points
		int CalcRRCsAlongArc(int j, double rad, double alphaMax, double dALimit, 
			double pAmb, int geom );
		int CalcArcWallPoint( int j, double rad, double alphaMax, double dALimit, int geom );
		void CalcContourWallPoint(int j, int iBottom);
		void CalcSpecialWallPoint(int j, double RWTD, double alpha);
		void CalcAxialMeshPoint(int j, int i);
		int CalcInteriorMeshPoints(int j, int iStart, int iEnd, int flag, int geom);
		int CheckRRCForNegativePoints(int j);
		dummyStruct CalcRRCsAlongCone(int j, double dr);
		void CalcConeWallPoint(int j, double wall_angle, int geom);

		//	Functions used to find the LRC DE
		dummyStruct CalcLRCDE( int j, int nLRC, double pAmb, int geom, int nRRCPlus, 
			int nozzleType, double paramMatch, int pointFlag);
		double CalcMdotBD( int j, double xD);
		dummyStruct FindPointE( int j, double xD, double mdot, int nozzleGeom, int nType,
			int nRRCPlus, int pointFlag);
		dummyStruct RungeKutta(double dr, double r0, double x0, double mach0, 
									 double theta0, double gamma0);
		dummyStruct RungeKuttaFehlberg(double h, double r0, double x0, double mach0, 
									 double theta0, double gamma0);
		double Deriv(int i, double r0, double mach0, double theta0,double gamma0 );
		int CalcDE(int iD, int jD, int jEnd, int nType, int geom);

		//	Functions used to calculate the remaining mesh above BD 
		void CalcRemainingMesh( int iD, int jD, int jEnd, int geom);
		void CalcBDERegion(int ii, int jStart, int jEnd, int geom);
		
		//	Calculate the nozzle exit plane properties
		void CropNozzleToLength( int jEnd);
		void CalcWallContour(int iD, int jD, int nRRC, int geom);
		
		//	Functions used to find the isentropic properties at a given point
		void CalcIsentropicP_T_RHO( int i, int j, double gamma, double mach);
		dummyStruct CalcIsentropicP_T_RHO( double gamma, double mach);
		
		//	Functions to calculate the mass flow
		double InterpolateMassFlowAlongJUsingX_2D( int j, double xP, int geom);
		void CalcMassFlowAndThrustAlongMesh(int jStart, int jEnd, double pAmb, int geom);

		//	General functions used to help the code
		void ResetGrid(int iStart, int iEnd, int jStart, int jEnd);

		//	Output Functions stored in MOC...IO.cpp
		void OutputInitialKernel(int jEnd);
		void OutputFinalKernel(int iD, int jD, int jEnd);
		void OutputUncroppedKernel(int jEnd);
		void OutputCenterlineData(CString);
		void OutputMOC_Grid( void);
		void OutputSummaryFile( void);
		void OutputJ(int j, CString fileName);
		void OutputPrimaryChars(int nType);
		void OutputStreamlines(int iEnd, int jEnd, int nSLi, int nSLj, int geom);	
		void OutputTDKRAODataFile(int jStart, int jEnd);

		//	Small functions that are used a couple of times to make the code simpler to read
		double CalcMu(double mach);
		double CalcA(double mach, double gamma);
		double CalcB(double mach, double theta, double r);
		double Calcb(double mach, double theta, double r);
		double lDyDx( double theta, double mu);
		double rDyDx( double theta, double mu);
		double CalcR( double mach, double theta, double r);
		double CalcRStar( double mach, double theta, double r);
		double TanAvg( double x, double y);
		double MM(double mach);
		double CalcPMFunction(double Mach, double gamma);
		double SetThetaB( int paramType, double err, double thetaB0);
	
				
		//	Functions used to calculate the nozzle surface area	
		double FindSAFx( double xNew, int geom);
		double CalcNozzleSurfaceArea(int lastRRC, int geom);

};

#endif