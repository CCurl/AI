
// Mind1Dlg.h : header file
//

#pragma once
#include "Mind.h"
#include "afxwin.h"

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

	void MakeCharBM(LPCTSTR Char);
	void TestBitmap();
	void Refresh();

	// CMind theMind;
	CMind theMind;
	CString theInfo;
	CString output1;
	CString output2;
	int ThinkTimerID, ThinkDelay;
	CString lastThought;
	//CImage theImage;
	//CStatic *thePic;
	CStatic anImage;
};
