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
#include <math.h>

const enum convergence_flag {NOT_CONVERGED, CONVERGED};
const enum exit_parameter_type { EXIT_NONE, EXIT_AREA, EXIT_PRES, EXIT_MACH,
	EXIT_AREA_AND_MDOT, EXIT_MACH_AND_MDOT, EXIT_PRES_AND_MDOT, EXIT_AREA_AND_PS};
const enum flow_regime { SUBSONIC, SONIC, SUPERSONIC};

const double ZERO = 0.0;

const double PI	= acos(-1.0);

//	Mass conversion
const double KG_PER_SLUG	=	14.5939; // F-345
const double SLUG_PER_KG	=	1.0/KG_PER_SLUG;
const double LBM_PER_KG		=	2.20462;	
const double KG_PER_LBM		=	1.0/LBM_PER_KG;

// force conversions
const double LBF_PER_N = .22480894;  // F-343
const double N_PER_LBF  = 1.0/LBF_PER_N;

//	gravity terms
const double GRAV			=	32.17400;
const double GRAV_SI		=	9.80665;

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

// angular conversions
const double DEG_PER_RAD	=	180.0/PI;
const double RAD_PER_DEG	=	1.0/DEG_PER_RAD;

//	Temperature Conversions
const double R_PER_K		=	1.8;
const double K_PER_R		=	1.0/R_PER_K;

//	Density
const double KG_OVER_M3_PER_LBM_OVER_FT3	=	16.01846;	
const double LBM_OVER_FT3_PER_KG_OVER_M3	=	1.0/KG_OVER_M3_PER_LBM_OVER_FT3;

//	Enthalpy
const double BTU_OVER_LBM_PER_KJ_OVER_KG	=	0.430081;
const double KJ_OVER_KG_PER_BTU_OVER_LBM	=	1.0/BTU_OVER_LBM_PER_KJ_OVER_KG;

//	Energy
const double J_PER_BTU		=	1055.056;
const double BTU_PER_J		=	1.0/J_PER_BTU;

//	Entropy
const double KJ_OVER_KG_K_PER_BTU_OVER_LBM_R =	KG_PER_LBM * K_PER_R * J_PER_BTU / 1000;
const double BTU_OVER_LBM_R_PER_KJ_OVER_KG_K =  1.0/KJ_OVER_KG_K_PER_BTU_OVER_LBM_R; 

//	Area
const double IN2_PER_M2		=	1550.003;
const double M2_PER_IN2		=	1.0/IN2_PER_M2;
const double FT2_PER_M2		=	IN2_PER_M2 / 144.;
const double M2_PER_FT2		=	1.0/FT2_PER_M2;

//	Gas Constant
const double GASCON			=	1545.317; // english unit


#endif