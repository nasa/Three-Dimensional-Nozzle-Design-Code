// 3D_MOCDlg.cpp : implementation file
//

#include "stdafx.h"
#include <afxdlgs.h>
#include "3D_MOC.h"
#include "3D_MOCDlg.h"
#include "nr.h"
#include "UserMessages.h"
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iomanip>
#include <cmath>
#include <cstring>
#include <cstdlib>
#include <cctype>
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
	ON_BN_CLICKED(IDC_openFolder_BUTTON, OnOpenFolderBUTTON)
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
	
	ShowOutputFolder();

	//	If a folder (or a file inside it) was given on the command line, open that folder.
	//	This is how MOC_Grid starts this program: the dialog comes up filled in from the MOC_Grid
	//	results, and everything the program writes goes to that folder.
	CString cmd = AfxGetApp()->m_lpCmdLine;
	cmd.Trim();
	if (!cmd.IsEmpty())
	{
		if (cmd[0] == '"')
		{
			int q = cmd.Find('"', 1);
			cmd = (q > 0) ? cmd.Mid(1, q - 1) : cmd.Mid(1);
		}
		DWORD attr = GetFileAttributes(cmd);
		if (attr != INVALID_FILE_ATTRIBUTES)
		{
			CString folder = cmd;
			if (!(attr & FILE_ATTRIBUTE_DIRECTORY))
			{
				int slash = cmd.ReverseFind('\\');
				folder = (slash > 0) ? cmd.Left(slash) : CString(".");
			}
			OpenOutputFolder(folder, TRUE);
		}
	}
	
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
	//	Every file the calculation writes goes to the working folder. Make sure that is the
	//	MOC_Grid output folder (and not the folder of the program), and keep the inputs there.
	if (m_OutputDir.IsEmpty() && !ChooseOutputFolder()) return;
	Write3DInputFile(m_OutputDir + "\\3D_MOC.inp");
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

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//	Hand-off from MOC_Grid.  A MOC_Grid output folder holds everything this program
//	needs: summary.out (flow conditions and the throat plane) and Summary.plt or wall.out
//	(the nozzle wall).  Opening that folder fills in the dialog, writes the geometry file
//	(*.geo) and 3D_MOC.inp into the folder, and makes the folder the working folder, so that
//	every output file of this program is saved there too.
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static CString TrimTrailingSlash(CString p)
{
	p.Trim();
	while (p.GetLength() > 3 && (p.Right(1) == "\\" || p.Right(1) == "/"))
		p = p.Left(p.GetLength() - 1);
	return p;
}

static BOOL FileExists(const CString& p)
{
	return GetFileAttributes(p) != INVALID_FILE_ATTRIBUTES;
}

//	TRUE if file a was written later than file b (or if b does not exist)
static BOOL IsNewer(const CString& a, const CString& b)
{
	WIN32_FILE_ATTRIBUTE_DATA da, db;
	if (!GetFileAttributesEx(a, GetFileExInfoStandard, &da)) return FALSE;
	if (!GetFileAttributesEx(b, GetFileExInfoStandard, &db)) return TRUE;
	return CompareFileTime(&da.ftLastWriteTime, &db.ftLastWriteTime) > 0;
}

static string ReadWholeFile(const CString& path)
{
	string s;
	ifstream f((LPCTSTR)path, ios::in);
	if (f.is_open())
	{
		ostringstream ss;
		ss << f.rdbuf();
		s = ss.str();
	}
	return s;
}

//	Finds the number that follows a label in the text, e.g. "Total Pressure (psia):\t116.03"
static BOOL FindValue(const string& text, const char* label, double& v)
{
	size_t p = text.find(label);
	if (p == string::npos) return FALSE;
	v = atof(text.c_str() + p + strlen(label));
	return TRUE;
}

