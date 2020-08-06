#if !defined(AFX_M_SYMDEF_DIALOG_H__CEB9E0C6_B24E_43B3_963C_EBF5986DE03A__INCLUDED_)
#define AFX_M_SYMDEF_DIALOG_H__CEB9E0C6_B24E_43B3_963C_EBF5986DE03A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// m_SymDef_DIALOG.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// m_SymDef_DIALOG dialog

class m_SymDef_DIALOG : public CDialog
{
// Construction
public:
	m_SymDef_DIALOG(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(m_SymDef_DIALOG)
	enum { IDD = IDD_SYMDEF_DIALOG };
	double	m_RSim;
	double	m_YSim;
	double	m_ZSim;
	int		m_nRev;
	int		m_SL1Match;
	int		m_SL2Match;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(m_SymDef_DIALOG)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(m_SymDef_DIALOG)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_M_SYMDEF_DIALOG_H__CEB9E0C6_B24E_43B3_963C_EBF5986DE03A__INCLUDED_)
