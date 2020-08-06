// MOC_GridDlg.h : header file
//

#if !defined(AFX_MOC_GRIDDLG_H__93DA4558_70CF_4351_9B49_437A177EBC9A__INCLUDED_)
#define AFX_MOC_GRIDDLG_H__93DA4558_70CF_4351_9B49_437A177EBC9A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMOC_GridDlgAutoProxy;

/////////////////////////////////////////////////////////////////////////////
// CMOC_GridDlg dialog

class CMOC_GridDlg : public CDialog
{
	DECLARE_DYNAMIC(CMOC_GridDlg);
	friend class CMOC_GridDlgAutoProxy;

// Construction
public:
	CMOC_GridDlg(CWnd* pParent = NULL);	// standard constructor
	virtual ~CMOC_GridDlg();
	int designParam, nozzleType, nozzleGeom;
	double designValues[2];
// Dialog Data
	//{{AFX_DATA(CMOC_GridDlg)
	enum { IDD = IDD_MOC_GRID_DIALOG };
	CEdit	m_nSLj_EDIT;
	CEdit	m_nSLi_EDIT;
	CButton	m_STTRun_BUTTON;
	CEdit	m_coneAngle_EDIT;
	CComboBox	m_print_COMBO;
	CButton	m_throat_CHECK;
	CEdit	m_vel_EDIT;
	CEdit	m_pExit_EDIT;
	CButton	m_CalcMOC_BUTTON;
	CEdit	m_nRRCAboveBD_EDIT;
	CButton	m_perfect_RADIO;
	CButton	m_exitMach_RADIO;
	CButton	m_Axi_RADIO;
	CButton m_2D_RADIO;
	CButton m_cone_RADIO;
	CButton m_minLength_RADIO;
	CButton m_endPoint_RADIO;
	CButton m_eps_RADIO;
	CButton m_length_RADIO;	
	CButton m_pExit_RADIO;
	CEdit	m_eps_EDIT;
	CEdit	m_length_EDIT;
	CEdit	m_mDesign_EDIT;
	CEdit	m_xE_EDIT;
	CEdit	m_rE_EDIT;
	double	m_mDesign;
	int		m_nC;
	double	m_RWTD;
	double	m_molWt_i;
	double	m_pres_i;
	double	m_temp_i;
	double	m_gamma_i;
	double	m_RWTU;
	double	m_dTLimit;
	double	m_pAmb;
	double	m_eps;
	double	m_length;
	double	m_rE;
	double	m_xE;
	int		m_nRRCAboveBD;
	double	m_pExit;
	int		m_nSLi;
	int		m_nSLj;
	double	m_vel;
	double	m_thetaBi;
	double	m_coneAngle;
	double	m_ispIdeal;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMOC_GridDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CMOC_GridDlgAutoProxy* m_pAutoProxy;
	HICON m_hIcon;
	BOOL CanExit();
	CString m_DataFileName;
	void UpdateScreen( void);
	// Generated message map functions
	//{{AFX_MSG(CMOC_GridDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnClose();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void On2dRADIO();
	afx_msg void OnAxiRADIO();
	afx_msg void OnendPointRADIO();
	afx_msg void OnepsRADIO();
	afx_msg void OnexitMachRADIO();
	afx_msg void OnlengthRADIO();
	afx_msg void OnminLengthRADIO();
	afx_msg void OnperfectRADIO();
	afx_msg void OnConeRADIO();
	afx_msg void OnCalcMOCBUTTON();
	afx_msg void OnpExitRADIO();
	afx_msg void OnthroatCHECK();
	afx_msg void OnSelchangePrintCOMBO();
	afx_msg void OnFileOpen();
	afx_msg void OnFileSave();
	afx_msg void OnFileSaveas();
	afx_msg void OnSTTRunBUTTON();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MOC_GRIDDLG_H__93DA4558_70CF_4351_9B49_437A177EBC9A__INCLUDED_)
