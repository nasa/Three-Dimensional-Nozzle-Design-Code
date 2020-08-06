// STT2001Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "STT2001.h"
#include "STT2001Dlg.h"
/*Tharen Rice 2020 change*/
/*#include "fstream.h"
#include "iomanip.h"*/
#include <fstream>
#include <iomanip>
// Tharen Rice. MAY 2020. Added this to try and make the GetSLData function work.
#include <iostream> 
#include <string> 
using namespace std;
/*END OF CHANGE*/

#include "math.h"
#include "engineering_constants.hpp"
#include "vector.hpp"
#include "float.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//UINT MyThreadProc( LPVOID pParam );
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
// CSTT2001Dlg dialog

CSTT2001Dlg::CSTT2001Dlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSTT2001Dlg::IDD, pParent), nparamPTS(4200)
{
	//{{AFX_DATA_INIT(CSTT2001Dlg)
	m_RC1 = 0;
	m_RC2 = 0;
	m_RC3 = 0.0;
	m_ZC1 = 0.0;
	m_ZC2 = 0.0;
	m_ZC3 = 0.0;
	m_YC1 = 0;
	m_YC2 = 0.0;
	m_YC3 = 0.0;
	m_Alpha1 = 0.0;
	m_Alpha2 = 0;
	m_Alpha3 = 0.0;
	m_Omega1 = 0;
	m_Omega2 = 0;
	m_Omega3 = 0.0;
	m_XEnd1 = 0.0;
	m_XEnd2 = 0;
	m_XEnd3 = 0.0;
	m_XStart1 = 0.0;
	m_XStart2 = 0;
	m_XStart3 = 0.0;
	m_Alpha4 = 0.0;
	m_Omega4 = 0.0;
	m_RC4 = 0.0;
	m_XEnd4 = 0.0;
	m_XStart4 = 0.0;
	m_YC4 = 0.0;
	m_ZC4 = 0.0;
	m_nSL1 = 0;
	m_nSL2 = 0;
	m_nSL3 = 0;
	m_nSL4 = 0;
	m_pAmbient = 14.7;
	m_aThroat = 0;
	m_IspIdeal =100.;
	m_CxxStr = _T("");
	m_IspCalcStr = _T("");
	m_projectedAreaStr = _T("");
	m_surfaceAreaStr = _T("");
	m_MassFlow = 0.0;
	m_RSLEnd = 0.0;
	m_RSLStart = 0.0;
	m_XSLEnd = 0.0;
	m_XSLStart = 0.0;
	m_YSLEnd = 0.0;
	m_YSLStart = 0.0;
	m_RSLStep = 0.0;
	m_XSLStep = 0.0;
	m_YSLStep = 0.0;
	m_AreaRatioStr = _T("");
	m_ExitAreaStr = _T("");
	m_MaxLengthStr = _T("");
	m_MaxLengthC = 0.0;
	m_MaxYStr = _T("");
	m_MinYStr = _T("");
	m_XAtMaxYStr = _T("");
	m_XAtMinYStr = _T("");
	m_FrictionFile = _T("");
	m_GridSF = 1;
	m_RSLCurrentStr = _T("");
	m_XSLCurrentStr = _T("");
	m_YSLCurrentStr = _T("");
	m_XStatus = 0.0;
	m_Alpha5 = 0.0;
	m_nSL5 = 0;
	m_Omega5 = 0.0;
	m_RC5 = 0.0;
	m_XEnd5 = 0.0;
	m_XStart5 = 0.0;
	m_YC5 = 0.0;
	m_ZC5 = 0.0;
	m_filePrefix = _T("STT2001");
	m_MOCSummaryFileName = _T("");
	m_integralPdAStr = _T("");
	m_pExitForceStr = _T("");
	m_throatForceStr = _T("");
	m_frictionForceStr = _T("");
	m_MOCGridFile = _T("");
	m_SLFileName = _T("");
	//}}AFX_DATA_INIT
	
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSTT2001Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSTT2001Dlg)
	DDX_Control(pDX, IDC_contourFlag_CHECK, m_contourFlag_CHECK);
	DDX_Control(pDX, IDC_ZC5_EDIT, m_ZC5_EDIT);
	DDX_Control(pDX, IDC_YC5_EDIT, m_YC5_EDIT);
	DDX_Control(pDX, IDC_XStart5_EDIT, m_XStart5_EDIT);
	DDX_Control(pDX, IDC_XEnd5_EDIT, m_XEnd5_EDIT);
	DDX_Control(pDX, IDC_Throat5_CHECK, m_Throat5_CHECK);
	DDX_Control(pDX, IDC_RC5_EDIT, m_RC5_EDIT);
	DDX_Control(pDX, IDC_Omega5_EDIT, m_Omega5_EDIT);
	DDX_Control(pDX, IDC_nSL5_EDIT, m_nSL5_EDIT);
	DDX_Control(pDX, IDC_Constraint5_CHECK, m_Constraint5_CHECK);
	DDX_Control(pDX, IDC_C5_COMBO, m_C5_COMBO);
	DDX_Control(pDX, IDC_Alpha5_EDIT, m_Alpha5_EDIT);
	DDX_Control(pDX, IDC_Plot_BUTTON, m_Plot_BUTTON);
	DDX_Control(pDX, IDC_MaxLengthC_EDIT, m_MaxLengthC_EDIT);
	DDX_Control(pDX, IDC_MaxLengthC_CHECK, m_MaxLengthC_CHECK);
	DDX_Control(pDX, IDC_PROGRESS, m_Progress_PROGRESS);
	DDX_Control(pDX, IDC_C4_COMBO, m_C4_COMBO);
	DDX_Control(pDX, IDC_C3_COMBO, m_C3_COMBO);
	DDX_Control(pDX, IDC_C2_COMBO, m_C2_COMBO);
	DDX_Control(pDX, IDC_C1_COMBO, m_C1_COMBO);
	DDX_Control(pDX, IDC_Status_EDIT, m_Status_EDIT);
	DDX_Control(pDX, IDC_SymDef_BUTTON, m_SymDef_BUTTON);
	DDX_Control(pDX, IDC_nSL1_EDIT, m_nSL1_EDIT);
	DDX_Control(pDX, IDC_nSL4_EDIT, m_nSL4_EDIT);
	DDX_Control(pDX, IDC_nSL3_EDIT, m_nSL3_EDIT);
	DDX_Control(pDX, IDC_nSL2_EDIT, m_nSL2_EDIT);
	DDX_Control(pDX, IDC_Constraint4_CHECK, m_Constraint4_CHECK);
	DDX_Control(pDX, IDC_ZC4_EDIT, m_ZC4_EDIT);
	DDX_Control(pDX, IDC_YC4_EDIT, m_YC4_EDIT);
	DDX_Control(pDX, IDC_XStart4_EDIT, m_XStart4_EDIT);
	DDX_Control(pDX, IDC_XEnd4_EDIT, m_XEnd4_EDIT);
	DDX_Control(pDX, IDC_Throat4_CHECK, m_Throat4_CHECK);
	DDX_Control(pDX, IDC_Throat3_CHECK, m_Throat3_CHECK);
	DDX_Control(pDX, IDC_Throat2_CHECK, m_Throat2_CHECK);
	DDX_Control(pDX, IDC_Throat1_CHECK, m_Throat1_CHECK);
	DDX_Control(pDX, IDC_RC4_EDIT, m_RC4_EDIT);
	DDX_Control(pDX, IDC_Omega4_EDIT, m_Omega4_EDIT);
	DDX_Control(pDX, IDC_Alpha4_EDIT, m_Alpha4_EDIT);
	DDX_Control(pDX, IDC_XStart3_EDIT, m_XStart3_EDIT);
	DDX_Control(pDX, IDC_XStart2_EDIT, m_XStart2_EDIT);
	DDX_Control(pDX, IDC_XStart1_EDIT, m_XStart1_EDIT);
	DDX_Control(pDX, IDC_XEnd3_EDIT, m_XEnd3_EDIT);
	DDX_Control(pDX, IDC_XEnd2_EDIT, m_XEnd2_EDIT);
	DDX_Control(pDX, IDC_XEnd1_EDIT, m_XEnd1_EDIT);
	DDX_Control(pDX, IDC_Omega3_EDIT, m_Omega3_EDIT);
	DDX_Control(pDX, IDC_Omega2_EDIT, m_Omega2_EDIT);
	DDX_Control(pDX, IDC_Omega1_EDIT, m_Omega1_EDIT);
	DDX_Control(pDX, IDC_EXE_BUTTON, m_EXE_BUTTON);
	DDX_Control(pDX, IDC_Alpha3_EDIT, m_Alpha3_EDIT);
	DDX_Control(pDX, IDC_Alpha2_EDIT, m_Alpha2_EDIT);
	DDX_Control(pDX, IDC_Alpha1_EDIT, m_Alpha1_EDIT);
	DDX_Control(pDX, IDC_YC3_EDIT, m_YC3_EDIT);
	DDX_Control(pDX, IDC_YC2_EDIT, m_YC2_EDIT);
	DDX_Control(pDX, IDC_YC1_EDIT, m_YC1_EDIT);
	DDX_Control(pDX, IDC_ZC3_EDIT, m_ZC3_EDIT);
	DDX_Control(pDX, IDC_ZC2_EDIT, m_ZC2_EDIT);
	DDX_Control(pDX, IDC_ZC1_EDIT, m_ZC1_EDIT);
	DDX_Control(pDX, IDC_RC3_EDIT, m_RC3_EDIT);
	DDX_Control(pDX, IDC_RC2_EDIT, m_RC2_EDIT);
	DDX_Control(pDX, IDC_RC1_EDIT, m_RC1_EDIT);
	DDX_Control(pDX, IDC_Constraint3_CHECK, m_Constraint3_CHECK);
	DDX_Control(pDX, IDC_Constraint2_CHECK, m_Constraint2_CHECK);
	DDX_Control(pDX, IDC_Constraint1_CHECK, m_Constraint1_CHECK);
	DDX_Text(pDX, IDC_RC1_EDIT, m_RC1);
	DDX_Text(pDX, IDC_RC2_EDIT, m_RC2);
	DDX_Text(pDX, IDC_RC3_EDIT, m_RC3);
	DDX_Text(pDX, IDC_ZC1_EDIT, m_ZC1);
	DDX_Text(pDX, IDC_ZC2_EDIT, m_ZC2);
	DDX_Text(pDX, IDC_ZC3_EDIT, m_ZC3);
	DDX_Text(pDX, IDC_YC1_EDIT, m_YC1);
	DDX_Text(pDX, IDC_YC2_EDIT, m_YC2);
	DDX_Text(pDX, IDC_YC3_EDIT, m_YC3);
	DDX_Text(pDX, IDC_Alpha1_EDIT, m_Alpha1);
	DDX_Text(pDX, IDC_Alpha2_EDIT, m_Alpha2);
	DDX_Text(pDX, IDC_Alpha3_EDIT, m_Alpha3);
	DDX_Text(pDX, IDC_Omega1_EDIT, m_Omega1);
	DDX_Text(pDX, IDC_Omega2_EDIT, m_Omega2);
	DDX_Text(pDX, IDC_Omega3_EDIT, m_Omega3);
	DDX_Text(pDX, IDC_XEnd1_EDIT, m_XEnd1);
	DDX_Text(pDX, IDC_XEnd2_EDIT, m_XEnd2);
	DDX_Text(pDX, IDC_XEnd3_EDIT, m_XEnd3);
	DDX_Text(pDX, IDC_XStart1_EDIT, m_XStart1);
	DDX_Text(pDX, IDC_XStart2_EDIT, m_XStart2);
	DDX_Text(pDX, IDC_XStart3_EDIT, m_XStart3);
	DDX_Text(pDX, IDC_Alpha4_EDIT, m_Alpha4);
	DDX_Text(pDX, IDC_Omega4_EDIT, m_Omega4);
	DDX_Text(pDX, IDC_RC4_EDIT, m_RC4);
	DDX_Text(pDX, IDC_XEnd4_EDIT, m_XEnd4);
	DDX_Text(pDX, IDC_XStart4_EDIT, m_XStart4);
	DDX_Text(pDX, IDC_YC4_EDIT, m_YC4);
	DDX_Text(pDX, IDC_ZC4_EDIT, m_ZC4);
	DDX_Text(pDX, IDC_nSL1_EDIT, m_nSL1);
	DDX_Text(pDX, IDC_nSL2_EDIT, m_nSL2);
	DDX_Text(pDX, IDC_nSL3_EDIT, m_nSL3);
	DDX_Text(pDX, IDC_nSL4_EDIT, m_nSL4);
	DDX_Text(pDX, IDC_pAmbient_EDIT, m_pAmbient);
	DDX_Text(pDX, IDC_aThroat_EDIT, m_aThroat);
	DDX_Text(pDX, IDC_IspIdeal_EDIT, m_IspIdeal);
	DDV_MinMaxDouble(pDX, m_IspIdeal, 1.e-004, 1000000000.);
	DDX_Text(pDX, IDC_Cxx_EDIT, m_CxxStr);
	DDX_Text(pDX, IDC_IspCalc_EDIT, m_IspCalcStr);
	DDX_Text(pDX, IDC_ProjectedArea_EDIT, m_projectedAreaStr);
	DDX_Text(pDX, IDC_SurfaceArea_EDIT, m_surfaceAreaStr);
	DDX_Text(pDX, IDC_MassFlow_EDIT, m_MassFlow);
	DDX_Text(pDX, IDC_RSLEnd_EDIT, m_RSLEnd);
	DDX_Text(pDX, IDC_RSLStart_EDIT, m_RSLStart);
	DDX_Text(pDX, IDC_XSLEnd_EDIT, m_XSLEnd);
	DDX_Text(pDX, IDC_XSLStart_EDIT, m_XSLStart);
	DDX_Text(pDX, IDC_YSLEnd_EDIT, m_YSLEnd);
	DDX_Text(pDX, IDC_YSLStart_EDIT, m_YSLStart);
	DDX_Text(pDX, IDC_RSLStep_EDIT, m_RSLStep);
	DDX_Text(pDX, IDC_XSLStep_EDIT, m_XSLStep);
	DDX_Text(pDX, IDC_YSLStep_EDIT, m_YSLStep);
	DDX_Text(pDX, IDC_AreaRatio_EDIT, m_AreaRatioStr);
	DDX_Text(pDX, IDC_ExitArea_EDIT, m_ExitAreaStr);
	DDX_Text(pDX, IDC_MaxLength_EDIT, m_MaxLengthStr);
	DDX_Text(pDX, IDC_MaxLengthC_EDIT, m_MaxLengthC);
	DDX_Text(pDX, IDC_MaxY_EDIT, m_MaxYStr);
	DDX_Text(pDX, IDC_MinY_EDIT, m_MinYStr);
	DDX_Text(pDX, IDC_XAtMaxY_EDIT, m_XAtMaxYStr);
	DDX_Text(pDX, IDC_XAtMinY_EDIT, m_XAtMinYStr);
	DDX_Text(pDX, IDC_FrictionFile_EDIT, m_FrictionFile);
	DDX_Text(pDX, IDC_GridSF_EDIT, m_GridSF);
	DDV_MinMaxInt(pDX, m_GridSF, 1, 20);
	DDX_Text(pDX, IDC_RSLCurrentStr_EDIT, m_RSLCurrentStr);
	DDX_Text(pDX, IDC_XSLCurrentStr_EDIT, m_XSLCurrentStr);
	DDX_Text(pDX, IDC_YSLCurrentStr_EDIT, m_YSLCurrentStr);
	DDX_Text(pDX, IDC_XStatus_EDIT, m_XStatus);
	DDX_Text(pDX, IDC_Alpha5_EDIT, m_Alpha5);
	DDX_Text(pDX, IDC_nSL5_EDIT, m_nSL5);
	DDX_Text(pDX, IDC_Omega5_EDIT, m_Omega5);
	DDX_Text(pDX, IDC_RC5_EDIT, m_RC5);
	DDX_Text(pDX, IDC_XEnd5_EDIT, m_XEnd5);
	DDX_Text(pDX, IDC_XStart5_EDIT, m_XStart5);
	DDX_Text(pDX, IDC_YC5_EDIT, m_YC5);
	DDX_Text(pDX, IDC_ZC5_EDIT, m_ZC5);
	DDX_Text(pDX, IDC_filePrefix_EDIT, m_filePrefix);
	DDX_Text(pDX, IDC_MOCSummaryFile_EDIT, m_MOCSummaryFileName);
	DDX_Text(pDX, IDC_integralPdA_EDIT, m_integralPdAStr);
	DDX_Text(pDX, IDC_pExitForce_EDIT, m_pExitForceStr);
	DDX_Text(pDX, IDC_throatForce_EDIT, m_throatForceStr);
	DDX_Text(pDX, IDC_frictionForce_EDIT, m_frictionForceStr);
	DDX_Text(pDX, IDC_MOCGridFile_EDIT, m_MOCGridFile);
	DDX_Text(pDX, IDC_SLFileName_EDIT, m_SLFileName);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSTT2001Dlg, CDialog)
	//{{AFX_MSG_MAP(CSTT2001Dlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_Constraint1_CHECK, OnConstraint1CHECK)
	ON_BN_CLICKED(IDC_Constraint2_CHECK, OnConstraint2CHECK)
	ON_BN_CLICKED(IDC_Constraint3_CHECK, OnConstraint3CHECK)
	ON_BN_CLICKED(IDC_EXE_BUTTON, OnExeButton)
	ON_BN_CLICKED(IDC_Constraint4_CHECK, OnConstraint4CHECK)
	ON_BN_CLICKED(IDC_Throat1_CHECK, OnThroat1CHECK)
	ON_BN_CLICKED(IDC_Throat2_CHECK, OnThroat2CHECK)
	ON_BN_CLICKED(IDC_Throat3_CHECK, OnThroat3CHECK)
	ON_BN_CLICKED(IDC_Throat4_CHECK, OnThroat4CHECK)
	ON_BN_CLICKED(IDC_SymDef_BUTTON, OnSymDefBUTTON)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_COMMAND(ID_FILE_SAVEAS, OnFileSaveas)
	ON_BN_CLICKED(IDC_MaxLengthC_CHECK, OnMaxLengthCCHECK)
	ON_BN_CLICKED(IDC_Plot_BUTTON, OnPlotBUTTON)
	ON_BN_CLICKED(IDC_Constraint5_CHECK, OnConstraint5CHECK)
	ON_BN_CLICKED(IDC_Throat5_CHECK, OnThroat5CHECK)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSTT2001Dlg message handlers

