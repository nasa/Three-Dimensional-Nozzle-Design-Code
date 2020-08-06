// Chart.cpp : implementation file
// Originator : Kris Jearakul  tuktun@hotmail.com
//				http://krisj.iwarp.com
// Comment	  : Use with your own risk !!

#include "stdafx.h"
#include "Chart.h"
#include "math.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChart

CChart::CChart()
{
	SetRange(-10,10,-10,10);
	strChartTitle = "Untitled Chart" ;
	strLabelX = "x" ;
	strLabelY = "y" ;
	iChartType = 2 ;
	nGridX = 10 ;
	nGridY = 10 ;
	bIsSerieAllocated = FALSE ;
	bLogScale = FALSE ;
	bFontIsCreate = FALSE ;
	bBkNeedToUpdate = TRUE ;
	nPlotIndex = 0 ;
	nPointCount = 0 ;
	nSerieCount = 1 ;

	m_BGColor = RGB(0,255,255);
	m_AxisColor = RGB(0,0,0);
	m_GridColor = RGB(120,120,120);


}

CChart::~CChart()
{

	for( int i = 0  ; i < nSerieCount ; i++){
		mpSerie[i].FreeSerie() ;
	}
	if(memBkDC.GetSafeHdc() != NULL)
	   memBkDC.SelectObject(m_pOldBkBitmap);
	if( bFontIsCreate ) {
		delete pLegendFontY ;
		delete pLegendFontX ;
		delete pTitleFont ;
	}
}


BEGIN_MESSAGE_MAP(CChart, CWnd)
	//{{AFX_MSG_MAP(CChart)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CChart message handlers


BOOL CChart::Create(DWORD dwStyle, CRect &rect, CWnd *pParent, UINT id)
{
	//DWORD style = dwStyle & ( ~WS_BORDER ) ;
	//static CString className = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW);
	BOOL result ;
	
	result = CWnd::CreateEx(WS_EX_CLIENTEDGE | WS_EX_STATICEDGE, 
                          NULL, NULL, dwStyle, 
                          rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top,
                          pParent->GetSafeHwnd(), (HMENU)id) ;
	if( !result )
		AfxMessageBox("Error creating window");
	m_ctlRect = rect ;
	pParent->ClientToScreen(m_ctlRect) ;
	ScreenToClient(m_ctlRect) ;
	CreateFont(); // Get system font for label draw 
	bFontIsCreate = TRUE ;
	CalcRect() ; // Compute rectangle 
	ResCalc(); // Compute resolution per dot .

	return result ;
}

void CChart::CalcRect()
{
	int offsetX , offsetY ;

	if(m_ctlRect)
	{
		m_clientRect.left  = m_ctlRect.left+2 ;
		m_clientRect.right = m_ctlRect.right-2 ;
		m_clientRect.top   = m_ctlRect.top+2 ;
		m_clientRect.bottom = m_ctlRect.bottom-2;
	} else {
		m_clientRect = m_ctlRect ;
		}

	//Calculate offset of window to be 4/5 of its window .

	offsetY   = m_clientRect.Height()/5 ;
	offsetX   = m_clientRect.Width()/5 ;
	m_axisRect.left = m_clientRect.left + (offsetX) ;
	m_axisRect.right = m_clientRect.right - (offsetX/2)   ;
	m_axisRect.top = m_clientRect.top + (offsetY) ;
	m_axisRect.bottom = m_clientRect.bottom - (offsetY) ;
}

//////////////////////////////////////////////////////////
// Draw box and fill color inside it .
void CChart::DrawBorder(CDC *pDC)
{
	CBrush brushCtl;//(m_BGColor);
	brushCtl.CreateSolidBrush(m_BGColor);
  	pDC->Rectangle(m_clientRect);
	pDC->FillRect(m_clientRect,&brushCtl) ;
}

