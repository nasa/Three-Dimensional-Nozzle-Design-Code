// STT2001.h : main header file for the STT2001 application
//

#if !defined(AFX_STT2001_H__779DC648_7451_4685_8985_BFCD147F15FD__INCLUDED_)
#define AFX_STT2001_H__779DC648_7451_4685_8985_BFCD147F15FD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CSTT2001App:
// See STT2001.cpp for the implementation of this class
//

class CSTT2001App : public CWinApp
{
public:
	CSTT2001App();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSTT2001App)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CSTT2001App)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STT2001_H__779DC648_7451_4685_8985_BFCD147F15FD__INCLUDED_)
