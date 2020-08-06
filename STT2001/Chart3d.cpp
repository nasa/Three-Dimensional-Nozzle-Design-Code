// Chart3d.cpp: implementation of the CChart3d class.
// Originator : Kris Jearakul  tuktun@hotmail.com
//				http://krisj.iwarp.com
// Comment	  : Use with your own risk !!
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Chart3d.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


const double PI = 4*atan(1)  ;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CChart3d::CChart3d()
{
	
	dTheta0 = -30 ;
	dTheta1 = 30 ;
	nGridZ = 2 ;
	SetRange3d(0,100,0,20,0,100) ;
}

CChart3d::~CChart3d()
{
	if(bFontIsCreate){
		delete pAXFont ;
		delete pAYFont ;
	}
}

void CChart3d::DrawAxis(CDC *pDC)
{
	CPen *old , pen(PS_SOLID, 2, m_AxisColor) ;
	old = pDC->SelectObject(&pen) ;
	//Draw axis 

	CPoint ptOrg ;

	//draw x axis ;
	ptOrg = Corrdinate3d(dR3dX[MIN],
		dR3dY[MIN],dR3dZ[MIN]) ;
	pDC->MoveTo(ptOrg) ;
	pDC->LineTo(Corrdinate3d(dR3dX[MAX],
		dR3dY[MIN],dR3dZ[MIN])) ;

	// draw y axis 
	ptOrg = Corrdinate3d(dR3dX[MIN],
		dR3dY[MIN],dR3dZ[MIN]) ;
	pDC->MoveTo(ptOrg) ;
	pDC->LineTo(Corrdinate3d(dR3dX[MIN],
		dR3dY[MAX],dR3dZ[MIN])) ;

	// draw z axis
	ptOrg = Corrdinate3d(dR3dX[MIN],
		dR3dY[MIN],dR3dZ[MIN]) ;
	pDC->MoveTo(ptOrg) ;
	pDC->LineTo(Corrdinate3d(dR3dX[MIN],
		dR3dY[MIN],dR3dZ[MAX]));

	pDC->SelectObject(old);

}

void CChart3d::DrawGrid(CDC* /*pDC*/ )
{ 

}


CPoint CChart3d::Corrdinate3d(double x, double y, double z)
{
	DPoint m_dPt ;

	m_dPt = MapTo2D(x,y,z) ;
	m_dPt.x += dOrgX ;
	m_dPt.y += dOrgY  ;

	return Corrdinate(m_dPt.x,m_dPt.y) ;
}

void CChart3d::SetRange3d(double dMinX, double dMaxX, 
	double dMinY, double dMaxY, double dMinZ, double dMaxZ)
{
	double dRy , dRx ;
	double miny , maxy , minx , maxx ;
	
	dRy = dMaxY - dMinY ;
	dRx = dMaxX - dMinX ;

	miny =  dMinY - (dRy) ; 
	maxy =  dMaxY + (dRy) ;

	//maxy = dMaxY + fabs(2 * dRy * sin(dTheta0 * PI/180)) ;
	
	dOrgY = dMinY ;
	dOrgX = dMinX ;

	dResXZ = 1.0 * fabs(((dMaxX - dMinX )*cos(dTheta0 * PI/180)) / 
		((dMaxZ - dMinZ)*cos(dTheta1 * PI / 180))) ;

	
	maxx = fabs (2 * dRx * cos(dTheta0 * PI /180)) ;
	minx = dMinX ;

	SetRange(minx,maxx,miny,maxy) ;
	
	dR3dX[MIN] = dMinX ;
	dR3dX[MAX] = dMaxX ;

	dR3dY[MIN] = dMinY ;
	dR3dY[MAX] = dMaxY ;
	
	dR3dZ[MIN] = dMinZ ;
	dR3dZ[MAX] = dMaxZ ;


} 

/*
// This is line out plotting routine ..
void CChart3d::Plot(CDC *pDC)
{

	
	if ( !nPlotIndex ) return ;
	
	CPen *pen, *old ;
	CPoint pt ;
	

	//ShiftSerie() ;
	for(int i = 0 ; i < nSerieCount ; i++ )
	{
		if (mpSerie[i].bIsPlotAvailable) 
		{
			pen = new CPen(PS_SOLID,0,mpSerie[i].m_plotColor);
			old = pDC->SelectObject(pen);
			// calculate the corrdinate of ploting point.
			pt = Corrdinate3d(mpSerie[i].dValueX[0],mpSerie[i].dValueY[0],
				(double) i) ;
			pDC->MoveTo(pt);
			//Start plot all available data .
			for(int index = 1 ; index <= nPlotIndex ; index++){
				pt = Corrdinate3d(mpSerie[i].dValueX[index],
						mpSerie[i].dValueY[index],(double)i) ;
			pDC->LineTo(pt) ;		
			}
			pDC->SelectObject(old);
			delete pen ;
		}
	}

} */