//	3D_MOC starts from a uniform plane at the throat (z0, radius R0). Choose its Mach number so
//	that the plane carries the same mass flow as the MOC_Grid throat, whose 2-D mass flow is a
//	little lower than the 1-D (sonic) value: A/A* = (1-D mass flow)/(2-D mass flow) = 1/CD.
static double MassConservingMach(const string& text, double g)
{
	double m1, m2;
	if (!FindValue(text, "1-D Mass Flow (lbm/s):", m1) || !FindValue(text, "2-D Mass Flow (lbm/s):", m2)
		|| m1 <= 0.0 || m2 <= 0.0) return 1.1;
	double ratio = m1 / m2;
	if (ratio <= 1.0000001) return 1.01;
	double lo = 1.0, hi = 3.0;
	for (int k = 0; k < 80; k++)
	{
		double mid = 0.5 * (lo + hi);
		double ar = (1.0 / mid) * pow((2.0 / (g + 1.0)) * (1.0 + 0.5 * (g - 1.0) * mid * mid),
			(g + 1.0) / (2.0 * (g - 1.0)));
		if (ar < ratio) lo = mid; else hi = mid;
	}
	return 0.5 * (lo + hi);
}

//	Reads the nozzle wall (X/R*, R/R*) from Summary.plt, or from wall.out if that is missing.
static BOOL ReadWallContour(const CString& folder, vector<double>& x, vector<double>& r)
{
	string line;
	x.clear();
	r.clear();
	{
		ifstream plt((LPCTSTR)(folder + "\\Summary.plt"), ios::in);
		BOOL inWall = FALSE;
		while (plt.is_open() && getline(plt, line))
		{
			if (line.compare(0, 4, "zone") == 0 || line.compare(0, 4, "ZONE") == 0 || line.compare(0, 4, "Zone") == 0)
			{
				if (inWall) break;
				inWall = (line.find("Wall contour") != string::npos);
				continue;
			}
			if (!inWall) continue;
			istringstream ss(line);
			double a, b;
			if (ss >> a >> b) { x.push_back(a); r.push_back(b); }
		}
	}
	if (x.size() < 10)
	{
		x.clear();
		r.clear();
		ifstream wf((LPCTSTR)(folder + "\\wall.out"), ios::in);
		while (wf.is_open() && getline(wf, line))
		{
			istringstream ss(line);
			double i, j, a, b;
			if (ss >> i >> j >> a >> b) { x.push_back(a); r.push_back(b); }
		}
	}
	//	Drop repeated points, which would make zero-length axial stations
	vector<double> xs, rs;
	for (size_t k = 0; k < x.size(); k++)
	{
		if (!xs.empty() && fabs(x[k] - xs.back()) < 1e-9 && fabs(r[k] - rs.back()) < 1e-9) continue;
		xs.push_back(x[k]);
		rs.push_back(r[k]);
	}
	x = xs;
	r = rs;
	return x.size() >= 10;
}

//	Writes the geometry file in the layout 3D_MOC reads: the number of axial stations, a header
//	line, then one "Z R0 X0 Y0" row per station (a circle of radius R0 centred on X0,Y0).
static BOOL WriteGeoFile(const CString& path, const vector<double>& x, const vector<double>& r)
{
	ofstream o((LPCTSTR)path, ios::out);
	if (!o.is_open()) return FALSE;
	o << setprecision(8);
	o << x.size() << "\t\t\t\n" << "Z\tR0\tX0\tY0\n";
	for (size_t k = 0; k < x.size(); k++)
		o << x[k] << "\t" << r[k] << "\t0\t0\n";
	return o.good();
}

