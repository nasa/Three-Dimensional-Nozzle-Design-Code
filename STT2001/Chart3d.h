// Chart3d.h: interface for the CChart3d class.
// Originator : Kris Jearakul  tuktun@hotmail.com
//				http://krisj.iwarp.com
// Comment	: Use with your own risk !!
//
//////////////////////////////////////////////////////////////////////

#ifndef _CHART3D_DOT_H 
#define _CHART3D_DOT_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Chart.h"
#include "math.h"


class DPoint
{
public :
	double x ;
	double y ;
	double Range() { return fabs(x - y) ; }
	DPoint& operator=(const DPoint &pt) {
		x = pt.x ;
		y = pt.y ;
		return(*this) ;
	}
} ;


class CChart3d : public CChart  
{
public:
	void SetAxisAngle(double x , double z );
	double dResXZ , dResXY , dResZY;
	void SetRange3d(double dMinX, double dMaxX, double dMinY, double dMaxY,double dMinZ, double dMaxZ);
	CPoint Corrdinate3d(double x , double y , double z );
	double dOrgY;
	double dOrgX;
	double dTheta0 ;
	double dTheta1 ;
	double dR3dZ[2] ;
	double dR3dX[2] ;
	double dR3dY[2] ;
	CChart3d();
	virtual ~CChart3d();
	void ShiftSerie();

protected:
	int nGridZ;
	CFont *pAYFont;
	CFont *pAXFont;
	DPoint MapTo2D(double x, double y, double z);
	virtual void DrawGridLabel(CDC *pDC);
	virtual void DrawAxis(CDC *pDC);
	virtual void DrawGrid(CDC *pDC) ;
	virtual void Plot(CDC *pDC); 
	virtual void CreateFont();
};



#endif 
