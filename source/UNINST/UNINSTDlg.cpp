//////////////////////////////////////////////////////////////////////////////////////
//                                                                                  //
//   _   _  _   _  ___  _   _  ____  _____  ____   _                                //
//  | | | || \ | ||_ _|| \ | |/ ___||_   _||  _ \ | |  __ _     ___  _ __   _ __    //
//  | | | ||  \| | | | |  \| |\___ \  | |  | | | || | / _` |   / __|| '_ \ | '_ \   //
//  | |_| || |\  | | | | |\  | ___) | | |  | |_| || || (_| | _| (__ | |_) || |_) |  //
//   \___/ |_| \_||___||_| \_||____/  |_|  |____/ |_| \__, |(_)\___|| .__/ | .__/   //
//                                                    |___/         |_|    |_|      //
//                                                                                  //
//////////////////////////////////////////////////////////////////////////////////////
//                                                                                  //
//           Copyright (c) 1998-2016 by S.F.T. Inc. - All rights reserved           //
//      Use, copying, and distribution of this software are licensed according      //
//        to the GPLv2, LGPLv2, or BSD license, as appropriate (see COPYING)        //
//                                                                                  //
//////////////////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "UNINST.h"
#include "UNINSTDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CUNINSTDlg dialog

CUNINSTDlg::CUNINSTDlg(HWND hParent /*=NULL*/)
: CMyDialogBaseClass(CUNINSTDlg::IDD, hParent)
{
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = MyGetApp()->LoadIcon(IDR_MAINFRAME);

  m_csCaption = "UNINSTALL will remove "
              + theApp.m_csAppName
              + " and all of its components from your system,"
                " including registry entries, and any shared"
                " components not currently in use by other"
                " applications.\r\n\n"
                "Press 'Un-Install' to continue, or 'Exit' to"
                " leave the application installed.";

  m_hWnd = NULL;
  m_hwndAppTitle = NULL;
  m_hwndTagLine = NULL;
}

BOOL CUNINSTDlg::MessageHandler(UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
  WORD wCtrl = LOWORD(wParam);
  WORD wNotify = HIWORD(wParam);

  switch(uiMsg)
  {
    case WM_INITDIALOG:
      return(OnInitDialog());

    case WM_PAINT:
      return(OnPaint());

    case WM_QUERYDRAGICON:
      return((BOOL)(INT_PTR)OnQueryDragIcon());

    case WM_COMMAND:
      switch(wCtrl)
      {
        case IDOK:
        case IDCANCEL:
          if(wNotify == BN_CLICKED)
          {
            EndDialog(wCtrl);
            return(TRUE);
          }
          break;

        case IDRETRY:
          if(wNotify == BN_CLICKED)
          {
            OnBnClickedRetry();
            return(TRUE);
          }
          break;
      }

      return(FALSE);
  }

  return(FALSE);  // default
}

/////////////////////////////////////////////////////////////////////////////
// CUNINSTDlg message handlers

BOOL CUNINSTDlg::OnInitDialog()
{
  m_hwndAppTitle = GetDlgItem(IDC_APP_TITLE);
  m_hwndTagLine = GetDlgItem(IDC_TAGLINE);
  SetDlgItemText(IDC_CAPTION, m_csCaption);

  if(theApp.m_csOSName.GetLength())
  {
    SetWindowText(theApp.m_csAppName + " Un-Install (" + theApp.m_csOSName + ")");
  }
  else
  {
    SetWindowText(theApp.m_csAppName + " Un-Install");
  }

  // re-assign font for top 2 boxes...

  HFONT hOldFont = (HFONT)::SendMessage(m_hwndAppTitle, WM_GETFONT, 0, 0);

  if(!hOldFont)
  {
    hOldFont = (HFONT)GetStockObject(SYSTEM_FONT);
  }

  if(hOldFont)
  {
    LOGFONT lf;
    ::GetObject(hOldFont, sizeof(lf), &lf);

    lf.lfWeight = 900;  // EXTREMELY bold
    lf.lfUnderline = TRUE;  // underlined

    lf.lfHeight = (int)(lf.lfHeight * 1.2);
    lf.lfWidth = (int)(lf.lfWidth * 1.2);

    lf.lfCharSet = ANSI_CHARSET;
    lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
    lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
    lf.lfQuality = DEFAULT_QUALITY;
    lf.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
    lstrcpy(lf.lfFaceName,"Arial");

    m_fontBold.CreateFontIndirect(&lf);

    lf.lfWeight = 400;  // normal
    lf.lfUnderline = FALSE;  // no underline

    lf.lfHeight = (int)(lf.lfHeight * .71); // comes out to .85
    lf.lfWidth = (int)(lf.lfWidth * .71);
    lf.lfItalic = TRUE;

    m_fontSmall.CreateFontIndirect(&lf);

    ::SendMessage(m_hwndAppTitle, WM_SETFONT, (WPARAM)(HFONT)m_fontBold, 0);
    ::SendMessage(m_hwndTagLine, WM_SETFONT, (WPARAM)(HFONT)m_fontSmall, 0);
  }

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

BOOL CUNINSTDlg::OnPaint() 
{
	if(IsIconic())
	{
		CMyPaintDC dc(m_hWnd); // device context for painting

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
    return(TRUE);
	}

  return(FALSE);
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CUNINSTDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

BOOL WINAPI CUNINSTDlg::AboutDlgProc(HWND hDlg, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
  WORD wCtrl = LOWORD(wParam);
  WORD wNotify = HIWORD(wParam);

  switch(uiMsg)
  {
    case WM_INITDIALOG:
      return(TRUE);

    case WM_COMMAND:
      switch(wCtrl)
      {
        case IDOK:
          ::EndDialog(hDlg, IDOK);
          return(TRUE);

      }

      break;      
  }

  return(FALSE);  // the default action
}

void CUNINSTDlg::OnBnClickedRetry()
{
  ::DialogBox(theApp.m_hInstance, MAKEINTRESOURCE(IDD_ABOUTBOX),
              m_hWnd, CUNINSTDlg::AboutDlgProc);
}

