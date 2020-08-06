// MOC_Grid.h : main header file for the MOC_GRID application
//

#if !defined(AFX_MOC_GRID_H__DE1DED4D_34BE_4CE4_8EC9_0A85E0A5D25A__INCLUDED_)
#define AFX_MOC_GRID_H__DE1DED4D_34BE_4CE4_8EC9_0A85E0A5D25A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CMOC_GridApp:
// See MOC_Grid.cpp for the implementation of this class
//

class CMOC_GridApp : public CWinApp
{
public:
	CMOC_GridApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMOC_GridApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CMOC_GridApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MOC_GRID_H__DE1DED4D_34BE_4CE4_8EC9_0A85E0A5D25A__INCLUDED_)
