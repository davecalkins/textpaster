
// TextPasterDlg.h : header file
//

#pragma once


// CTextPasterDlg dialog
class CTextPasterDlg : public CDialogEx
{
// Construction
public:
	CTextPasterDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_TEXTPASTER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;
	CFont textFont;

	void DoCountdown(DWORD timeMS);
	void HandleHotKey();
	void DoPaste();

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDestroy();
	afx_msg void OnHotKey(UINT nHotKeyId, UINT nKey1, UINT nKey2);
	CStatic m_MsgCtrl;
};
