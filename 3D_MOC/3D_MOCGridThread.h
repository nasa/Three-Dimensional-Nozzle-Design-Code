#if !defined(AFX_3D_MOCGRIDTHREAD_H__B88EF7AB_A515_47E3_97F3_3EB06BCDCF45__INCLUDED_)
#define AFX_3D_MOCGRIDTHREAD_H__B88EF7AB_A515_47E3_97F3_3EB06BCDCF45__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// 3D_MOCGridThread.h : header file
//

#include "3D_MOCGrid.hpp"


/////////////////////////////////////////////////////////////////////////////
// C3D_MOCGridThread thread

class C3D_MOCGridThread : public CWinThread 
{
	DECLARE_DYNCREATE(C3D_MOCGridThread)
protected:
	C3D_MOCGridThread();           // protected constructor used by dynamic creation

// Attributes
public:
	C3D_MOCGrid moc;
	CDialog *p3D_MOCDlg; // Pointer the dialog in which to input data
	// Tharen Rice: May 2020: Changed the 'void' to 'afx_msg LRESULT' to fix a C2440 error
	afx_msg LRESULT CalculateNozzle( WPARAM w, LPARAM l);
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(C3D_MOCGridThread)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~C3D_MOCGridThread();

	// Generated message map functions
	//{{AFX_MSG(C3D_MOCGridThread)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_3D_MOCGRIDTHREAD_H__B88EF7AB_A515_47E3_97F3_3EB06BCDCF45__INCLUDED_)