//	Fills the dialog from the MOC_Grid files in the folder and writes the geometry file there.
BOOL CMy3D_MOCDlg::FillFromMocGrid(const CString& folder, CString& note)
{
	string text = ReadWholeFile(folder + "\\summary.out");
	double pTot, tTot, g, mw;
	if (!FindValue(text, "Total Pressure (psia):", pTot) || !FindValue(text, "Total Temperature (R):", tTot)
		|| !FindValue(text, "Gamma:", g) || !FindValue(text, "Molecular Weight:", mw)
		|| pTot <= 0.0 || tTot <= 0.0 || g <= 1.0 || mw <= 0.0)
	{
		note = "summary.out does not contain the flow conditions (total pressure, total temperature, gamma, molecular weight).";
		return FALSE;
	}
	if (text.find("Axisymmetric Nozzle") == string::npos)
	{
		note = "This MOC_Grid run is not axisymmetric. 3D_MOC starts from a circular throat, so only axisymmetric MOC_Grid results can be used.";
		return FALSE;
	}
	vector<double> x, r;
	if (!ReadWallContour(folder, x, r))
	{
		note = "Could not read the nozzle wall from Summary.plt or wall.out.";
		return FALSE;
	}

	//	Initial plane: uniform flow carrying the MOC throat mass flow, from the total conditions
	double mach = MassConservingMach(text, g);
	if (mach < 1.001) mach = 1.001;
	double a = 1.0 + 0.5 * (g - 1.0) * mach * mach;
	m_mach0 = mach;
	m_gamma0 = g;
	m_molWt0 = mw;
	m_p0 = pTot * pow(a, -g / (g - 1.0));
	m_t0 = tTot / a;
	m_theta0 = 0.0;
	m_psi0 = 0.0;
	m_z0 = x[0];

	//	The geometry file is named after the folder, like the STT2001 files
	CString leaf = folder;
	leaf.TrimRight('\\');
	leaf = leaf.Mid(leaf.ReverseFind('\\') + 1);
	if (leaf.IsEmpty() || leaf.Find(':') >= 0) leaf = "nozzle";
	for (int i = 0; i < leaf.GetLength(); i++)
		if (!isalnum((unsigned char)leaf[i]) && leaf[i] != '-') leaf.SetAt(i, '_');
	m_geomFile = leaf + ".geo";
	CString geo = folder + "\\" + m_geomFile;

	CString geoNote;
	if (!FileExists(geo) || IsNewer(folder + "\\summary.out", geo))
	{
		//	Keep an earlier geometry file: it may have been edited by hand
		if (FileExists(geo))
			MoveFileEx(geo, folder + "\\" + leaf + "_previous.geo", MOVEFILE_REPLACE_EXISTING);
		if (!WriteGeoFile(geo, x, r))
		{
			note = "Could not write the geometry file:\n" + geo;
			return FALSE;
		}
		geoNote = "Wrote the nozzle geometry to " + m_geomFile + " (throat radius 1 in, so X/R* and R/R* are in inches).";
	}
	else geoNote = "Kept the existing " + m_geomFile + ".";

	CString nums;
	nums.Format("Initial plane: Mach %.4g, static pressure %.5g psia, static temperature %.5g R.", m_mach0, m_p0, m_t0);
	note = "Filled in from the MOC_Grid results.\n" + geoNote + "\n" + nums;
	return TRUE;
}

//	3D_MOC.inp: one "name<TAB>value" line per setting
BOOL CMy3D_MOCDlg::Write3DInputFile(const CString& path)
{
	ofstream o((LPCTSTR)path, ios::out);
	if (!o.is_open()) return FALSE;
	o << setprecision(10);
	o << "geometry_file\t" << (LPCTSTR)m_geomFile << "\n"
		<< "pressure_psia\t" << m_p0 << "\n"
		<< "temperature_R\t" << m_t0 << "\n"
		<< "mach\t" << m_mach0 << "\n"
		<< "mol_wt\t" << m_molWt0 << "\n"
		<< "gamma\t" << m_gamma0 << "\n"
		<< "theta_deg\t" << m_theta0 << "\n"
		<< "psi_deg\t" << m_psi0 << "\n"
		<< "axial_location_in\t" << m_z0 << "\n"
		<< "radial_divisions\t" << m_nDiv << "\n"
		<< "print_every_x\t" << m_xOutputIncrement << "\n"
		<< "print_every_y\t" << m_yOutputIncrement << "\n"
		<< "print_every_z\t" << m_zOutputIncrement << "\n"
		<< "print_every_step\t" << m_stepStep << "\n"
		<< "surface_fit\t" << (LPCTSTR)m_SurfaceFit << "\n";
	return o.good();
}

BOOL CMy3D_MOCDlg::Load3DInputFile(const CString& path)
{
	ifstream f((LPCTSTR)path, ios::in);
	if (!f.is_open()) return FALSE;
	string line;
	int found = 0;
	while (getline(f, line))
	{
		size_t t = line.find('\t');
		if (t == string::npos) continue;
		string key = line.substr(0, t);
		string val = line.substr(t + 1);
		while (!val.empty() && (val[val.size() - 1] == '\r' || val[val.size() - 1] == ' ')) val.erase(val.size() - 1);
		double d = atof(val.c_str());
		int n = atoi(val.c_str());
		if (key == "geometry_file") { m_geomFile = val.c_str(); found++; }
		else if (key == "pressure_psia") { m_p0 = d; found++; }
		else if (key == "temperature_R") { m_t0 = d; found++; }
		else if (key == "mach") { m_mach0 = d; found++; }
		else if (key == "mol_wt") { m_molWt0 = d; found++; }
		else if (key == "gamma") { m_gamma0 = d; found++; }
		else if (key == "theta_deg") { m_theta0 = d; found++; }
		else if (key == "psi_deg") { m_psi0 = d; found++; }
		else if (key == "axial_location_in") { m_z0 = d; found++; }
		else if (key == "radial_divisions") { m_nDiv = n; found++; }
		else if (key == "print_every_x") { m_xOutputIncrement = n; found++; }
		else if (key == "print_every_y") { m_yOutputIncrement = n; found++; }
		else if (key == "print_every_z") { m_zOutputIncrement = n; found++; }
		else if (key == "print_every_step") { m_stepStep = n; found++; }
		else if (key == "surface_fit") { m_SurfaceFit = val.c_str(); found++; }
	}
	return found >= 5;
}

