
// Mind1Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "Mind1.h"
#include "Mind1Dlg.h"
#include "afxdialogex.h"

// Project specific
#include "Mind.h"
#include "StringUtils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMind1Dlg dialog

CMind1Dlg::CMind1Dlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMind1Dlg::IDD, pParent)
	, theInfo(_T(""))
	, lastThought(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMind1Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_Info, theInfo);
	DDX_Text(pDX, IDC_Output, output1);
	DDX_Text(pDX, IDC_Output2, output2);
	// DDX_Text(pDX, IDC_Thought, lastThought);
}

BEGIN_MESSAGE_MAP(CMind1Dlg, CDialog)
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CMind1Dlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CMind1Dlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_Save, &CMind1Dlg::OnBnClickedSave)
	ON_BN_CLICKED(IDC_Load, &CMind1Dlg::OnBnClickedLoad)
END_MESSAGE_MAP()


// CMind1Dlg message handlers

BOOL CMind1Dlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	ThinkTimerID = 101;
	ThinkDelay = 1000;
	Refresh();
	SetTimer(ThinkTimerID, ThinkDelay, NULL);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMind1Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

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

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMind1Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CMind1Dlg::OnBnClickedOk()
{
	UpdateData(TRUE);

	theMind.BuildAssociation(theInfo, 0);
	theInfo.Empty();
	Refresh();
}


void CMind1Dlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	CDialog::OnCancel();
}

void CMind1Dlg::OnTimer(UINT_PTR TimerID)
{
	KillTimer(ThinkTimerID);

	bool refresh = theMind.Think(lastThought);
	SetDlgItemText(IDC_Thought, lastThought);

	if (refresh)
	{
		Refresh();
	}

	SetTimer(ThinkTimerID, ThinkDelay, NULL);
}

void CMind1Dlg::OnBnClickedSave()
{
	theMind.Save();
}

void CMind1Dlg::Refresh()
{
	theMind.DumpConcepts(output1, NULL);
	theMind.DumpAssociations(output2, NULL);
	output1.Replace(_T("\n"), _T("\r\n"));
	output2.Replace(_T("\n"), _T("\r\n"));
	UpdateData(FALSE);
}

void CMind1Dlg::OnBnClickedLoad()
{
	theMind.Load();
	Refresh();
}
