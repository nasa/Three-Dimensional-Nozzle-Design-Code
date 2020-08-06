// PlotDialog.cpp : implementation file
//

#include "stdafx.h"
#include "PlotDialog.h"
#include "engineering_constants.hpp"
#include "math.h"
#include "float.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// PlotDialog dialog


PlotDialog::PlotDialog(CWnd* pParent /*=NULL*/)
	: CDialog(PlotDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(PlotDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void PlotDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(PlotDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	//	Set chart size
	CRect Rect;
	GetClientRect(&Rect) ;
	long width = Rect.Width();
    long height = Rect.Height();
//	Rect.right = Rect.left+(width/2)-5  ; used for small plots
	Rect.bottom = 3*height/7;
	centerlineChart.Create(WS_CHILD|WS_VISIBLE,Rect,this,IDD_Plot_DIALOG+1 );
	centerlineChart.Invalidate();
	//	Set chart size for the theta chart
	Rect.right = width;
//	Rect.left = width/2 + 5; // used for small plots
	Rect.top = Rect.bottom + 3;
	Rect.bottom = 6*height/7;
	thetaChart.Create(WS_CHILD|WS_VISIBLE,Rect,this,IDD_Plot_DIALOG+2 );
	thetaChart.Invalidate();

	//	Set chart size for the contour chart
/*	Rect.right = width/2 -5;
	Rect.left = 0;
	Rect.top = Rect.bottom + 10;
	Rect.bottom = height;
	contourChart.Create(WS_CHILD|WS_VISIBLE,Rect,this,IDD_Plot_DIALOG+3 );
	contourChart.Invalidate();*/


}


BEGIN_MESSAGE_MAP(PlotDialog, CDialog)
	//{{AFX_MSG_MAP(PlotDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// PlotDialog message handlers

void PlotDialog::Initialize2DChartData( int n, const double* x1, const double* y1, 
													const double* x2, const double* y2)
{
	double theta1, theta2;
	int i;
	
	//	these functions contain the chart function that make the LOOK of the chart
	FormatCenterlineChart(n);
	FormatThetaChart(n);
	
	SetChartMaxMin(n, x1, y1, x2, y2);
	

	for ( i = 0; i < n; i++)
	{
		if ( i > 0 )
		{
			if ( x1[i] != x1[i-1]) theta1 = atan((y1[i] - y1[i-1])/(x1[i] - x1[i-1]))*DEG_PER_RAD;
			if ( x2[i] != x2[i-1]) theta2 = atan((y2[i] - y2[i-1])/(x2[i] - x2[i-1]))*DEG_PER_RAD;
		}
		else 
		{
			theta1 = 0.0;
			theta2 = 0.0;
		}
		centerlineChart.SetXYValue(x1[i],y1[i],i,0);
		centerlineChart.SetXYValue(x2[i],y2[i],i,1);
		thetaChart.SetXYValue(x1[i],theta1,i,0);
		thetaChart.SetXYValue(x2[i],theta2,i,1);
	}

	return;
}


void PlotDialog::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::DestroyWindow();
}

void PlotDialog::OnOK() 
{
	// TODO: Add extra validation here
	
	CDialog::DestroyWindow();
}

void PlotDialog::FormatCenterlineChart( int n)
{
	//	Setup the chart look for centerline chart
	centerlineChart.SetChartTitle("Centerline Plot");
	centerlineChart.SetChartLabel("Axial Distance (in)","Radial Distance (in)");
	centerlineChart.SetAxisStyle(0);
	centerlineChart.nSerieCount = 2;
	if(!centerlineChart.AllocSerie(2*n))
	{
		AfxMessageBox("Could not allocate memory for centerline plot");
		exit(1);
	}
	centerlineChart.m_BGColor = RGB(255,255,235); // Yellow
	centerlineChart.m_AxisColor = RGB(0,0,0); // Black
	centerlineChart.m_GridColor = RGB(128,128,128); // Grey
	centerlineChart.mpSerie[0].m_plotColor = RGB(0,0,255); // Blue
	centerlineChart.mpSerie[1].m_plotColor = RGB(255,0,128); // Pinkish

	return;
}

void PlotDialog::FormatThetaChart( int n)
{
	thetaChart.SetChartTitle("Angle Plot");
	thetaChart.SetChartLabel("Axial Distance (in)","Wall Slope (deg)");
	thetaChart.nSerieCount = 2;
	thetaChart.SetAxisStyle(0);
	if(!thetaChart.AllocSerie(2*n))
	{
		AfxMessageBox("Could not allocate memory for Angle plot");
		exit(1);
	}
	thetaChart.m_BGColor = RGB(255,255,235); // Yellow
	thetaChart.m_AxisColor = RGB(0,0,0); // Black
	thetaChart.m_GridColor = RGB(128,128,128); // Grey
	thetaChart.mpSerie[0].m_plotColor = RGB(0,0,255); // Blue
	thetaChart.mpSerie[1].m_plotColor = RGB(255,0,128); // Pinkish
	return;
}

void PlotDialog::SetChartMaxMin( int n, const double *x1, const double *y1, const double *x2, 
								const double *y2)
{
	double xmin, xmax, ymin, ymax, tmin, tmax, slope1, slope2;
	int i, ixmin, ixmax, iymin, iymax, itmin, itmax;
	//	Find the max and min values
	for ( i = 0; i < n; i++)
	{
		if ( i > 0 )
		{
			xmin = __min( xmin, __min(x1[i], x2[i]));
			xmax = __max( xmax, __max(x1[i], x2[i]));
			ymin = __min( ymin, __min( y1[i], y2[i]));
			ymax = __max( ymax, __max( y1[i], y2[i]));	
			slope1 = (y1[i]-y1[i-1])/(x1[i]-x1[i-1]);
			slope2 = (y2[i]-y2[i-1])/(x2[i]-x2[i-1]);
			if ( _finite(slope1) && _finite(slope2))
			{
				tmin = __min( tmin, __min( slope1, slope2 ));
				tmax = __max( tmax, __max( slope1, slope2 ));
			}
		}
		else	// i = 0, first time 
		{
			xmin = __min( x1[i], x2[i]);
			xmax = __max( x1[i], x2[i]);
			ymin = y1[i];
			ymax = y1[i];
			tmin = 100.;
			tmax = -100.;
		}
	}
	tmin = atan(tmin)*DEG_PER_RAD;
	tmax = atan(tmax)*DEG_PER_RAD;
	//	Set the ranges to value that is divisible by 5.
	for (i = 1; i <= 9; i++)
	{
		if ((int(xmin)-i)/5. == int((int(xmin)-i)/5)) ixmin = int(xmin)-i;
		if ((int(ymin)-i)/5. == int((int(ymin)-i)/5)) iymin = int(ymin)-i;
		if ((int(xmax)+i)/5. == int((int(xmax)+i)/5)) ixmax = int(xmax)+i;
		if ((int(ymax)+i)/5. == int((int(ymax)+i)/5)) iymax = int(ymax)+i;
		if ((int(tmin)-i)/5. == int((int(tmin)-i)/5)) itmin = int(tmin)-i;
		if ((int(tmax)+i)/5. == int((int(tmax)+i)/5)) itmax = int(tmax)+i;
	}

	centerlineChart.SetRange(ixmin,ixmax,iymin,iymax);
	thetaChart.SetRange(ixmin,ixmax, itmin, itmax);
	return;
}