
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
	//DDX_Text(pDX, IDC_Info, theInfo);
	//DDX_Text(pDX, IDC_Output, output1);
	//DDX_Text(pDX, IDC_Output2, output2);
	// DDX_Text(pDX, IDC_Thought, lastThought);
	DDX_Control(pDX, IDC_STATIC_P, anImage);
	DDX_Control(pDX, IDC_NetPic, netPic);
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

	MakeCharBM(_T("F"));

	ThinkTimerID = 101;
	ThinkDelay = 1000;
	theMind.Load(FN_CONCEPTS);
	Refresh();
	SetTimer(ThinkTimerID, ThinkDelay, NULL);
	DrawNet();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMind1Dlg::TestBitmap()
{
}

int CMind1Dlg::Train(TCHAR Ch)
{
	if (char_rec.NumLayers() == 0)
	{
		char_rec.mind = &theMind;
		char_rec.root = NULL;
		int inputs = 15 * 18;
		char_rec.NumLayers(3);
		char_rec.DefineLayer(0, inputs);
		char_rec.DefineLayer(1, (inputs * 10) / 10);
		char_rec.DefineLayer(2, 1);
		char_rec.BuildConnections();
	}
	CBitmap bitMap;
	bitMap.Attach(anImage.GetBitmap());

	BITMAP bm;
	bitMap.GetBitmap(&bm);

	DWORD sz = bitMap.GetBitmapBits(0, NULL);
	int numBits = bm.bmHeight*bm.bmWidthBytes;
	bm.bmBits = malloc(numBits);
	DWORD num = bitMap.GetBitmapBits(numBits, bm.bmBits);

	int nnum = 0;
	CString tmp, line;
	BYTE *theBits = (BYTE *)bm.bmBits;
	BYTE *pBits = theBits;
	for (int rowNum = 0; rowNum < bm.bmHeight; rowNum++)
	{
		line.Empty();
		int offset = rowNum*bm.bmWidthBytes;
		pBits = &theBits[offset];
		BYTE data = *pBits;
		int bitNum = 0;
		while (bitNum < bm.bmWidth)
		{
			BYTE bit = (data & 0x80);
			data = data << 1;

			double val = (bit) ? 0 : 1;
			char_rec.SetInput(nnum++, val);

			char ch = (bit) ? '_' : '*';
			line.AppendChar(ch);
			if ((++bitNum) % 8 == 0)
			{
				data = *(++pBits);
				tmp.Append(line);
				line.Empty();
			}
		}
		tmp.Append(line);
		tmp.Append(_T("\n"));
	}
	tmp.Replace(_T("\n"), _T("\r\n"));
	GetDlgItem(IDC_Output)->SetWindowText(tmp);

	//ATLTRACE(_T("\n%s"), tmp);
	free(bm.bmBits);

	anImage.SetBitmap((HBITMAP)bitMap.Detach());
	anImage.Invalidate();

	char_rec.Go();

	return 0;
}

void CMind1Dlg::DrawNet()
{
	CBitmap bitMap;
	bitMap.CreateBitmap(200, 200, 1, 24, NULL);
	bitMap.DeleteTempMap();
	//bitMap.Attach(anImage.GetBitmap());

	BITMAP bm;
	bitMap.GetBitmap(&bm);

	RECT r;
	r.top = r.left = 0;
	r.right = bm.bmWidth;
	r.bottom = bm.bmHeight;

	CBrush br(RGB(200,0,0));
	CPen myPen(PS_SOLID, 3, RGB(0, 255, 0)), *oldPen;
	CDC memDC, *curDC = GetDC();
	memDC.CreateCompatibleDC(curDC);
	HGDIOBJ old = memDC.SelectObject(&bitMap);
	memDC.SetBkMode(OPAQUE);
	memDC.FillSolidRect(&r, RGB(255, 255, 255));
	memDC.TextOutW(0, 0, _T("Myyy"), 3);
	oldPen = (CPen *)memDC.SelectObject(&myPen);
	memDC.MoveTo(5, 5);
	memDC.LineTo(22, 99);
	memDC.SelectObject(oldPen);
	memDC.SelectObject(old);
	ReleaseDC(curDC);
	memDC.DeleteDC();

	netPic.SetBitmap((HBITMAP)bitMap.Detach());
	netPic.Invalidate();
}

