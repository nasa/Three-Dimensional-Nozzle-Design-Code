// MOC_GridDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MOC_Grid.h"
#include "MOC_GridDlg.h"
#include "DlgProxy.h"
#include "MOC_GridCalc_BDE.h"
// Tharen Rice May 2020 Change: New call for fstream
//#include "fstream.h"
#include <fstream>
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

void CMOC_GridDlg::OnCalcMOCBUTTON() 
{
	int flag = 0;
	m_STTRun_BUTTON.EnableWindow(0);
	//	This is the function that starts the whole calculation process
	MOC_GridCalc *moc = new MOC_GridCalc;
	UpdateData(TRUE);
	m_CalcMOC_BUTTON.EnableWindow(0);
	if(moc->SetInitialProperties( m_pres_i, m_temp_i,  m_molWt_i, 
		 m_gamma_i,  m_pAmb,  m_nC,  m_RWTU,  m_RWTD,  m_dTLimit, m_nRRCAboveBD, 
		 m_nSLi, m_nSLj, m_vel, m_throat_CHECK.GetCheck(), m_ispIdeal));
		
	else 
	{
		AfxMessageBox("Could not set properties");
		exit(1);
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
	flag = moc->CreateMOCGrid();
			
	m_CalcMOC_BUTTON.EnableWindow(1);
	
	if (flag && m_print_COMBO.GetCurSel()) m_STTRun_BUTTON.EnableWindow(1);
	delete moc;
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

void CMOC_GridDlg::OnFileOpen() 
{
	int a,b,c,d,e,f,g,h,i,j,k;
	static char BASED_CODE szFilter[] = "Input Files (*.inp)|*.inp|All Files (*.*)|*.*||";
 	CFileDialog myDialog(TRUE,"inp",m_DataFileName,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,szFilter);
	if ( IDOK == myDialog.DoModal() )
	{
		m_DataFileName = myDialog.GetFileName();
		//Tharen Rice MAY 2020 edit: Changed ifstream call
		//ifstream ifile(m_DataFileName,ios::nocreate | ios::in);
		ifstream ifile(m_DataFileName, ios::in);
		if(!ifile.is_open())
		{
			CString tmp;
			tmp = m_DataFileName;
			tmp += " does not exist";
			AfxMessageBox(tmp);
			exit(1);
		}
		ifile  >> a >> b >>	c >> d >> e >> f >> g >> h >> i >> j >> k 
			>>	m_mDesign >> m_nC >> m_RWTD >> m_molWt_i
			>>	m_pres_i >>	m_temp_i >>	m_gamma_i >> m_RWTU >>	m_dTLimit >> m_pAmb
			>>	m_eps >> m_length >> m_rE >>m_xE >> m_nRRCAboveBD >> m_pExit
			>>	m_nSLi >> m_nSLj >>	m_vel >> m_thetaBi >> m_coneAngle;;
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
	}
	return;
}



void CMOC_GridDlg::OnFileSave() 
{
	UpdateData(TRUE);
	static char BASED_CODE szFilter[] = "Input Files (*.inp)|*.inp|All Files (*.*)|*.*||";
	CFileDialog myDialog(FALSE,"inp",m_DataFileName,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,szFilter);
	if (m_DataFileName == "")
		OnFileSaveas();
	else
	{
		ofstream ofile(m_DataFileName);
		m_DataFileName = myDialog.GetFileName();
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
		<< "\n"  << m_coneAngle ;
		ofile.close();	
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
		m_DataFileName = myDialog.GetFileName();
		ofstream ofile(m_DataFileName);
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
		<< "\n"  << m_coneAngle ;
		ofile.close();
	}
	return;
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
	fstream ofile;
	ofile.open("$$.bat", ios::out);
	char cDir[80];
	_getdcwd( 'C' - 'A' + 1, cDir, 80 );

	// Tharen Rice May 2020: The user will need to edit the location of the
	// Streamline tracing tool executable
	//CString path = "c:\\docume~1\\DALESSM1\\Desktop\\STT";
	// Tharen Rice: MAY 2020.  The path to the streamline tracing tool is hardcoded here.
	// This must be changed to work on your system
		CString path = "..\\STT2001";
	//	This function executes the Streamline Tracing tool
	CString command;
	command = "cd ";
	command += cDir;
	ofile << command << endl;

	command = "copy summary.out ";
	command += path;
	ofile << command << endl;	
	command = "copy moc_grid.plt ";
	command += path;
	ofile << command << endl;	
	command = "copy moc_sl.plt ";
	command += path;
	ofile << command << endl;	

	command = "cd ";
	command += path;
	ofile << command << endl << "STT2001.exe" << endl;
//	ofile << command << endl << "DEBUG\\STT2001.exe" << endl;

	ofile.close();
	system("$$.bat");
	return;	
}
