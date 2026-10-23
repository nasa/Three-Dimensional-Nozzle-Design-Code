// MOC_GridDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MOC_Grid.h"
#include "MOC_GridDlg.h"
#include "DlgProxy.h"
#include <afxdlgs.h>
#include "MOC_GridCalc_BDE.h"
// Tharen Rice May 2020 Change: New call for fstream
//#include "fstream.h"
#include <fstream>
#include <sstream>
#include <string>
#include <cctype>
using namespace std;
// End of changes
#include "direct.h"


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
// CMOC_GridDlg dialog

IMPLEMENT_DYNAMIC(CMOC_GridDlg, CDialog);

CMOC_GridDlg::CMOC_GridDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMOC_GridDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMOC_GridDlg)
	m_mDesign = 4.0;
	m_nC = 101;
	m_RWTD = 1.0;
	m_molWt_i = 28.96;
	m_pres_i = 1000;
	m_temp_i = 530;
	m_gamma_i = 1.4;
	m_RWTU = 1.0;
	m_dTLimit = .5;
	m_pAmb = 0.0;
	m_eps = 15;
	m_length = 5.03258;
	m_rE = 1.6;
	m_xE = 4.308;
	m_nRRCAboveBD = 100;
	m_pExit = 13.691;
	m_nSLi = 10;
	m_nSLj = 50;
	m_vel = 3022.0;
	m_thetaBi = 25.0;
	m_coneAngle = 15.0;
	m_ispIdeal = 100.0;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pAutoProxy = NULL;
}

CMOC_GridDlg::~CMOC_GridDlg()
{
	// If there is an automation proxy for this dialog, set
	//  its back pointer to this dialog to NULL, so it knows
	//  the dialog has been deleted.
	if (m_pAutoProxy != NULL)
		m_pAutoProxy->m_pDialog = NULL;
}

