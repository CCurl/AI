
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

int GetDesired(int x, int y)
{
	return (x^y);
}

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
	DDX_Control(pDX, IDC_LIST1, theList);
}

BEGIN_MESSAGE_MAP(CMind1Dlg, CDialog)
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CMind1Dlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CMind1Dlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_Save, &CMind1Dlg::OnBnClickedSave)
	ON_BN_CLICKED(IDC_Load, &CMind1Dlg::OnBnClickedLoad)
	ON_BN_CLICKED(IDC_Step, &CMind1Dlg::OnClick_Step)
	ON_BN_CLICKED(IDC_ErrAdj, &CMind1Dlg::OnClick_ErrAdj)
	ON_BN_CLICKED(IDC_Reset, &CMind1Dlg::OnBnClickedReset)
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
	//SetTimer(ThinkTimerID, ThinkDelay, NULL);
	SetDlgItemText(IDC_LearningRate, _T("20"));


	if (nn_binary.NumLayers() == 0)
	{
		nn_binary.mind = &theMind;
		nn_binary.root = NULL;
		nn_binary.NumLayers(3);
		nn_binary.DefineLayer(0, 2);
		nn_binary.DefineLayer(1, 5);
		nn_binary.DefineLayer(2, 1);
		nn_binary.BuildConnections();
		CNeuron *n = nn_binary.NeuronAt(2, 0);
		//n->activation_function = SIGMOID_BOOL;
		n->activation_function = SIGMOID;
		DrawNet(1);
		OnBnClickedReset();
	}


	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMind1Dlg::Test1()
{
	CString thought, strIn1, strIn2;

	GetDlgItemText(IDC_IN1, strIn1);
	GetDlgItemText(IDC_IN2, strIn2);
	int in1 = _wtol(strIn1);
	int in2 = _wtol(strIn2);
	int tot = in1 * 2 + in2;

	tot = (tot + 1) % 4;
	in2 = tot % 2;
	in1 = tot >> 1;

	if (in1 + in2 == 0)
	{
		if ((theMind.epoch > 2500) || (abs(max_err) < 0.001))
		{
			all_done = true;
		}
		theMind.epoch++;
		max_err = 0;
	}

	nn_binary.SetInput(0, in1);
	nn_binary.SetInput(1, in2);

	strIn1.Format(_T("%d"), in1);
	strIn2.Format(_T("%d"), in2);
	SetDlgItemText(IDC_IN1, strIn1);
	SetDlgItemText(IDC_IN2, strIn2);

	CNeuron tmp;

	double actual = nn_binary.Go();
	double desired = tmp.Sigmoid(GetDesired(in1, in2));
	double diff = desired - actual;
	max_err = max(abs(diff), max_err);

	//for (int i = 0; i < 2; i++)
	//{
	//	for (int j = 0; j < 2; j++)
	//	{
	//		nn_binary.SetInput(0, i);
	//		nn_binary.SetInput(1, j);
	//		double actual = nn_binary.Go();
	//		double expected = double(i^j);
	//		double diff = expected - actual;
	//		cumulative_err += (diff);
	//		nn_binary.AdjustWeights(diff);
	//		thought.AppendFormat(_T("(%d,%d,%.0f,%.4f,%.4f) "), i, j, expected, actual, diff);
	//	}
	//}

	thought.AppendFormat(_T("epoch %d"), theMind.epoch);
	SetDlgItemText(IDC_Thought, thought);

	strIn2.Format(_T("%.3f"), desired);
	SetDlgItemText(IDC_EXPECTED, strIn2);

	strIn2.Format(_T("%6.3f"), diff);
	SetDlgItemText(IDC_Err, strIn2);

	if (tot == 0)
		DrawNet(2);
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

	//char_rec.Go();

	return 0;
}

void CMind1Dlg::DrawCircle(CClientDC& dc, int x, int y, int radius, CPen& pen)
{
	CPen *old = dc.SelectObject(&pen);
	dc.MoveTo(x+radius, y);
	dc.AngleArc(x, y, radius, 0, 360);
	dc.SelectObject(old);
}

void CMind1Dlg::DrawLine(CClientDC& dc, int FromX, int FromY, int ToX, int ToY, CPen& pen)
{
	CPen *oldPen = dc.SelectObject(&pen);
	if (FromX > 0)
	{
		dc.MoveTo(FromX, FromY);
	}
	dc.LineTo(ToX, ToY);
	dc.SelectObject(oldPen);
}

void CMind1Dlg::DrawRectangle(CClientDC& dc, int top, int left, int right, int bottom, CPen& pen)
{
	CPen *oldPen = dc.SelectObject(&pen);
	dc.MoveTo(left, top);
	dc.LineTo(right, top);
	dc.LineTo(right, bottom);
	dc.LineTo(left, bottom);
	dc.LineTo(left, top);
	dc.SelectObject(oldPen);
}

void CMind1Dlg::WriteText(CClientDC& dc, int x, int y, LPCTSTR text, COLORREF color)
{
	dc.SetTextColor(color);
	dc.TextOutW(x, y, text, wcslen(text));
}