void CChart::DrawAxis(CDC *pDC)
{
	CPen *old , pen(PS_SOLID, 2, m_AxisColor) ;
	old = pDC->SelectObject(&pen) ;
	double dcenter ;
	switch( iChartType ) {
	
	case 0:
		pDC->MoveTo(Corrdinate(dRangeX[MIN],dRangeY[MIN]));
		pDC->LineTo(Corrdinate(dRangeX[MAX],dRangeY[MIN]));
		pDC->MoveTo(Corrdinate(dRangeX[MIN],dRangeY[MIN]));
		pDC->LineTo(Corrdinate(dRangeX[MIN],dRangeY[MAX]));
		break;
	
	case 1:
		dcenter = (dRangeY[MAX] + dRangeY[MIN]) / 2 ;
		pDC->MoveTo(Corrdinate(dRangeX[MIN],dcenter));
		pDC->LineTo(Corrdinate(dRangeX[MAX],dcenter));
		pDC->MoveTo(Corrdinate(dRangeX[MIN],dRangeY[MIN]));
		pDC->LineTo(Corrdinate(dRangeX[MIN],dRangeY[MAX]));
		break;

	case 2:
		//draw vertical axis
		dcenter = (dRangeX[MAX] + dRangeX[MIN]) /2 ;
		pDC->MoveTo(Corrdinate(dcenter,dRangeY[MIN]));
		pDC->LineTo(Corrdinate(dcenter,dRangeY[MAX]));
		
		//draw horizontal axis
		dcenter = (dRangeY[MAX] + dRangeY[MIN]) /2 ;
		pDC->MoveTo(Corrdinate(dRangeX[MIN],dcenter));
		pDC->LineTo(Corrdinate(dRangeX[MAX],dcenter));
		break;

	}
	
	pDC->SelectObject(old) ;
}

CPoint CChart::Corrdinate(double x, double y)
{
	double rx , ry ;
	int xPixel , yPixel ;
	CPoint retPt ;

	rx = x - dRangeX[MIN] ; // Calculate horizontal offset from origin
	ry = y - dRangeY[MIN] ; // Calculate vertical offset from origin .

	// Convert offset to be number of pixel on screen .
	xPixel = (int)(rx / dResX) ; 
	yPixel = (int)(ry / dResY) ;

	//Calulate point to be drawn .
	retPt.x= xPixel + m_axisRect.left ;
	retPt.y= m_axisRect.bottom - yPixel; 
	return retPt ;
}

void CChart::Plot(CDC *pDC)
{
	
	// If there is no data available for plot then return .
	if ( !nPlotIndex ) return ;
		
	CPen *old , *pen ;

	
	CPoint pt ;
	int i;
	
	for( i = 0 ; i < nSerieCount ; i++) {
		// Create the new pen as the color of serie 
		pen = new CPen(PS_SOLID,0,mpSerie[i].m_plotColor);
		old = pDC->SelectObject(pen);
		// calculate the corrdinate of ploting point.
		pt = Corrdinate(mpSerie[i].dValueX[0],mpSerie[i].dValueY[0]) ;
		pDC->MoveTo(pt);
		//Start plot all available data .
		for(int index = 1 ; index <= nPlotIndex ; index++){
			pt = Corrdinate(mpSerie[i].dValueX[index],
						mpSerie[i].dValueY[index]) ;
		pDC->LineTo(pt) ;		
		}
		pDC->SelectObject(old);
		delete pen ;
	}
	
}

void CChart::DrawChartTitle(CDC *pDC)
{
	int x , y ;
	int oldbkmode ;
	CFont *old ;


	old = pDC->SelectObject(pTitleFont);
	oldbkmode = pDC->SetBkMode(TRANSPARENT) ;

	CSize textSize = pDC->GetTextExtent(strChartTitle,
		strChartTitle.GetLength()) ;

	//Calculate centre window corrdinate of text ;
	pDC->SetTextAlign(TA_LEFT);
	y = m_ctlRect.top + (textSize.cy/2) ;
	x = m_clientRect.left + 
		(m_clientRect.Width()/2) - (textSize.cx/2) ;

	pDC->TextOut(x,y,strChartTitle) ;
	pDC->SetBkMode(oldbkmode);
	pDC->SelectObject(old);
}