void CMOC_GridDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMOC_GridDlg)
	DDX_Control(pDX, IDC_nSLj_EDIT, m_nSLj_EDIT);
	DDX_Control(pDX, IDC_nSLi_EDIT, m_nSLi_EDIT);
	DDX_Control(pDX, IDC_STTRun_BUTTON, m_STTRun_BUTTON);
	DDX_Control(pDX, IDC_3DRun_BUTTON, m_3DRun_BUTTON);
	DDX_Control(pDX, IDC_coneAngle_EDIT, m_coneAngle_EDIT);
	DDX_Control(pDX, IDC_Print_COMBO, m_print_COMBO);
	DDX_Control(pDX, IDC_throat_CHECK, m_throat_CHECK);
	DDX_Control(pDX, IDC_vel_EDIT, m_vel_EDIT);
	DDX_Control(pDX, IDC_pExit_EDIT, m_pExit_EDIT);
	DDX_Control(pDX, IDC_CalcMOC_BUTTON, m_CalcMOC_BUTTON);
	DDX_Control(pDX, IDC_nRRCAboveBD_EDIT, m_nRRCAboveBD_EDIT);
	DDX_Control(pDX, IDC_perfect_RADIO, m_perfect_RADIO);
	DDX_Control(pDX, IDC_exitMach_RADIO, m_exitMach_RADIO);
	DDX_Control(pDX, IDC_Axi_RADIO, m_Axi_RADIO);
	DDX_Control(pDX, IDC_2D_RADIO, m_2D_RADIO);
	DDX_Control(pDX, IDC_cone_RADIO, m_cone_RADIO);
	DDX_Control(pDX, IDC_minLength_RADIO, m_minLength_RADIO);
	DDX_Control(pDX, IDC_endPoint_RADIO, m_endPoint_RADIO);
	DDX_Control(pDX, IDC_eps_RADIO, m_eps_RADIO);
	DDX_Control(pDX, IDC_length_RADIO, m_length_RADIO);
	DDX_Control(pDX, IDC_pExit_RADIO, m_pExit_RADIO);
	DDX_Control(pDX, IDC_eps_EDIT, m_eps_EDIT);
	DDX_Control(pDX, IDC_length_EDIT, m_length_EDIT);
	DDX_Control(pDX, IDC_mDesign_EDIT, m_mDesign_EDIT);
	DDX_Control(pDX, IDC_xE_EDIT, m_xE_EDIT);
	DDX_Control(pDX, IDC_rE_EDIT, m_rE_EDIT);
	DDX_Text(pDX, IDC_mDesign_EDIT, m_mDesign);
	DDV_MinMaxDouble(pDX, m_mDesign, 1.1, 100.);
	DDX_Text(pDX, IDC_nC_EDIT, m_nC);
	DDV_MinMaxInt(pDX, m_nC, 2, 301);
	DDX_Text(pDX, IDC_RWTD_EDIT, m_RWTD);
	DDV_MinMaxDouble(pDX, m_RWTD, 0., 9999.);
	DDX_Text(pDX, IDC_molWt_i_EDIT, m_molWt_i);
	DDX_Text(pDX, IDC_pres_i_EDIT, m_pres_i);
	DDX_Text(pDX, IDC_temp_i_EDIT, m_temp_i);
	DDX_Text(pDX, IDC_gamma_i_EDIT, m_gamma_i);
	DDX_Text(pDX, IDC_RWTU_EDIT, m_RWTU);
	DDV_MinMaxDouble(pDX, m_RWTU, 0., 9999.);
	DDX_Text(pDX, IDC_dTLimit_EDIT, m_dTLimit);
	DDV_MinMaxDouble(pDX, m_dTLimit, 0., 10.);
	DDX_Text(pDX, IDC_pAmb_EDIT, m_pAmb);
	DDV_MinMaxDouble(pDX, m_pAmb, 0., 9000000000.);
	DDX_Text(pDX, IDC_eps_EDIT, m_eps);
	DDV_MinMaxDouble(pDX, m_eps, 0., 9000000000.);
	DDX_Text(pDX, IDC_length_EDIT, m_length);
	DDV_MinMaxDouble(pDX, m_length, 0., 9000000000.);
	DDX_Text(pDX, IDC_rE_EDIT, m_rE);
	DDX_Text(pDX, IDC_xE_EDIT, m_xE);
	DDV_MinMaxDouble(pDX, m_xE, 0., 9000000000.);
	DDX_Text(pDX, IDC_nRRCAboveBD_EDIT, m_nRRCAboveBD);
	DDV_MinMaxInt(pDX, m_nRRCAboveBD, 20, 1000);
	DDX_Text(pDX, IDC_pExit_EDIT, m_pExit);
	DDX_Text(pDX, IDC_nSLi_EDIT, m_nSLi);
	DDV_MinMaxInt(pDX, m_nSLi, 1, 100);
	DDX_Text(pDX, IDC_nSLj_EDIT, m_nSLj);
	DDV_MinMaxInt(pDX, m_nSLj, 1, 200);
	DDX_Text(pDX, IDC_vel_EDIT, m_vel);
	DDV_MinMaxDouble(pDX, m_vel, 0., 9000000000.);
	DDX_Text(pDX, IDC_THETAB_EDIT, m_thetaBi);
	DDV_MinMaxDouble(pDX, m_thetaBi, 0.1, 40.);
	DDX_Text(pDX, IDC_coneAngle_EDIT, m_coneAngle);
	DDV_MinMaxDouble(pDX, m_coneAngle, 0., 45.);
	DDX_Text(pDX, IDC_ispIdeal_EDIT, m_ispIdeal);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMOC_GridDlg, CDialog)
	//{{AFX_MSG_MAP(CMOC_GridDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_2D_RADIO, On2dRADIO)
	ON_BN_CLICKED(IDC_Axi_RADIO, OnAxiRADIO)
	ON_BN_CLICKED(IDC_endPoint_RADIO, OnendPointRADIO)
	ON_BN_CLICKED(IDC_eps_RADIO, OnepsRADIO)
	ON_BN_CLICKED(IDC_exitMach_RADIO, OnexitMachRADIO)
	ON_BN_CLICKED(IDC_length_RADIO, OnlengthRADIO)
	ON_BN_CLICKED(IDC_minLength_RADIO, OnminLengthRADIO)
	ON_BN_CLICKED(IDC_perfect_RADIO, OnperfectRADIO)
	ON_BN_CLICKED(IDC_cone_RADIO, OnConeRADIO)
	ON_BN_CLICKED(IDC_CalcMOC_BUTTON, OnCalcMOCBUTTON)
	ON_BN_CLICKED(IDC_pExit_RADIO, OnpExitRADIO)
	ON_BN_CLICKED(IDC_throat_CHECK, OnthroatCHECK)
	ON_CBN_SELCHANGE(IDC_Print_COMBO, OnSelchangePrintCOMBO)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_COMMAND(ID_FILE_SAVEAS, OnFileSaveas)
	ON_BN_CLICKED(IDC_STTRun_BUTTON, OnSTTRunBUTTON)
	ON_BN_CLICKED(IDC_3DRun_BUTTON, On3DRunBUTTON)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMOC_GridDlg message handlers

BOOL CMOC_GridDlg::OnInitDialog()
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
	
	m_Axi_RADIO.SetCheck(1);
	m_minLength_RADIO.SetCheck(1);
	m_exitMach_RADIO.SetCheck(1);
	OnAxiRADIO();
	OnexitMachRADIO();
	OnminLengthRADIO();
	
	OnthroatCHECK();
	m_print_COMBO.SetCurSel(0);
	OnSelchangePrintCOMBO();
	m_STTRun_BUTTON.EnableWindow(0);
	m_3DRun_BUTTON.EnableWindow(0);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMOC_GridDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CMOC_GridDlg::OnPaint() 
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
HCURSOR CMOC_GridDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

