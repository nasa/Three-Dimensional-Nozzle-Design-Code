/**********************************************************************************************
*
*					The Johns Hopkins University Applied Physics Laboratory
*					        Research and Technology Development Center
*						    Aeronautical Science and Technology Group
*							                  MS 10A-118
*							             Johns Hopkins Road
*								           Laurel, MD 20723
*								             443-778-3863
*								          443-778-5850 (FAX)
*
*
*	Program:			engineering_Constants.hpp	
*
*	Start Date:			03/02/00
*
*	Last Modified:		03/02/00
*
*	Author:				Tharen Rice   x3863
*
*	Sponsor:			IRAD X83RX5XX
*
*	Comments:			This program will include various engineering constants
*						that are used regularly.  Most are taken from the CRC
*						books
*
*
***********************************************************************************************/


#ifndef _ENGINEERING_CONSTANTS_
#define _ENGINEERING_CONSTANTS_

#define xmin( a, b) ((a) < (b) ? (a) : (b))
#define xmax( a, b) ((a) > (b) ? (a) : (b))

enum convergence_flag {NOT_CONVERGED, CONVERGED};
enum fail_flag {FAIL, OK};
enum flow_regime { SUBSONIC, SONIC, SUPERSONIC};

const double ZERO = 0.0;

const double PI	= 3.14159265358979323846264338327950288419716939937511;

const double GASCON = 1545; // Universal Gas Constant ft-lbf/lbm-mol-R

//	gravity terms
const double GRAV			=	32.174;
const double GRAV_SI		=	9.80665;

//	Mass conversion
const double KG_PER_SLUG	=	14.5939; // F-345
const double SLUG_PER_KG	=	1.0/KG_PER_SLUG;
const double LBM_PER_KG		=	2.20462;	
const double KG_PER_LBM		=	1.0/LBM_PER_KG;

// force conversions
const double LBF_PER_N = .22480894;  // F-343
const double N_PER_LBF  = 1.0/LBF_PER_N;

//	pressure conversion
const double PA_PER_PSF		=	47.8803; // F-344
const double PSF_PER_PA		=	1.0/PA_PER_PSF;

const double PA_PER_ATM		=	1.01325e5; // F-326
const double ATM_PER_PA		=	1.0/PA_PER_ATM;

const double ATM_PER_PSF	=	.000472541;  // F-344
const double PSF_PER_ATM	=	1.0/ATM_PER_PSF;

const double ATM_PER_PSI	=	ATM_PER_PSF / 144.;  // F-344
const double PSI_PER_ATM	=	1.0/ATM_PER_PSI;

const double PA_PER_PSI		=	PA_PER_PSF * 144.;
const double PSI_PER_PA		=	1.0/PA_PER_PSI;

// length conversions
const double M_PER_FT		=	0.3048;  // F-333
const double FT_PER_M		=	1.0/M_PER_FT;

const double M_PER_IN		=	0.0254;  // F-333
const double IN_PER_M		=	1.0/M_PER_IN;

const double FT_PER_KM		=	3280.8399;  // F-338
const double KM_PER_FT		=	1.0/FT_PER_KM;

const double FT_PER_NMI		=	6076.1155; // F-342 NMI = nautical mile
const double NMI_PER_FT		=	1.0/FT_PER_NMI;

//	Area
const double IN2_PER_M2		=	IN_PER_M*IN_PER_M;
const double M2_PER_IN2		=	1.0/IN2_PER_M2;
const double FT2_PER_M2		=	IN2_PER_M2 / 144.;
const double M2_PER_FT2		=	1.0/FT2_PER_M2;

// Volume
const double FT3_PER_M3		=	FT2_PER_M2*FT_PER_M;
const double M3_PER_FT3		=	1/FT3_PER_M3;

// angular conversions
const double DEG_PER_RAD	=	180.0/PI;
const double RAD_PER_DEG	=	1.0/DEG_PER_RAD;

//	Temperature Conversions
const double R_PER_K		=	1.8;
const double K_PER_R		=	1.0/R_PER_K;

//	Power
const double BTU_OVER_S_PER_WATT			= 0.0009486608;
const double WATT_PER_BTU_OVER_S			= 1/BTU_OVER_S_PER_WATT;

//	Density
const double KG_OVER_M3_PER_LBM_OVER_FT3	=	KG_PER_LBM/M3_PER_FT3;	
const double LBM_OVER_FT3_PER_KG_OVER_M3	=	1.0/KG_OVER_M3_PER_LBM_OVER_FT3;
const double KG_OVER_M3_PER_SLUG_OVER_FT3	=	16.01846*GRAV;	
const double SLUG_OVER_FT3_PER_KG_OVER_M3	=	1.0/KG_OVER_M3_PER_LBM_OVER_FT3/GRAV;

//	Enthalpy
const double BTU_OVER_LBM_PER_KJ_OVER_KG	=	0.430081;
const double KJ_OVER_KG_PER_BTU_OVER_LBM	=	1.0/BTU_OVER_LBM_PER_KJ_OVER_KG;

//	Energy
const double J_PER_BTU		=	1055.056;
const double BTU_PER_J		=	1.0/J_PER_BTU;

//	Entropy
const double BTU_OVER_LBM_R_PER_KJ_OVER_KG_K =  BTU_OVER_LBM_PER_KJ_OVER_KG/R_PER_K; 
const double KJ_OVER_KG_K_PER_BTU_OVER_LBM_R =	1.0/BTU_OVER_LBM_R_PER_KJ_OVER_KG_K;

// Viscosity
// Poise = g/cm-s
//	Pa-s = 10 Poise There for Pa-s = 10000 mPoise
const double LBF_S_OVER_FT2_PER_MILLIPOISE	=	PSF_PER_PA/10000;
const double MILLIPOISE_PER_LBF_S_OVER_FT2	=	1/LBF_S_OVER_FT2_PER_MILLIPOISE;

// Thermal conductivity
const double BTU_OVER_FT2_S_R_PER_WATT_OVER_M2_K	= BTU_OVER_S_PER_WATT*M2_PER_FT2*K_PER_R;
const double WATT_OVER_M2_K_PER_BTU_OVER_FT2_S		= BTU_OVER_FT2_S_R_PER_WATT_OVER_M2_K; 

// Heat Transfer
const double BTU_OVER_LBM_FT2_R_PER_KJ_OVER_KG_M2_K	= BTU_OVER_LBM_PER_KJ_OVER_KG*M2_PER_FT2*K_PER_R;
const double KJ_OVER_KG_M2_K_PER_BTU_OVER_LBM_FT2_R	= 1.0/BTU_OVER_LBM_FT2_R_PER_KJ_OVER_KG_M2_K;

#endif
