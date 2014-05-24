
// TextPasterDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TextPaster.h"
#include "TextPasterDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include <vector>
using namespace std;

// CTextPasterDlg dialog




CTextPasterDlg::CTextPasterDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CTextPasterDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTextPasterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX,IDC_MSG,m_MsgCtrl);
}

BEGIN_MESSAGE_MAP(CTextPasterDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_WM_HOTKEY()
END_MESSAGE_MAP()


// CTextPasterDlg message handlers

#define HOTKEY_ID 0x100

const CString defaultMsg = _T("Ctrl+Shift+/");

BOOL CTextPasterDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// register a system wide hotkey to be handled by this window (via WM_HOTKEY messages)
	const UINT hotKeyModifiers = MOD_CONTROL | MOD_SHIFT; 
	const TCHAR hotKeyChar = _T('/');
	BOOL res = RegisterHotKey(GetSafeHwnd(),HOTKEY_ID,hotKeyModifiers,VkKeyScan(hotKeyChar));

	// set message control to have a large bold font
	LOGFONT lf = {0};
	m_MsgCtrl.GetFont()->GetLogFont(&lf);
	CDC* pDC = GetDC();
	const LONG textHeightPts = 32;
	lf.lfHeight = -MulDiv(textHeightPts, pDC->GetDeviceCaps(LOGPIXELSY), 72);
	lf.lfWeight = FW_BOLD;
	ReleaseDC(pDC);
	textFont.CreateFontIndirectW(&lf);
	m_MsgCtrl.SetFont(&textFont);

	// initial message
	m_MsgCtrl.SetWindowText(defaultMsg);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTextPasterDlg::OnPaint()
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
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTextPasterDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CTextPasterDlg::OnDestroy()
{
	UnregisterHotKey(GetSafeHwnd(),HOTKEY_ID);

	CDialogEx::OnDestroy();
}

void ProcessWaitingMessages()
{
	MSG m;
	while (PeekMessage(&m,NULL,0,0,PM_REMOVE))
	{
		TranslateMessage(&m);
		DispatchMessage(&m);
	}
}

// show a countdown in the message window while waiting the specified amount of time and running the message pump
void CTextPasterDlg::DoCountdown(DWORD timeMS)
{
	DWORD start = GetTickCount();
	while (TRUE)
	{
		DWORD now = GetTickCount();
		DWORD delta = now - start;

		if (delta >= timeMS)
			break;

		DWORD timeLeft = (DWORD)((((double)timeMS - (double)delta) / 1000.0) + 0.5);
		CString newStr;
		newStr.Format(_T("%d"), timeLeft);

		CString curStr;
		m_MsgCtrl.GetWindowText(curStr);

		if (newStr != curStr)
			m_MsgCtrl.SetWindowText(newStr);

		ProcessWaitingMessages();
	}
}

void CTextPasterDlg::OnHotKey(UINT nHotKeyId, UINT nKey1, UINT nKey2)
{
	// hotkey was triggered
	if (nHotKeyId == HOTKEY_ID)
	{
		HandleHotKey();
	}

	CDialogEx::OnHotKey(nHotKeyId, nKey1, nKey2);
}

void CTextPasterDlg::HandleHotKey()
{
	// prevent re-entry
	static BOOL alreadyIn = FALSE;
	if (alreadyIn)
		return;
	alreadyIn = TRUE;

	// do the countdown
	const DWORD countDownIntervalMS = 3000;
	DoCountdown(countDownIntervalMS);

	// do the paste operation
	DoPaste();

	// done, revert back to default message
	m_MsgCtrl.SetWindowText(defaultMsg);

	alreadyIn = FALSE;
}

void CTextPasterDlg::DoPaste()
{
	// get text from clipboard
	CString text;
	if (OpenClipboard())
	{
		HANDLE hData = GetClipboardData(CF_UNICODETEXT);
		if (hData)
		{
			LPTSTR pData = (LPTSTR)GlobalLock(hData);
			if (pData)
			{
				text = pData;
				GlobalUnlock(hData);
			}
		}

		CloseClipboard();
	}

	// no text?  nothing to do
	if (text.IsEmpty())
		return;

	// from docs for VkKeyScan API function
	const BYTE ssShift     = 1;
	const BYTE ssCtrl      = 2;
	const BYTE ssAlt       = 4;
	const BYTE ssHankaku   = 8;
	const BYTE ssReserved1 = 16;
	const BYTE ssReserved2 = 32;

	// send each char in the string
	for (int i = 0; i < text.GetLength(); i++)
	{
		TCHAR c = text[i];

		// get virtual key code and shift state for this char
		SHORT res = VkKeyScan(c);
		BYTE vkCode = LOBYTE(res);
		BYTE shiftState = HIBYTE(res);

		vector<INPUT> inArr;
		INPUT in = {0};
		in.type = INPUT_KEYBOARD;

		// press shift
		if (shiftState & ssShift)
		{
			in.ki.wVk = VK_SHIFT;
			inArr.push_back(in);
		}

		// press key
		in.ki.wVk = vkCode;
		inArr.push_back(in);

		// release key
		in.ki.wVk = vkCode;
		in.ki.dwFlags = KEYEVENTF_KEYUP;
		inArr.push_back(in);

		// release shift
		if (shiftState & ssShift)
		{
			in.ki.wVk = VK_SHIFT;
			in.ki.dwFlags = KEYEVENTF_KEYUP;
			inArr.push_back(in);
		}

		// send the above input events
		SendInput(inArr.size(),&inArr[0],sizeof(INPUT));

		// delay a bit between each char
		const DWORD perCharDelayMS = 10;
		Sleep(perCharDelayMS);

		// run the message pump
		ProcessWaitingMessages();
	}
}


