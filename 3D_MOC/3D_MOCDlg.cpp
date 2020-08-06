// 3D_MOCDlg.cpp : implementation file
//

#include "stdafx.h"
#include "3D_MOC.h"
#include "3D_MOCDlg.h"
#include "nr.h"
#include "UserMessages.h"
using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMy3D_MOCDlg dialog

CMy3D_MOCDlg::CMy3D_MOCDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMy3D_MOCDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMy3D_MOCDlg)
	m_gamma0 = 1.4;
	m_geomFile = _T("ideal.geo");
	m_mach0 = 1.1;
	m_molWt0 = 28.96;
	m_nZ = 11;
	m_p0 = 1000;
	m_summaryFile = _T("Summary.out");
	m_t0 = 530;
	m_z0 = 0.0;
	m_psi0 = 0.0;
	m_theta0 = 0.0;
	m_xOutputIncrement = 1;
	m_yOutputIncrement = 1;
	m_zOutputIncrement = 10;
	m_nX = 0.0;
	m_nDiv = 36;
	m_totalSteps = 0;
	m_SurfaceFit = _T("All Point Spline");
	m_step = 0;
	m_stepStep = 999;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMy3D_MOCDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMy3D_MOCDlg)
	DDX_Control(pDX, IDC_exe_BUTTON, m_exe_BUTTON);
	DDX_Text(pDX, IDC_gamma0_EDIT1, m_gamma0);
	DDV_MinMaxDouble(pDX, m_gamma0, 0., 3.);
	DDX_Text(pDX, IDC_geomFile_EDIT, m_geomFile);
	DDX_Text(pDX, IDC_mach0_EDIT, m_mach0);
	DDV_MinMaxDouble(pDX, m_mach0, 0., 999.);
	DDX_Text(pDX, IDC_molWt0_EDIT, m_molWt0);
	DDX_Text(pDX, IDC_nZ_EDIT, m_nZ);
	DDV_MinMaxInt(pDX, m_nZ, 10, 1000);
	DDX_Text(pDX, IDC_p0_EDIT, m_p0);
	DDV_MinMaxDouble(pDX, m_p0, 0., 9000000000.);
	DDX_Text(pDX, IDC_summaryFile_EDIT, m_summaryFile);
	DDX_Text(pDX, IDC_t0_EDIT, m_t0);
	DDX_Text(pDX, IDC_z0_EDIT, m_z0);
	DDV_MinMaxDouble(pDX, m_z0, 0., 9000000000.);
	DDX_Text(pDX, IDC_psi0_EDIT, m_psi0);
	DDX_Text(pDX, IDC_theta0_EDIT, m_theta0);
	DDX_Text(pDX, IDC_xOuputIncrement_EDIT, m_xOutputIncrement);
	DDV_MinMaxInt(pDX, m_xOutputIncrement, 1, 1000);
	DDX_Text(pDX, IDC_yOutputIncrement_EDIT, m_yOutputIncrement);
	DDV_MinMaxInt(pDX, m_yOutputIncrement, 1, 1000);
	DDX_Text(pDX, IDC_zOutputIncrement_EDIT, m_zOutputIncrement);
	DDV_MinMaxInt(pDX, m_zOutputIncrement, 0, 1000);
	DDX_Text(pDX, IDC_nX_EDIT, m_nX);
	DDX_Text(pDX, IDC_nDiv_EDIT, m_nDiv);
	DDV_MinMaxInt(pDX, m_nDiv, 12, 360);
	DDX_Text(pDX, IDC_total_EDIT, m_totalSteps);
	DDX_CBString(pDX, IDC_SurfaceFit_COMBO, m_SurfaceFit);
	DDX_Text(pDX, IDC_step_EDIT, m_step);
	DDX_Text(pDX, IDC_stepStep_EDIT, m_stepStep);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMy3D_MOCDlg, CDialog)
	//{{AFX_MSG_MAP(CMy3D_MOCDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_exe_BUTTON, OnexeBUTTON)
	//}}AFX_MSG_MAP
	ON_MESSAGE( WM_COUNTER_UPDATE, UpdateStepCounter)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMy3D_MOCDlg message handlers

BOOL CMy3D_MOCDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMy3D_MOCDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMy3D_MOCDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMy3D_MOCDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CMy3D_MOCDlg::OnexeBUTTON()
{
	UpdateData(TRUE);
	pMocThread = (C3D_MOCGridThread*)AfxBeginThread(RUNTIME_CLASS(C3D_MOCGridThread));
	pMocThread->moc.SetPrintMode(1, m_xOutputIncrement, m_yOutputIncrement, m_zOutputIncrement,
		m_stepStep);
	pMocThread->moc.SetSplineFit(m_SurfaceFit);
	if (pMocThread->moc.SetInitialPropertiesForCircularThroat(m_p0, m_t0, m_mach0, m_molWt0, m_gamma0,
		m_theta0, m_psi0, m_nDiv, m_geomFile))

		// Tharen Rice: May 2020. Instead of calling the next function, I bypassed it
		// and called the function that it called.  Maybe this will work.
		pMocThread->CalculateNozzle((WPARAM)this, (LPARAM)NULL);

		//pMocThread->PostThreadMessage(WM_CALC_NOZZLE, (WPARAM)this, (LPARAM)NULL);
	return;
}

//Tharen Rice May 2020
//void CMy3D_MOCDlg::UpdateStepCounter(WPARAM w,LPARAM l)
LRESULT CMy3D_MOCDlg::UpdateStepCounter(WPARAM w, LPARAM l)
{
	int *pStep, *pTotal;

	pStep = (int*) w;
	pTotal = (int*) l;
	m_step = *pStep;
	m_totalSteps = *pTotal-1;
	
	UpdateData(FALSE);
//	delete pStep;
//	delete pTotal;
	return 0;
}