void CChart::DrawGrid(CDC *pDC)
{
	CPen *old , pen(PS_SOLID, 0, m_GridColor);
	CPoint m_start, m_stop ;
	int i  ;
	double x ,y ;
	double step ;

	old = pDC->SelectObject(&pen);
	
	// Draw vertical grid 

	step = (dRangeX[MAX] - dRangeX[MIN]) / (double)nGridX ;
	for( i = 0 ; i <= nGridX ; i++ )
	{
		x = dRangeX[MIN] + (step * (double)i) ;
		m_start = Corrdinate(x,dRangeY[MIN]);
		m_stop  = Corrdinate(x,dRangeY[MAX]);
		pDC->MoveTo(m_start);
		pDC->LineTo(m_stop);
	}
	
	// Draw horizontal grid.

	step = (dRangeY[MAX] - dRangeY[MIN]) / (double)nGridY ;
	for( i = 0 ; i <= nGridY ; i++ )
	{
		y = dRangeY[MIN] + (step * (double)i) ;
		m_start = Corrdinate(dRangeX[MIN],y) ;
		m_stop  = Corrdinate(dRangeX[MAX],y) ;
		pDC->MoveTo(m_start);
		pDC->LineTo(m_stop);
	}
	
	pDC->SelectObject(old);
}

BOOL CChart::SetRange(double dMinX, double dMaxX, 
					   double dMinY,double dMaxY)
{

	bBkNeedToUpdate = TRUE ; // Background need to be re-draw
	dRangeY[MIN] = dMinY ;
	dRangeY[MAX] = dMaxY ;
	dRangeX[MAX] = dMaxX ;
	dRangeX[MIN] = dMinX ;
    ResCalc();
	return TRUE ;	
}

////////////////////////////////////////////////////////
//Calculate resolution per dot
void CChart::ResCalc()
{
	double dpixelx, dpixely ;
	
	dpixelx = (double)m_axisRect.Width() ;
	dpixely = (double)m_axisRect.Height() ;
	dResY = (dRangeY[MAX] - dRangeY[MIN]) / dpixely ;
	dResX = (dRangeX[MAX] - dRangeX[MIN]) / dpixelx ;
}

///////////////////////////////////////////////////////
// Set Axis style 
//
void CChart::SetAxisStyle(int iStyle)
{
	if( (iStyle > 2) || (iStyle < 0) ) return ;
	iChartType = iStyle ;
}

////////////////////////////////////////////////////////
// Draw 1/5 fine scale in log mode
// in order to implement log grid set bLogScale = TRUE
void CChart::DrawLogGrid(CDC *pDC)
{
	// Change this number for changing number of fine scales.
	const int FINE_SCALE = 5 ; 
	CPen *old , pen(PS_SOLID, 0, RGB(192,192,192)) ;
	CPoint m_start, m_stop ;
	int i  ;
	int j ;
	double y ;
	double step ;
	double Ymax , Ymin ;

	Ymax = dRangeY[MAX] ;
	Ymin = dRangeY[MIN] ;
	//Remap scale to 0 - nGridY
	SetRange(dRangeX[MIN],dRangeX[MAX],0,nGridY);
	old = pDC->SelectObject(&pen);
	for( j = (int)dRangeY[MIN] ; j < (int)dRangeY[MAX] ; j ++) {
	   y = (double)j;
	   step = (pow(10,y+1) - pow(10,y)) /(double)FINE_SCALE ;
		for(i = 0 ; i < FINE_SCALE ; i++ )
		{
		
			y = log10(pow(10,y) + step) ;
			m_start = Corrdinate(dRangeX[MIN],y) ;
			m_stop  = Corrdinate(dRangeX[MAX],y) ;
			pDC->MoveTo(m_start);
			pDC->LineTo(m_stop);

		}
	}
	SetRange(dRangeX[MIN],dRangeX[MAX],Ymin,Ymax ) ;
	pDC->SelectObject(old);
	
}

////////////////////////////////////////
// Allocate memory for plotting 
BOOL CChart::AllocSerie(int nSerie)
{
	
	if( !bIsSerieAllocated )
	{
		for(int i = 0 ; i < nSerieCount ; i++ ) {
			if ( !mpSerie[i].AllocSerie(nSerie)) {
				AfxMessageBox("Can not allocate serie") ;
				return FALSE ;
			}
		}
	}
	
	nPointCount = nSerie ;
	bIsSerieAllocated = TRUE ;
	return TRUE ;

}