BOOL CSTT2001Dlg::OnInitDialog()
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
	
	TurnCheckBoxesOff();
	m_Throat1_CHECK.SetCheck(1);
	m_Constraint1_CHECK.SetCheck(1);
	m_C1_COMBO.SetCurSel(1);
	m_C2_COMBO.SetCurSel(0);
	UpdateData(FALSE);
	OnConstraint1CHECK();
	OnThroat1CHECK();
	m_Plot_BUTTON.EnableWindow(0);
	m_Status_EDIT.SetWindowText("Waiting for program execution.");
	CSTT2001Dlg::UpdateWindow();

	nparamGRIDX = 100;
	
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CSTT2001Dlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CSTT2001Dlg::OnPaint() 
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
HCURSOR CSTT2001Dlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CSTT2001Dlg::OnConstraint1CHECK() 
{
	UpdateData(TRUE);
	int i = m_Constraint1_CHECK.GetCheck();
//	m_ZC1_EDIT.EnableWindow(i);
	m_YC1_EDIT.EnableWindow(i);
	m_RC1_EDIT.EnableWindow(i);
	m_Alpha1_EDIT.EnableWindow(i);
	m_Omega1_EDIT.EnableWindow(i);
	m_Throat1_CHECK.EnableWindow(i);
	m_nSL1_EDIT.EnableWindow(i);
	m_XStart1_EDIT.EnableWindow(i);
	m_XEnd1_EDIT.EnableWindow(i);
	m_C1_COMBO.EnableWindow(0);
	OnThroat1CHECK();
	return;
}

void CSTT2001Dlg::OnConstraint2CHECK() 
{
	UpdateData(TRUE);
	int i = m_Constraint2_CHECK.GetCheck();
//	m_ZC2_EDIT.EnableWindow(i);
	m_YC2_EDIT.EnableWindow(i);
	m_RC2_EDIT.EnableWindow(i);
	m_Alpha2_EDIT.EnableWindow(i);
	m_Omega2_EDIT.EnableWindow(i);
	m_Throat2_CHECK.EnableWindow(i);
	m_nSL2_EDIT.EnableWindow(i);
	m_XStart2_EDIT.EnableWindow(i);
	m_XEnd2_EDIT.EnableWindow(i);
	m_C2_COMBO.EnableWindow(0);
	OnThroat2CHECK();
	return;
}

void CSTT2001Dlg::OnConstraint3CHECK() 
{
	UpdateData(TRUE);
	int i = m_Constraint3_CHECK.GetCheck();
//	m_ZC3_EDIT.EnableWindow(i);
	m_YC3_EDIT.EnableWindow(i);
	m_RC3_EDIT.EnableWindow(i);
	m_Alpha3_EDIT.EnableWindow(i);
	m_Omega3_EDIT.EnableWindow(i);
	m_Throat3_CHECK.EnableWindow(i);
	m_nSL3_EDIT.EnableWindow(i);
	m_XStart3_EDIT.EnableWindow(i);
	m_XEnd3_EDIT.EnableWindow(i);
	m_C3_COMBO.EnableWindow(0);
	OnThroat3CHECK();
	return;
}

void CSTT2001Dlg::OnConstraint4CHECK() 
{
	UpdateData(TRUE);
	int i = m_Constraint4_CHECK.GetCheck();
//	m_ZC4_EDIT.EnableWindow(i);
	m_YC4_EDIT.EnableWindow(i);
	m_RC4_EDIT.EnableWindow(i);
	m_Alpha4_EDIT.EnableWindow(i);
	m_Omega4_EDIT.EnableWindow(i);
	m_Throat4_CHECK.EnableWindow(i);
	m_nSL4_EDIT.EnableWindow(i);
	m_XStart4_EDIT.EnableWindow(i);
	m_XEnd4_EDIT.EnableWindow(i);
	m_C4_COMBO.EnableWindow(0);
	OnThroat4CHECK();
	return;
}

void CSTT2001Dlg::OnConstraint5CHECK() 
{
	UpdateData(TRUE);
	int i = m_Constraint5_CHECK.GetCheck();
//	m_ZC5_EDIT.EnableWindow(i);
	m_YC5_EDIT.EnableWindow(i);
	m_RC5_EDIT.EnableWindow(i);
	m_Alpha5_EDIT.EnableWindow(i);
	m_Omega5_EDIT.EnableWindow(i);
	m_Throat5_CHECK.EnableWindow(i);
	m_nSL5_EDIT.EnableWindow(i);
	m_XStart5_EDIT.EnableWindow(i);
	m_XEnd5_EDIT.EnableWindow(i);
	m_C5_COMBO.EnableWindow(0);
	OnThroat5CHECK();
	
}



void CSTT2001Dlg::OnThroat1CHECK() 
{
	UpdateData(TRUE);
	if (m_Constraint1_CHECK.GetCheck()) 
	{
		int i = m_Throat1_CHECK.GetCheck();
		m_XStart1_EDIT.EnableWindow(!i);
		m_XEnd1_EDIT.EnableWindow(!i);
		m_nSL1_EDIT.EnableWindow(i);
		m_C1_COMBO.EnableWindow(i);
	}
	return;
}

void CSTT2001Dlg::OnThroat2CHECK() 
{
	UpdateData(TRUE);
	if (m_Constraint2_CHECK.GetCheck()) 
	{
		int i = m_Throat2_CHECK.GetCheck();
		m_XStart2_EDIT.EnableWindow(!i);
		m_XEnd2_EDIT.EnableWindow(!i);
		m_nSL2_EDIT.EnableWindow(i);
		m_C2_COMBO.EnableWindow(i);
	}
	return;
}

void CSTT2001Dlg::OnThroat3CHECK() 
{
	UpdateData(TRUE);
	if (m_Constraint3_CHECK.GetCheck()) 
	{
		int i = m_Throat3_CHECK.GetCheck();
		m_XStart3_EDIT.EnableWindow(!i);
		m_XEnd3_EDIT.EnableWindow(!i);
		m_nSL3_EDIT.EnableWindow(i);
		m_C3_COMBO.EnableWindow(i);
	}
	return;	
}

void CSTT2001Dlg::OnThroat4CHECK() 
{
	UpdateData(TRUE);
	if (m_Constraint4_CHECK.GetCheck()) 
	{
		int i = m_Throat4_CHECK.GetCheck();
		m_XStart4_EDIT.EnableWindow(!i);
		m_XEnd4_EDIT.EnableWindow(!i);
		m_nSL4_EDIT.EnableWindow(i);
		m_C4_COMBO.EnableWindow(i);
	}
	return;	
}

void CSTT2001Dlg::OnThroat5CHECK() 
{
	UpdateData(TRUE);
	if (m_Constraint5_CHECK.GetCheck()) 
	{
		int i = m_Throat5_CHECK.GetCheck();
		m_XStart5_EDIT.EnableWindow(!i);
		m_XEnd5_EDIT.EnableWindow(!i);
		m_nSL5_EDIT.EnableWindow(i);
		m_C5_COMBO.EnableWindow(i);
	}
	return;	
	
}


void CSTT2001Dlg::OnExeButton( void) 
{
	int goFlag, totalSteps, SLCount = 999;
	double isp,yXStatus = 0, thetaXStatus= 0;
	//	This function is the work horse of the program. 
	UpdateData(TRUE);
	m_EXE_BUTTON.EnableWindow(0);
	m_Plot_BUTTON.EnableWindow(0);

	//	Determine the number of the SLs to initialize by counting only the enabled SLS
	if ( m_Constraint1_CHECK.GetCheck() && m_Throat1_CHECK.GetCheck()) SLCount += m_nSL1;
	if ( m_Constraint2_CHECK.GetCheck() && m_Throat2_CHECK.GetCheck()) SLCount += m_nSL2;
	if ( m_Constraint3_CHECK.GetCheck() && m_Throat3_CHECK.GetCheck()) SLCount += m_nSL3;
	if ( m_Constraint4_CHECK.GetCheck() && m_Throat4_CHECK.GetCheck()) SLCount += m_nSL4;
	if ( m_Constraint5_CHECK.GetCheck() && m_Throat5_CHECK.GetCheck()) SLCount += m_nSL5;
	// this value sets the number of grid points in the axial direction
	nparamGRIDX *= m_GridSF;
	InitializeArrays(SLCount);

	if ( m_SymDefDlg.m_SL1Match > SLCount ||  m_SymDefDlg.m_SL2Match > SLCount)
	{
		AfxMessageBox("SL Matching values 1 & 2 must be < total SLs");
		exit(1);
	}
	//	First: get the SL data from the input data file
	fstream loopFile;
	CString fileStr = m_filePrefix;
	fileStr += "_all_runs.dat";
	// Tharen Rice May 2020. Replaced the commented line with a new one
	//loopFile.open(fileStr, ios::out, filebuf::sh_none  );
	loopFile.open(fileStr, ios::out);
	// END of Change

	loopFile << "Prefix \t ZSL \t XSL \t YSL \t RSL \t ISP \t Cfg \t Surface Area (in2) \t" 
	 << "Throat Area (in2)\t Axial Projected Area (in2)\t Exit Area (in2)\t" 
	 << "Area Ratio\t Area Ratio(MOC)\t Mass Flow (lbm/s)\t" 
	 << "Throat Momentum (lbf)\t Pressure Force (lbf)\t Pa Ae Loss (lbf)\t" 
	 << "Friction Loss (lbf) \t Max Length (in) \t Min Y(in) \t Max Y(in)\t" 
	 << "X @ Min Y(in) \t X @ Max Y(in) \t X Status (in) \t XStatus Ymin (in)\t"
	 << "XStatus Theta @ Min Y (deg)" << endl; 
	m_Status_EDIT.SetWindowText("Retrieving Streamline Data from input file.");
	CSTT2001Dlg::UpdateWindow();
	GetSLData();
	
	// Here loop through the various SL definitions.  For each SL, reset the calculated
	//	values
	
//	if ( m_ZSLStep == 0.0) m_ZSLStep = m_ZSLEnd - m_ZSLStart;
	if ( m_RSLStep == 0.0) m_RSLStep = m_RSLEnd - m_RSLStart;
	if ( m_YSLStep == 0.0) m_YSLStep = m_YSLEnd - m_YSLStart;
	if ( m_XSLStep == 0.0) m_XSLStep = m_XSLEnd - m_XSLStart;
	totalSteps = 1;
//	if ( m_ZSLStep != 0.0) totalSteps *= int((m_ZSLEnd - m_ZSLStart)/m_ZSLStep+1);
	if ( m_RSLStep != 0.0) totalSteps *= int((m_RSLEnd - m_RSLStart)/m_RSLStep+1); 
	if ( m_YSLStep != 0.0) totalSteps *= int((m_YSLEnd - m_YSLStart)/m_YSLStep+1);
	if ( m_XSLStep != 0.0) totalSteps *= int((m_XSLEnd - m_XSLStart)/m_XSLStep+1);
	m_Progress_PROGRESS.SetRange(0,totalSteps);
	m_Progress_PROGRESS.SetPos(0);
	m_Progress_PROGRESS.SetStep(1);

/*	for ( m_ZSL = m_ZSLStart; m_ZSL <= m_ZSLEnd; m_ZSL += m_ZSLStep)
	{*/	
	m_ZSL = 0.0;
	for ( m_RSL = m_RSLStart; m_RSL <= m_RSLEnd; m_RSL += m_RSLStep)
	{	
	for ( m_YSL = m_YSLStart; m_YSL <= m_YSLEnd; m_YSL += m_YSLStep)
	{	
	for ( m_XSL = m_XSLStart; m_XSL <= m_XSLEnd; m_XSL += m_XSLStep)
	{
		//	Initialize all the calculated variables to 0.0
		m_Force = 0.0;
		m_SurfaceArea= 0.0;
		m_ProjectedArea = 0.0;
		m_IspCalc = 0.0;
		m_thrust1= 0.0; 
		m_fricLoss= 0.0; 
		m_pALoss= 0.0; 
		m_aExit= 0.0; 
		m_epsMOC= 0.0; 
		m_IspCalc= 0.0;
		nNewSLs = 0;
		maxLength = 0.0;

		//	Update the current values boxes
		m_RSLCurrentStr.Format( "%.2f", m_RSL);
		m_XSLCurrentStr.Format( "%.2f", m_XSL);
		m_YSLCurrentStr.Format( "%.2f", m_YSL);

	//	At this point the streamline data is known for all of the streamlines
	//	at the THETA = 0 rotation (at Y=0 on the XY plane). It is assumed that the
	//	throat constraints are at the same X and the first point along the streamlines
	//	xsl[i][0].
	//	Next calculate the streamlines that define the nozzle throat at every THETA location
	//	as defined by '360/m_Theta'
	m_Status_EDIT.SetWindowText("Calculating Throat streamlines.");
	CSTT2001Dlg::UpdateWindow();
	goFlag = 1;
	if ( m_Constraint1_CHECK.GetCheck() && m_Throat1_CHECK.GetCheck() && goFlag) 
		goFlag = CalcThroatSLs( m_YC1, m_ZC1, m_RC1, m_Alpha1, m_Omega1, m_nSL1, m_C1_COMBO.GetCurSel());
	if ( m_Constraint2_CHECK.GetCheck() && m_Throat2_CHECK.GetCheck() && goFlag) 
		goFlag = CalcThroatSLs( m_YC2, m_ZC2, m_RC2, m_Alpha2, m_Omega2, m_nSL2, m_C2_COMBO.GetCurSel());
	if ( m_Constraint3_CHECK.GetCheck() && m_Throat3_CHECK.GetCheck() && goFlag) 
		goFlag = CalcThroatSLs( m_YC3, m_ZC3, m_RC3, m_Alpha3, m_Omega3, m_nSL3, m_C3_COMBO.GetCurSel());
	if ( m_Constraint4_CHECK.GetCheck() && m_Throat4_CHECK.GetCheck() && goFlag) 
		goFlag = CalcThroatSLs( m_YC4, m_ZC4, m_RC4, m_Alpha4, m_Omega4, m_nSL4, m_C4_COMBO.GetCurSel());
	if ( m_Constraint5_CHECK.GetCheck() && m_Throat5_CHECK.GetCheck() && goFlag) 
		goFlag = CalcThroatSLs( m_YC5, m_ZC5, m_RC5, m_Alpha5, m_Omega5, m_nSL5, m_C5_COMBO.GetCurSel());
	
	isp = 0;
	
	//	Output the full, untrimmed throat SLs. This has to be done, because the
	//	variables that are output are eventually trimmed.  DO this is only one run 
	//	is being calculated
	if (totalSteps == 1)
	{
		m_Status_EDIT.SetWindowText("Outputing SLs to X_ThroatSL.out file");
		CSTT2001Dlg::UpdateWindow();
		OutputThroatSLsToFile( 200);
	}

	if (goFlag)
	{
		int cFlag=0;
		m_Status_EDIT.SetWindowText("Trimming SLs for given constraints.");
		CSTT2001Dlg::UpdateWindow();
		//	Now trim the throat streamlines to the constraints
		if ( m_Constraint1_CHECK.GetCheck() && !m_Throat1_CHECK.GetCheck())
		{
			cFlag = 1;
			TrimSLs( m_YC1, m_ZC1, m_RC1, m_Alpha1, m_Omega1, m_XStart1, m_XEnd1);
		}
		if ( m_Constraint2_CHECK.GetCheck() && !m_Throat2_CHECK.GetCheck()) 
		{
			cFlag = 1;
			TrimSLs( m_YC2, m_ZC2, m_RC2, m_Alpha2, m_Omega2, m_XStart2, m_XEnd2);
		}
		if ( m_Constraint3_CHECK.GetCheck() && !m_Throat3_CHECK.GetCheck()) 
		{
			cFlag = 1;
			TrimSLs( m_YC3, m_ZC3, m_RC3, m_Alpha3, m_Omega3, m_XStart3, m_XEnd3);
		}
		if ( m_Constraint4_CHECK.GetCheck() && !m_Throat4_CHECK.GetCheck()) 
		{
			cFlag = 1;
			TrimSLs( m_YC4, m_ZC4, m_RC4, m_Alpha4, m_Omega4, m_XStart4, m_XEnd4);
		}
		if ( m_Constraint5_CHECK.GetCheck() && !m_Throat5_CHECK.GetCheck()) 
		{
			cFlag = 1;
			TrimSLs( m_YC5, m_ZC5, m_RC5, m_Alpha5, m_Omega5, m_XStart5, m_XEnd5);
		}

		// If none of the constraints are checked, set a huge constraint that cannot be
		//	met.  This will allow for all the calculations to be done correctly.
		if ( !cFlag) TrimSLs ( 0.0, 0.0, 999999., 0.0, 0.0, -999999., 999999.);

		//	Use the MOC information available to get the maximum X where the 
		//	MOC grid is no longer valid because of the trimmed SLs.
		double maxX = -1;
		if (m_contourFlag_CHECK.GetCheck()) maxX = FindMaxX(nNewSLs);
		//	Trim SLs to length if the max length constraint is checked
		if (m_MaxLengthC_CHECK.GetCheck())
		{
			if ( maxX < 0.0) TrimSLsToMaxLength(m_MaxLengthC);
			else TrimSLsToMaxLength(__min(m_MaxLengthC,maxX));
		}
		else if (!m_MaxLengthC_CHECK.GetCheck())
			if ( maxX > 0.0) TrimSLsToMaxLength(maxX);


		m_Status_EDIT.SetWindowText("Converting SLs to axial grid.");
		CSTT2001Dlg::UpdateWindow();
		CalcGridSLs();
		
		if ( m_SymDefDlg.m_nRev > 1) 
		{
			m_Status_EDIT.SetWindowText("Trimming SLs based on revolution inputs.");
			CSTT2001Dlg::UpdateWindow();
			TrimSLsDueToAxiRevolution( m_SymDefDlg.m_nRev, m_SymDefDlg.m_YSim,
			m_SymDefDlg.m_ZSim, m_SymDefDlg.m_RSim);
		}
	
		m_Status_EDIT.SetWindowText("Integrating nozzle pressures.");
		CSTT2001Dlg::UpdateWindow();
		CalcNozzleParameters();
		m_Status_EDIT.SetWindowText("Calculating nozzle peformance.");
		CSTT2001Dlg::UpdateWindow();
		UpdateData(FALSE);
		
		if ( m_NAtMinY < nNewSLs && m_NAtMinY >= 0) 
			CalcXStatus(nparamGRIDX, m_XStatus, xgrid[m_NAtMinY], 
					ygrid[m_NAtMinY], yXStatus, thetaXStatus);
		GetPerformanceDataFromMOCSummaryFile();
		CSTT2001Dlg::UpdateWindow();
	}	// End of if GO flag
	else
	{
		m_Status_EDIT.SetWindowText("Could not calculate Throat Streamlines.");
		CSTT2001Dlg::UpdateWindow();
	}
		
	
	loopFile << m_SLFileName << "\t" << m_ZSL << "\t" << m_XSL << "\t" << m_YSL << "\t" << m_RSL << "\t" 
		<< m_IspCalc << "\t" << m_IspCalc/m_IspIdeal << "\t" << m_SurfaceArea << "\t" << m_aThroat << "\t" 
		<< m_ProjectedArea << "\t" << m_aExit << "\t" << m_aExit/m_aThroat 
		<< "\t" << m_epsMOC << "\t" << m_MassFlow << "\t" << m_thrust1 << "\t" << m_Force 
		<< "\t" << -m_pALoss << "\t" << -m_fricLoss << "\t" << maxLength << "\t" << minY 
		<< "\t" << maxY << "\t" << m_XAtMinY << "\t" << m_XAtMaxY << "\t" 
		<< m_XStatus << "\t" << yXStatus << "\t" << thetaXStatus << endl;
	m_Progress_PROGRESS.StepIt();
	
	}}} // end of loop
	loopFile.close();
	if (goFlag)
	{
		m_Status_EDIT.SetWindowText("Outputing SLs to X_TrimmedSL.out file");
		CSTT2001Dlg::UpdateWindow();
		OutputTrimmedSLsToFile( 300);
		CSTT2001Dlg::UpdateWindow();
	
		//	Send the plot data to the plot dialog.  This must be done before the SL arrays are
		//	deleted. 
		m_PlotDlg.Initialize2DChartData( nparamGRIDX, xgrid[m_NAtMaxY], ygrid[m_NAtMaxY], 
			xgrid[m_NAtMinY], ygrid[m_NAtMinY]);
		OutputCenterlinePlot(nparamGRIDX, xgrid[m_NAtMaxY], ygrid[m_NAtMaxY], 
			xgrid[m_NAtMinY], ygrid[m_NAtMinY]);
		m_Status_EDIT.SetWindowText("Calculation complete.");
		
		m_Plot_BUTTON.EnableWindow(1);
	}
	nparamGRIDX = 100;
	DeleteArrays(SLCount);
	m_EXE_BUTTON.EnableWindow(1);
		CSTT2001Dlg::UpdateWindow();
	return;	
}

