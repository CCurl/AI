
// Mind1Dlg.h : header file
//

#pragma once
#include "Mind.h"
#include "afxwin.h"
#include "NNTest.h"

// CMind1Dlg dialog
class CMind1Dlg : public CDialog
{
// Construction
public:
	CMind1Dlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_MIND1_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedSave();
	afx_msg void OnBnClickedLoad();
	afx_msg void OnTimer(UINT_PTR TimerID);

	void DrawCircle(CClientDC& dc, int x, int y, int radius, CPen& pen);
	void DrawLine(CClientDC& dc, int FromX, int FromY, int ToX, int ToY, CPen& pen);
	void DrawRectangle(CClientDC& dc, int top, int left, int right, int bottom, CPen& pen);
	void WriteText(CClientDC& dc, int x, int y, LPCTSTR text, COLORREF color);
	void DrawNet(int Which);
	void MakeCharBM(LPCTSTR Char);
	void Refresh();
	void Test1();
	int Train(TCHAR Ch);

	CMind theMind;
	CString theInfo;
	CString output1;
	CString output2;
	int ThinkTimerID, ThinkDelay;
	CString lastThought;
	CNeuralNet char_rec;
	CNeuralNet nn_binary;
	CStatic anImage;
	bool all_done;
	double max_err;

	afx_msg void OnClick_Step();
	afx_msg void OnClick_ErrAdj();
	afx_msg void OnBnClickedReset();
	CListBox theList;
};