void CMy3D_MOCDlg::ShowOutputFolder()
{
	CString shown = "Output folder: (not chosen yet)";
	CString title = "3D Method of Characteristics Tool";
	if (!m_OutputDir.IsEmpty())
	{
		shown = "Output folder: " + m_OutputDir;
		title += " - " + m_OutputDir;
	}
	SetDlgItemText(IDC_outDir_STATIC, shown);
	SetWindowText(title);
}

//	Makes the folder the working folder (every output file of this program is written to the
//	working folder by bare name) and fills in the dialog from what the folder holds.
BOOL CMy3D_MOCDlg::OpenOutputFolder(const CString& folderIn, BOOL quiet)
{
	CString folder = TrimTrailingSlash(folderIn);
	DWORD attr = GetFileAttributes(folder);
	if (attr == INVALID_FILE_ATTRIBUTES || !(attr & FILE_ATTRIBUTE_DIRECTORY) || !SetCurrentDirectory(folder))
	{
		AfxMessageBox("That folder cannot be used:\n" + folder);
		return FALSE;
	}
	m_OutputDir = folder;
	ShowOutputFolder();

	CString inp = folder + "\\3D_MOC.inp";
	CString summary = folder + "\\summary.out";
	CString note;
	BOOL loaded = FALSE;

	//	Settings saved by an earlier run win, unless MOC_Grid has calculated again since then
	if (FileExists(inp) && (!FileExists(summary) || IsNewer(inp, summary)))
	{
		if (Load3DInputFile(inp))
		{
			loaded = TRUE;
			note = "Loaded the settings saved in 3D_MOC.inp.";
			if (!FileExists(folder + "\\" + m_geomFile))
				note += "\nWarning: the geometry file " + m_geomFile + " is not in this folder.";
		}
	}
	if (!loaded && FileExists(summary))
	{
		CString why;
		if (FillFromMocGrid(folder, why))
		{
			loaded = TRUE;
			note = why;
			Write3DInputFile(inp);
		}
		else note = why;
	}
	if (!loaded && note.IsEmpty())
		note = "No MOC_Grid results (summary.out) were found in this folder.\nEnter the geometry file and flow conditions yourself; the output files will still be saved here.";

	UpdateData(FALSE);
	if (!loaded || !quiet) AfxMessageBox(note, loaded ? MB_ICONINFORMATION : MB_ICONWARNING);
	return loaded;
}

//	Used when Calculate is pressed before any folder was opened
BOOL CMy3D_MOCDlg::ChooseOutputFolder()
{
	CFolderPickerDialog dlg(NULL, 0, this, 0);
	dlg.m_ofn.lpstrTitle = _T("Choose the folder where all of the output files will be saved (the MOC_Grid output folder)");
	if (dlg.DoModal() != IDOK) return FALSE;
	CString folder = TrimTrailingSlash(dlg.GetPathName());
	if (!SetCurrentDirectory(folder))
	{
		AfxMessageBox("That folder cannot be used:\n" + folder);
		return FALSE;
	}
	m_OutputDir = folder;
	ShowOutputFolder();
	return TRUE;
}

void CMy3D_MOCDlg::OnOpenFolderBUTTON()
{
	UpdateData(TRUE);
	CFolderPickerDialog dlg(m_OutputDir.IsEmpty() ? (LPCTSTR)NULL : (LPCTSTR)m_OutputDir, 0, this, 0);
	dlg.m_ofn.lpstrTitle = _T("Choose the MOC_Grid output folder");
	if (dlg.DoModal() != IDOK) return;
	OpenOutputFolder(dlg.GetPathName(), FALSE);
}