void CSTT2001Dlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}

void CSTT2001Dlg::OnOK() 
{
	// TODO: Add extra validation here
	
	CDialog::OnOK();
}

void CSTT2001Dlg::TurnCheckBoxesOff()
{
	m_Constraint1_CHECK.SetCheck(0);
	m_Constraint2_CHECK.SetCheck(0);
	m_Constraint3_CHECK.SetCheck(0);
	m_Constraint4_CHECK.SetCheck(0);
	m_Constraint5_CHECK.SetCheck(0);
	m_ZC1_EDIT.EnableWindow(0);
	m_ZC2_EDIT.EnableWindow(0);
	m_ZC3_EDIT.EnableWindow(0);
	m_ZC4_EDIT.EnableWindow(0);
	m_ZC5_EDIT.EnableWindow(0);
	m_RC1_EDIT.EnableWindow(0);
	m_RC2_EDIT.EnableWindow(0);
	m_RC3_EDIT.EnableWindow(0);
	m_RC4_EDIT.EnableWindow(0);
	m_RC5_EDIT.EnableWindow(0);
	m_YC1_EDIT.EnableWindow(0);
	m_YC2_EDIT.EnableWindow(0);
	m_YC3_EDIT.EnableWindow(0);
	m_YC4_EDIT.EnableWindow(0);
	m_YC5_EDIT.EnableWindow(0);
	m_Alpha1_EDIT.EnableWindow(0);
	m_Alpha2_EDIT.EnableWindow(0);
	m_Alpha3_EDIT.EnableWindow(0);
	m_Alpha4_EDIT.EnableWindow(0);
	m_Alpha5_EDIT.EnableWindow(0);
	m_Omega1_EDIT.EnableWindow(0);
	m_Omega2_EDIT.EnableWindow(0);
	m_Omega3_EDIT.EnableWindow(0);
	m_Omega4_EDIT.EnableWindow(0);
	m_Omega5_EDIT.EnableWindow(0);
	m_nSL1_EDIT.EnableWindow(0);
	m_nSL2_EDIT.EnableWindow(0);
	m_nSL3_EDIT.EnableWindow(0);
	m_nSL4_EDIT.EnableWindow(0);
	m_nSL5_EDIT.EnableWindow(0);
	m_XStart1_EDIT.EnableWindow(0);
	m_XStart2_EDIT.EnableWindow(0);
	m_XStart3_EDIT.EnableWindow(0);
	m_XStart4_EDIT.EnableWindow(0);
	m_XStart5_EDIT.EnableWindow(0);
	m_XEnd1_EDIT.EnableWindow(0);
	m_XEnd2_EDIT.EnableWindow(0);
	m_XEnd3_EDIT.EnableWindow(0);
	m_XEnd4_EDIT.EnableWindow(0);
	m_XEnd5_EDIT.EnableWindow(0);
	m_Throat1_CHECK.EnableWindow(0);
	m_Throat2_CHECK.EnableWindow(0);
	m_Throat3_CHECK.EnableWindow(0);
	m_Throat4_CHECK.EnableWindow(0);
	m_Throat5_CHECK.EnableWindow(0);
	m_C1_COMBO.EnableWindow(0);
	m_C2_COMBO.EnableWindow(0);
	m_C3_COMBO.EnableWindow(0);
	m_C4_COMBO.EnableWindow(0);
	m_C5_COMBO.EnableWindow(0);
	return;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CSTT2001Dlg::GetSLData()
{
	//	This function extracts the SL data (X,R,PSI) from the input data file
	//  This file has the .plt suffix
	CString fileStr = m_SLFileName;
	//Tharen Rice MAy 2020 change
	//fstream ifile(fileStr,ios::nocreate | ios::in);
	fstream ifile(fileStr,  ios::in);
	// End of Change
	if(!ifile.is_open())
	{
		fileStr += " does not exist";
		AfxMessageBox(fileStr);
		exit(1);
	}

	//	The data file contains SL data in the form of
	//	Line 1-3: Dummy header
	//	zone t Lines: massflow function 0: centerline  100:wall
	//	Other lines: x,dummy1,dummy2,r,M,p,t,rho,theta,g,mdot,j
	
	//char cPeek;
	char cstr[10];
 	string strLine;
	int i,k,j;
	double x,r,M,p,t,rho,theta,g, mdot, dummy1, dummy2;
	
	// Tharen Rice. May 2020. Find the 1st set of data. It begins with the "zone t=" line
	// skip the 1st 3 lines
	getline(ifile,strLine);
	getline(ifile, strLine);
	getline(ifile, strLine);
	i = -1;
	// This function will return 0 if not found
	/*while(strLine.find("zone t="))
	//while ( cLine.Left(7) != "zone t=") 
	{
		getline(ifile,strLine);
		if (ifile.eof())
		{
			AfxMessageBox("Could not get SL Data");
			exit(1);
		}
	}
	// Extract the PSI Value from the zone line
	//	Get the first PSI value
	for (int jjk = 0; jjk < 3; jjk++)
	{
		cstr[jjk] = strLine[21 + jjk];
	}	
	psi[i] = atof(cstr);*/
	k = 0;

	// Keep reading the data. When you get to another zone t= section, stop and record the psi value.
	// Go to the next line
	//getline(ifile, strLine);
	//	now get the x,r data the SL until you reach the end of the file.
	//	Set the x1 and x2 values to dummy values
	x1 = 9e9;
	x2 = -9e9;
	while ( !ifile.eof())
	{
		// peek at the next line.  If it does not start with z, then output the data to variable
		//getline(ifile, strLine);
		// If the line starts with z, the record the next psi value
		char zstr = ifile.peek();
		if (zstr == 'z')
		{
			i++;

			// Read the line 
			getline(ifile, strLine);
			for (int jjk = 0; jjk < 3; jjk++)
			{
				cstr[jjk] = strLine[21 + jjk];
			}
			psi[i] = atof(cstr);
			k = 0;
		}
		else
		{
			//extract the data from the file as below
			ifile >> x >> dummy1 >> dummy2 >> r >> M >> p >> t >> rho >> theta >> g >> mdot >> j >> ws;
			// I am using this to get to the next line. I used to use eatwhite, but that function went away.
		//	getline(ifile, strLine);
			// i: streamline number
			// k: point on the streamline
			xsl[i][k] = x;
			rsl[i][k] = r;
			msl[i][k] = M;
			psl[i][k] = p;
			tsl[i][k] = t;
			thsl[i][k] = theta;
			jsl[i][k] = j;
			lastSLPt[i] = k++;
			x1 = __min(x1,x);
			x2 = __max(x2,x);
		}
	} // end of reading data
	nSL = i+1;  // number of streamlines
 	return;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int CSTT2001Dlg::CalcThroatSLs( double yc, double zc, double rc, double alpha, 
								double omega, int nThetaSLS, int isurface)
{
	//	This function is called to determine the streamlines that intersect the throat plane
	//	of the known geometry.  At this point the streamline data, psi[i], xsl[i][j], 
	//	rsl[i][j] and pointsSL[i] are known.  For each rotation angle 360/nRotations find the 
	//	streamline that intersects with the equation defining the throat constraint.  More than
	//	likely, the SL will have to be interpolated (linear).
	//	Based on inputs, the surface that is wanted is either the inner or outer side of
	//	the nozzle
	//	inner: isurface = 0 ,  it will be changed to -1
	//	outer: isurface = 1
	double dTheta, theta, y,z,r, dx, dr, rtmp, rt, ptmp, beta;
	int i,j,k,n,p;
	circleFlag = 0;
	//	Calculate the rotation step in degrees
	if ( fabs(omega - alpha) >= 360)
	{
		alpha = 0.;
		dTheta = 360./nThetaSLS;
		omega = 360. - dTheta;
		circleFlag = 1;
	}
	else dTheta = (omega - alpha)/ (nThetaSLS - 1);

	//	Step through each rotation to find the streamline whose first point intersects the throat.
	n = nNewSLs-1; // sets the current n value to current number of SLs
	
	for ( p = 0; p < nThetaSLS; p++)
	{
		theta = alpha + p*dTheta;
		n++;

		//	Determine if the SLs are going clockwise or counter-clockwise.  If dtheta
		//	< 0, then it is clockwise(1) else its counterclockwise (-1);
		//	Upper Surface isurface = 1, clockwise W = -UxV
		//	Upper Surface, counter   W = UxV
		//	Lower Surface isurface = -1, clockwise W = UxV
		//	Lower Surface, counter   W = -UxV
		//	If it is the lower surface, set isurface = -1;
		if (isurface == 0) isurface = -1;
		if ( dTheta != fabs(dTheta)) rotateSL[n] = isurface;
		else rotateSL[n] = -isurface; 
		//	for the first point of each streamline, calculate the radius using the equation of the circle
		//	defined by the constraint.  If the calculated radius is higher, then interpolate between this
		//	SL and the one preceeding it to find the SL that intersects the throat.
		for ( i = 0; i < nSL; i++)
		{
			//	Y and Z are w/rt an origin at 0,0. 
			y = rc * cos(theta*RAD_PER_DEG) + yc;
			z = rc * sin(theta*RAD_PER_DEG) + zc;
			//	calculate the angle from the constraint point to the SL center, this will be used
			//	later to determine the points along the SL
			beta = atan2(z-m_ZSL,y-m_YSL);
			// r = the radius of the x,y point w/rt the center of the SL flowfield
			r = sqrt((y-m_YSL)*(y-m_YSL) + (z-m_ZSL)*(z-m_ZSL));
		
			if ( r <= rsl[i][0]*m_RSL) // rsl[i][0] is beyond constraint
			{
				//	The current throat radius r is between SL i and i-1.  Calculate the term dr,
				//	the ration between the i and i-1 to be used later in the interpolation process.
				//	Also calculate the new mass flow parameter, newPsi based on dr
				if ( i > 0) 
				{
					dr = (rsl[i][0] - r/m_RSL)/(rsl[i][0] - rsl[i-1][0]);
					newPsi[n] = psi[i] - dr * (psi[i] - psi[i-1]);
				}
				else
				{
					//	i = 0. The calculated r is less than or equal to the first SL.
					//	i = 0. rsl[i-1][0] = rsl[-1][0] = 0.0;
					//	i = 0. psi[i-1]    = psi[-1]    = 0.0;
					dr = (rsl[i][0] - r/m_RSL)/rsl[i][0];
					newPsi[n] = psi[i] *(1 - dr);
				}

				//	For the throat plane, the y, z and p values are equal to the values
				//	already calculated
				xt[n][0] = xsl[i][0]*m_RSL + m_XSL;
				yt[n][0] = y;
				zt[n][0] = z;
				pt[n][0] = psl[i][0];  //Constant pressure at throat plane
				nt[n] = i;
				
				//	For each X coordinate on the Ith SL, find the all the points 
				//	Also, all the points should be w/rt a 0,0 origin
				for ( j = 1; j <= lastSLPt[i]; j++)
				{
					//  The x values of the throat SL ,xt[n][j] will be set equal to the 
					//	values of the  xsl[i][j]
					//	There needs to be a double interpolation. The preceeding SL does not 
					//	have the same x values.
					//	dx represent the factor that affects the [i-1] SL.
					//	k will step through the points on the i-1 SL
					//	find a temporary radius(r) and pressure(p) value for the interpolated
					//	[i-1] SL.
					k = 1;
					if ( i > 0)
					{
						while (xsl[i-1][++k] < xsl[i][j] && k <= lastSLPt[i-1]);
						if ( k != 1 && xsl[i-1][k] != xsl[i-1][k-1]) 
							dx = (xsl[i-1][k] - xsl[i][j]) / (xsl[i-1][k] - xsl[i-1][k-1]);
						else dx = 0;
						rtmp = rsl[i-1][k] - dx * (rsl[i-1][k] - rsl[i-1][k-1]);
						ptmp = psl[i-1][k] - dx * (psl[i-1][k] - psl[i-1][k-1]);
					}
					else
					{
						rtmp = 0;
						ptmp = psl[i][j];
					}
					//	Now find the radius interpolated between the SLs, using the dr
					//  this is a check to make sure all things are correct. This 
					//	values should equal r and the throat
					rt = (rsl[i][j] - dr * (rsl[i][j] - rtmp))*m_RSL;
					pt[n][j] = psl[i][j] - dr * (psl[i][j] - ptmp);
					xt[n][j] = xsl[i][j]*m_RSL + m_XSL;
					yt[n][j] = rt * cos(beta) + m_YSL;
					zt[n][j] = rt * sin(beta) + m_ZSL;
					newSLPts[n] = j;
					if ( j == lastSLPt[i])
					{
						xt_end[n] = xt[n][j];
						yt_end[n] = yt[n][j];
						zt_end[n] = zt[n][j];
					}	//	end of IF loop
				}
				break;  // break from SL loop
			} // End of if loop
		} // end of for loop
		if ( i == nSL) return 0;
	}
	nNewSLs = n+1;
	return 1;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CSTT2001Dlg::OutputThroatSLsToFile( int level)
{
	//	This function takes the streamlines defined by xt,yt,zt and 
	//	outputs them in an ICEMCFD bulk data format. 
	int n,j;
	double  dist;
	// ICEM CFD BULKIN data format
	fstream ofile;
	CString fileName = m_filePrefix;
	fileName += "_ThroatSL.out";
	ofile.open(fileName,ios::out);
	ofile << "REMARK/ streamline points data file created by STT2001" << endl;
	ofile << "FORMAT/3F10.4" << endl;
	level --;
	dist = 1;
	for ( n = 0; n < nNewSLs; n++) // Loop through streamlines
	{
		//	If the SLS are too close together, then do not output them
		if ( n > 0) dist = sqrt( (yt[n][0] - yt[n-1][0])*(yt[n][0] - yt[n-1][0]) + 
							(zt[n][0] - zt[n-1][0])*(zt[n][0] - zt[n-1][0]));
		if ( dist > .01)
		{
			level++;
			ofile << "LEVEL/" << level << endl;
			ofile << "POINT/" << newSLPts[n] << endl;
			for ( j = 0; j <= newSLPts[n]; j++)	
			{
				ofile.flags(ios::fixed);
				ofile.precision(4);
				ofile << setw(10) << xt[n][j] << setw(10) << yt[n][j] 
					<< setw(10) << zt[n][j] << endl;
			}
		}
	}

	//	Create 2 more sets of points that includes just the first and last points
	//	of each of the SLS.
	level++;
	ofile << "LEVEL/" << level << endl;
	ofile << "POINT/" << nNewSLs << endl;
	for ( n = 0; n < nNewSLs; n++) // Loop through streamlines
	{
		ofile.flags(ios::fixed);
		ofile.precision(4);
		ofile << setw(10) << xt[n][0] << setw(10) << yt[n][0] 
			<< setw(10) << zt[n][0] << endl;
	}

	level++;
	ofile << "LEVEL/" << level << endl;
	ofile << "POINT/" << nNewSLs << endl;
	for ( n = 0; n < nNewSLs; n++) // Loop through streamlines
	{
		ofile.flags(ios::fixed);
		ofile.precision(4);
		ofile << setw(10) << xt[n][newSLPts[n]] << setw(10) << yt[n][newSLPts[n]] 
			<< setw(10) << zt[n][newSLPts[n]] << endl;
	}
	ofile.close();
	// The next part of this function, give a summary of the throat plane based on the
	//	STT calculation
	fileName = m_filePrefix;
	fileName += "_ThroatSummary.out";
	ofile.open(fileName,ios::out);
	
	ofile << "# \t  SL used(wall = " << nSL << ") \t XT \t YT \t ZT \t Pres \t Ideal end" << endl;
	for ( n = 0; n < nNewSLs; n++) // Loop through streamlines
	{
		ofile << n << "\t" << nt[n] << "\t" << xt[n][0] << "\t" << yt[n][0] 
			<< "\t" << zt[n][0] << "\t" << pt[n][0] << "\t" << xt_end[n] << endl;
	}

	return;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CSTT2001Dlg::DeleteArrays( int j)
{
	//	This function deletes the xsl, rsl and psl arrays after the throat SL have been
	//	found
	int i;
	for ( i = 0; i < j; i++)
	{
		delete [] jsl[i];
		delete [] xsl[i];
		delete [] rsl[i];
		delete [] psl[i];
		delete [] xt[i];
		delete [] yt[i];
		delete [] zt[i];
		delete [] pt[i];
		delete [] msl[i];
		delete [] thsl[i];
		delete [] tsl[i];
		delete [] xgrid[i];
		delete [] ygrid[i];
		delete [] zgrid[i];
		delete [] pgrid[i];
	}

	delete [] jsl;
	delete [] xsl;
	delete [] psl;
	delete [] rsl;
	delete [] tsl;
	delete [] thsl;
	delete [] msl;
	delete [] xt;
	delete [] yt;
	delete [] zt;
	delete [] xt_end;
	delete [] yt_end;
	delete [] zt_end;
	delete [] pt;
	delete [] psi;
	delete [] lastSLPt;
	delete [] newPsi;
	delete [] newSLPts;
	delete [] nt;
	delete [] xgrid;
	delete [] ygrid;
	delete [] zgrid;
	delete [] pgrid;
	
	return;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CSTT2001Dlg::TrimSLs( double yc, double zc, double rc, double alpha, double omega,
						  double xStart, double xEnd)
{
	//	This function trims the streamline boundary curves defined be (xt,yt,zt) based on the 
	//	equation of the circle defines by (y-yc)^2 + (z-zc)^2 = rc^2
	//	For each streamline point determine if that point is inside
	//	the constraint circle.  If it is, leave the point alone. If its not do alot of things
	int n,j,itrim;
	double theta,x,y,z,r,rMinus, dr, dx, p; // angle of streamline w/rt up
	itrim = 0;
	for ( n = 0; n < nNewSLs; n++) // Loop through each SL
	{
		for ( j = 0; j <= newSLPts[n]; j++)	
		{
			//	for each point along the streamline, calculate the r in the equation defined
			//	by the constraint
			r = sqrt((yt[n][j] - yc)*(yt[n][j] - yc) + (zt[n][j] - zc)*(zt[n][j] - zc));
			if ( r >= rc) // beyond constraint
			{
				//	Before the SL can be trimmed it has to meet the other constraints of 
				//	X and theta
				theta = atan2(zt[n][j],yt[n][j]) * DEG_PER_RAD;
				if ( alpha >= 0.0 && omega >=0.0 && theta < 0.0) theta =  360 + theta;
				else if ( alpha <= 0.0 && omega <=0.0 && theta > 0.0) theta = theta - 360.;
				if ( xt[n][j] >= xStart && xt[n][j] <= xEnd  && theta >= __min(alpha,omega) && 
					theta <= __max(alpha,omega))
				{
					//	set flag to tell that at least one SL has been trimmed
					itrim = 1;
					//	The SL can be trimmed. Interpolate along the SL in X to find the 
					//	yt,zt combination that equals rc
					rMinus = sqrt((yt[n][j-1] - yc)*(yt[n][j-1] - yc) + 
						(zt[n][j-1] - zc)*(zt[n][j-1] - zc));
					dr = (r - rc) / (r - rMinus);
					x = xt[n][j] - dr * (xt[n][j] - xt[n][j-1]);
				
					dx = ( xt[n][j] - x) / (xt[n][j] - xt[n][j-1]);
					y = yt[n][j] - dx * ( yt[n][j] - yt[n][j-1]);
					z = zt[n][j] - dx * ( zt[n][j] - zt[n][j-1]);
					p = pt[n][j] - dx * ( pt[n][j] - pt[n][j-1]);

					xt[n][j] = x;
					yt[n][j] = y;
					zt[n][j] = z;
					pt[n][j] = p;
					newSLPts[n] = j;
				} 
				break; // break out of j loop;
			}  // end of beyond constraint loop
		} // end of streamline points
	} // end of SL loop
//	if ( itrim == 0) AfxMessageBox("Current constraint did not trim any SLs");
	return;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CSTT2001Dlg::CalcGridSLs( void)
{
	//	This function will convert the trimmed SL to a grid defined in the 
	//	X direction. This will be used for output as well as the for the 
	//	calculation of the nozzle pressure integral
	//	The grid will be space accordingly. Half of the grid points will be 
	//	in the first third of the nozzle.
	int j,k,n;
	double dx, x3;
	//	k: 0 - 999: number of new, equally spaced points on each SL. Used with ?grid[][] only
	//	n: number of SLs. equal for xt and xgrid
	//	j: number of points on the xt SLs 0-299;
	//	x2 is the nozzle end point
	//	x1 is the nozzle start point
	//	x3 is 1/3 the nozzle length X

	x3 = x1 + (x2-x1)/3.;

	maxLength = 0.0;
	minY = 9e9;
	maxY = -9e9;
	for ( n = 0; n < nNewSLs; n++)
	{
		for ( k = 0; k < nparamGRIDX; k++)
		{
			if ( k < nparamGRIDX/2) xgrid[n][k] = (x1+k*(x3-x1))*m_RSL/((nparamGRIDX-1)/2) + m_XSL;
			else xgrid[n][k] = (x3+k*(x2-x3))*m_RSL/((nparamGRIDX-1)/2) + m_XSL;
			j = 0;
			while (xt[n][j] < xgrid[n][k] && j <= newSLPts[n]) j++;
			//	If j = 0, then set y,z and p equal to y,z and p [n][0]
			//	If j == nparamPTS, the there does not exist a point xt, that is
			//	larger then xgrid[k]. Set y, z equal to the preceeding values.
			//	Set P = -9e9 so that it is known not to use in the performance
			//	calculation
			if ( j == 0)
			{
				ygrid[n][k] = yt[n][j];
				zgrid[n][k] = zt[n][j];
				pgrid[n][k] = pt[n][j];
			}
			else if ( j > newSLPts[n])
			{
				if ( k > 0)
				{
					xgrid[n][k] = xt[n][j-1];
					ygrid[n][k] = yt[n][j-1];
					zgrid[n][k] = zt[n][j-1];
					pgrid[n][k] = pt[n][j-1];
				}
				else
				{
					xgrid[n][k] = xt[n][j];
					ygrid[n][k] = yt[n][j];
					zgrid[n][k] = zt[n][j];
					pgrid[n][k] = pt[n][j];
				}
			}
			else if ( j+2 > newSLPts[n] || j==1 )
			{
				//	use linear interpolation
				if ( xt[n][j] == xt[n][j-1]) dx = 0;
				else dx = (xt[n][j]-xgrid[n][k])/(xt[n][j]-xt[n][j-1]);
				ygrid[n][k] = yt[n][j] - dx*(yt[n][j]-yt[n][j-1]);
				zgrid[n][k] = zt[n][j] - dx*(zt[n][j]-zt[n][j-1]);
				pgrid[n][k] = pt[n][j] - dx*(pt[n][j]-pt[n][j-1]);
			}
		else
			{
				//	Use Divided difference method using 5 data points.
				//	j-2, j-1, j, j+1, j+2;
				double xf[5], yf[5][5], zf[5][5], pf[5][5];
				int jj,i,N=4;
				xf[0] = xt[n][j-2];
				xf[1] = xt[n][j-1];
				xf[2] = xt[n][j];
				xf[3] = xt[n][j+1];
				xf[4] = xt[n][j+2];
				yf[0][0] = yt[n][j-2];
				yf[0][1] = yt[n][j-1];
				yf[0][2] = yt[n][j];
				yf[0][3] = yt[n][j+1];
				yf[0][4] = yt[n][j+2];
				zf[0][0] = zt[n][j-2];
				zf[0][1] = zt[n][j-1];
				zf[0][2] = zt[n][j];
				zf[0][3] = zt[n][j+1];
				zf[0][4] = zt[n][j+2];
				pf[0][0] = pt[n][j-2];
				pf[0][1] = pt[n][j-1];
				pf[0][2] = pt[n][j];
				pf[0][3] = pt[n][j+1];
				pf[0][4] = pt[n][j+2];

				for (i = 1; i <= 4; i++)
				{
					for (jj = 0; jj <= N-i; jj++)
					{
						yf[i][jj] = (yf[i-1][jj+1]-yf[i-1][jj])/(xf[jj+i]-xf[jj]);
						zf[i][jj] = (zf[i-1][jj+1]-zf[i-1][jj])/(xf[jj+i]-xf[jj]);
						pf[i][jj] = (pf[i-1][jj+1]-pf[i-1][jj])/(xf[jj+i]-xf[jj]);
					}
				}
				
				ygrid[n][k] = yf[0][0] + 
					yf[1][0]*(xgrid[n][k]-xt[n][j-2]) + 
					yf[2][0]*(xgrid[n][k]-xt[n][j-2])*(xgrid[n][k]-xt[n][j-1]) + 	
					yf[3][0]*(xgrid[n][k]-xt[n][j-2])*(xgrid[n][k]-xt[n][j-1])*
					(xgrid[n][k]-xt[n][j]) + 
					yf[4][0]*(xgrid[n][k]-xt[n][j-2])*(xgrid[n][k]-xt[n][j-1])*
					(xgrid[n][k]-xt[n][j])*(xgrid[n][k]-xt[n][j+1]); 
				zgrid[n][k] = zf[0][0] + 
					zf[1][0]*(xgrid[n][k]-xt[n][j-2]) + 
					zf[2][0]*(xgrid[n][k]-xt[n][j-2])*(xgrid[n][k]-xt[n][j-1]) + 	
					zf[3][0]*(xgrid[n][k]-xt[n][j-2])*(xgrid[n][k]-xt[n][j-1])*
					(xgrid[n][k]-xt[n][j]) + 
					zf[4][0]*(xgrid[n][k]-xt[n][j-2])*(xgrid[n][k]-xt[n][j-1])*
					(xgrid[n][k]-xt[n][j])*(xgrid[n][k]-xt[n][j+1]); 
				pgrid[n][k] = pf[0][0] + 
					pf[1][0]*(xgrid[n][k]-xt[n][j-2]) + 
					pf[2][0]*(xgrid[n][k]-xt[n][j-2])*(xgrid[n][k]-xt[n][j-1]) + 	
					pf[3][0]*(xgrid[n][k]-xt[n][j-2])*(xgrid[n][k]-xt[n][j-1])*
					(xgrid[n][k]-xt[n][j]) + 
					pf[4][0]*(xgrid[n][k]-xt[n][j-2])*(xgrid[n][k]-xt[n][j-1])*
					(xgrid[n][k]-xt[n][j])*(xgrid[n][k]-xt[n][j+1]); 
				
				if ( !_finite(ygrid[n][k]))
				{
					//	use linear interpolation
					if ( xt[n][j] == xt[n][j-1]) dx = 0;
					else dx = (xt[n][j]-xgrid[n][k])/(xt[n][j]-xt[n][j-1]);
					ygrid[n][k] = yt[n][j] - dx*(yt[n][j]-yt[n][j-1]);
					zgrid[n][k] = zt[n][j] - dx*(zt[n][j]-zt[n][j-1]);
					pgrid[n][k] = pt[n][j] - dx*(pt[n][j]-pt[n][j-1]);
				}
			}
			if(pgrid[n][k] < 0.0)
			{
				// As a last resort ,use linear interpolation
				if ( xt[n][j] == xt[n][j-1]) dx = 0;
				else dx = (xt[n][j]-xgrid[n][k])/(xt[n][j]-xt[n][j-1]);
				ygrid[n][k] = yt[n][j] - dx*(yt[n][j]-yt[n][j-1]);
				zgrid[n][k] = zt[n][j] - dx*(zt[n][j]-zt[n][j-1]);
				pgrid[n][k] = pt[n][j] - dx*(pt[n][j]-pt[n][j-1]);
				if(pgrid[n][k] < 0.0)
				{
					AfxMessageBox("Calculated Grid Pressure < 0.0");
					exit(1);
				}
			}
		/*	else 
			{
				//	use linear interpolation
				if ( xt[n][j] == xt[n][j-1]) dx = 0;
				else dx = (xt[n][j]-xgrid[n][k])/(xt[n][j]-xt[n][j-1]);
				ygrid[n][k] = yt[n][j] - dx*(yt[n][j]-yt[n][j-1]);
				zgrid[n][k] = zt[n][j] - dx*(zt[n][j]-zt[n][j-1]);
				pgrid[n][k] = pt[n][j] - dx*(pt[n][j]-pt[n][j-1]);
			}
			if(pgrid[n][k] < 0.0)
			{
				AfxMessageBox("Calculated Grid Pressure < 0.0");
				exit(1);
			}
		*/
		}
	}

	//	This part saves parameters that tells what where the max and mins are
	//	for the trimmed nozzle design.
	double oldMax = maxY, oldMin = minY;
	for ( n = 0; n < nNewSLs; n++)
	{
		maxLength = __max(maxLength,xgrid[n][nparamGRIDX-1]);
		if (maxLength == xgrid[n][nparamGRIDX-1]) m_NAtMaxX = n;
		maxY = __max(maxY,ygrid[n][nparamGRIDX-1]);
		minY = __min(minY,ygrid[n][nparamGRIDX-1]);
		if ( oldMax != maxY) 
		{
			m_XAtMaxY = xgrid[n][nparamGRIDX-1];
			m_NAtMaxY = n;
		}
		if ( oldMin != minY) 
		{
			m_XAtMinY = xgrid[n][nparamGRIDX-1];
			m_NAtMinY = n;
		}
		oldMax = maxY;
		oldMin = minY;
	}
	return;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CSTT2001Dlg::TrimSLsDueToAxiRevolution( int nRevs, double YSim, double ZSim, 
											double RSim)
{
	//	This function takes the trimmed SL and trims them even more if there is
	//	any symmetry to the design.  Such as 3 engine pods located 120 degrees apart.
	//	The current trimming was only done on one of these nozzles.  There may be some
	//	interferences between instances.
	//	Recap of some of the variables that will be used
	//	nNewSLs = number of streamlines currently defined for 1 instance
	//	xgrid, ygrid, zgrid : coordinates for the streamline points
	//	nparamGRIDX: number of points in each streamline
	//	n: number of SLs counter for the SL that is being checked
	//	k: # of SL that is being stepped through, used as a constraint
	//	j: number of points/SL counter
	//	i: revolution counter
	//	The 'grid' values are equal in X
	//	For each SL step through the points.
	//	For each point, calculate the angle and radius of that point with respect to 
	//	the line of symmetry. 
	//	For all of the other SLs, at the same X rotate the point +theta and calculate the
	//	angle and radius w/rt the line of symmetry.  If its at the same theta, and
	//	the radius is less, then cut the original SL to that point. 
	//	
	int i,j,k,n,m, islMatch;
	double theta, y2, z2, rad2, y3,rad3, z3, theta2, theta3, theta4, rad4, y4,z4;
	double rTmp, tTmp, dTheta, dRad;
	double **yTrim, **zTrim; //**xTrim, **pTrim;

	maxLength = 0.0;
	minY = 9e9;
	maxY = -9e9;
	//  First, create a temporary copy of the ygrid and zgrid. These will be used to 
	//	trim the real grid.
	yTrim = new double*[nNewSLs];
	zTrim = new double*[nNewSLs];
	for ( n = 0; n < nNewSLs; n++)
	{
		yTrim[n] = new double[nparamGRIDX];
		zTrim[n] = new double[nparamGRIDX];
		for ( j = 0; j < nparamGRIDX; j++)
		{
			yTrim[n][j] = ygrid[n][j];
			zTrim[n][j] = zgrid[n][j];
		}
	}

	for ( i = 1; i < nRevs; i++)
	{
		theta = -i*2*PI/nRevs;  // rotation angle along the line of symmetry
	for ( n = 0; n < nNewSLs; n++)
	{
		for ( j = 1; j < nparamGRIDX; j++)  // does not do the 1st point
		{
			//	For each point along the nth SL, calculate that point wrt to
			//  the center of rotation. Refer to that point a 2.
			y2 = ygrid[n][j] - YSim;
			z2 = zgrid[n][j] - ZSim;
			theta2 = atan2(y2,z2);
			rad2 = sqrt(y2*y2 + z2*z2);

			//	For all of the other SLs, calculate the radius of their
			//	Jth point when rotated theta.  Using the symmetry SL defined
			//	in the input SL1Match and SL2Match, determine the other SL that should match
			//	up 
		
			if ( n < m_SymDefDlg.m_SL2Match) 
				islMatch = m_SymDefDlg.m_SL2Match - (n + m_SymDefDlg.m_SL1Match);
			else if ( n >= m_SymDefDlg.m_SL2Match &&  n <= nNewSLs)
				islMatch = nNewSLs - n + m_SymDefDlg.m_SL2Match - 1;
/*			if ( n < m_nSL1) islMatch = m_nSL1-n-1; 
			else if ( n >= m_nSL1 && n < m_nSL2+m_nSL1) 
				islMatch = 2*m_nSL1 + m_nSL2 -1 -n;
			else if ( n >= m_nSL2+m_nSL1 && n < m_nSL2+m_nSL1+m_nSL3)
				islMatch = 2*m_nSL1 + 2*m_nSL2 + m_nSL3 -1 -n;
			else if ( n >= m_nSL2+m_nSL1+m_nSL3 && n < m_nSL2+m_nSL1+m_nSL3+m_nSL4)
				islMatch = 2*m_nSL1 + 2*m_nSL2 + 2*m_nSL3 + m_nSL4 -1 -n;
			else if ( n >= m_nSL2+m_nSL1+m_nSL3+m_nSL4 && n < m_nSL2+m_nSL1+m_nSL3+m_nSL4+m_nSL5)
				islMatch = 2*m_nSL1 + 2*m_nSL2 + 2*m_nSL3 + 2*m_nSL4 + m_nSL5 -1 -n;*/
			
			//	This flag is set elsewhere.  If true, then the islMatch calc is different
			if (circleFlag)
			{
				islMatch = m_nSL1 - n;
				if ( islMatch >= m_nSL1) islMatch = 0;
			}
			for ( k = j; k <= j+1; k++)
			{
			    y3 = yTrim[islMatch][k] - YSim;
				z3 = zTrim[islMatch][k] - ZSim;
				theta3 = atan2(y3,z3)+theta;
				rad3 = sqrt(y3*y3 + z3*z3);
				if (theta3 > 2*PI) theta3 -= 2*PI;
				else if ( theta3 < -2*PI) theta3 += 2*PI;
				y4 = yTrim[islMatch][k-1] - YSim;
				z4 = zTrim[islMatch][k-1] - ZSim;
				theta4 = atan2(y4,z4)+theta;
				if (theta4 > 2*PI) theta4 -= 2*PI;
				else if ( theta4 < -2*PI) theta4 += 2*PI;
				rad4 = sqrt(y4*y4 + z4*z4);
				//	There has to be a check to the angles involved.  The acute angle between
				//	3 and 4 is desired
				if ( fabs(theta3 - theta4) > PI)
				{
					if ( theta4 >= 0.0) theta4 -= 2*PI;
					else theta4 += 2*PI;
				}
				if ( theta2 > 0.0 && theta3 < 0.0 && theta4 < 0.0) theta2 -= 2*PI;
				else if ( theta2 < 0.0 && theta3 > 0.0 && theta4 > 0.0) theta2 += 2*PI;
				//  Set theta3 to the minimum and theta4 to the maximum
				if ( theta3 > theta4)
				{
					tTmp= theta4;
					rTmp= rad4;
					theta4 = theta3;
					rad4 = rad3;
					theta3= tTmp;
					rad3 = rTmp;
				}
				dTheta = 0.5 * (theta4 - theta3);
				dRad = 0.5 * fabs(rad4 - rad3);
				//	if theta2 is between theta3 and theta4, check the radius
				//	the error on the radius check is 1/2 the angle 
				if ( theta2 >= theta3 - dTheta && theta2 <= theta4 + dTheta)
				{
					if (rad2 >= __min(rad3,rad4)- dRad && rad2 <= __max(rad3,rad4)+dRad)
					{
						//	The SL has to be cut between k and k-1
						//	TODO LATER					
						for ( m = j; m < nparamGRIDX; m++)
						{
							xgrid[n][m] = xgrid[n][j];
							ygrid[n][m] = ygrid[n][j];
							zgrid[n][m] = zgrid[n][j];
							pgrid[n][m] = pgrid[n][j];
						} // end of for m loop
						k = nparamGRIDX; // break the k loop;
						j = k; // break the j loop;
					}
				}  // end of if newRad loop
			} // end of for k loop
		} // end of for j loop
	} // end of for n loop
	} // end of for i loop

	// Loop through all of these point to find the max length
	double oldMax = maxY, oldMin = minY;
	for ( n = 0; n < nNewSLs; n++)
	{
		maxLength = __max(maxLength,xgrid[n][nparamGRIDX-1]);
		if (maxLength == xgrid[n][nparamGRIDX-1]) m_NAtMaxX = n;
		maxY = __max(maxY,ygrid[n][nparamGRIDX-1]);
		minY = __min(minY,ygrid[n][nparamGRIDX-1]);
		if ( oldMax != maxY) 
		{
			m_XAtMaxY = xgrid[n][nparamGRIDX-1];
			m_NAtMaxY = n;
		}
		if ( oldMin != minY) 
		{
			m_XAtMinY = xgrid[n][nparamGRIDX-1];
			m_NAtMinY = n;
		}
		oldMax = maxY;
		oldMin = minY;
	}
	return;
}


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CSTT2001Dlg::OutputTrimmedSLsToFile( int level)
{
	//	This function takes the streamlines defined by xt,yt,zt and 
	//	outputs them in an ICEMCFD bulk data format. 
	int n,j,k,m,d1;
	double dist;
	// ICEM CFD BULKIN data format
	fstream ofile;
	CString fileName = m_filePrefix;
	fileName += "_TrimSL.out";
	ofile.open(fileName,ios::out);
	ofile << "REMARK/ streamline points data file created by STT2001" << endl;
	d1 = 2;
//	ofile << "FORMAT/3F10.4" << endl;
	level --;
	for ( n = 0; n < nNewSLs; n++) // Loop through streamlines
	{
		if ( n > 0) dist = sqrt( (ygrid[n][0] - ygrid[n-1][0])*
			(ygrid[n][0] - ygrid[n-1][0]) + (zgrid[n][0] - zgrid[n-1][0])*
			(zgrid[n][0] - zgrid[n-1][0]));
		else dist = 1.0;
		if ( dist > .01)
		{
			level++;
			ofile << "LEVEL/" << level << endl;
			k = nparamGRIDX-1;
			//	loop through the data once determine the number of unique points
			for ( j = 0; j < nparamGRIDX-1; j++)	
			{
				if ( xgrid[n][j] == xgrid[n][j+1]) 
				{
					k = j;
					break;
				}
			}
			ofile << "POINT/" << k+1 << endl;
			for ( j = 0; j <= k; j++)	
			{
				ofile.flags(ios::fixed);
				ofile.precision(d1);
				ofile << setw(10) << xgrid[n][j] << setw(10) << ygrid[n][j] 
				<< setw(10) << zgrid[n][j] << endl;
			}
		}
	}

	//	Create 2 more sets of points that includes just the first and last points
	//	of each of the SLS.
	level++;
	ofile << "LEVEL/" << level << endl;
	ofile << "POINT/" << nNewSLs << endl;
	for ( n = 0; n < nNewSLs; n++) // Loop through streamlines
	{
		ofile.flags(ios::fixed);
		ofile.precision(d1);
		ofile << setw(10) << xgrid[n][0] << setw(10) << ygrid[n][0] 
			<< setw(10) << zgrid[n][0] << endl;
	}

	level++;
	ofile << "LEVEL/" << level << endl;
	ofile << "POINT/" << nNewSLs << endl;
	for ( n = 0; n < nNewSLs; n++) // Loop through streamlines
	{
		ofile.flags(ios::fixed);
		ofile.precision(d1);
		ofile << setw(10) << xgrid[n][nparamGRIDX-1] << setw(10) << ygrid[n][nparamGRIDX-1] 
			<< setw(10) << zgrid[n][nparamGRIDX-1] << endl;
	}
	ofile.close();



	//	Now make 3 files in plot3d format. One file contains the trimmed 
	//	grid.  One file contains the pressure data. One file contains the 
	//  endpoints of the SLs where the flow is turned back to the walls
	CString gridFileName = m_filePrefix;
	CString dataFileName = m_filePrefix;
	CString endFileName = m_filePrefix;
	gridFileName += "_trimmed_P3D.xyz";
	dataFileName += "_trimmed_P3D.dat";
	endFileName += "_end_P3D.xyz";

	fstream gridFile, dataFile, endFile;
	gridFile.open(gridFileName,ios::out);
	dataFile.open(dataFileName,ios::out);
	endFile.open(endFileName,ios::out);
	
	gridFile << nparamGRIDX << " " << nNewSLs+1 << "  1" << endl;
	dataFile << nparamGRIDX << " " << nNewSLs+1 << "  1  1" << endl;
	endFile << nNewSLs+1 << " 1 1" << endl;
	gridFile.flags(ios::fixed);
	gridFile.precision(d1);
	dataFile.flags(ios::fixed);
	dataFile.precision(d1);
	endFile.flags(ios::fixed);
	endFile.precision(d1);
	//	Output all of the x and p
	for ( n = 0; n <= nNewSLs; n++) // Loop through streamlines
	{
			k = 0;
			for ( j = 0; j < nparamGRIDX; j++)	
			{
				if ( n == nNewSLs) m = 0;
				else m = n;
				if ( fabs(xgrid[m][j]) > 0.00001) gridFile << setw(10) << xgrid[m][j] ;
				else gridFile << setw(10) << 0.0;
				dataFile << setw(10) << pgrid[m][j] ;
				if ( k++ > 8)
				{
					k = 0;
					gridFile << endl;
					dataFile << endl;
				}
			}
			endFile << setw(10) << xt_end[m] << " ";
			gridFile << endl;
			dataFile << endl;
	}
	endFile << endl;
	
	//	Output the y
	for ( n = 0; n <= nNewSLs; n++) // Loop through streamlines
	{
			k = 0;
			//	loop through the data once determine the number of unique points
			//	Out put all of the x and p
			for ( j = 0; j < nparamGRIDX; j++)	
			{
				if ( n == nNewSLs) m = 0;
				else m = n;
				if ( fabs(ygrid[m][j]) > 0.00001) gridFile << setw(10) << ygrid[m][j] ;
				else gridFile << setw(10) << 0.0; 
				if ( k++ > 8)
				{
					k = 0;
					gridFile << endl;
				}
			}
			endFile << setw(10) << yt_end[m] << " ";
			gridFile << endl;
	}
	endFile << endl;
	
	//	Output the z
	for ( n = 0; n <= nNewSLs; n++) // Loop through streamlines
	{
			k = 0;
			//	loop through the data once determine the number of unique points
			//	Out put all of the x and p
			for ( j = 0; j < nparamGRIDX; j++)	
			{
				if ( n == nNewSLs) m = 0;
				else m = n;
				if ( fabs(zgrid[m][j]) > 0.00001) gridFile << setw(10) << zgrid[m][j] ;
				else gridFile << setw(10) << 0.0; 
				if ( k++ > 8)
				{
					k = 0;
					gridFile << endl;
				}
			}
			endFile << setw(10) << zt_end[m] << " ";
			gridFile << endl;
	}

	////////////////////////////////////////////////////////////////////
	///	TECPLOT FORMAT
	///////////////////////////////////////////////////////////////////
	//	Output one TECPLOT output file
	CString tpFileName = m_filePrefix;
	tpFileName += ".plt";
	ofile.open(tpFileName,ios::out);
	ofile << "VARIABLES = \"X(in)\",\"Y(in)\",\"Z(in)\",\"Pressure\"" << endl
/*		<< "\"Temperature\",\"Density\",\"Gamma\",\"Theta\""
		<< "\"Velocity\",\"RHO*V\",\"Massflow\"" << endl*/
		<< "TITLE = \"STT2001 Ouput\"" << endl
		<< "text  x=5  y=93  t=\"Trimmed Nozzle Data\"" << endl;
	ofile << "zone t=\"All data\"" << " I = " << nNewSLs+1 << " J =" << nparamGRIDX
		<< " K = 1" << endl;
	//	Right running characteristic file
	for ( j = 0; j < nparamGRIDX; j++)
	{
		for ( n = 0; n <= nNewSLs; n++)
		{
			if ( n == nNewSLs) m = 0;
			else m = n;
			ofile << xgrid[m][j] << "\t" << ygrid[m][j] << "\t" << 
				zgrid[m][j] << "\t" << pgrid[m][j] <<  endl;
		}
	}
	ofile.close();

	////////////////////////////////////////////////////////////////////
	///	TECPLOT FORMAT
	///////////////////////////////////////////////////////////////////
	//	Output one TECPLOT output file for the whole engine
	double S, THETAP, THETAT,Y,Z;
	tpFileName = m_filePrefix;
	tpFileName += "_Engine.plt";
	ofile.open(tpFileName,ios::out);
	ofile << "VARIABLES = \"X(in)\",\"Y(in)\",\"Z(in)\",\"Pressure\"" << endl
/*		<< "\"Temperature\",\"Density\",\"Gamma\",\"Theta\""
		<< "\"Velocity\",\"RHO*V\",\"Massflow\"" << endl*/
		<< "TITLE = \"STT2001 Ouput\"" << endl
		<< "text  x=5  y=93  t=\"Trimmed Nozzle Data\"" << endl;

	for ( int i = 0; i< m_SymDefDlg.m_nRev; i++)
	{
		ofile << "zone t=\"All data\"" << " I = " << nNewSLs+1 << " J =" << nparamGRIDX
			<< " K = 1" << endl;
		//	Right running characteristic file
		for ( j = 0; j < nparamGRIDX; j++)
		{
			for ( n = 0; n <= nNewSLs; n++)
			{
				if ( n == nNewSLs) m = 0;
				else m = n;
				// To include the rotation the following has to be done
				//	Find the distance of each point to the center of revelution
				S = sqrt(zgrid[m][j]*zgrid[m][j] + pow(ygrid[m][j]-m_SymDefDlg.m_YSim,2));
				//	find the angle from the point to the Y axis
				THETAP = atan(zgrid[m][j]/(ygrid[m][j]-m_SymDefDlg.m_YSim));
				//	Calculate the total angle of rotation
				THETAT = THETAP + i*2*PI/m_SymDefDlg.m_nRev;
				//	calculate the new Y and Z
				Y = S*cos(THETAT);
				Z = S*sin(THETAT);
				ofile << xgrid[m][j] << "\t" << Y << "\t" << 
					Z << "\t" << pgrid[m][j] <<  endl;
			}
		}
	}
	ofile.close();
	endFile.close();
	gridFile.close();
	dataFile.close();
	return;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CSTT2001Dlg::CalcNozzleParameters( void)
{
	//	Once the SL's are known, calculate the surface area, axial area and 
	//	Thrust (P*A) for the nozzle.  Jordan Wilkerson first did this using a MATLAB
	//	program. The SL grid can be broken down into triangles, given 3 vertices.
	//	The surface area of that triangle is the magnitude of the resultant vector.
	//  The projected area of the triangle is the dot product of the vector with the
	//	x axis * the surface area.
	
	int n,k,i;
	double area, mag, pAvg[2],cosTheta,pMax;
	Vector U,V,W;
	CString fileStr;
	char line[80];

	//	Set a maximum pressure to use in the calculation.  This should be the pressure
	//	at the throat
	pMax = -99;
	for (i = 0; i <= nNewSLs; i++) pMax = __max(pMax, psl[i][0]);
	pMax *= 1.05;
	//	Now for each [n][j] you need 4 points
	//	PAvg[0] uses P1,P2 & P3
	//	PAvg[1] uses P4,P2 & P3
	Vector P1,P2,P3,P4;
	//	These loops will calculate the axial projected area as a function of X
	fileStr = m_filePrefix;
	fileStr += "_AvsX.out";
	fstream AvsXFile(fileStr,ios::out);
	if(!AvsXFile.is_open())
	{
		fileStr += " does not exist";
		AfxMessageBox(fileStr);
		exit(1);
	}
	AvsXFile << "X \t Area \t Surface Area \t Pressure Force" << endl;

	for ( k = 0; k < nparamGRIDX-1; k++)
	{
		for ( n = 0; n < nNewSLs; n++)
		{
			// P1 and P2 are different points on the n SL
			P1.Set(xgrid[n][k], ygrid[n][k], zgrid[n][k]);
			P2.Set(xgrid[n][k+1], ygrid[n][k+1], zgrid[n][k+1]);
			if ( n != nNewSLs-1)
			{
				//			   P1              P2             P3
				pAvg[0] = ( pgrid[n][k] + pgrid[n][k+1] + pgrid[n+1][k])/ 3.;
				//				P4				P2				P3
				pAvg[1] = ( pgrid[n+1][k+1] + pgrid[n][k+1] + pgrid[n+1][k])/ 3.;
				// P3 and P4 are different points on the n+1 SL
				P3.Set(xgrid[n+1][k], ygrid[n+1][k], zgrid[n+1][k]);
				P4.Set(xgrid[n+1][k+1], ygrid[n+1][k+1], zgrid[n+1][k+1]);
			}
			else
			{
				//				P1			P2				P3
				pAvg[0] = ( pgrid[n][k] + pgrid[n][k+1] + pgrid[0][k])/ 3.;
				//				P4             P2				P3
				pAvg[1] = ( pgrid[0][k+1] + pgrid[n][k+1] + pgrid[0][k])/ 3.;
				P3.Set(xgrid[0][k], ygrid[0][k], zgrid[0][k]);
				P4.Set(xgrid[0][k+1], ygrid[0][k+1], zgrid[0][k+1]);
			}

			if (pAvg[0] < 0.0 || pAvg[1] < 0.0) AfxMessageBox("Average pressure < 0.0");
			else if (pAvg[0] > pMax || pAvg[1] > pMax)
			{
				CString tmp;
				tmp = "Average pressure: ";
				tmp += _gcvt( __max(pAvg[0],pAvg[1]), 7, line );
				tmp += "  > Max pressure: ";
				tmp += _gcvt( pMax, 7, line );
			//	AfxMessageBox(tmp);
			}

			//	if one of the pressure terms = 0.0, then do not use in the calculation
			if ( pgrid[n][k] != 0.0 && pgrid[n][k+1] != 0.0 && pgrid[n+1][k] != 0.0)
			{
				//	Calc area of first triangle
				U.Set( P2(1) - P1(1), P2(2) - P1(2), P2(3) - P1(3));
				V.Set( P3(1) - P1(1), P3(2) - P1(2), P3(3) - P1(3));
				W = U.crossprod(V);   // U x V
				mag = W.mag();
				area = 0.5 * mag;
				m_SurfaceArea += area;
				// W is a vector perpendicular the orignal UV plane
				// Because UxV != VxU the direction of W w/rt to positive X axis
				//	is dependent on whether the value is taken from the upper or lower
				//	nozzle surface and whether the SL are going clockwise or counter-clockwise
				//	Here are the cases
				//	Upper Surface COMBO = 1, clockwise W = -UxV
				//	Upper Surface, counter   W = UxV
				//	Lower Surface COMBO = 0, clockwise W = UxV
				//	Lower Surface, counter   W = -UxV
				if ( mag != 0)
				{
					cosTheta = rotateSL[n]*W(1)/ mag;
					m_ProjectedArea += area * cosTheta;
					m_Force += pAvg[0] * area * cosTheta;
				}
			}

			if( pgrid[n+1][k] != 0.0 && pgrid[n][k+1] != 0.0 && pgrid[n+1][k+1] != 0.0)
			{
				// Calc area of the second triangle
				//	Calc area of first triangle
				U.Set( P3(1) - P4(1), P3(2) - P4(2), P3(3) - P4(3));
				V.Set( P2(1) - P4(1), P2(2) - P4(2), P2(3) - P4(3));
				W = U.crossprod(V);
				mag = W.mag();
				area = 0.5 * mag;
				m_SurfaceArea += area;
				// Dot U (which is now a vector perpendicular the orignal UV plane
				// with X to find the angle between the 2 vectors.  The U vector 180
				// degrees from what you want, therefore use a -.
				if ( mag != 0)
				{
					cosTheta = rotateSL[n]*W(1)/ mag;
					m_ProjectedArea += area * cosTheta;
					m_Force += pAvg[1] * area * cosTheta;
				}
			}
		/*	if (m_ProjectedArea < 0.0 || m_SurfaceArea < 0.0)
			{
				AfxMessageBox("Calculated Area < 0.0");
				exit(1);
			}*/
		}
		AvsXFile << xgrid[m_NAtMaxX][k+1] << "\t" << m_ProjectedArea + m_aThroat 
			<< "\t" << m_SurfaceArea << "\t" << m_Force <<  endl;
	}
	AvsXFile.close();
	//	Now the calculation will be done as a function of the SLs.
	//	Hopefully the the two solutions will be the same.  If not, then 
	//	flag it.
	//	Initialize temporary variables and open up an output file to capture
	//	the information.
	double tmpForce = 0.0, tmpArea = 0.0, tmpSA= 0.0;
	fileStr = m_filePrefix;
	fileStr += "_AvsSL.out";
	fstream AvsSLFile(fileStr,ios::out);
	if(!AvsSLFile.is_open())
	{
		fileStr += " does not exist";
		AfxMessageBox(fileStr);
		exit(1);
	}
	AvsSLFile << "SL# \t Area \t Surface Area \t Pressure Force" << endl;
	for ( n = 0; n < nNewSLs; n++)
	{
		for ( k = 0; k < nparamGRIDX-1; k++)
		{
			// P1 and P2 are different points on the n SL
			P1.Set(xgrid[n][k], ygrid[n][k], zgrid[n][k]);
			P2.Set(xgrid[n][k+1], ygrid[n][k+1], zgrid[n][k+1]);
			if ( n != nNewSLs-1)
			{
				//			   P1              P2             P3
				pAvg[0] = ( pgrid[n][k] + pgrid[n][k+1] + pgrid[n+1][k])/ 3.;
				//				P4				P2				P3
				pAvg[1] = ( pgrid[n+1][k+1] + pgrid[n][k+1] + pgrid[n+1][k])/ 3.;
				// P3 and P4 are different points on the n+1 SL
				P3.Set(xgrid[n+1][k], ygrid[n+1][k], zgrid[n+1][k]);
				P4.Set(xgrid[n+1][k+1], ygrid[n+1][k+1], zgrid[n+1][k+1]);
			}
			else
			{
				//				P1			P2				P3
				pAvg[0] = ( pgrid[n][k] + pgrid[n][k+1] + pgrid[0][k])/ 3.;
				//				P4             P2				P3
				pAvg[1] = ( pgrid[0][k+1] + pgrid[n][k+1] + pgrid[0][k])/ 3.;
				P3.Set(xgrid[0][k], ygrid[0][k], zgrid[0][k]);
				P4.Set(xgrid[0][k+1], ygrid[0][k+1], zgrid[0][k+1]);
			}

			if (pAvg[0] < 0.0 || pAvg[1] < 0.0) AfxMessageBox("Average pressure < 0.0");
			else if (pAvg[0] > pMax || pAvg[1] > pMax)
			{
				CString tmp;
				tmp = "Average pressure: ";
				tmp += _gcvt( __max(pAvg[0],pAvg[1]), 7, line );
				tmp += "  > Max pressure: ";
				tmp += _gcvt( pMax, 7, line );
			//	AfxMessageBox(tmp);
			}

			//	if one of the pressure terms = 0.0, then do not use in the calculation
			if ( pgrid[n][k] != 0.0 && pgrid[n][k+1] != 0.0 && pgrid[n+1][k] != 0.0)
			{
				//	Calc area of first triangle
				U.Set( P2(1) - P1(1), P2(2) - P1(2), P2(3) - P1(3));
				V.Set( P3(1) - P1(1), P3(2) - P1(2), P3(3) - P1(3));
				W = U.crossprod(V);   // U x V
				mag = W.mag();
				area = 0.5 * mag;
				tmpSA += area;
				// W is a vector perpendicular the orignal UV plane
				// Because UxV != VxU the direction of W w/rt to positive X axis
				//	is dependent on whether the value is taken from the upper or lower
				//	nozzle surface and whether the SL are going clockwise or counter-clockwise
				//	Here are the cases
				//	Upper Surface COMBO = 1, clockwise W = -UxV
				//	Upper Surface, counter   W = UxV
				//	Lower Surface COMBO = 0, clockwise W = UxV
				//	Lower Surface, counter   W = -UxV
				if ( mag != 0)
				{
					cosTheta = rotateSL[n]*W(1)/ mag;
					tmpArea += area * cosTheta;
					tmpForce += pAvg[0] * area * cosTheta;
				}
			}

			if( pgrid[n+1][k] != 0.0 && pgrid[n][k+1] != 0.0 && pgrid[n+1][k+1] != 0.0)
			{
				// Calc area of the second triangle
				//	Calc area of first triangle
				U.Set( P3(1) - P4(1), P3(2) - P4(2), P3(3) - P4(3));
				V.Set( P2(1) - P4(1), P2(2) - P4(2), P2(3) - P4(3));
				W = U.crossprod(V);
				mag = W.mag();
				area = 0.5 * mag;
				tmpSA += area;
				// Dot U (which is now a vector perpendicular the orignal UV plane
				// with X to find the angle between the 2 vectors.  The U vector 180
				// degrees from what you want, therefore use a -.
				if ( mag != 0)
				{
					cosTheta = rotateSL[n]*W(1)/ mag;
					tmpArea += area * cosTheta;
					tmpForce += pAvg[1] * area * cosTheta;
				}
			}
		}
		AvsSLFile << n << "\t" << tmpArea + m_aThroat << "\t"
			<< tmpSA << "\t" << tmpForce << endl;
	}

	double errSA = fabs(m_SurfaceArea - tmpSA)/tmpSA*100;
	double errArea = fabs(m_ProjectedArea - tmpArea)/tmpArea*100;
	double errForce = fabs(m_Force - tmpForce)/tmpForce*100;
	if ( errSA > 0.1 || errArea > 0.1 || errForce > 0.1)
		AfxMessageBox("Integrated nozzle values did not match: > 0.1% difference");
	return;
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CSTT2001Dlg::InterpolatePressureData(int i, double *xp, double *p, int npts)
{
	//	Now interpolate on this data to find the pressures at the xsl[i][j] values
	// NPTS is the number of points in the p[k] array, but k starts at 0
	int j,k;
	double dx;
	for ( j = 0; j <= lastSLPt[i]; j++)
	{
		k = 0;
		while (xp[++k] < xsl[i][j] && k < npts);
		// Set the pressure of the last xp point equal to the pressure
		//	of the xsl[i][j] point
		if ( k == 1 ) psl[i][j] = p[k];
		else 
		{
			if ( k == npts)
			{
				psl[i][j] = p[k-1];
			}
			else 
			{
				dx = (xp[k] - xsl[i][j]) / (xp[k] - xp[k-1]);
				psl[i][j] = p[k] - dx * (p[k] - p[k-1]);
			}
		}
	}
	return;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CSTT2001Dlg::ThrowNoThroatSLDefinedError( void)
{
	CString str = "Throat SL cannot be defined by any known SL\n";
	str += "R/R* for the SL data may be < 1.0. Try increase SL radius";
	AfxMessageBox( str);
 	exit(1);
} 

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CSTT2001Dlg::OnSymDefBUTTON() 
{
	// Open up the symmetry definitions dialog box
	m_SymDefDlg.DoModal();
	return;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CSTT2001Dlg::OnFileOpen() 
{
	// TODO: Add your command handler code here
	static char BASED_CODE szFilter[] = "Input Files (*.inp)|*.inp|All Files (*.*)|*.*||";
 	CFileDialog myDialog(TRUE,"inp",m_DataFileName,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,szFilter);
	if ( IDOK == myDialog.DoModal() )
	{
		char a[50], b[50], cc[50], bb[50], dd[50];
		int d,e,f,g,h,r,s,t,u,v,w,x,aa,gg,rr,xx,yy;
		m_DataFileName = myDialog.GetFileName();
		//Tharen Rice MAy 2020 change
		//fstream ifile(fileStr,ios::nocreate | ios::in);
		fstream ifile(m_DataFileName, ios::in);
		// End of Change
		if(!ifile.is_open())
		{
			CString tmp;
			tmp = m_DataFileName;
			tmp += " does not exist";
			AfxMessageBox(tmp);
			exit(1);
		}
		ifile >>    a >> b >> dd >> bb >> cc 
				>>	m_RSLStart	>> m_RSLEnd >> m_RSLStep
				>>	m_XSLStart	>> m_XSLEnd >> m_XSLStep  
				>>	m_YSLStart	>> m_YSLEnd >> m_YSLStep
				>>	m_ZSLStart	>> m_ZSLEnd >> m_ZSLStep  
				>>	m_RC1		>> m_RC2	>> m_RC3	>> m_RC4 >> m_RC5	
				>>	m_YC1		>> m_YC2	>> m_YC3	>> m_YC4 >> m_YC5 
				>>	m_ZC1		>> m_ZC2	>> m_ZC3	>> m_ZC4 >> m_ZC5 
				>>	m_Alpha1	>> m_Alpha2	>> m_Alpha3	>> m_Alpha4 >> m_Alpha5	
				>>	m_Omega1	>> m_Omega2	>> m_Omega3	>> m_Omega4 >> m_Omega5	 
				>>	m_nSL1		>> m_nSL2	>> m_nSL3	>> m_nSL4 >> m_nSL5	 
				>>	m_XStart1	>> m_XStart2>> m_XStart3>> m_XStart4 >> m_XStart5
				>>	m_XEnd1		>> m_XEnd2	>> m_XEnd3	>> m_XEnd4 >> m_XEnd5	 
				>>	m_pAmbient	>> m_aThroat>> m_IspIdeal>>m_MassFlow
				>> m_SymDefDlg.m_RSim >> m_SymDefDlg.m_YSim	>> 	m_SymDefDlg.m_ZSim  
				>> m_SymDefDlg.m_nRev >> m_SymDefDlg.m_SL1Match >> m_SymDefDlg.m_SL2Match
				>> d >> e >> f >> g >> gg >> h >>	t >> s >> r 
				>> rr >> u >> v >> w >> x >> xx 
				>> aa >> m_MaxLengthC >> m_GridSF >> m_XStatus >> yy; 
			
		m_Throat1_CHECK.SetCheck(d);
		m_Throat2_CHECK.SetCheck(e);
		m_Throat3_CHECK.SetCheck(f);
		m_Throat4_CHECK.SetCheck(g);
		m_Throat5_CHECK.SetCheck(gg);
		m_Constraint1_CHECK.SetCheck(h);
		m_Constraint2_CHECK.SetCheck(t);
		m_Constraint3_CHECK.SetCheck(s);
		m_Constraint4_CHECK.SetCheck(r);
		m_Constraint5_CHECK.SetCheck(rr);
		m_C1_COMBO.SetCurSel(u);
		m_C2_COMBO.SetCurSel(v);
		m_C3_COMBO.SetCurSel(w);
		m_C4_COMBO.SetCurSel(x);
		m_C5_COMBO.SetCurSel(xx);
		m_contourFlag_CHECK.SetCheck(yy);
		m_MaxLengthC_CHECK.SetCheck(aa);
		UpdateScreen();
		m_filePrefix = a;
		m_SLFileName = b;
		m_MOCSummaryFileName = bb;
		m_FrictionFile = cc;
		m_MOCGridFile = dd;
		
		UpdateScreen();
		ifile.close();
	}
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CSTT2001Dlg::OnFileSave() 
{
	UpdateData(TRUE);
	// TODO: Add your command handler code here
	if (m_DataFileName == "")
		OnFileSaveas();
	else
	{
		ofstream ofile(m_DataFileName);
		ofile	<<  m_filePrefix << "\t" << m_SLFileName << "\t" << m_MOCGridFile << "\t"
				<< m_MOCSummaryFileName << "\t"<< m_FrictionFile << "\n"
				<<	m_RSLStart	<< "\t" << m_RSLEnd << "\t" << m_RSLStep<< "\n" 
				<<	m_XSLStart	<< "\t" << m_XSLEnd << "\t" << m_XSLStep<< "\n" 
				<<	m_YSLStart	<< "\t" << m_YSLEnd << "\t" << m_YSLStep<< "\n" 
				<<	m_ZSLStart	<< "\t" << m_ZSLEnd << "\t" << m_ZSLStep<< "\n" 
				<<	m_RC1		<< "\t" << m_RC2	<< "\t" << m_RC3	<< "\t"	<< m_RC4	<< "\t"	<< m_RC5	<< "\n"
				<<	m_YC1		<< "\t" << m_YC2	<< "\t" << m_YC3	<< "\t" << m_YC4	<< "\t" << m_YC5	<< "\n" 
				<<	m_ZC1		<< "\t" << m_ZC2	<< "\t" << m_ZC3	<< "\t" << m_ZC4	<< "\t" << m_ZC5	<< "\n" 
				<<	m_Alpha1	<< "\t" << m_Alpha2	<< "\t" << m_Alpha3	<< "\t"	<< m_Alpha4	<< "\t"	<< m_Alpha5	<< "\n"
				<<	m_Omega1	<< "\t" << m_Omega2	<< "\t" << m_Omega3	<< "\t" << m_Omega4	<< "\t" << m_Omega5	<< "\n" 
				<<	m_nSL1		<< "\t" << m_nSL2	<< "\t" << m_nSL3	<< "\t" << m_nSL4	<< "\t" << m_nSL5	<< "\n"
				<<	m_XStart1	<< "\t" << m_XStart2<< "\t" << m_XStart3<< "\t" << m_XStart4<< "\t" << m_XStart5<< "\n"
				<<	m_XEnd1		<< "\t" << m_XEnd2	<< "\t" << m_XEnd3	<< "\t" << m_XEnd4	<< "\t" << m_XEnd5	<< "\n" 
				<<	m_pAmbient	<< "\t" << m_aThroat<< "\t" << m_IspIdeal<<"\t"	<< m_MassFlow<<"\n" 
				<< m_SymDefDlg.m_RSim << "\t" << m_SymDefDlg.m_YSim	<< "\t"	<< m_SymDefDlg.m_ZSim  
				<< "\t" << m_SymDefDlg.m_nRev << "\t" << m_SymDefDlg.m_SL1Match << "\t" << m_SymDefDlg.m_SL2Match << "\n"
				<< m_Throat1_CHECK.GetCheck() << "\t" << m_Throat2_CHECK.GetCheck() << "\t"
				<< m_Throat3_CHECK.GetCheck() << "\t" << m_Throat4_CHECK.GetCheck() << "\t"
				<< m_Throat5_CHECK.GetCheck() << "\n"
				<< m_Constraint1_CHECK.GetCheck() << "\t" << m_Constraint2_CHECK.GetCheck() << "\t" 
				<< m_Constraint3_CHECK.GetCheck() << "\t" << m_Constraint4_CHECK.GetCheck() << "\t"
				<< m_Constraint5_CHECK.GetCheck() << "\n"
				<< m_C1_COMBO.GetCurSel() << "\t" << m_C2_COMBO.GetCurSel()	<< "\t" 
				<< m_C3_COMBO.GetCurSel() << "\t" << m_C4_COMBO.GetCurSel() << "\t" 
				<< m_C5_COMBO.GetCurSel() << "\n"
				<< m_MaxLengthC_CHECK.GetCheck() << "\t" << m_MaxLengthC << "\t"
				<< m_GridSF << "\t" << m_XStatus << "\t" 
				<< m_contourFlag_CHECK.GetCheck();
		ofile.close();	
	}
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CSTT2001Dlg::OnFileSaveas() 
{
	// TODO: Add your command handler code here
	UpdateData(TRUE);
	static char BASED_CODE szFilter[] = "Input Files (*.inp)|*.inp|All Files (*.*)|*.*||";
	CFileDialog myDialog(FALSE,"inp",m_DataFileName,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,szFilter);
	if ( IDOK == myDialog.DoModal() )
	{
		m_DataFileName = myDialog.GetFileName();
		ofstream ofile(m_DataFileName);
		ofile	<<  m_filePrefix << "\t" << m_SLFileName << "\t" << m_MOCGridFile << "\t"
				<< m_MOCSummaryFileName << "\t"<< m_FrictionFile << "\n"
				<<	m_RSLStart	<< "\t" << m_RSLEnd << "\t" << m_RSLStep<< "\n" 
				<<	m_XSLStart	<< "\t" << m_XSLEnd << "\t" << m_XSLStep<< "\n" 
				<<	m_YSLStart	<< "\t" << m_YSLEnd << "\t" << m_YSLStep<< "\n" 
				<<	m_ZSLStart	<< "\t" << m_ZSLEnd << "\t" << m_ZSLStep<< "\n" 
				<<	m_RC1		<< "\t" << m_RC2	<< "\t" << m_RC3	<< "\t"	<< m_RC4	<< "\t"	<< m_RC5	<< "\n"
				<<	m_YC1		<< "\t" << m_YC2	<< "\t" << m_YC3	<< "\t" << m_YC4	<< "\t" << m_YC5	<< "\n" 
				<<	m_ZC1		<< "\t" << m_ZC2	<< "\t" << m_ZC3	<< "\t" << m_ZC4	<< "\t" << m_ZC5	<< "\n" 
				<<	m_Alpha1	<< "\t" << m_Alpha2	<< "\t" << m_Alpha3	<< "\t"	<< m_Alpha4	<< "\t"	<< m_Alpha5	<< "\n"
				<<	m_Omega1	<< "\t" << m_Omega2	<< "\t" << m_Omega3	<< "\t" << m_Omega4	<< "\t" << m_Omega5	<< "\n" 
				<<	m_nSL1		<< "\t" << m_nSL2	<< "\t" << m_nSL3	<< "\t" << m_nSL4	<< "\t" << m_nSL5	<< "\n"
				<<	m_XStart1	<< "\t" << m_XStart2<< "\t" << m_XStart3<< "\t" << m_XStart4<< "\t" << m_XStart5<< "\n"
				<<	m_XEnd1		<< "\t" << m_XEnd2	<< "\t" << m_XEnd3	<< "\t" << m_XEnd4	<< "\t" << m_XEnd5	<< "\n" 
				<<	m_pAmbient	<< "\t" << m_aThroat<< "\t" << m_IspIdeal<<"\t"	<< m_MassFlow<<"\n" 
				<< m_SymDefDlg.m_RSim << "\t" << m_SymDefDlg.m_YSim	<< "\t"	<< m_SymDefDlg.m_ZSim  
				<< "\t" << m_SymDefDlg.m_nRev << "\t" << m_SymDefDlg.m_SL1Match << "\t" << m_SymDefDlg.m_SL2Match << "\n" 
				<< m_Throat1_CHECK.GetCheck() << "\t" << m_Throat2_CHECK.GetCheck() << "\t"
				<< m_Throat3_CHECK.GetCheck() << "\t" << m_Throat4_CHECK.GetCheck() << "\t"
				<< m_Throat5_CHECK.GetCheck() << "\n"
				<< m_Constraint1_CHECK.GetCheck() << "\t" << m_Constraint2_CHECK.GetCheck() << "\t" 
				<< m_Constraint3_CHECK.GetCheck() << "\t" << m_Constraint4_CHECK.GetCheck() << "\t"
				<< m_Constraint5_CHECK.GetCheck() << "\n"
				<< m_C1_COMBO.GetCurSel() << "\t" << m_C2_COMBO.GetCurSel()	<< "\t" 
				<< m_C3_COMBO.GetCurSel() << "\t" << m_C4_COMBO.GetCurSel() << "\t" 
				<< m_C5_COMBO.GetCurSel() << "\n"
				<< m_MaxLengthC_CHECK.GetCheck() << "\t" << m_MaxLengthC << "\t"
				<< m_GridSF << "\t" << m_XStatus << "\t" 
				<< m_contourFlag_CHECK.GetCheck();
	}
	return;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CSTT2001Dlg::UpdateScreen( void)
{
	//	This function is used to update the screen based on the inputs from the 
	//	input file.
	UpdateData(FALSE);
	OnConstraint1CHECK();
	OnConstraint2CHECK();
	OnConstraint3CHECK();
	OnConstraint4CHECK();
	OnConstraint5CHECK();
	OnThroat1CHECK();
	OnThroat2CHECK();
	OnThroat3CHECK();
	OnThroat4CHECK();
	OnThroat5CHECK();
	OnMaxLengthCCHECK();

}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CSTT2001Dlg::InitializeArrays( int j)
{
	int i;
	// Initialize Dynamically allocated arrays
	psi = new double[j];
	newPsi = new double[j];
	xt_end = new double[j];
	yt_end = new double[j];
	zt_end = new double[j];
	
	lastSLPt = new int[j];
	newSLPts = new int[j];
	nt = new int[j];

	jsl = new int*[j];

	xsl = new double*[j];
	rsl = new double*[j];
	psl = new double*[j];
	msl = new double*[j];
	thsl = new double*[j];
	tsl = new double*[j];
	xt = new double*[j];
	yt = new double*[j];
	zt = new double*[j];
	pt = new double*[j];
	xgrid = new double*[j];
	ygrid = new double*[j];
	zgrid = new double*[j];
	pgrid = new double*[j];
	
	for ( i = 0; i < j ; i++)
	{
		jsl[i] = new int[nparamPTS];
		xsl[i] = new double[nparamPTS];
		rsl[i] = new double[nparamPTS];
		psl[i] = new double[nparamPTS];
		msl[i] = new double[nparamPTS];
		tsl[i] = new double[nparamPTS];
		thsl[i] = new double[nparamPTS];
		xt[i] = new double[nparamPTS];
		yt[i] = new double[nparamPTS];
		zt[i] = new double[nparamPTS];
		pt[i] = new double[nparamPTS];
		xgrid[i] = new double[nparamGRIDX];
		ygrid[i] = new double[nparamGRIDX];
		zgrid[i] = new double[nparamGRIDX];
		pgrid[i] = new double[nparamGRIDX];
	}
	return;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CSTT2001Dlg::OnMaxLengthCCHECK() 
{
	//	enable the max length exit window based on the check
	m_MaxLengthC_EDIT.EnableWindow(m_MaxLengthC_CHECK.GetCheck());
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CSTT2001Dlg::TrimSLsToMaxLength( double maxX)
{
	//	This functions trims all of the SLs to a given length
	//	Step through all of the SLs. If the given X is greater then maxX
	//	then interpolate the values at maxX.
	int n,j,k;
	double dx;
	m_NAtMaxX = 0;
	for ( n = 0; n < nNewSLs; n++) // Loop through each SL
	{
		for ( j = 0; j <= newSLPts[n]; j++)	
		{
			if ( xt[n][j] > maxX)
			{
				m_NAtMaxX = n;
				if ( j > 0)
				{
					dx = (xt[n][j] - maxX)/(xt[n][j] - xt[n][j-1]); 
					xt[n][j] = xt[n][j] - dx * (xt[n][j] - xt[n][j-1]);
					yt[n][j] = yt[n][j] - dx * (yt[n][j] - yt[n][j-1]);
					zt[n][j] = zt[n][j] - dx * (zt[n][j] - zt[n][j-1]);
					pt[n][j] = pt[n][j] - dx * (pt[n][j] - pt[n][j-1]);
				}
				else
				{	
					xt[n][j] = 0.0;
					yt[n][j] = 0.0;
					zt[n][j] = 0.0;
					pt[n][j] = 0.0;
				}
				for ( k = j+1; k <= newSLPts[n]; k++)
				{
					xt[n][k] = xt[n][j];
					yt[n][k] = yt[n][j];
					zt[n][k] = zt[n][j];
					pt[n][k] = pt[n][j];
				}
				break; // break out of j loop
				//	set max length to  maxX
				maxLength = maxX;
			}
		}
	}
	return;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
double CSTT2001Dlg::GetFrictionLoss( double eps)
{
	//	This file gets the friction data from the specified data file and 
	//	interpolates to find the friction based on the calculated area ratio
	//	of the new nozzle.
	//	0 = area ratio
	//	1 = friction (lbf)	
	int i,itotal;
	double aRatio[25],fric[25];
	//Tharen Rice MAy 2020 change. Commented the 1st line out and added the rest
    //fstream ifile(fileStr,ios::nocreate | ios::in);
	fstream ifile(m_FrictionFile, ios::in);
	// End of Change
	if(!ifile.is_open())
	{
		CString tmp;
		tmp = m_FrictionFile;
		tmp += " does not exist";
		AfxMessageBox(tmp);
		exit(1);
	}

	ifile >> itotal;
	if (itotal >= 25) 
	{
		AfxMessageBox("The friction data file can have a max of 25 points");
		exit(1);
	}
	
	for ( i = 0; i < itotal; i++) ifile >> aRatio[i] >> fric[i];
	ifile.close();

	i = 0;
	while ( eps > aRatio[++i]);

	if ( i > 0) return fric[i] - (aRatio[i] - eps)/(aRatio[i] - aRatio[i-1])*(fric[i] - fric[i-1]);
	else return fric[i];
}
	

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
BOOL CSTT2001Dlg::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CDialog::Create(IDD, pParentWnd);
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CSTT2001Dlg::OnPlotBUTTON() 
{
	// TODO: Add your control notification handler code here
	m_PlotDlg.Create(IDD_Plot_DIALOG, NULL);
	return;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CSTT2001Dlg::OutputCenterlinePlot( int n, const double* x1, const double* y1, 
													const double* x2, const double* y2)
{
	double theta1, theta2;
	int i;
	
	//	Create an output file
	fstream clFile;
	CString fileStr = m_filePrefix;
	fileStr += "_cl.dat";
	// Tharen Rice May 2020. Replaced the commented line with a new one
	//clFile.open(fileStr, ios::out, filebuf::sh_none  );
	clFile.open(fileStr, ios::out);
	// END of Change
	clFile << "Xc (in) \t Yc (in) \t Tc (deg) \t\t Xb (in)\tYb (in)\tTb (deg)" << endl;


	for ( i = 0; i < n; i++)
	{
		if ( i > 0 )
		{
			if ( x1[i] != x1[i-1]) theta1 = atan((y1[i] - y1[i-1])/(x1[i] - x1[i-1]))*DEG_PER_RAD;
			if ( x2[i] != x2[i-1]) theta2 = atan((y2[i] - y2[i-1])/(x2[i] - x2[i-1]))*DEG_PER_RAD;
		}
		else 
		{
			theta1 = 0.0;
			theta2 = 0.0;
		}
		clFile << x1[i] << "\t" << y1[i] << "\t" << theta1 << "\t\t" 
			<< x2[i] << "\t" << y2[i] << "\t" << theta2 << endl;
	}

	return;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CSTT2001Dlg::CalcXStatus(int n, const double x, const double *x1, 
									const double *y1, double &y, double &theta)
{
	//	This function finds the min Y and corresponding angle at a point x.
	int i;	
	for ( i = 1; i < n; i++)
	{
		if ( x1[i] != x1[i-1]) theta = atan((y1[i] - y1[i-1])/(x1[i] - x1[i-1]))*DEG_PER_RAD;
		if ( x1[i] > x) break;
	}
	y= y1[i];
	return;
}

/*********************************************************************************
/*********************************************************************************
/**********************************************************************************/
void CSTT2001Dlg::GetPerformanceDataFromMOCSummaryFile( void)
{
	//	This files gets important data from the MOC output file needed to calculate
	//	nozzle performance
	fstream SUMFile;
	CString fileStr = m_MOCSummaryFileName;
	// Tharen Rice May 2020 Change
	//SUMFile.open(fileStr,ios::nocreate | ios::in);
	SUMFile.open(fileStr, ios::in);
	//End of change
	if(!SUMFile.is_open())
	{
		fileStr += " does not exist";
		AfxMessageBox(fileStr);
		exit(1);
	}
	ofstream outFile;
	fileStr = m_filePrefix;
	fileStr += "_STT_summary.out";
	outFile.open(fileStr);
	
		CString fuel, oxid;
	//char line[80];
	double isp2DMOC, Cxx, aSurfMOC, wdotMOC, FexitMOC, SexitMOC, rStarMOC, pAmbMOC;
	double aExitMOC, thrust1MOC;
	thrust1MOC = -99;
	wdotMOC = -99;
	m_epsMOC = -99;
	isp2DMOC = -99;
	aSurfMOC = -99;
	FexitMOC = -99;
	rStarMOC = -99;
	pAmbMOC = -99;
	SexitMOC = -99;
	aExitMOC = -99;
	//Tharen Rice: May 2020 Added this to read in the data correctly
	string strLine;
	//CString cLine;
	//	Find the flow properties at the nozzle entrance in the MOC summary file
	while(!SUMFile.eof())
	{
		//Tharen Rice: May 2020. Updated these calls so they work given simpler string
		// commands
		getline(SUMFile, strLine);
		// To make the rest of the stuff work.  if the line is blank, create a dummy string
		if (strLine.length() > 0)
		{
			char cLine[10];
			//	SUMFile.getline(line,80);
			// Depending on what the line says, get the correct data
			if (strLine.compare(0, 16, "2-D Gross Thrust") == 0)
				//	if (!strncmp(line,"2-D Gross Thrust",16))
			{
				strLine.copy(cLine, 10, 23);
				thrust1MOC = atof(cLine);
				//cLine = line;
				//thrust1MOC = atof(cLine.Mid(23,10));		
			}
			else if (strLine.compare(0, 12, "Surface Area") == 0)
				//else if (!strncmp(line,"Surface Area",12))
			{
				//cLine = line;
				strLine.copy(cLine, 10, 19);
				aSurfMOC = atof(cLine);
				//aSurfMOC = atof(cLine.Mid(19,10));
			}
			else if (strLine.compare(0, 13, "2-D Mass Flow") == 0)
				//else if (!strncmp(line,"2-D Mass Flow",13))
			{
				//cLine = line;
				strLine.copy(cLine, 10, 22);
				wdotMOC = atof(cLine);
				//wdotMOC = atof(cLine.Mid(22,10));		
			}
			//else if (!strncmp(line,"Expansion Ratio",15))
			else if (strLine.compare(0, 15, "Expansion Ratio") == 0)
			{
				//cLine = line;
				strLine.copy(cLine, 10, 17);
				m_epsMOC = atof(cLine);
				//m_epsMOC = atof(cLine.Mid(17,10));		
			}
			//else if (!strncmp(line,"Gross Thrust",11))
			else if (strLine.compare(0, 12, "Gross Thrust") == 0)
			{
				//cLine = line;
				strLine.copy(cLine, 10, 19);
				FexitMOC = atof(cLine);
				//FexitMOC = atof(cLine.Mid(19,10));		
			}
			//else if (!strncmp(line,"Stream Thrust",12))
			else if (strLine.compare(0, 13, "Stream Thrust") == 0)
			{
				//cLine = line;
				strLine.copy(cLine, 10, 20);
				SexitMOC = atof(cLine);
				//SexitMOC = atof(cLine.Mid(20,10));		
			}
			//else if (!strncmp(line,"Throat Radius R*",15))
			else if (strLine.compare(0, 16, "Throat Radius R*") == 0)
			{
				strLine.copy(cLine, 10, 22);
				//cLine = line;
				rStarMOC = atof(cLine);
				//rStarMOC = atof(cLine.Mid(22,10));		
			}
			//else if (!strncmp(line,"Ambient Pressure",16))
			else if (strLine.compare(0, 16, "Ambient Pressure") == 0)
			{
				//cLine = line;
				strLine.copy(cLine, 10, 24);
				pAmbMOC = atof(cLine);
				//pAmbMOC = atof(cLine.Mid(24,10));		
			}
			//else if (!strncmp(line,"Exit Area",9))
			else if (strLine.compare(0, 9, "Exit Area") == 0)
			{
				//cLine = line;
				strLine.copy(cLine, 10, 16);
				aExitMOC = atof(cLine);
				//aExitMOC = atof(cLine.Mid(16,10));		
			}
			//else if (!strncmp(line,"Isp",3))
			else if (strLine.compare(0, 3, "Isp") == 0)
			{
				//cLine = line;
				strLine.copy(cLine, 10, 17);
				isp2DMOC = atof(cLine);
				//isp2DMOC = atof(cLine.Mid(17,10));		
				break;
			}
		}
	}
	
	if (thrust1MOC < 0.0 || wdotMOC < 0.0 || m_epsMOC < 0.0 || isp2DMOC < 0.0 
		|| aSurfMOC < 0.0 || FexitMOC < 0.0 || rStarMOC < 0.0 || pAmbMOC < 0.0 
		|| SexitMOC < 0.0)
	{
		AfxMessageBox("All of the summary data was NOT read.\n");
		exit(1);
	}

	m_aExit = m_aThroat + m_ProjectedArea;
	//	Now its time to calculate the nozzle parameters
	//	The loss due to friction will be the ratio of surface areas of the TDK nozzle
	//	and the STT nozzle.  All of these number are the loss in terms of a thrust
	//	the current nozzle throat is based on the area ratio of the MOC nozzle and the 
	//	STT nozzle 
	m_thrust1 = thrust1MOC*m_aThroat/(PI*rStarMOC*rStarMOC);
	m_fricLoss = GetFrictionLoss(m_SurfaceArea);
	m_pALoss = m_pAmbient*m_aExit;
	m_IspCalc = (m_thrust1 + m_Force - m_fricLoss - m_pALoss)/m_MassFlow;
	Cxx = m_IspCalc/m_IspIdeal;

	outFile << "This is the summary file for STT2001\n\nMOC DATA for a full nozzle" << endl
		<< "Throat Radius (in):\t" << rStarMOC << endl 
		<< "Mass Flow (lbm/s):\t" << wdotMOC << endl
	 << "Area Ratio:\t" << m_epsMOC << endl
	 << "Exit Area (in2):\t" << aExitMOC << endl
	 << "Surface Area (in2):\t" << aSurfMOC << endl
	 << "2-D Stream Thrust @ throat(lbf):\t" << thrust1MOC << endl
	 << "Stream Thrust @ Exit (lbf):\t" << SexitMOC << endl
	 << "Ambient Pressure (psia):\t" << pAmbMOC << endl
	 << "Gross Thrust @ exit(lbf):\t" << FexitMOC << endl
	 << "Gross ISP at Exit (lbf-sec/lbm):\t" << isp2DMOC << endl
	 
		 
	 << "\nSTT2001 Calculated Parameters" << endl
	 << "Nozzle Surface Area (in2):\t" << m_SurfaceArea << endl
	 << "Nozzle Throat Area (in2):\t" << m_aThroat << endl
	 << "Nozzle Wall Axial Projected Area (in2):\t" << m_ProjectedArea << endl 
	 << "Nozzle Exit Area (in2):\t" << m_aExit << endl
	 << "Nozzle Area Ratio:\t" << m_aExit/m_aThroat << endl
	 << "Mass Flow (lbm/s):\t" << m_MassFlow << endl
	 << "Throat Thrust(lbf):\t" << m_thrust1 << endl
	 << "Pressure Force (lbf):\t" << m_Force << endl 
	 <<	"Exit Stream Thrust (lbf):\t" << m_thrust1 + m_Force << endl
	 <<	"Pa Ae Loss (lbf):\t" << -m_pALoss << endl
	 << "Exit Gross Thrust (lbf):\t" << m_thrust1 + m_Force - m_pALoss << endl
	 << "Friction Loss (lbf):\t" << -m_fricLoss << endl
	 << "Exit Gross Thrust w/ Friction (lbf):\t" << m_thrust1 + m_Force - m_fricLoss - m_pALoss
	 << endl << endl;

	//  Now its time to breakdown the Isp in terms of kinetics, friction, pressure and 
	//	divergence losses.  These numbers will be reported as a percentage loss of the 
	//	total Ideal Isp.  The friction and underexpansion losses are easy to 
	//	calculate.  The kinetics and divergence losses are harder to separate. For now
	//	it will be assumed that the ratio of divergence/kinetics losses for the 
	//	designed nozzle is the same as the divergence/kinetics losses of the known flowfield
	//	if Cs were computed.
	
	double totalLoss = m_fricLoss +m_pALoss;
	double fricLoss = m_fricLoss/ (m_MassFlow*m_IspIdeal);
	double pALoss = m_pALoss / (m_MassFlow*m_IspIdeal);
	//	Divergence loss is the difference in the other values
//	double divToKin = (ispODK - ispTDK)/(ispODE - ispODK);
//	kinLoss = (1 - Cxx - fricLoss - pALoss)/(1 + divToKin);
//	divLoss = divToKin * kinLoss;
	
	outFile
	 << "Calculated Isp (lbf-s/lbm)\t" << m_IspCalc << endl
	 << "Ideal Isp (lbf-s/lbm)\t" << m_IspIdeal << endl
	 << "Cfg\t" << Cxx << endl
	 <<	"Losses Summary\tCfg Loss\t(% of Total Loss)" << endl
//	 << "Divergence Loss\t" << divLoss*100 << "\t" << divLoss*100/(1 - Cxx) << endl
//	 << "Kinetics Loss\t" << kinLoss*100 << "\t" << kinLoss*100/(1 - Cxx) << endl
	 << "Friction Loss\t" << fricLoss  << "\t" << m_fricLoss*100/totalLoss << endl
	 << "Exit Pressure Loss\t" << pALoss << "\t" << m_pALoss*100/totalLoss << endl;
//	 << "Div. & Kin. Loss\t" << 1- (Cxx + fricLoss+pALoss) << "\t" 
//	 << (1- (Cxx + m_fricLoss+m_pALoss))*100/(1-Cxx) << endl; 

	//	Set the CString variables that are shown in the Dialog box
	m_integralPdAStr.Format( "%.1f", m_Force);
	m_frictionForceStr.Format( "%.1f", -m_fricLoss);
	m_throatForceStr.Format( "%.1f", m_thrust1);
	m_pExitForceStr.Format( "%.1f", -m_pALoss);
	m_surfaceAreaStr.Format( "%.2f", m_SurfaceArea);
	m_projectedAreaStr.Format( "%.2f", m_ProjectedArea);
	m_CxxStr.Format( "%.4f", Cxx);
	m_IspCalcStr.Format( "%.2f",m_IspCalc);
	m_MaxLengthStr.Format( "%.2f",maxLength);
	m_AreaRatioStr.Format( "%.2f",m_aExit/m_aThroat); 
	m_ExitAreaStr.Format( "%.2f",m_aExit); 
	m_MaxYStr.Format( "%.2f", maxY);
	m_MinYStr.Format( "%.2f", minY);
	m_XAtMaxYStr.Format( "%.2f", m_XAtMaxY);
	m_XAtMinYStr.Format( "%.2f", m_XAtMinY);	

	UpdateData(FALSE);
	return ;

}

/*********************************************************************************
/*********************************************************************************
/**********************************************************************************/
double CSTT2001Dlg::FindMaxX(int nSLs)
{
	//	This function is called when the user wants to crop the nozzle streamlines
	//	because a portion of the MOC grid downstream of the trimmed streamline is
	//	not valid.
	//	Based on the MOC grid, the streamline that is closest to the nozzle
	//	centerline will produce the smallest X value. 
	//	This function will search look at each streamline end point and determine
	//	at what X the MOC grid becomes invalid.  The minimum calculated X will
	//	be returned.
	//	Search thru the nt[n] variable for the minimum i values. This is the 
	//	closest streamline
	//	minTrim refers to the SL that has been trimmed do to the constraints
	const int kParam = 199; 
	// the jParan value has to be set to a value that is greater than the number
	// of j entries in the MOC_Grid.plt file.
	const int jParam = 220; 
	int i, n,k,j, iMatch, jMatch = -99, jj, m;
	double minTrimX = 9e9,jf, minTrimR, minDist=9e9, dist, xRW, rRW;
	int minTrimI = 99999,iMOC[jParam][kParam], lastPt[jParam], iAxial;
	double xMOC[jParam][kParam], rMOC[jParam][kParam], rad;
	//	minI refers to the streamline that needs to be investigated
	//	Also search thru the last points of all of the strealines to find the 
	//	minimum x point
	for ( n = 0; n < nSLs; n++)
	{
		if (xt[n][newSLPts[n]] < minTrimX)
		{
			minTrimX = xt[n][newSLPts[n]];
			minTrimR = sqrt( pow(yt[n][newSLPts[n]],2) + pow(zt[n][newSLPts[n]],2));
			minTrimI = n;
		}
	}

	//	Open the MOC_Grid file
	CString fileStr = m_MOCGridFile;
	//Tharen Rice MAy 2020 change
	//fstream ifile(fileStr,ios::nocreate | ios::in);
	fstream ifile(fileStr, ios::in);
	// End of Change
	if(!ifile.is_open())
	{
		fileStr += " does not exist";
		AfxMessageBox(fileStr);
		exit(1);
	}
	//  Read in the I,J,X and R data
	//	The data file contains Grid data in the form of
	//	Line 1-3: Dummy header
	//	Line even..: J = X: centerline  1:wall
	//	Line odd...: x/R, r/R, Mach, Theta, massflow, i
	//	i: streamline number: 0 = centerline,  maximum i = wall 
	
	char cstr[10];
	double xIn,rIn,a,b,c;
	int iIn;
	string strLine;

	// Tharen Rice. May 2020. I blew away all of the old functionality and wrote the code below.
	//  Find the 1st set of data. It begins with the "zone t=" line
	// skip the 1st 3 lines
	getline(ifile, strLine);
	getline(ifile, strLine);
	getline(ifile, strLine);
	
	int firstTimeThru = 1;
	while (!ifile.eof())
	{
		// peek at the next line.  If it does not start with z, then output the data to variable
		// At the end of all of this, j, xMOC, rMOC and iMOC should have been extracted from the 
		// MOC_Grid.plt file.

		// If the line starts with z, the record the next psi value
		char zstr = ifile.peek();
		if (zstr == 'z')
		{
			if (firstTimeThru)
			{
				firstTimeThru = 0;
			}
			else
			{
				lastPt[j] = k - 1;
			}
			k = 0;
			//	Get the first PSI value
			getline(ifile, strLine);
			for (int jjk = 0; jjk < 3; jjk++)
			{
				cstr[jjk] = strLine[12 + jjk];
			}

			jf = atof(cstr);
			j = (int)jf;

			if (j >= jParam)
			{
				AfxMessageBox("FindMaxX: More data then what has been initialized in j");
				exit(1);
			}
		
		}
		else
		{
			ifile >> xIn >> rIn >> a >> b >> c >> iIn >> ws;
			// i: streamline number
			// k: point on the streamline
			xMOC[k][j] = xIn;
			rMOC[k][j] = rIn;
			iMOC[k][j] = iIn;
			k++;
			if (k >= kParam)
			{
				AfxMessageBox("FindMaxX: More data then what has been initialized in k");
				exit(1);
			}


		}
	} // end of reading data


	lastPt[j] = k-1;

	minDist = 9e9;
	//	Search the grid to find the closest grid point to the MinTrim point
	for ( jj = 0; jj <= j; jj++)
	{
		k = 0;
		while ( xMOC[k][jj] < minTrimX && k <= lastPt[jj]) k++;
		if ( k <= lastPt[jj])
		{
			// Find the distance between the two point
			dist = sqrt( pow(xMOC[k][jj] - minTrimX,2) + pow(rMOC[k][jj]-minTrimR,2));
			if ( dist < minDist)
			{
				minDist = dist;
				iMatch = iMOC[k][jj];
				jMatch = jj;
			}
		}
	}
	// Tharen Rice: May 2020: I was catching an error that jMatch was not being defined
	// when the loop above what not finding the mindist. To fix this, I set jMatch to 
	// a negative number. If it is still negative here, then it is time to exit
	if (jMatch < 0)
	{
		return -1.0;
	}


	//	It is now known that the closest MOC Grid point to the minTrimX point is 
	//	denoted by iMatch, jMatch. Several things can now happen.
	//	The last point along jMatch is crucial. If that point is on the MOC_grid
	//	centerline (r = 0), then the reflective wave has to be found.  If the 
	//	last point is not a r = 0, it means that the reflective wave is beyond 
	//	the nozzle exit plane, and therefore, -1.0 should be returned.

	iAxial = iMOC[lastPt[jMatch]][jMatch];
	xRW = xMOC[iAxial][jMatch];
	rRW = rMOC[iAxial][jMatch];

	if (rMOC[iAxial][jMatch] > 0.0) return -1.0;
	else
	{
		//	It will be assumed that he reflective wave will start and iAxial,jMatch 
		//	and will travel thru subsequent J's following a given convention
		//	i-m, j+m. This is not exactly right, but it is close
		for ( m = 1; m <= iAxial; m++)
		{
			xRW = xMOC[iAxial-m][jMatch+m];
			rRW = rMOC[iAxial-m][jMatch+m];
			
			//	Step through all of the points of all of the streamlines
			//	Find the first point on each streamline that is downstream of the 
			//	xRW point. At that point, if rRW is greater then the SL point
			//	then return the xRW as the maximum X values
			for ( n = 0; n < nSLs; n++)
			{
				i = 0;
				while ( xt[n][i] < xRW && i <= newSLPts[n] ) i++;
				if ( i <= newSLPts[n])
				{
					rad = sqrt(pow(yt[n][i-1],2) + pow(zt[n][i-1],2));
					if (rad < rRW)
					{
						return xRW;
					}
				}
			}	// End of for n loop
		}	// End of for jj loop (MOC Grid iteration)
	}	// end of else
	return -1.0;
}