void CChart3d::Plot(CDC *pDC)
{

	
	if ( !nPlotIndex ) return ;
	
	CPen *pen, *old ;
	CBrush brush ;
	CPoint pt[4] ;
	CRgn m_Rgn;

	
	for(int i = nSerieCount-1 ; i >= 0 ; i-- )
	{
		if (mpSerie[i].bIsPlotAvailable) 
		{
			brush.CreateSolidBrush(mpSerie[i].m_plotColor);			
			//old = pDC->SelectObject(brush);
			pen = new CPen(PS_SOLID,0,RGB(0,0,0));
			old = pDC->SelectObject(pen);
			
			for(int index = 0 ; index < (nPlotIndex-1) ; index++){
				pt[0] = Corrdinate3d(mpSerie[i].dValueX[index],
							mpSerie[i].dValueY[index],(double)i) ;
				
				pt[1] = Corrdinate3d(mpSerie[i].dValueX[index+1],
							mpSerie[i].dValueY[index+1],(double)i) ;

				if( pt[0] != pt[1] ) {

					pt[2] = Corrdinate3d(mpSerie[i].dValueX[index+1],
								0.0,(double)i) ;
					pt[3] = Corrdinate3d(mpSerie[i].dValueX[index],
								0.0,(double)i) ;
					
					pDC->MoveTo(pt[0]) ;
					pDC->LineTo(pt[1]);
					if ( index == (nPlotIndex-2))
					
						pDC->LineTo(pt[2]) ;
						
					m_Rgn.CreatePolygonRgn(pt,4,WINDING) ;
							pDC->FillRgn(&m_Rgn,&brush) ;
					
					m_Rgn.DeleteObject() ;
				}
			
			}
			pDC->SelectObject(old);
			delete pen ;
			brush.DeleteObject() ;
		}
	}

} 

void CChart3d::ShiftSerie()
{
	int i ;

	for ( i = nSerieCount - 1 ; i > 0 ; i--)
	{
		if ( mpSerie[i-1].bIsPlotAvailable ) {
			memcpy(mpSerie[i].dValueX,mpSerie[i-1].dValueX,
				sizeof(double)*nPointCount);
			memcpy(mpSerie[i].dValueY,mpSerie[i-1].dValueY,
				sizeof(double)*nPointCount);
			mpSerie[i].bIsPlotAvailable = TRUE ;
		}

	}
}

