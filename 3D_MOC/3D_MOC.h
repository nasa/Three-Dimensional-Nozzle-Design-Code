// 3D_MOC.h : main header file for the 3D_MOC application
//

#if !defined(AFX_3D_MOC_H__CE09A04B_C345_4D58_BD11_76A7EC4545F3__INCLUDED_)
#define AFX_3D_MOC_H__CE09A04B_C345_4D58_BD11_76A7EC4545F3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CMy3D_MOCApp:
// See 3D_MOC.cpp for the implementation of this class
//

class CMy3D_MOCApp : public CWinApp
{
public:
	CMy3D_MOCApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMy3D_MOCApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CMy3D_MOCApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_3D_MOC_H__CE09A04B_C345_4D58_BD11_76A7EC4545F3__INCLUDED_)
