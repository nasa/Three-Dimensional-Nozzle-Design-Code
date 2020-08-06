// 3D_MOCDlg.h : header file
//

#if !defined(AFX_3D_MOCDLG_H__5DB01805_502B_4EF3_970A_5A7A1F7CE7A7__INCLUDED_)
#define AFX_3D_MOCDLG_H__5DB01805_502B_4EF3_970A_5A7A1F7CE7A7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "3D_MOCGridThread.h"
/////////////////////////////////////////////////////////////////////////////
// CMy3D_MOCDlg dialog

class CMy3D_MOCDlg : public CDialog
{
// Construction
public:
	CMy3D_MOCDlg(CWnd* pParent = NULL);	// standard constructor
	
// Dialog Data
	//{{AFX_DATA(CMy3D_MOCDlg)
	enum { IDD = IDD_3D_MOC_DIALOG };
	CButton	m_exe_BUTTON;
	double	m_gamma0;
	CString	m_geomFile;
	double	m_mach0;
	double	m_molWt0;
	int		m_nZ;
	double	m_p0;
	CString	m_summaryFile;
	double	m_t0;
	double	m_z0;
	double	m_psi0;
	double	m_theta0;
	int		m_xOutputIncrement;
	int		m_yOutputIncrement;
	int		m_zOutputIncrement;
	double	m_nX;
	int		m_nDiv;
	int		m_totalSteps;
	CString	m_SurfaceFit;
	int		m_step;
	int		m_stepStep;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMy3D_MOCDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;
	C3D_MOCGridThread *pMocThread;
	//Tharen Rice May 2020. commented out the next line and added the 2nd
	//void UpdateStepCounter(WPARAM w,LPARAM l); // w = step, l = total
	afx_msg LRESULT UpdateStepCounter(WPARAM w, LPARAM l); // w = step, l = total

	// Generated message map functions
	//{{AFX_MSG(CMy3D_MOCDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnexeBUTTON();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_3D_MOCDLG_H__5DB01805_502B_4EF3_970A_5A7A1F7CE7A7__INCLUDED_)
