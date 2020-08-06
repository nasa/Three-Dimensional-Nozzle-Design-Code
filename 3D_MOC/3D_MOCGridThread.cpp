// 3D_MOCGridThread.cpp : implementation file
//

#include "stdafx.h"
#include "3D_MOC.h"
#include "3D_MOCGridThread.h"

#include "UserMessages.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// C3D_MOCGridThread

IMPLEMENT_DYNCREATE(C3D_MOCGridThread, CWinThread)

C3D_MOCGridThread::C3D_MOCGridThread()
{
}

C3D_MOCGridThread::~C3D_MOCGridThread()
{
}

BOOL C3D_MOCGridThread::InitInstance()
{
	// TODO:  perform and per-thread initialization here
	return TRUE;
}

int C3D_MOCGridThread::ExitInstance()
{
	// TODO:  perform any per-thread cleanup here
	return CWinThread::ExitInstance();
}

// Tharen Rice May 2020: Replaced CWinThread with CDialog
BEGIN_MESSAGE_MAP(C3D_MOCGridThread, CWinThread)
	//{{AFX_MSG_MAP(C3D_MOCGridThread)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}} //AFX_MSG_MAP
	// Tharen Rice May 2020: Commented the next line.  It is actually controlling the 
	// execution of the code.  I have bypassed the Grid Thread function all together to 
	// fix it
	//ON_MESSAGE(WM_CALC_NOZZLE, CalculateNozzle)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// C3D_MOCGridThread message handlers

// Tharen Rice: May 2020.  I was getting a C2440 compile error.  To fix it, I
//  changed the 'void' in the next function call to LRESULT.

LRESULT C3D_MOCGridThread::CalculateNozzle( WPARAM w, LPARAM l)
{
	moc.CalcNozzle( (CDialog*) w);
	return 0;
}