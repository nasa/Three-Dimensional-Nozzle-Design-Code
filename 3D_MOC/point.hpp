// Point Classes


#ifndef BASEPOINT_CLASS
#define BASEPOINT_CLASS

#include "dummyStruct.h"

class Point
{
public:
	double z;
	double p;
	double t;
	double mach;
	double g;
	double molWt;
	double rho;
	double theta; 
	double q; // velocity
	double psi;
	double L;
	double delta; // parametric angle

};
	

#endif
#ifndef XYZPOINT_CLASS
#define XYZPOINT_CLASS

class XYZPoint : public Point
{
public:
	double x;
	double y;
};

#endif

#ifndef RTZPOINT_CLASS
#define RTZPOINT_CLASS

class RTZPoint : public Point
{
public:
	double r;
	double Theta;	
};

#endif

