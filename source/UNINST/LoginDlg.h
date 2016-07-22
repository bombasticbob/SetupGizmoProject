//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//          _                   _         ____   _            _             //
//         | |     ___    __ _ (_) _ __  |  _ \ | |  __ _    | |__          //
//         | |    / _ \  / _` || || '_ \ | | | || | / _` |   | '_ \         //
//         | |___| (_) || (_| || || | | || |_| || || (_| | _ | | | |        //
//         |_____|\___/  \__, ||_||_| |_||____/ |_| \__, |(_)|_| |_|        //
//                       |___/                      |___/                   //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//       Copyright (c) 1998-2016 by S.F.T. Inc. - All rights reserved       //
//  Use, copying, and distribution of this software are licensed according  //
//    to the GPLv2, LGPLv2, or BSD license, as appropriate (see COPYING)    //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////



#ifndef _LOGINDLG_H_INCLUDED_
#define _LOGINDLG_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CLoginDlg dialog

//#include "DialogHelpers.h"

class CLoginDlg : public CMyDialogBaseClass
{
// Construction
public:
  CLoginDlg(HWND hParent = NULL); // standard constructor

  enum { IDD = IDD_LOGON };
  CEdit m_wndDomain;
  CString m_csUserName;
  CString m_csPassword;
  CString m_csDomain;

protected:
  void UpdateData(BOOL bSaveFlag);
  BOOL MessageHandler(UINT uiMsg, WPARAM wParam, LPARAM lParam);

// Implementation
protected:
  HICON m_hIcon;

  BOOL OnInitDialog();
  BOOL OnPaint();
  HCURSOR OnQueryDragIcon();
  void OnCancel();
  void OnOK();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // _LOGINDLG_H_INCLUDED_