void CMind1Dlg::DrawNet(int Which)
{
	// return;
	CClientDC dc(this);
	CRect client_r, r;
	GetClientRect(&client_r);

	CPen redPen(PS_SOLID, 2, RGB(200, 0, 0));
	CPen grnPen(PS_SOLID, 20, RGB(0, 150, 0));
	CPen bluPen(PS_SOLID, 2, RGB(0, 0, 150));
	CPen blkPen(PS_SOLID, 1, RGB(0, 0, 0));

	r.left = 600; r.right = client_r.right - 20;
	r.top = 20; r.bottom = client_r.bottom - 100;

	int radius = 20, x = r.left + 30, y = r.top + 30;
	int xMid = (r.left + r.right) / 2;
	int yMid = (r.top + r.bottom) / 2;
	int xStep = r.Width() / (nn_binary.NumLayers() + 1) + 80;

	CString txt;
	x = r.left + xStep - 160;
	if (Which == 1)
	{
		DrawRectangle(dc, r.top, r.left, r.right, r.bottom, blkPen);

		// Nodes
		for (int ln = 0; ln < nn_binary.NumLayers(); ln++)
		{
			CNNetLayer *l = nn_binary.layers[ln];
			int yStep = r.Height() / (l->num_neurons + 1) + 10;
			y = r.top + yStep - 30;
			for (int nn = 0; nn < l->num_neurons; nn++)
			{
				DrawCircle(dc, x, y, radius, redPen);
				y += yStep;
			}
			x += xStep;
		}
	}

	if (Which == 2)
	{
		// Connections
		for (int ln = 0; ln < nn_binary.NumLayers(); ln++)
		{
			CNNetLayer *l = nn_binary.layers[ln];
			int yStep = r.Height() / (l->num_neurons + 1) + 10;
			y = r.top + yStep - 30;
			for (int nn = 0; nn < l->num_neurons; nn++)
			{
				//DrawCircle(dc, x, y, radius, redPen);
				CNeuron *n = l->NeuronAt(nn);
				txt.Format(_T("%.3f, %.3f "), n->input, n->output);
				WriteText(dc, x - radius, y + radius + 5, txt, RGB(0, 0, 0));
				txt.Format(_T("%.6f "), n->error_term);
				WriteText(dc, x - radius, y + radius + 30, txt, RGB(0, 0, 0));
				POSITION pos = n->boutons.GetHeadPosition();
				int dy = y+35, dx = x + radius + 70;
				while (pos)
				{
					CDendrite *d = n->boutons.GetNext(pos);
					txt.Format(_T("%.3f "), d->weight);
					//txt.Format(_T("%.3f, %.3f "), d->weight, d->weight_adjusted);
					WriteText(dc, dx, dy, txt, RGB(0, 0, 0));
					dy += 25;
				}
				y += yStep;
			}
			x += xStep;
		}
	}
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
		CDialog::OnPaint();
		DrawNet(1);
		DrawNet(2);
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

	bool refresh = false; // theMind.Think(lastThought);

	Test1();
	OnClick_ErrAdj();
	
	//SetDlgItemText(IDC_Thought, lastThought);
	//last_thought = lastThought;

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
	if (all_done)
	{
		if (theMind.epoch <= 20)
		{
			lastThought.Format(_T("holy crap!"), theMind.epoch, max_err);
		}
		lastThought.Format(_T("%ld epochs, err %04f"), theMind.epoch, max_err);
		theList.AddString(lastThought);
		theList.SetCurSel(theList.GetCount()-1);
		OnBnClickedReset();
	}
	SetTimer(ThinkTimerID, 50, NULL);
}

void CMind1Dlg::OnBnClickedSave()
{
	KillTimer(ThinkTimerID);
	//theMind.Save();
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
	SetTimer(ThinkTimerID, 10, NULL);
	//theMind.Load(FN_CONCEPTS);
	//Refresh();
}


void CMind1Dlg::OnClick_Step()
{
	Test1();
	DrawNet(2);
}


void CMind1Dlg::OnClick_ErrAdj()
{
	CString strIn1, strIn2, strLearningRate;
	GetDlgItemText(IDC_IN1, strIn1);
	GetDlgItemText(IDC_IN2, strIn2);
	GetDlgItemText(IDC_LearningRate, strLearningRate);
	
	int in1 = _wtol(strIn1);
	int in2 = _wtol(strIn2);
	double LearningRate = _wtof(strLearningRate);

	CNeuron tmp;

	double LR = tmp.Sigmoid(max_err*10) * 20;
	strLearningRate.Format(_T("%2.f"), LR);
	SetDlgItemText(IDC_LearningRate, strLearningRate);

	double target = GetDesired(in1, in2);
	double desired = tmp.Sigmoid(target);

	nn_binary.AdjustWeights(desired, LR);

	// DrawNet(2);
}

void CMind1Dlg::OnBnClickedReset()
{
	CNNetLayer *l = NULL;
	int range = 8;
	for (int ln = 0; ln < nn_binary.NumLayers(); ln++)
	{
		l = nn_binary.layers[ln];
		l->bias = ((double)rand() / (double)RAND_MAX) * 2 - 1;
		//l->bias = ((double)rand() / (double)RAND_MAX);
		for (int nn = 0; nn < l->num_neurons; nn++)
		{
			CNeuron *n = l->NeuronAt(nn);
			POSITION pos = n->boutons.GetHeadPosition();
			while (pos)
			{
				CDendrite *d = n->boutons.GetNext(pos);
				double w = ((double)rand() / (double)RAND_MAX) * range - (range/2);
				//double w = ((double)rand() / (double)RAND_MAX);
				d->weight_adjusted = d->weight = w;
			}
		}
	}
	l->bias = 0;

	theMind.epoch = 0;
	all_done = false;
	max_err = 0;

	SetDlgItemText(IDC_IN1, _T("0"));
	SetDlgItemText(IDC_IN2, _T("0"));

	//DrawNet(2);
}
