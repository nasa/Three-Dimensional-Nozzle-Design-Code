// DlgProxy.h : header file
//

#if !defined(AFX_DLGPROXY_H__2EFECD98_D50A_4FF2_91ED_40A79CE5F92F__INCLUDED_)
#define AFX_DLGPROXY_H__2EFECD98_D50A_4FF2_91ED_40A79CE5F92F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMOC_GridDlg;

/////////////////////////////////////////////////////////////////////////////
// CMOC_GridDlgAutoProxy command target

class CMOC_GridDlgAutoProxy : public CCmdTarget
{
	DECLARE_DYNCREATE(CMOC_GridDlgAutoProxy)

	CMOC_GridDlgAutoProxy();           // protected constructor used by dynamic creation

// Attributes
public:
	CMOC_GridDlg* m_pDialog;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMOC_GridDlgAutoProxy)
	public:
	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CMOC_GridDlgAutoProxy();

	// Generated message map functions
	//{{AFX_MSG(CMOC_GridDlgAutoProxy)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	DECLARE_OLECREATE(CMOC_GridDlgAutoProxy)

	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CMOC_GridDlgAutoProxy)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGPROXY_H__2EFECD98_D50A_4FF2_91ED_40A79CE5F92F__INCLUDED_)
