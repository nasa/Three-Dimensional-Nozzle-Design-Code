// MOCPlotDialog.cpp : implementation file
//

#include "stdafx.h"
#include "MOCPlotDialog.h"
#include "engineering_constants.hpp"
#include "math.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// MOCPlotDialog dialog


MOCPlotDialog::MOCPlotDialog(CWnd* pParent /*=NULL*/)
	: CDialog(MOCPlotDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(MOCPlotDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void MOCPlotDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(MOCPlotDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	//	Set chart size
	CRect Rect;
	GetClientRect(&Rect) ;
	long width = Rect.Width();
    long height = Rect.Height();
//	Rect.right = Rect.left + 6*width/7 ; 
	Rect.bottom = 6*height/7;
	MOCChart.Create(WS_CHILD|WS_VISIBLE,Rect,this,IDD_MOCPlot_DIALOG );
	MOCChart.Invalidate();
}


BEGIN_MESSAGE_MAP(MOCPlotDialog, CDialog)
	//{{AFX_MSG_MAP(MOCPlotDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// MOCPlotDialog message handlers

void MOCPlotDialog::FormatMOCChart(int n)
{
	MOCChart.SetChartTitle("Calculated MOC Nozzle Contour");
	MOCChart.SetChartLabel("Axial Distance/R*","Radial Distance/R*");
	MOCChart.nSerieCount = 1;
	MOCChart.SetAxisStyle(0);
	if(!MOCChart.AllocSerie(n+2))
	{
		AfxMessageBox("Could not allocate memory for Chart");
		exit(1);
	}
	MOCChart.m_BGColor = RGB(255,255,235); // Yellow
	MOCChart.m_AxisColor = RGB(0,0,0); // Black
	MOCChart.m_GridColor = RGB(128,128,128); // Grey
	MOCChart.mpSerie[0].m_plotColor = RGB(0,0,255); // Blue
	MOCChart.mpSerie[1].m_plotColor = RGB(255,0,128); // Pinkish
	return;
}

void MOCPlotDialog::PlotMOCChartPoints( double *x, double *y, int n, double maxX, double maxY)
{
	int i;
	MOCChart.SetRange(0.0,maxX,0.0,maxY);
	for ( i = 0; i < n; i++) MOCChart.SetXYValue(x[i],y[i],i,0);
	return;
}

void MOCPlotDialog::OnOK() 
{
	// TODO: Add extra validation here
	CDialog::EndDialog(1);
}

void MOCPlotDialog::Invalidate(void)
{
	MOCChart.Invalidate();
	return;
}
