///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                           //
//   ____                                     ____   _                            _  ____   _                                //
//  |  _ \  ___  _ __ ___    ___ __   __ ___ / ___| | |__    __ _  _ __  ___   __| ||  _ \ | |  __ _     ___  _ __   _ __    //
//  | |_) |/ _ \| '_ ` _ \  / _ \\ \ / // _ \\___ \ | '_ \  / _` || '__|/ _ \ / _` || | | || | / _` |   / __|| '_ \ | '_ \   //
//  |  _ <|  __/| | | | | || (_) |\ V /|  __/ ___) || | | || (_| || |  |  __/| (_| || |_| || || (_| | _| (__ | |_) || |_) |  //
//  |_| \_\\___||_| |_| |_| \___/  \_/  \___||____/ |_| |_| \__,_||_|   \___| \__,_||____/ |_| \__, |(_)\___|| .__/ | .__/   //
//                                                                                             |___/         |_|    |_|      //
//                                                                                                                           //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                           //
//                                Copyright (c) 1998-2016 by S.F.T. Inc. - All rights reserved                               //
//                           Use, copying, and distribution of this software are licensed according                          //
//                             to the GPLv2, LGPLv2, or BSD license, as appropriate (see COPYING)                            //
//                                                                                                                           //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "UNINST.h"
#include "RemoveSharedDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRemoveSharedDlg dialog

#define _T(X) X /* temporary */

CRemoveSharedDlg::CRemoveSharedDlg(HWND hParent /*=NULL*/)
	: CMyDialogBaseClass(CRemoveSharedDlg::IDD, hParent)
{
	//{{AFX_DATA_INIT(CRemoveSharedDlg)
	m_csFileName = _T("");
	m_csMessage = _T("");
	//}}AFX_DATA_INIT

  m_csMessage.LoadString(IDS_REMOVE_SHARED);  // load from resource (easier)
}

void CRemoveSharedDlg::Update(BOOL bSaveFlag)
{
  if(bSaveFlag)
  {
    GetDlgItemText(IDC_FILENAME, m_csFileName);
    GetDlgItemText(IDC_MESSAGE, m_csMessage);
  }
  else
  {
    SetDlgItemText(IDC_FILENAME, m_csFileName);
    SetDlgItemText(IDC_MESSAGE, m_csMessage);
  }
}

BOOL CRemoveSharedDlg::MessageHandler(UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
  if(uiMsg == WM_INITDIALOG)
  {
    Update(0);  // that's all I need
    return(TRUE);
  }

  if(uiMsg == WM_COMMAND)
  {
    WORD wCtrl = LOWORD(wParam);
    WORD wNotify = HIWORD(wParam);

    switch(wCtrl)
    {
      case IDC_YES_TO_ALL:
        if(wNotify == BN_CLICKED)
        {
          OnYesToAll();
          return(TRUE);
        }
        break;
      case IDNO:
        if(wNotify == BN_CLICKED)
        {
          OnNo();
          return(TRUE);
        }
        break;
      case IDYES:
        if(wNotify == BN_CLICKED)
        {
          OnYes();
          return(TRUE);
        }
        break;
      case IDCANCEL:
        if(wNotify == BN_CLICKED)
        {
          OnCancel();
          return(TRUE);
        }
        break;
    }
  }

  return(FALSE);
}

/////////////////////////////////////////////////////////////////////////////
// CRemoveSharedDlg message handlers

void CRemoveSharedDlg::OnYesToAll() 
{
  EndDialog(IDC_YES_TO_ALL);
}

void CRemoveSharedDlg::OnCancel() 
{
  EndDialog(IDCANCEL);
}

void CRemoveSharedDlg::OnNo() 
{
  EndDialog(IDNO);
}

void CRemoveSharedDlg::OnYes() 
{
  EndDialog(IDYES);
}