BOOL CChart::SetXYValue(double x, double y, int index, int iSerieIdx)
{
	if(!bIsSerieAllocated ) return FALSE ;
	if (iSerieIdx > (nSerieCount-1)) return FALSE ;

	// Prevent writting to unallocated buffer 
	if(index >= nPointCount ) return FALSE ;
	
	// Clip the ploting area if it exceed ranged .
	if(x > dRangeX[MAX] ) x = dRangeX[MAX] ;
	if(x < dRangeX[MIN] ) x = dRangeX[MIN] ;
	if(y > dRangeY[MAX] ) y = dRangeY[MAX] ;
	if(y < dRangeY[MIN] ) y = dRangeY[MIN];

	
	mpSerie[iSerieIdx].dValueX[index] = x ;
	mpSerie[iSerieIdx].dValueY[index] = y ;
	mpSerie[iSerieIdx].bIsPlotAvailable = TRUE ;

	nPlotIndex = index ;
	return TRUE ;

}


//////////////////////////////////////////////////
// Set chart title
void CChart::SetChartTitle(CString str)
{
	strChartTitle = str ;
	bBkNeedToUpdate = TRUE ; // Background need to be re-draw
}

////////////////////////////////////////////////////
// Set number of grids on x axis and y axis
void CChart::SetGridXYNumber(int gridx, int gridy)
{
	nGridX = gridx ;
	nGridY = gridy ;
	bBkNeedToUpdate = TRUE ;
}

//////////////////////////////////////////////////////
// Create desired fonts for draw legend in x axis and 
// y axis .
void CChart::CreateFont()
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
	pLegendFontY->CreateFontIndirect(&d_lf);

	d_lf.lfHeight = -1 * MulDiv(12, cyPixels, 72);
	d_lf.lfWeight = FW_BOLD ;
	pTitleFont = new CFont();
	pTitleFont->CreateFontIndirect(&d_lf);


	d_lf.lfWeight = 0 ;
	d_lf.lfOrientation = 900 ; // Rotate font 90 degree for x axis
	d_lf.lfEscapement = 900 ;  
	d_lf.lfHeight = -1 * MulDiv(7, cyPixels, 72);
	pLegendFontX = new CFont() ;
	pLegendFontX->CreateFontIndirect(&d_lf);

}

void CChart::DrawGridLabel(CDC *pDC)
{
	CFont *oldFont ;

	oldFont = pDC->SelectObject(pLegendFontY);

	int i ;
	double res , y  ; 
	CPoint cal_pt ;
	CSize txtSize ;
	CString str ;

	
	res = (dRangeY[MAX] - dRangeY[MIN]) / nGridY ;
	pDC->SetTextAlign(TA_RIGHT);
	pDC->SetBkMode(TRANSPARENT);

	for ( i = 0 ; i <= nGridY ; i++)
	{
		y = dRangeY[MIN] + (res * (double)i) ;
		cal_pt = Corrdinate(dRangeX[MIN],y) ;
		str.Format("%5.1f",y) ;

		txtSize = pDC->GetTextExtent(str) ;
		cal_pt.x -= 5 ;
		cal_pt.y -= txtSize.cy/2 ;

		pDC->TextOut(cal_pt.x,cal_pt.y,str) ;
	}

	txtSize = pDC->GetTextExtent(strLabelX);
	cal_pt.x = m_ctlRect.CenterPoint().x + (txtSize.cx/2) ;
	cal_pt.y = m_ctlRect.bottom - txtSize.cy - 5;
	pDC->TextOut(cal_pt.x,cal_pt.y,strLabelX);

	pDC->SelectObject(pLegendFontX);

	res = (dRangeX[MAX] - dRangeX[MIN]) / nGridX ;
	for ( i = 0 ; i <= nGridX ; i ++ )
	{
		y = dRangeX[MIN] + ( res * (double)i);
		cal_pt = Corrdinate(y,dRangeY[MIN]);
		str.Format("%5.1f",y);

		txtSize = pDC->GetTextExtent(str) ;
		cal_pt.x -= txtSize.cy/2 ;
		cal_pt.y += 5 ;
		pDC->TextOut(cal_pt.x,cal_pt.y,str);
	
	}
	txtSize = pDC->GetTextExtent(strLabelY);
	cal_pt.x = m_ctlRect.left + (txtSize.cy/2) ;
	cal_pt.y = m_ctlRect.CenterPoint().y  - (txtSize.cx/2) ;
	pDC->TextOut(cal_pt.x,cal_pt.y,strLabelY);
	pDC->SelectObject(oldFont);

}

void CChart::ClearChart()
{
	nPlotIndex = 0 ;
	InvalidateRect(m_clientRect);
}