// Automation servers should not exit when a user closes the UI
//  if a controller still holds on to one of its objects.  These
//  message handlers make sure that if the proxy is still in use,
//  then the UI is hidden but the dialog remains around if it
//  is dismissed.

void CMOC_GridDlg::OnClose() 
{
	if (CanExit())
		CDialog::OnClose();
}

void CMOC_GridDlg::OnOK() 
{
	if (CanExit())
		CDialog::OnOK();
}

void CMOC_GridDlg::OnCancel() 
{
	if (CanExit())
		CDialog::OnCancel();
}

BOOL CMOC_GridDlg::CanExit()
{
	// If the proxy object is still around, then the automation
	//  controller is still holding on to this application.  Leave
	//  the dialog around, but hide its UI.
	if (m_pAutoProxy != NULL)
	{
		ShowWindow(SW_HIDE);
		return FALSE;
	}

	return TRUE;
}

void CMOC_GridDlg::On2dRADIO() 
{
	if (m_2D_RADIO.GetCheck()) nozzleGeom = TWOD;
	return;	
}

void CMOC_GridDlg::OnAxiRADIO() 
{
	if (m_Axi_RADIO.GetCheck()) nozzleGeom = AXI;
	return;	
}


void CMOC_GridDlg::OnepsRADIO() 
{
	if (m_eps_RADIO.GetCheck())
	{
		designParam = EPS;
		m_mDesign_EDIT.EnableWindow(0);
		m_eps_EDIT.EnableWindow(1);
		m_length_EDIT.EnableWindow(0);
		m_pExit_EDIT.EnableWindow(0);
	}
	return;
}

void CMOC_GridDlg::OnexitMachRADIO() 
{
	if (m_exitMach_RADIO.GetCheck())
	{
		designParam = EXITMACH;
		m_mDesign_EDIT.EnableWindow(1);
		m_eps_EDIT.EnableWindow(0);
		m_length_EDIT.EnableWindow(0);
		m_pExit_EDIT.EnableWindow(0);
	}
	return;	
}

void CMOC_GridDlg::OnlengthRADIO() 
{
	if (m_length_RADIO.GetCheck())
	{
		designParam = NOZZLELENGTH;
		//	Enable/Disable the correct windows and RADIOS
		m_mDesign_EDIT.EnableWindow(0);
		m_eps_EDIT.EnableWindow(0);
		m_length_EDIT.EnableWindow(1);
		m_pExit_EDIT.EnableWindow(0);
	}
	return;
}

void CMOC_GridDlg::OnpExitRADIO() 
{
	if (m_pExit_RADIO.GetCheck())
	{
		designParam = EXITPRESSURE;
		m_mDesign_EDIT.EnableWindow(0);
		m_eps_EDIT.EnableWindow(0);
		m_length_EDIT.EnableWindow(0);
		m_pExit_EDIT.EnableWindow(1);
	}
}

void CMOC_GridDlg::OnendPointRADIO() 
{
	if (m_endPoint_RADIO.GetCheck())
	{
		nozzleType = FIXEDEND;
		designParam = ENDPOINT;
		//	Enable/Disable the correct windows and RADIOS
		m_rE_EDIT.EnableWindow(1);
		m_xE_EDIT.EnableWindow(1);
		m_coneAngle_EDIT.EnableWindow(0);
		m_mDesign_EDIT.EnableWindow(0);
		m_eps_EDIT.EnableWindow(0);
		m_length_EDIT.EnableWindow(0);
		m_pExit_EDIT.EnableWindow(0);
		m_exitMach_RADIO.EnableWindow(0);
		m_eps_RADIO.EnableWindow(0);
		m_length_RADIO.EnableWindow(0);
		m_pExit_RADIO.EnableWindow(0);
	}
	return;	
}

void CMOC_GridDlg::OnminLengthRADIO() 
{
	if (m_minLength_RADIO.GetCheck())
	{
		nozzleType = RAO;
		//	Enable/Disable the correct windows and RADIOS
		//	The code automatically unChecks the PERFECT, OPTIMUM and ENDPOINT RADIO.
		//	The ENDPOINT EDIT boxes must be disabled. Also, the code checks if any of the
		//	design parameters RADIOS have been checked.  If they were then proceed.
		m_rE_EDIT.EnableWindow(0);
		m_xE_EDIT.EnableWindow(0);
		m_coneAngle_EDIT.EnableWindow(0);
		//	Enable all of the design parameter RADIO 
		m_exitMach_RADIO.EnableWindow(1);
		m_length_RADIO.EnableWindow(1);
		m_endPoint_RADIO.EnableWindow(1);
		m_pExit_RADIO.EnableWindow(1);
		m_eps_RADIO.EnableWindow(1);
		if (m_eps_RADIO.GetCheck()) OnepsRADIO();
		else if (m_exitMach_RADIO.GetCheck()) OnexitMachRADIO();
		else if (m_length_RADIO.GetCheck()) OnlengthRADIO();
		else if (m_pExit_RADIO.GetCheck()) OnpExitRADIO();
	}
	return;	
}

