#if !defined(AFX_MOCPlotDialog_H__A5E0E4EC_947D_48E7_AE08_B1AE1A2A3AC1__INCLUDED_)
#define AFX_MOCPlotDialog_H__A5E0E4EC_947D_48E7_AE08_B1AE1A2A3AC1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MOCPlotDialog.h : header file
//
#include "chart.h"
#include "Resource.h"

/////////////////////////////////////////////////////////////////////////////
// MOCPlotDialog dialog

class MOCPlotDialog : public CDialog
{
// Construction
public:
	MOCPlotDialog(CWnd* pParent = NULL);   // standard constructor
	void Initialize2DChartData(	int n, const double* xbody, const double* ybody, 
		const double* xcowl, const double* ycowl);
	void FormatMOCChart(int npoints);
	void PlotMOCChartPoints( double *x, double *y, int n, double maxX, double maxY);
	void Invalidate(void);
// Dialog Data
	//{{AFX_DATA(MOCPlotDialog)
	enum { IDD = IDD_MOCPlot_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(MOCPlotDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	
// Implementation
protected:
	
	CChart MOCChart;
	
	void SetChartMaxMin( int n, const double *x1, const double *y1, const double *x2, 
		const double *y2);

//	CChart3d contourChart; 
	// Generated message map functions
	//{{AFX_MSG(MOCPlotDialog)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MOCPlotDialog_H__A5E0E4EC_947D_48E7_AE08_B1AE1A2A3AC1__INCLUDED_)
