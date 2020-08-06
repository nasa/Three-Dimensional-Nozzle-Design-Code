// STT2001Dlg.h : header file
//

#if !defined(AFX_STT2001DLG_H__736B32D3_89F7_4C81_9DFA_93A03C2F42F5__INCLUDED_)
#define AFX_STT2001DLG_H__736B32D3_89F7_4C81_9DFA_93A03C2F42F5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "m_SymDef_DIALOG.h"
#include "PlotDialog.h"
#include "chart.h"

/////////////////////////////////////////////////////////////////////////////
// CSTT2001Dlg dialog

class CSTT2001Dlg : public CDialog
{
// Construction
public:
	CSTT2001Dlg(CWnd* pParent = NULL);	// standard constructor
//	void CalcNozzlePerformance( void);

// Dialog Data
	//{{AFX_DATA(CSTT2001Dlg)
	enum { IDD = IDD_STT2001_DIALOG };
	CButton	m_contourFlag_CHECK;
	CEdit	m_ZC5_EDIT;
	CEdit	m_YC5_EDIT;
	CEdit	m_XStart5_EDIT;
	CEdit	m_XEnd5_EDIT;
	CButton	m_Throat5_CHECK;
	CEdit	m_RC5_EDIT;
	CEdit	m_Omega5_EDIT;
	CEdit	m_nSL5_EDIT;
	CButton	m_Constraint5_CHECK;
	CComboBox	m_C5_COMBO;
	CEdit	m_Alpha5_EDIT;
	CButton	m_Plot_BUTTON;
	CEdit	m_MaxLengthC_EDIT;
	CButton	m_MaxLengthC_CHECK;
	CProgressCtrl	m_Progress_PROGRESS;
	CComboBox	m_C4_COMBO;
	CComboBox	m_C3_COMBO;
	CComboBox	m_C2_COMBO;
	CComboBox	m_C1_COMBO;
	CEdit	m_Status_EDIT;
	CButton	m_SymDef_BUTTON;
	CEdit	m_nSL1_EDIT;
	CEdit	m_nSL4_EDIT;
	CEdit	m_nSL3_EDIT;
	CEdit	m_nSL2_EDIT;
	CButton	m_Constraint4_CHECK;
	CEdit	m_ZC4_EDIT;
	CEdit	m_YC4_EDIT;
	CEdit	m_XStart4_EDIT;
	CEdit	m_XEnd4_EDIT;
	CButton	m_Throat4_CHECK;
	CButton	m_Throat3_CHECK;
	CButton	m_Throat2_CHECK;
	CButton	m_Throat1_CHECK;
	CEdit	m_RC4_EDIT;
	CEdit	m_Omega4_EDIT;
	CEdit	m_Alpha4_EDIT;
	CEdit	m_XStart3_EDIT;
	CEdit	m_XStart2_EDIT;
	CEdit	m_XStart1_EDIT;
	CEdit	m_XEnd3_EDIT;
	CEdit	m_XEnd2_EDIT;
	CEdit	m_XEnd1_EDIT;
	CEdit	m_Omega3_EDIT;
	CEdit	m_Omega2_EDIT;
	CEdit	m_Omega1_EDIT;
	CButton	m_EXE_BUTTON;
	CEdit	m_Alpha3_EDIT;
	CEdit	m_Alpha2_EDIT;
	CEdit	m_Alpha1_EDIT;
	CEdit	m_YC3_EDIT;
	CEdit	m_YC2_EDIT;
	CEdit	m_YC1_EDIT;
	CEdit	m_ZC3_EDIT;
	CEdit	m_ZC2_EDIT;
	CEdit	m_ZC1_EDIT;
	CEdit	m_RC3_EDIT;
	CEdit	m_RC2_EDIT;
	CEdit	m_RC1_EDIT;
	CButton	m_Constraint3_CHECK;
	CButton	m_Constraint2_CHECK;
	CButton	m_Constraint1_CHECK;
	double	m_RC1;
	double	m_RC2;
	double	m_RC3;
	double	m_ZC1;
	double	m_ZC2;
	double	m_ZC3;
	double	m_YC1;
	double	m_YC2;
	double	m_YC3;
	double	m_Alpha1;
	double	m_Alpha2;
	double	m_Alpha3;
	double	m_Omega1;
	double	m_Omega2;
	double	m_Omega3;
	double	m_XEnd1;
	double	m_XEnd2;
	double	m_XEnd3;
	double	m_XStart1;
	double	m_XStart2;
	double	m_XStart3;
	double	m_Alpha4;
	double	m_Omega4;
	double	m_RC4;
	double	m_XEnd4;
	double	m_XStart4;
	double	m_YC4;
	double	m_ZC4;
	int	m_nSL1;
	int	m_nSL2;
	int	m_nSL3;
	int	m_nSL4;
	double	m_pAmbient;
	double	m_aThroat;
	double	m_IspIdeal;
	CString	m_CxxStr;
	CString	m_IspCalcStr;
	CString	m_projectedAreaStr;
	CString	m_surfaceAreaStr;
	double	m_MassFlow;
	double	m_RSLEnd;
	double	m_RSLStart;
	double	m_XSLEnd;
	double	m_XSLStart;
	double	m_YSLEnd;
	double	m_YSLStart;
	double	m_RSLStep;
	double	m_XSLStep;
	double	m_YSLStep;
	CString	m_AreaRatioStr;
	CString	m_ExitAreaStr;
	CString	m_MaxLengthStr;
	double	m_MaxLengthC;
	CString	m_MaxYStr;
	CString	m_MinYStr;
	CString	m_XAtMaxYStr;
	CString	m_XAtMinYStr;
	CString	m_FrictionFile;
	int		m_GridSF;
	CString	m_RSLCurrentStr;
	CString	m_XSLCurrentStr;
	CString	m_YSLCurrentStr;
	double	m_XStatus;
	double	m_Alpha5;
	int		m_nSL5;
	double	m_Omega5;
	double	m_RC5;
	double	m_XEnd5;
	double	m_XStart5;
	double	m_YC5;
	double	m_ZC5;
	CString	m_filePrefix;
	CString	m_MOCSummaryFileName;
	CString	m_integralPdAStr;
	CString	m_pExitForceStr;
	CString	m_throatForceStr;
	CString	m_frictionForceStr;
	CString	m_MOCGridFile;
	CString	m_SLFileName;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSTT2001Dlg)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CSTT2001Dlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnConstraint1CHECK();
	afx_msg void OnConstraint2CHECK();
	afx_msg void OnConstraint3CHECK();
	afx_msg void OnExeButton();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnConstraint4CHECK();
	afx_msg void OnThroat1CHECK();
	afx_msg void OnThroat2CHECK();
	afx_msg void OnThroat3CHECK();
	afx_msg void OnThroat4CHECK();
	afx_msg void OnSymDefBUTTON();
	afx_msg void OnFileOpen();
	afx_msg void OnFileSave();
	afx_msg void OnFileSaveas();
	afx_msg void OnMaxLengthCCHECK();
	afx_msg void OnPlotBUTTON();
	afx_msg void OnConstraint5CHECK();
	afx_msg void OnThroat5CHECK();
	//}}AFX_MSG
	void GetSLData();
	void GetSLDataWithoutExitPlane();
	void TurnCheckBoxesOff();
	void InitializeArrays( int j);
	void DeleteArrays( int j);
	int CalcThroatSLs( double m_YC, double m_ZC, double m_RC, double alpha, double omega,
		int nSL, int isurface_type);
	void TrimSLs( double m_YC, double m_ZC, double m_RC, double alpha, double omega, 
		double xstart, double xend);
	void OutputThroatSLsToFile( int level);
	void OutputTrimmedSLsToFile( int level);
	void ThrowNoThroatSLDefinedError( void);
	void CalcNozzleParameters( void);
	void InterpolatePressureData(int SLValue, double *xData, double *pressure, int nPts);
	void TrimSLsToMaxLength( double m_MaxLengthC);
	void UpdateScreen( void);
	void CalcIdealCfgIspForSimulatedNozzle( CString oxid, CString fuel, double ofRatio, 
		double PC, double pAmbient);
	void TrimSLsDueToAxiRevolution( int revs, double Y0, double Z0, double R0);
	void CalcGridSLs( void);
	double GetFrictionLoss( double area_ratio);
	void OutputCenterlinePlot( int n, const double* x1, const double* y1, 
													const double* x2, const double* y2);
	void CalcXStatus( int n, const double X, const double *x1, const double *y1,
		double &x, double &theta);
	void GetPerformanceDataFromMOCSummaryFile( void);
	double FindMaxX(int nSLs);
	int **jsl;

	double *psi, **xsl, **rsl, **psl, **msl, **tsl, **thsl;

		double x1, x2, m_Force, m_SurfaceArea, m_ProjectedArea;
	double *xt_end, *yt_end, *zt_end;
	//	streamlines defined at the intersection with the throat
	//	[SL number][points]
	double **xt,**yt,**zt,*newPsi, **pt, **xgrid, **ygrid, **zgrid, **pgrid;
	double m_RSL, m_XSL, m_YSL, m_ZSL, maxLength, minY, maxY;
	double m_thrust1, m_fricLoss, m_pALoss, m_aExit, m_epsMOC, m_IspCalc;
	double m_XAtMaxY, m_XAtMinY;
	double m_ZSLStart, m_ZSLEnd, m_ZSLStep; // these are palceholders
	int nSL, *newSLPts, nNewSLs,*lastSLPt, old_nSL, *nt ; // # of streamlines
	int rotateSL[999], circleFlag, m_NAtMaxY, m_NAtMinY , nparamGRIDX, m_NAtMaxX;
	const int nparamPTS;
	CString m_DataFileName;
	CChart m_SLChart;
	m_SymDef_DIALOG m_SymDefDlg;
	PlotDialog m_PlotDlg;
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STT2001DLG_H__736B32D3_89F7_4C81_9DFA_93A03C2F42F5__INCLUDED_)