void CMOC_GridDlg::OnperfectRADIO() 
{
	if (m_perfect_RADIO.GetCheck())
	{
		nozzleType = PERFECT;
		//	Enable/Disable the correct windows and RADIOS
		m_rE_EDIT.EnableWindow(0);
		m_xE_EDIT.EnableWindow(0);
		m_coneAngle_EDIT.EnableWindow(0);
		//	Enable all of the design parameter RADIO 
		m_exitMach_RADIO.EnableWindow(1);
		m_length_RADIO.EnableWindow(1);
		m_endPoint_RADIO.EnableWindow(1);
		m_pExit_RADIO.EnableWindow(1);
		m_eps_RADIO.EnableWindow(1);
		if (m_eps_RADIO.GetCheck()) OnepsRADIO();
		else if (m_exitMach_RADIO.GetCheck()) OnexitMachRADIO();
		else if (m_length_RADIO.GetCheck()) OnlengthRADIO();
		else if (m_pExit_RADIO.GetCheck()) OnpExitRADIO();
	}
	return;	
}

void CMOC_GridDlg::OnConeRADIO() 
{
	if (m_cone_RADIO.GetCheck())
	{
		nozzleType = CONE;
		//	Enable/Disable the correct windows and RADIOS
		m_rE_EDIT.EnableWindow(0);
		m_xE_EDIT.EnableWindow(0);
		m_coneAngle_EDIT.EnableWindow(1);
		//	Enable all of the design parameter RADIO 
		m_exitMach_RADIO.EnableWindow(1);
		m_length_RADIO.EnableWindow(1);
		m_endPoint_RADIO.EnableWindow(1);
		m_pExit_RADIO.EnableWindow(1);
		m_eps_RADIO.EnableWindow(1);
		if (m_eps_RADIO.GetCheck()) OnepsRADIO();
		else if (m_exitMach_RADIO.GetCheck()) OnexitMachRADIO();
		else if (m_length_RADIO.GetCheck()) OnlengthRADIO();
		else if (m_pExit_RADIO.GetCheck()) OnpExitRADIO();
	}
	return;	
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//	Helpers for the MOC_Grid -> STT2001 hand-off
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//	Returns the full path of a file in a sibling tool folder. The programs are installed
//	side by side (...\MOC_Grid\MOC_Grid.exe, ...\STT2001\STT2001.exe), so this takes the
//	folder holding this exe, goes up one level and appends 'relative'.
static CString GetToolPath(const char* relative)
{
	TCHAR buf[MAX_PATH];
	GetModuleFileName(NULL, buf, MAX_PATH);
	CString p(buf);
	int s = p.ReverseFind('\\');
	if (s > 0) p = p.Left(s);		//	...\MOC_Grid
	s = p.ReverseFind('\\');
	if (s > 0) p = p.Left(s);		//	...  (the folder holding all of the tools)
	p += "\\";
	p += relative;
	return p;
}

//	TRUE if any number in the file is NaN or infinity. A MOC run that fails to converge
//	writes these, and the downstream programs cannot use such a file.
static BOOL FileHasInvalidNumbers(const CString& name)
{
	ifstream f((LPCTSTR)name);
	if (!f.is_open()) return FALSE;
	string line, tok;
	while (getline(f, line))
	{
		istringstream ss(line);
		while (ss >> tok)
		{
			size_t st = (tok[0] == '-' || tok[0] == '+') ? 1 : 0;
			string u;
			for (size_t i = st; i < tok.size(); i++) u += (char)tolower((unsigned char)tok[i]);
			if (u == "nan" || u == "inf" || u == "infinity" 
				|| u.compare(0, 4, "nan(") == 0 || u.compare(0, 4, "inf(") == 0) return TRUE;
		}
	}
	return FALSE;
}

void CMOC_GridDlg::OnCalcMOCBUTTON() 
{
	int flag = 0;
	m_STTRun_BUTTON.EnableWindow(0);
	m_3DRun_BUTTON.EnableWindow(0);
	if (!UpdateData(TRUE)) return;	//	an input was out of range; the user has been told

	//	1. Ask where all of the output files should go. All of the files that MOC_Grid
	//	writes use plain names, so making that folder the working directory sends them there.
	CFolderPickerDialog folderDlg(m_OutputDir.IsEmpty() ? (LPCTSTR)NULL : (LPCTSTR)m_OutputDir, 0, this, 0);
	folderDlg.m_ofn.lpstrTitle = _T("Choose the folder where all of the output files will be saved");
	if (folderDlg.DoModal() != IDOK) return;
	m_OutputDir = folderDlg.GetPathName();
	if (!SetCurrentDirectory(m_OutputDir))
	{
		AfxMessageBox("That folder cannot be used. Please choose another one.");
		return;
	}

	//	2. Save the inputs first, so they are kept even if the calculation fails.
	m_DataFileName = m_OutputDir + "\\MOC_Grid.inp";
	WriteInputFile(m_DataFileName);

	//	3. Run the calculation. This is the function that starts the whole process.
	MOC_GridCalc *moc = new MOC_GridCalc;
	m_CalcMOC_BUTTON.EnableWindow(0);
	if(!moc->SetInitialProperties( m_pres_i, m_temp_i,  m_molWt_i, 
		 m_gamma_i,  m_pAmb,  m_nC,  m_RWTU,  m_RWTD,  m_dTLimit, m_nRRCAboveBD, 
		 m_nSLi, m_nSLj, m_vel, m_throat_CHECK.GetCheck(), m_ispIdeal))
	{
		AfxMessageBox("Could not set properties");
		delete moc;
		m_CalcMOC_BUTTON.EnableWindow(1);
		return;
	}

	//	Set the design variable, designValues[0] and [1] to the correct values depending on the 
	//	designParamtype

	if ( designParam == EXITMACH) designValues[0] = m_mDesign;
	else if ( designParam == EPS) designValues[0] = m_eps;
	else if ( designParam == NOZZLELENGTH) designValues[0] = m_length;
	else if ( designParam == EXITPRESSURE) designValues[0] = m_pExit;
	
	if ( nozzleType == FIXEDEND)
	{
		designParam = ENDPOINT;
		designValues[0] = m_xE;
		designValues[1] = m_rE;
	}
	else if (nozzleType == CONE) designValues[1] = m_coneAngle;
			
	moc->SetSolutionParameters(  nozzleGeom,  nozzleType ,  designParam,  designValues[0],
			 designValues[1], m_thetaBi);	
	moc->SetPrintMode(m_print_COMBO.GetCurSel());
	flag = moc->CreateMOCGrid();	//	shows the nozzle plot when it is done
			
	m_CalcMOC_BUTTON.EnableWindow(1);
	delete moc;

	//	4. Report what happened
	CString msg;
	if (!flag)
	{
		AfxMessageBox("The MOC calculation did not complete. Try different inputs.");
		return;
	}
	if (FileHasInvalidNumbers("summary.out") || FileHasInvalidNumbers("MOC_SL.plt") 
		|| FileHasInvalidNumbers("MOC_Grid.plt"))
	{
		msg = "The calculation did not converge. Part of the solution is not a number (NaN), ";
		msg += "so these results cannot be used for streamline tracing.\n\n";
		msg += "Try different inputs (for example a different exit Mach number or nozzle type, ";
		msg += "more starting characteristics, or a smaller DTHETAB Max).\n\n";
		msg += "Your inputs were saved to:\n" + m_DataFileName;
		AfxMessageBox(msg);
		return;
	}
	//	3D_MOC only needs summary.out and Summary.plt, which every good run writes
	m_3DRun_BUTTON.EnableWindow(1);
	if (!m_print_COMBO.GetCurSel())
	{
		msg = "The results were saved to:\n" + m_OutputDir + "\n\n";
		msg += "To trace streamlines, set the Print Option to Full and calculate again.\n";
		msg += "To run the 3D MOC tool on this nozzle, click 'Run 3D MOC'.";
		AfxMessageBox(msg);
		return;
	}

	//	5. Get the folder ready for the streamline tracing tool: a copy of the friction
	//	table (never overwritten, so edits to it are kept) and a ready-to-run STT2001.inp
	CString friction = GetToolPath("STT2001\\friction_table.txt");
	if (!CopyFile(friction, m_OutputDir + "\\friction_table.txt", TRUE) 
		&& GetLastError() != ERROR_FILE_EXISTS)
	{
		AfxMessageBox("Could not find friction_table.txt next to STT2001.exe, so it could not be copied to the output folder.\n"
			"Copy it there yourself before running the streamline tracing tool.");
	}
	CString leaf = m_OutputDir;
	leaf.TrimRight('\\');
	leaf = leaf.Mid(leaf.ReverseFind('\\') + 1);
	if (leaf.IsEmpty() || leaf.Find(':') >= 0) leaf = "STT";
	for (int i = 0; i < leaf.GetLength(); i++)
		if (!isalnum((unsigned char)leaf[i]) && leaf[i] != '-') leaf.SetAt(i, '_');
	if (!WriteSTTInputFile(leaf)) return;

	m_STTRun_BUTTON.EnableWindow(1);
	msg = "Done. All output files and the input files (MOC_Grid.inp and STT2001.inp) were saved to:\n";
	msg += m_OutputDir + "\n\nClick 'Run Streamline Tracing Tool' or 'Run 3D MOC' to continue.";
	AfxMessageBox(msg);
	return;
}

void CMOC_GridDlg::OnthroatCHECK() 
{
	// TODO: Add your control notification handler code here
	if(m_throat_CHECK.GetCheck()) m_vel_EDIT.EnableWindow(1);
	else m_vel_EDIT.EnableWindow(0); 
}

void CMOC_GridDlg::OnSelchangePrintCOMBO() 
{
	// TODO: Add your control notification handler code here
	//	If 'full' then enable the Streamline output values
	if (m_print_COMBO.GetCurSel())
	{
		m_nSLi_EDIT.EnableWindow(1);
		m_nSLj_EDIT.EnableWindow(1);
	}
	else
	{
		m_nSLi_EDIT.EnableWindow(0);
		m_nSLj_EDIT.EnableWindow(0);
	}
	UpdateData(TRUE);
}

//	Reads a MOC_Grid *.inp file into the dialog.
BOOL CMOC_GridDlg::ReadInputFile(const CString& path)
{
	int a,b,c,d,e,f,g,h,i,j,k;
	ifstream ifile((LPCTSTR)path, ios::in);
	if(!ifile.is_open())
	{
		CString tmp = path;
		tmp += " does not exist";
		AfxMessageBox(tmp);
		return FALSE;
	}
	ifile  >> a >> b >>	c >> d >> e >> f >> g >> h >> i >> j >> k 
		>>	m_mDesign >> m_nC >> m_RWTD >> m_molWt_i
		>>	m_pres_i >>	m_temp_i >>	m_gamma_i >> m_RWTU >>	m_dTLimit >> m_pAmb
		>>	m_eps >> m_length >> m_rE >>m_xE >> m_nRRCAboveBD >> m_pExit
		>>	m_nSLi >> m_nSLj >>	m_vel >> m_thetaBi >> m_coneAngle;
	if (ifile.fail())
	{
		CString tmp = path;
		tmp += " is not a valid MOC_Grid input file.";
		AfxMessageBox(tmp);
		return FALSE;
	}
	//	The ideal Isp was added to the end of the file later; older files do not have it
	double isp;
	int printMode = 0;
	if (ifile >> isp) m_ispIdeal = isp;
	if (ifile >> printMode) { /* print option: 0 = Normal, 1 = Full */ }
	ifile.close();

	m_throat_CHECK.SetCheck(a);
	m_Axi_RADIO.SetCheck(d);
	m_2D_RADIO.SetCheck(e);
	m_perfect_RADIO.SetCheck(b);
	m_exitMach_RADIO.SetCheck(c);
	m_cone_RADIO.SetCheck(f);
	m_minLength_RADIO.SetCheck(g);
	m_endPoint_RADIO.SetCheck(h);
	m_eps_RADIO.SetCheck(i);
	m_length_RADIO.SetCheck(j);
	m_pExit_RADIO.SetCheck(k);
	UpdateScreen();
	if (printMode == 0 || printMode == 1)
	{
		m_print_COMBO.SetCurSel(printMode);
		OnSelchangePrintCOMBO();
	}

	m_DataFileName = path;
	int slash = path.ReverseFind('\\');
	if (slash > 0)
	{
		m_OutputDir = path.Left(slash);
		//	A folder that already holds results can be handed straight to the other tools
		m_STTRun_BUTTON.EnableWindow(GetFileAttributes(m_OutputDir + "\\STT2001.inp") != INVALID_FILE_ATTRIBUTES);
		m_3DRun_BUTTON.EnableWindow(GetFileAttributes(m_OutputDir + "\\summary.out") != INVALID_FILE_ATTRIBUTES);
	}
	return TRUE;
}

//	Writes the current dialog values to a MOC_Grid *.inp file. The caller must have called
//	UpdateData(TRUE) so that the values are current.
BOOL CMOC_GridDlg::WriteInputFile(const CString& path)
{
	ofstream ofile((LPCTSTR)path);
	if (!ofile.is_open())
	{
		CString tmp = "Could not write the input file:\n";
		tmp += path;
		AfxMessageBox(tmp);
		return FALSE;
	}
	ofile    << m_throat_CHECK.GetCheck()   << "\t" <<	m_perfect_RADIO.GetCheck()
	<< "\t"	 <<	m_exitMach_RADIO.GetCheck() << "\t" <<	m_Axi_RADIO.GetCheck()
	<< "\t"  << m_2D_RADIO.GetCheck()       << "\t"  << m_cone_RADIO.GetCheck()
	<< "\t"  << m_minLength_RADIO.GetCheck()<< "\t"  << m_endPoint_RADIO.GetCheck()
	<< "\t"  << m_eps_RADIO.GetCheck()      << "\t"  << m_length_RADIO.GetCheck()	
	<< "\t"  << m_pExit_RADIO.GetCheck() 
	<< "\n"  <<	m_mDesign << "\t"  << m_nC     << "\t"  << m_RWTD     << "\t" << m_molWt_i
	<< "\n"  <<	m_pres_i  << "\t"  << m_temp_i << "\t"  << m_gamma_i  << "\t" << m_RWTU     
	<< "\n"  <<	m_dTLimit << "\t"  << m_pAmb
	<< "\n"  <<	m_eps     << "\t"  << m_length << "\t"  << m_rE       << "\t" << m_xE       
	<< "\n"  << m_nRRCAboveBD << "\t"  << m_pExit
	<< "\n"  <<	m_nSLi    << "\t"  << m_nSLj   << "\t"  << m_vel      << "\t" << m_thetaBi 
	<< "\n"  << m_coneAngle
	<< "\n"  << m_ispIdeal << "\t" << m_print_COMBO.GetCurSel();
	ofile.close();
	return TRUE;
}

void CMOC_GridDlg::OnFileOpen() 
{
	static char BASED_CODE szFilter[] = "Input Files (*.inp)|*.inp|All Files (*.*)|*.*||";
 	CFileDialog myDialog(TRUE,"inp",NULL,OFN_HIDEREADONLY | OFN_FILEMUSTEXIST,szFilter);
	if ( IDOK == myDialog.DoModal() )
		ReadInputFile(myDialog.GetPathName());
	return;
}

void CMOC_GridDlg::OnFileSave() 
{
	if (m_DataFileName == "")
		OnFileSaveas();
	else
	{
		UpdateData(TRUE);
		WriteInputFile(m_DataFileName);
	}
	return;
}

void CMOC_GridDlg::OnFileSaveas() 
{
	UpdateData(TRUE);
	static char BASED_CODE szFilter[] = "Input Files (*.inp)|*.inp|All Files (*.*)|*.*||";
	CFileDialog myDialog(FALSE,"inp",m_DataFileName,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,szFilter);
	if ( IDOK == myDialog.DoModal() )
	{
		//	GetPathName() is the full path. (GetFileName() is only the name, which would
		//	save the file in the program's working folder instead of the chosen one.)
		CString path = myDialog.GetPathName();
		if (WriteInputFile(path)) m_DataFileName = path;
	}
	return;
}	
	

//	Writes STT2001.inp into the current (output) folder.  It names the files that
//	MOC_Grid just wrote and fills in the other values from this run, so the streamline
//	tracing tool is ready to run as soon as it opens. The default trimming set-up is one
//	throat circle of 72 streamlines around the full axisymmetric flowfield (R* = 1 in), which
//	rebuilds the MOC nozzle. Edit the trimming rows in STT2001 to design a 3-D nozzle.
BOOL CMOC_GridDlg::WriteSTTInputFile(const CString& prefix)
{
	//	The 2-D mass flow comes from the MOC summary file
	double mdot = 0.0;
	{
		ifstream sf("summary.out");
		string line;
		while (getline(sf, line))
		{
			if (line.compare(0, 13, "2-D Mass Flow") == 0)
			{
				size_t c = line.find(':');
				if (c != string::npos) mdot = atof(line.c_str() + c + 1);
				break;
			}
		}
	}
	if (mdot <= 0.0) mdot = 1.0;
	const double throatArea = 3.14159265358979;	//	pi * R*^2, with R* = 1 in

	//	Keep any earlier STT2001.inp (it may hold edited trimming rows)
	CString inp = m_OutputDir + "\\STT2001.inp";
	if (GetFileAttributes(inp) != INVALID_FILE_ATTRIBUTES)
		MoveFileEx(inp, m_OutputDir + "\\STT2001_previous.inp", MOVEFILE_REPLACE_EXISTING);

	ofstream ofile(inp);
	if (!ofile.is_open())
	{
		CString tmp = "Could not write:\n";
		tmp += inp;
		AfxMessageBox(tmp);
		return FALSE;
	}
	//	The layout must match STT2001's File>Open / File>Save
	ofile << (LPCTSTR)prefix << "\tMOC_SL.plt\tMOC_Grid.plt\tsummary.out\tfriction_table.txt\n"
		<< "1\t1\t1\n"				//	R of the flowfield: start, end, step
		<< "0\t0\t1\n"				//	X
		<< "0\t0\t1\n"				//	Y
		<< "0\t0\t1\n"				//	Z (not used)
		<< "1\t0\t0\t0\t0\n"		//	constraint radius Rc
		<< "0\t0\t0\t0\t0\n"		//	Yc
		<< "0\t0\t0\t0\t0\n"		//	Zc
		<< "0\t0\t0\t0\t0\n"		//	start angle
		<< "360\t0\t0\t0\t0\n"		//	end angle
		<< "72\t0\t0\t0\t0\n"		//	number of streamlines
		<< "0\t0\t0\t0\t0\n"		//	X start
		<< "0\t0\t0\t0\t0\n"		//	X end
		<< m_pAmb << "\t" << throatArea << "\t" << m_ispIdeal << "\t" << mdot << "\n"
		<< "0\t0\t0\t0\t0\t0\n"	//	symmetry definition
		<< "1\t0\t0\t0\t0\n"		//	row 1 is the throat
		<< "1\t0\t0\t0\t0\n"		//	row 1 is enabled
		<< "0\t0\t0\t0\t0\n"		//	surface: inner
		<< "0\t0\t1\t0\t1";			//	max length off, value, grid scale factor, X status, crop nozzle on
	ofile.close();
	return TRUE;
}


void CMOC_GridDlg::UpdateScreen( void)
{
	//	This function is used to update the screen based on the inputs from the 
	//	input file.
	UpdateData(FALSE);
	On2dRADIO();
	OnAxiRADIO();
	OnendPointRADIO();
	OnepsRADIO();
	OnexitMachRADIO();
	OnlengthRADIO();
	OnminLengthRADIO();
	OnperfectRADIO();
	OnConeRADIO();
	OnpExitRADIO();
	OnthroatCHECK();
	return;
}

void CMOC_GridDlg::OnSTTRunBUTTON() 
{
	//	Starts the streamline tracing tool with the STT2001.inp that was saved with the
	//	MOC_Grid results. The tool is started IN the output folder, so it loads that file
	//	and finds all of the other files there.
	CString inp = m_OutputDir + "\\STT2001.inp";
	if (m_OutputDir.IsEmpty() || GetFileAttributes(inp) == INVALID_FILE_ATTRIBUTES)
	{
		AfxMessageBox("Calculate the MOC grid with the Print Option set to Full first.");
		return;
	}
	CString exe = GetToolPath("STT2001\\STT2001.exe");
	if (GetFileAttributes(exe) == INVALID_FILE_ATTRIBUTES)
	{
		CString tmp = "Could not find the streamline tracing tool:\n";
		tmp += exe;
		AfxMessageBox(tmp);
		return;
	}

	CString cmdLine = "\"" + exe + "\" \"" + inp + "\"";
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	ZeroMemory(&pi, sizeof(pi));
	si.cb = sizeof(si);
	LPTSTR buf = cmdLine.GetBuffer();
	BOOL ok = CreateProcess(NULL, buf, NULL, NULL, FALSE, 0, NULL, m_OutputDir, &si, &pi);
	DWORD err = GetLastError();
	cmdLine.ReleaseBuffer();
	if (!ok)
	{
		CString tmp;
		tmp.Format("Could not start the streamline tracing tool (error %lu):\n%s", err, (LPCTSTR)exe);
		AfxMessageBox(tmp);
		return;
	}
	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);
	return;	
}