void CMind1Dlg::MakeCharBM(LPCTSTR Char)
{
	CBitmap bitMap;
	bitMap.CreateBitmap(15, 18, 1, 1, NULL);
	bitMap.DeleteTempMap();
	//bitMap.Attach(anImage.GetBitmap());

	BITMAP bm;
	bitMap.GetBitmap(&bm);

	RECT r;
	r.top = r.left = 0;
	r.right = bm.bmWidth;
	r.bottom = bm.bmHeight;

	CDC memDC, *curDC = GetDC();
	memDC.CreateCompatibleDC(curDC);
	HGDIOBJ old = memDC.SelectObject(&bitMap);
	memDC.SetBkMode(OPAQUE);
	memDC.FillSolidRect(&r, RGB(255, 255, 255));
	memDC.TextOutW(0, 0, Char, 1);
	memDC.SelectObject(old);
	ReleaseDC(curDC);
	memDC.DeleteDC();

	anImage.SetBitmap((HBITMAP)bitMap.Detach());
	anImage.Invalidate();
	Train(*Char);
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
		//CDialog::OnPaint();
		//CPaintDC dc(this); // device context for painting
		CClientDC dcLine(this);
		CRect client_r, r;
		GetClientRect(&client_r);
		r.left = 550; r.right = client_r.right - 10;
		r.top = 50; r.bottom = client_r.bottom - 10;
		CPen myPen(PS_SOLID, 1, RGB(0, 0, 0)), *oldPen;
		//dc.FillSolidRect(&r, RGB(255, 200, 100));
		oldPen = (CPen *)dcLine.SelectObject(&myPen);
		//dcLine.TextOutW(770, 100, _T("Myyy"), 4);
		dcLine.MoveTo(r.left, r.top);
		dcLine.LineTo(r.right, r.top);
		dcLine.LineTo(r.right, r.bottom);
		dcLine.LineTo(r.left, r.bottom);
		dcLine.LineTo(r.left, r.top);
		dcLine.SelectObject(oldPen);
		myPen.Detach();
		myPen.CreatePen(PS_SOLID, 3, RGB(200, 0, 0));
		oldPen = (CPen *)dcLine.SelectObject(&myPen);
		dcLine.MoveTo(r.left, r.top);
		dcLine.LineTo(r.right, r.bottom);
		dcLine.SelectObject(oldPen);
		//Invalidate();
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
	GetDlgItem(IDC_Info)->GetWindowText(theInfo);

	MakeCharBM(theInfo);

	//theMind.text_system.ReceiveInput(theInfo);

	theInfo.Empty();
	GetDlgItem(IDC_Info)->SetWindowText(theInfo);
	Refresh();
}


void CMind1Dlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	KillTimer(ThinkTimerID);

	//CImage img;
	//img.loa
	CDialog::OnCancel();
}

void CMind1Dlg::OnTimer(UINT_PTR TimerID)
{
	static CString last_thought;
	KillTimer(ThinkTimerID);

	bool refresh = theMind.Think(lastThought);

	SetDlgItemText(IDC_Thought, lastThought);
	last_thought = lastThought;

	//if (!theMind.text_system.last_output.IsEmpty())
	//{
	//	GetDlgItem(IDC_Output)->SetWindowText(theMind.text_system.last_output);
	//	theMind.text_system.last_output.Empty();
	//}

	if (refresh)
	{
		Refresh();
	}

	//SetTimer(ThinkTimerID, ThinkDelay, NULL);
	SetTimer(ThinkTimerID, 50, NULL);
}

void CMind1Dlg::OnBnClickedSave()
{
	theMind.Save();
}

void CMind1Dlg::Refresh()
{
	//theMind.DumpConcepts(output1, NULL);
	//theMind.DumpAssociations(output2, NULL);
	//output1.Replace(_T("\n"), _T("\r\n"));
	//output2.Replace(_T("\n"), _T("\r\n"));

	//GetDlgItem(IDC_Output)->SetWindowText(output1);
	//GetDlgItem(IDC_Output2)->SetWindowText(output2);
}

void CMind1Dlg::OnBnClickedLoad()
{
	theMind.Load(FN_CONCEPTS);
	Refresh();
}
