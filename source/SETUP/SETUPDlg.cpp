//////////////////////////////////////////////////////////////////////////////////
//                                                                              //
//   ____   _____  _____  _   _  ____   ____   _                                //
//  / ___| | ____||_   _|| | | ||  _ \ |  _ \ | |  __ _     ___  _ __   _ __    //
//  \___ \ |  _|    | |  | | | || |_) || | | || | / _` |   / __|| '_ \ | '_ \   //
//   ___) || |___   | |  | |_| ||  __/ | |_| || || (_| | _| (__ | |_) || |_) |  //
//  |____/ |_____|  |_|   \___/ |_|    |____/ |_| \__, |(_)\___|| .__/ | .__/   //
//                                                |___/         |_|    |_|      //
//                                                                              //
//////////////////////////////////////////////////////////////////////////////////
//                                                                              //
//         Copyright (c) 1998-2016 by S.F.T. Inc. - All rights reserved         //
//    Use, copying, and distribution of this software are licensed according    //
//      to the GPLv2, LGPLv2, or BSD license, as appropriate (see COPYING)      //
//                                                                              //
//////////////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "SETUP.h"
#include "SETUPDlg.h"

//#ifdef _DEBUG
//#define new DEBUG_NEW
//#undef THIS_FILE
//static char THIS_FILE[] = __FILE__;
//#endif

/////////////////////////////////////////////////////////////////////////////
// CSETUPDlg dialog

#define _T(X) X /* temporary */

CSETUPDlg::CSETUPDlg(HWND hParent /*=NULL*/)
	: CMyDialogBaseClass(CSETUPDlg::IDD, hParent)
{
	m_csWelcome = _T("");

  m_csTitle = "SFT SETUP Application";  // default

	m_hIcon = /*AfxGetApp()->*/theApp.LoadIcon(IDR_MAINFRAME);
}

void CSETUPDlg::UpdateData(BOOL bSaveFlag)
{
  if(bSaveFlag)
  {
    GetDlgItemText(IDC_WELCOME, m_csWelcome);
  }
  else
  {
    SetDlgItemText(IDC_WELCOME, m_csWelcome);
  }
}

BOOL CSETUPDlg::MessageHandler(UINT uiMsg, WPARAM wParam, LPARAM lParam)
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
      return((BOOL)OnQueryDragIcon());

    case WM_COMMAND:
      switch(wCtrl)
      {
        case IDOK:
          UpdateData(1);
          // flows through to next part

        case IDCANCEL:
          EndDialog(wCtrl);
          return TRUE;
      }
      break;
  }

  return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CSETUPDlg message handlers

BOOL CSETUPDlg::OnInitDialog()
{
	UpdateData(0);

  if(theApp.m_csOSName.GetLength())
  {
    m_csTitle += " (" + theApp.m_csOSName + ")";
  }
//  else if(theApp.m_bIsNT)
//  {
//    if(theApp.m_dwMajorVer == 4)
//      m_csTitle += " (Win NT 4.x)";
//    else if(theApp.m_dwMajorVer == 5 && !theApp.m_dwMinorVer)
//      m_csTitle += " (Win 2000)";
//    else if(theApp.m_dwMajorVer >= 5)
//      m_csTitle += " (Win XP+)";
//  }
//  else
//  {
//    if(theApp.m_dwMajorVer == 4)
//    {
//      if(theApp.m_dwMinorVer < 10)
//      {
//        m_csTitle += " (Win 95)";
//      }
//      else if(theApp.m_dwMinorVer < 90)
//      {
//        m_csTitle += " (Win 98)";
//      }
//      else // if(m_dwMinorVer >= 90)
//      {
//        m_csTitle += " (Win ME)";
//      }
//    }
//  }

  SetWindowText(m_csTitle);

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

BOOL CSETUPDlg::OnPaint()
{
	if (IsIconic())
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

    return TRUE;
	}
	else
	{
    return FALSE;
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CSETUPDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}
