// 3D_MOC.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "3D_MOC.h"
#include "3D_MOCDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMy3D_MOCApp

BEGIN_MESSAGE_MAP(CMy3D_MOCApp, CWinApp)
	//{{AFX_MSG_MAP(CMy3D_MOCApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMy3D_MOCApp construction

CMy3D_MOCApp::CMy3D_MOCApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CMy3D_MOCApp object

CMy3D_MOCApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CMy3D_MOCApp initialization

BOOL CMy3D_MOCApp::InitInstance()
{
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
	//Tharen Rice May 2020: commented out the next 2 lines
//#else
	//Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	CMy3D_MOCDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