void CChart::PrintChart(CDC *pDC,int x , int y)
{
	
	int xPixel ;
	int yPixel  ;
	int oldmapmode ;

	CDC *dc = GetDC();
	
	xPixel = pDC->GetDeviceCaps(LOGPIXELSX);
	yPixel = pDC->GetDeviceCaps(LOGPIXELSY);
	
	//Calculate ratio to be zoomed.
	xPixel =  xPixel /dc->GetDeviceCaps(LOGPIXELSX);
	yPixel =  yPixel /dc->GetDeviceCaps(LOGPIXELSY);
	ReleaseDC(dc);
	
	oldmapmode = pDC->SetMapMode(MM_ANISOTROPIC);
	pDC->SetViewportExt(xPixel,yPixel);
	pDC->SetViewportOrg(x,y);

	DrawBorder(pDC);
	pDC->DrawEdge(m_ctlRect,BDR_SUNKENINNER|BDR_SUNKENOUTER, BF_RECT);
	DrawChartTitle(pDC);
	if ( bLogScale )
	   DrawLogGrid(pDC);
	DrawGrid(pDC);
	
	DrawAxis(pDC) ;
	DrawGridLabel(pDC);
	Plot(pDC) ;

	pDC->SetMapMode(oldmapmode);
}

void CChart::SetChartLabel(CString strX, CString strY)
{
	strLabelX = strX ;
	strLabelY = strY ;
}


void CChart::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	CDC memPlotDC ;
	CBitmap *oldBitmap ;
	CBitmap m_plotBitmap ;
	
	// Check if background need to be redrawn
	if (bBkNeedToUpdate){
		DrawBackGround(&dc);
		bBkNeedToUpdate = FALSE ;
	}
	memPlotDC.CreateCompatibleDC(&dc) ;
	m_plotBitmap.CreateCompatibleBitmap(&dc,
		m_clientRect.Width(), m_clientRect.Height());
	oldBitmap = (CBitmap*)memPlotDC.SelectObject(&m_plotBitmap);
	
	// BitBlt background .
	memPlotDC.BitBlt(0,0, m_clientRect.Width(), m_clientRect.Height(),
		&memBkDC,0,0,SRCCOPY);
	Plot(&memPlotDC);

	dc.BitBlt(0,0, m_clientRect.Width(), m_clientRect.Height(),
		&memPlotDC,0,0,SRCCOPY);
	memPlotDC.SelectObject(oldBitmap);

}

void CChart::DrawBackGround(CDC *pDC)
{

		
	if(memBkDC.GetSafeHdc() == NULL) {
		memBkDC.CreateCompatibleDC(pDC);
		m_BkBitmap.CreateCompatibleBitmap(pDC,
			m_clientRect.Width(), m_clientRect.Height());
	}
	

	m_pOldBkBitmap = (CBitmap*)memBkDC.SelectObject(&m_BkBitmap) ;
	DrawBorder(&memBkDC);
	DrawAxis(&memBkDC);
	DrawGrid(&memBkDC);
	if(bLogScale)
		DrawLogGrid(&memBkDC);
	DrawGridLabel(&memBkDC);
	DrawChartTitle(&memBkDC);

}

void CChart::Invalidate(BOOL bErase )
{
	if ( bErase )
		bBkNeedToUpdate = TRUE ;

	CWnd::Invalidate(bErase) ;
}



////////////////////////////////////////////////////
// Source of CPlotSerie 

CPlotSerie::CPlotSerie() 
{
		dValueX = NULL ;
		dValueY = NULL ;
		bIsPlotAvailable = FALSE ;
		bBufferAllocated = FALSE ;
		m_plotColor = RGB(0,0,0);
}

BOOL CPlotSerie::AllocSerie(UINT nPoint)
{
	dValueX = (double*)malloc(nPoint * sizeof (double) ) ;
	dValueY = (double*)malloc(nPoint * sizeof (double) ) ;
	if ( (dValueX == NULL) || (dValueY == NULL) ) 
		return FALSE ;
	bBufferAllocated = TRUE ;
	return TRUE ;
}

BOOL CPlotSerie::FreeSerie()
{
	if ( bBufferAllocated ) {
		free(dValueX) ;
		free(dValueY) ;
	}
	bBufferAllocated = FALSE ;
	return TRUE ;
}