void CChart3d::DrawGridLabel(CDC *pDC)
{
	CFont *oldFont ;
	CPoint cal_pt ;
	CSize txtSize ;
	CString str ;
	int oldbkmode ;
	int i ;
	double x , y , z ;
	double res ;

	oldFont = pDC->SelectObject(pLegendFontX);

	oldbkmode = pDC->SetBkMode(TRANSPARENT) ;
	pDC->SetTextAlign(TA_RIGHT);

	//Calculate location of label X .
	x = (dR3dX[MAX] - dR3dX[MIN]) / 2 ;
	y = dR3dY[MIN] - ((dR3dY[MAX]-dR3dY[MIN])/5);
	z = dR3dZ[MIN] - ((dR3dZ[MAX]-dR3dZ[MIN])/5);
	
	txtSize = pDC->GetTextExtent(strLabelX);
	cal_pt = Corrdinate3d(x,y,z) ;

	cal_pt.x -= (txtSize.cx/ 2 ) ;
	cal_pt.y += (txtSize.cy /2) ;
	pDC->TextOut(cal_pt.x,cal_pt.y,strLabelX);

	// Draw grid label of X Axis  
	pDC->SelectObject(pAXFont);
	res = (dR3dX[MAX] - dR3dX[MIN]) / nGridX ;
	for ( i = 0 ; i <= nGridX ; i ++ )
	{
		x = dR3dX[MIN] + ( res * (double)i);
		cal_pt = Corrdinate3d(x,0,0);
		str.Format("%5.1f",x);

		txtSize = pDC->GetTextExtent(str) ;
		cal_pt.x -= txtSize.cx/2 ;
		cal_pt.y += 5 ;
		pDC->TextOut(cal_pt.x,cal_pt.y,str);
	
	}

	// Draw grid label of Y Axis 
	pDC->SelectObject(pLegendFontX);
	res = (dR3dY[MAX] - dR3dY[MIN]) / nGridY ;
	for ( i = 0 ; i <= nGridY ; i++)
	{
		y = dR3dY[MIN] + (res * (double)i) ;
		cal_pt = Corrdinate3d(dR3dX[MIN],y,dR3dZ[MIN]) ;
		str.Format("%5.2f",y) ;

		txtSize = pDC->GetTextExtent(str) ;
		cal_pt.x -= 5 ;
		cal_pt.y -= txtSize.cy/2 ;

		pDC->TextOut(cal_pt.x,cal_pt.y,str) ;
	}

	//draw grid label of Z Axis 
	res = (dR3dZ[MAX] - dR3dZ[MIN]) / nGridZ ;
	for ( i = 0 ; i <= nGridZ ; i++)
	{
		z = dR3dZ[MIN] + (res * (double)i) ;

		cal_pt = Corrdinate3d(dR3dX[MAX],dR3dY[MIN],z) ;
		str.Format("%.1f",z) ;

		txtSize = pDC->GetTextExtent(str) ;
		cal_pt.x += txtSize.cx + 2 ;
		cal_pt.y += txtSize.cy/2 ;

		pDC->TextOut(cal_pt.x,cal_pt.y,str) ;

	}



	//Calculate location of label Y
	
	
	pDC->SelectObject(pLegendFontY);
	txtSize = pDC->GetTextExtent(strLabelY);
	cal_pt.x = m_ctlRect.left + (txtSize.cy/2) ;
	cal_pt.y = m_ctlRect.CenterPoint().y  - (txtSize.cx/2) ;
	pDC->TextOut(cal_pt.x,cal_pt.y,strLabelY);

	pDC->SetBkMode(oldbkmode);
	pDC->SelectObject(oldFont);
}
void CChart3d::CreateFont()
{
	//Create system font ..
	LOGFONT d_lf ;
	
	// Init desired font
	memset(&d_lf, 0, sizeof(LOGFONT));
	lstrcpy(d_lf.lfFaceName, "Times New Roman") ;

	// Initial font size 
	// Get a screen DC 
	CWindowDC wdc(NULL) ;
	const int cyPixels = wdc.GetDeviceCaps(LOGPIXELSY);
	d_lf.lfHeight = -1 * MulDiv(7, cyPixels, 72);

	// Create a new font 7 pts.
	pLegendFontY = new CFont() ;
	d_lf.lfOrientation = 900  ; // Rotate font 90 degree for x axis
	d_lf.lfEscapement = 900 ;  
	pLegendFontY->CreateFontIndirect(&d_lf);

	d_lf.lfHeight = -1 * MulDiv(12, cyPixels, 72);
	d_lf.lfWeight = FW_BOLD ;
	d_lf.lfOrientation = 0  ; 
	d_lf.lfEscapement = 0 ;  
	pTitleFont = new CFont();
	pTitleFont->CreateFontIndirect(&d_lf);


	d_lf.lfWeight = 0 ;
	d_lf.lfOrientation = (int)dTheta0 * 10  ; // Rotate font theta0 degree for x axis
	d_lf.lfEscapement = (int)dTheta0 * 10  ;  
	d_lf.lfHeight = -1 * MulDiv(7, cyPixels, 72);
	pLegendFontX = new CFont() ;
	pLegendFontX->CreateFontIndirect(&d_lf);

	d_lf.lfWeight = 0 ;
	d_lf.lfOrientation = (int)dTheta1 * 10  ; // Rotate font theta0 degree for x axis
	d_lf.lfEscapement = (int)dTheta1 * 10  ;  
	d_lf.lfHeight = -1 * MulDiv(7, cyPixels, 72);
	pAXFont = new CFont() ;
	pAXFont->CreateFontIndirect(&d_lf);

	d_lf.lfWeight = 0 ;
	d_lf.lfOrientation = (int)dTheta0 * 10   ; 
	d_lf.lfEscapement =  (int)dTheta0 * 10   ;  
	d_lf.lfHeight = -1 * MulDiv(7, cyPixels, 72);
	pAYFont = new CFont() ;
	pAYFont->CreateFontIndirect(&d_lf);

}


DPoint CChart3d::MapTo2D(double x, double y, double z)
{
	DPoint m_dPt ;
	

	m_dPt.x = (z * dResXZ  * cos(dTheta1 * PI / 180.0)) + 
				( x * cos(dTheta0 * PI / 180))   ;
	
	m_dPt.y = (dResY/dResX)* ((  dResXZ * z * sin(dTheta1 * PI / 180)) 
				+ (x *sin(dTheta0 * PI / 180)))  + y;
	
	return m_dPt ;


}

void CChart3d::SetAxisAngle(double x, double z)
{
	dTheta0 = x ;
	dTheta1 = z ;
	// force background  to be redrawn.
	bBkNeedToUpdate = TRUE ;
}
