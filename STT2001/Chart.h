///////////////////////////////////////////////////////////
// Originator : Kris Jearakul  tuktun@hotmail.com
//				http://krisj.iwarp.com
// Comment	: Use with your own risk !!


#ifndef _CHART_DOT_H
#define _CHART_DOT_H
#include "afxwin.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#ifndef MAX
#define MAX 1
#endif

#ifndef MIN
#define MIN 0
#endif

#define MAX_SERIES 60

class CPlotSerie 
{

public :
	CPlotSerie() ;
	BOOL bIsPlotAvailable ;
	double *dValueX ;
	double *dValueY ;
	COLORREF m_plotColor ;
	BOOL AllocSerie(UINT nPoint);
	BOOL FreeSerie();

protected:
	BOOL bBufferAllocated;
} ;


/////////////////////////////////////////////////////////////////////////////
// Chart.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CChart window
class CChart : public CWnd
{

public:
	CChart();
	CBitmap m_BkBitmap;
	BOOL Create(DWORD dwStyle, CRect &rect, CWnd *pParent, UINT id);
	void SetChartLabel(CString strX,CString strY);
	void PrintChart(CDC *pDC, int x, int y);
	void ClearChart();
	void SetGridXYNumber(int gridx, int gridy);
	void SetChartTitle(CString str);
	//void MoveWindow(CRect &Rect);
	BOOL AllocSerie(int nSerie);
	BOOL SetXYValue(double x, double y, int index, int iSerieIdx );
	void SetAxisStyle(int iStyle);
	BOOL SetRange( double dMinX, double dMaxX, double dMinY, double dMaxY);
	CPoint Corrdinate(double x, double y);
	virtual void Invalidate(BOOL bErase = FALSE);
	virtual ~CChart();

	CPlotSerie mpSerie[MAX_SERIES] ;
	BOOL bLogScale ;
	int nSerieCount ;
	CRect  m_ctlRect ;
	CRect  m_clientRect ;
	COLORREF m_BGColor ;
	COLORREF m_AxisColor ;
	COLORREF m_GridColor ;
	COLORREF m_TextTitleColor ;
	COLORREF m_TextLabelColor ;

	// Generated message map functions
protected:
	BOOL bFontIsCreate ;
	CBitmap *m_pOldBkBitmap;
	CDC memBkDC;
	void DrawBackGround(CDC *pDC);
	BOOL bBkNeedToUpdate;
	void SetChartTitle(CDC *pDC);
	virtual void Plot(CDC *pDC);
	virtual void DrawBorder(CDC *pDC);
	virtual void DrawChartTitle(CDC *pDC);
	virtual void DrawGrid(CDC *pDC);
	virtual void DrawAxis(CDC *pDC);
	virtual void DrawGridLabel(CDC *pDC);
	virtual void CreateFont();
	void DrawLogGrid(CDC *pDC);
	void ResCalc();
	void CalcRect();

	CString strLabelX, strLabelY ;
	CString strChartTitle ;
	CFont *pLegendFontY ;
	CFont *pLegendFontX ;
	CFont *pTitleFont;
	CRect m_axisRect;
	int iChartType ;
	int nPointCount ;
	int nPlotIndex ;
	int nGridY, nGridX ;
	double dResY, dResX ;
	double dRangeY[2], dRangeX[2];
	
	BOOL bIsSerieAllocated ;





	//{{AFX_MSG(CChart)
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};



/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif 
