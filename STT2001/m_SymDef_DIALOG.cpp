// m_SymDef_DIALOG.cpp : implementation file
//

#include "stdafx.h"
#include "STT2001.h"
#include "m_SymDef_DIALOG.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// m_SymDef_DIALOG dialog


m_SymDef_DIALOG::m_SymDef_DIALOG(CWnd* pParent /*=NULL*/)
	: CDialog(m_SymDef_DIALOG::IDD, pParent)
{
	//{{AFX_DATA_INIT(m_SymDef_DIALOG)
	m_RSim = 0.0;
	m_YSim = 0.0;
	m_ZSim = 0.0;
	m_nRev = 3;
	m_SL1Match = 0;
	m_SL2Match = 0;
	//}}AFX_DATA_INIT
}


void m_SymDef_DIALOG::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(m_SymDef_DIALOG)
	DDX_Text(pDX, IDC_RSim_EDIT, m_RSim);
	DDX_Text(pDX, IDC_YSim_EDIT, m_YSim);
	DDX_Text(pDX, IDC_ZSim_EDIT, m_ZSim);
	DDX_Text(pDX, IDC_nRev_EDIT, m_nRev);
	DDX_Text(pDX, IDC_SL1Match_EDIT, m_SL1Match);
	DDX_Text(pDX, IDC_SL2Match_EDIT, m_SL2Match);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(m_SymDef_DIALOG, CDialog)
	//{{AFX_MSG_MAP(m_SymDef_DIALOG)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// m_SymDef_DIALOG message handlers
