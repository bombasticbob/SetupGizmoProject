//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//      ____                      ____   _                                  //
//     |  _ \  _ __  ___    __ _ |  _ \ | |  __ _     ___  _ __   _ __      //
//     | |_) || '__|/ _ \  / _` || | | || | / _` |   / __|| '_ \ | '_ \     //
//     |  __/ | |  | (_) || (_| || |_| || || (_| | _| (__ | |_) || |_) |    //
//     |_|    |_|   \___/  \__, ||____/ |_| \__, |(_)\___|| .__/ | .__/     //
//                         |___/            |___/         |_|    |_|        //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//       Copyright (c) 1998-2016 by S.F.T. Inc. - All rights reserved       //
//  Use, copying, and distribution of this software are licensed according  //
//    to the GPLv2, LGPLv2, or BSD license, as appropriate (see COPYING)    //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "uninst.h"
#include "ProgDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProgressDialog dialog


CProgressDialog::CProgressDialog(HWND hParent /*=NULL*/)
	: CMyDialogBaseClass(CProgressDialog::IDD, hParent)
{
  m_csProgressText = "INITIALIZATION...";
  m_csProgress = "0";  // initial value must be '0'
}

void CProgressDialog::UpdateData(BOOL bSaveFlag)
{
  if(!m_Progress.m_hWnd)
    m_Progress.Attach(GetDlgItem(IDC_PROGRESS));

  if(bSaveFlag)
  {
    GetDlgItemText(IDC_PROGRESS_TEXT, m_csProgressText);
    GetDlgItemText(IDC_PROGRESS, m_csProgress);
  }
  else
  {
    SetDlgItemText(IDC_PROGRESS_TEXT, m_csProgressText);
    SetDlgItemText(IDC_PROGRESS, m_csProgress);
  }
}

/////////////////////////////////////////////////////////////////////////////
// CProgressDialog message handlers

BOOL CProgressDialog::MessageHandler(UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
  if(uiMsg == WM_INITDIALOG)
    return(OnInitDialog());
  else if(uiMsg == WM_COMMAND)
    return(OnCommand(wParam, lParam));

  return(FALSE);  // not handled
}

BOOL CProgressDialog::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	if(wParam == 0x1234)  // special command message
  {
    m_csProgressText = (LPCSTR)lParam;
    UpdateData(0);  // load controls from members

    UpdateWindow();  // always re-paint if I need to

    return(1);  // "handled"
  }
  else if(wParam == 0x4321)
  {
    UpdateData(1);  // load members from controls
    CString csOld = m_csProgress;

    m_csProgress = (LPCSTR)lParam;
    UpdateData(0);  // load controls from members

    if(csOld.CompareNoCase(m_csProgress))
    {
      m_Progress.InvalidateRect(NULL, TRUE);
      m_Progress.UpdateWindow();
    }

    UpdateWindow();  // always re-paint if I need to

    return(1);  // "handled"
  }

  return(FALSE);  // I didn't handle anything
}

BOOL CProgressDialog::OnInitDialog() 
{
  UpdateData(0);  // that's all I need

	SetWindowText(theApp.m_csAppName + " SETUP Progress");
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
