//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//      ____         _    _      ____   _                                   //
//     |  _ \  __ _ | |_ | |__  |  _ \ | |  __ _     ___  _ __   _ __       //
//     | |_) |/ _` || __|| '_ \ | | | || | / _` |   / __|| '_ \ | '_ \      //
//     |  __/| (_| || |_ | | | || |_| || || (_| | _| (__ | |_) || |_) |     //
//     |_|    \__,_| \__||_| |_||____/ |_| \__, |(_)\___|| .__/ | .__/      //
//                                         |___/         |_|    |_|         //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//       Copyright (c) 1998-2016 by S.F.T. Inc. - All rights reserved       //
//  Use, copying, and distribution of this software are licensed according  //
//    to the GPLv2, LGPLv2, or BSD license, as appropriate (see COPYING)    //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "setup.h"
#include "PathDlg.h"
#include "PathBrowse.h"


//#ifdef _DEBUG
//#define new DEBUG_NEW
//#undef THIS_FILE
//static char THIS_FILE[] = __FILE__;
//#endif

/////////////////////////////////////////////////////////////////////////////
// CPathDialog dialog

#define _T(X) X /* for now */

CPathDialog::CPathDialog(HWND hParent /*=NULL*/)
	: CMyDialogBaseClass(CPathDialog::IDD, hParent)
{
	//{{AFX_DATA_INIT(CPathDialog)
	m_csPathName = _T("");
	m_csPathCaption = _T("");
	//}}AFX_DATA_INIT

  m_csPathCaption = "SETUP will install the application to the path "
                    "specified below.  If you like, you may change it to\n"
                    "another drive and/or path, and SETUP will create this"
                    " path for you if it does not already exist.";

  m_csTitle = "";
}

void CPathDialog::UpdateData(BOOL bSaveFlag)
{
  if(bSaveFlag)
  {
    GetDlgItemText(IDC_PATHNAME, m_csPathName);
//    GetDlgItemText(IDC_PATH_CAPTION, m_csPathCaption);
  }
  else
  {
    SetDlgItemText(IDC_PATHNAME, m_csPathName);
    SetDlgItemText(IDC_PATH_CAPTION, m_csPathCaption);
  }
}

BOOL CPathDialog::MessageHandler(UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
  WORD wCtrl = LOWORD(wParam);
  WORD wNotify = HIWORD(wParam);

  switch(uiMsg)
  {
    case WM_INITDIALOG:
      return(OnInitDialog());

    case WM_COMMAND:
      switch(wCtrl)
      {
        case IDOK:
          OnOK();
          return TRUE;

        case IDCANCEL:
          EndDialog(IDCANCEL);
          return TRUE;

        case IDC_BROWSE:
          if(wNotify == BN_CLICKED)
          {
            OnBrowse();
            return TRUE;
          }
          break;
      }

      break;
  }

  return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CPathDialog message handlers

void CPathDialog::OnOK() 
{
	UpdateData(1);

  if(m_csPathName.GetLength() > 260)
  {
    // TODO:  an exception perhaps?

    m_csPathName = m_csPathName.Left(260);
    SetDlgItemText(IDC_PATHNAME, m_csPathName);

    MyMessageBox("WARNING:  path name exceeds the allowed maximum of 260 characters; value truncated\r\n");
    PostMessage(m_hWnd, WM_NEXTDLGCTL, (WPARAM)GetDlgItem(IDC_PATHNAME), (LPARAM)TRUE);
    return;
  }

  if(m_csPathName.FindOneOf("?*'\"") >= 0)
  {
    MyMessageBox((IDS_ERROR24), MB_OK | MB_ICONHAND | MB_SETFOREGROUND);
    return;
  }

  CString csTemp;
  LPSTR lp1;
  if(GetFullPathName(m_csPathName, MAX_PATH, 
                     csTemp.GetBufferSetLength(MAX_PATH), &lp1))
  {
    csTemp.ReleaseBuffer(-1);
    m_csPathName = csTemp;
  }
  else
  {
    csTemp.ReleaseBuffer(0);
  }

  BOOL bIsDir, bIsDrive;

  if(!CheckForExistence(m_csPathName, bIsDir, bIsDrive))
  {
    if(bIsDrive)
    {
      MyMessageBox((IDS_ERROR25), MB_OK | MB_ICONHAND | MB_SETFOREGROUND);
      return;
    }
  }
  else if(!bIsDir)
  {
    if(MyMessageBox((IDS_ERROR26), MB_OKCANCEL | MB_ICONEXCLAMATION | MB_SETFOREGROUND)
       != IDOK)
    {
      EndDialog(IDCANCEL);
      return;
    }
  }

  TRACE("Path name is \"%s\"\r\n", (LPCSTR)m_csPathName);

	EndDialog(IDOK);
}

BOOL CPathDialog::OnInitDialog() 
{
	UpdateData(0);

  if(m_csTitle.GetLength())
    SetWindowText(m_csTitle);

	SetForegroundWindow();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	               // EXCEPTION: OCX Property Pages should return FALSE
}

void CPathDialog::OnBrowse() 
{
  UpdateData(1);

  CPathBrowse dlg(m_csPathName);

  dlg.m_csTitle = m_csPathCaption;

  if(dlg.DoModal() == IDOK)
    m_csPathName = dlg.GetPathName();

  UpdateData(0);
}
