// DlgProxy.cpp : implementation file
//

#include "stdafx.h"
#include "MOC_Grid.h"
#include "DlgProxy.h"
#include "MOC_GridDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMOC_GridDlgAutoProxy

IMPLEMENT_DYNCREATE(CMOC_GridDlgAutoProxy, CCmdTarget)

CMOC_GridDlgAutoProxy::CMOC_GridDlgAutoProxy()
{
	EnableAutomation();
	
	// To keep the application running as long as an automation 
	//	object is active, the constructor calls AfxOleLockApp.
	AfxOleLockApp();

	// Get access to the dialog through the application's
	//  main window pointer.  Set the proxy's internal pointer
	//  to point to the dialog, and set the dialog's back pointer to
	//  this proxy.
	ASSERT (AfxGetApp()->m_pMainWnd != NULL);
	ASSERT_VALID (AfxGetApp()->m_pMainWnd);
	ASSERT_KINDOF(CMOC_GridDlg, AfxGetApp()->m_pMainWnd);
	m_pDialog = (CMOC_GridDlg*) AfxGetApp()->m_pMainWnd;
	m_pDialog->m_pAutoProxy = this;
}

CMOC_GridDlgAutoProxy::~CMOC_GridDlgAutoProxy()
{
	// To terminate the application when all objects created with
	// 	with automation, the destructor calls AfxOleUnlockApp.
	//  Among other things, this will destroy the main dialog
	if (m_pDialog != NULL)
		m_pDialog->m_pAutoProxy = NULL;
	AfxOleUnlockApp();
}

void CMOC_GridDlgAutoProxy::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	CCmdTarget::OnFinalRelease();
}

BEGIN_MESSAGE_MAP(CMOC_GridDlgAutoProxy, CCmdTarget)
	//{{AFX_MSG_MAP(CMOC_GridDlgAutoProxy)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CMOC_GridDlgAutoProxy, CCmdTarget)
	//{{AFX_DISPATCH_MAP(CMOC_GridDlgAutoProxy)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_IMOC_Grid to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {64AD481E-9EB6-4090-9F6C-D9AB592DD003}
static const IID IID_IMOC_Grid =
{ 0x64ad481e, 0x9eb6, 0x4090, { 0x9f, 0x6c, 0xd9, 0xab, 0x59, 0x2d, 0xd0, 0x3 } };

BEGIN_INTERFACE_MAP(CMOC_GridDlgAutoProxy, CCmdTarget)
	INTERFACE_PART(CMOC_GridDlgAutoProxy, IID_IMOC_Grid, Dispatch)
END_INTERFACE_MAP()

// The IMPLEMENT_OLECREATE2 macro is defined in StdAfx.h of this project
// {1506B334-8117-4B15-A2BA-2C0D344ED59B}
IMPLEMENT_OLECREATE2(CMOC_GridDlgAutoProxy, "MOC_Grid.Application", 0x1506b334, 0x8117, 0x4b15, 0xa2, 0xba, 0x2c, 0xd, 0x34, 0x4e, 0xd5, 0x9b)

/////////////////////////////////////////////////////////////////////////////
// CMOC_GridDlgAutoProxy message handlers