void CMOC_GridDlg::On3DRunBUTTON() 
{
	//	Starts the 3D MOC tool on the folder that holds the MOC_Grid results. 3D_MOC fills
	//	in its own inputs from the files in that folder (it writes the nozzle geometry file
	//	and 3D_MOC.inp there) and saves everything it calculates to the same folder.
	if (m_OutputDir.IsEmpty() || GetFileAttributes(m_OutputDir + "\\summary.out") == INVALID_FILE_ATTRIBUTES)
	{
		AfxMessageBox("Calculate the MOC grid first.");
		return;
	}
	CString exe = GetToolPath("3D_MOC\\3DCalc.exe");
	if (GetFileAttributes(exe) == INVALID_FILE_ATTRIBUTES)
	{
		CString tmp = "Could not find the 3D MOC tool:\n";
		tmp += exe;
		AfxMessageBox(tmp);
		return;
	}

	CString cmdLine = "\"" + exe + "\" \"" + m_OutputDir + "\"";
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	ZeroMemory(&pi, sizeof(pi));
	si.cb = sizeof(si);
	LPTSTR buf = cmdLine.GetBuffer();
	BOOL ok = CreateProcess(NULL, buf, NULL, NULL, FALSE, 0, NULL, m_OutputDir, &si, &pi);
	DWORD err = GetLastError();
	cmdLine.ReleaseBuffer();
	if (!ok)
	{
		CString tmp;
		tmp.Format("Could not start the 3D MOC tool (error %lu):\n%s", err, (LPCTSTR)exe);
		AfxMessageBox(tmp);
		return;
	}
	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);
	return;	
}
