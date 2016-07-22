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

class CLoginDlg : public CMyDialogBaseClass /*CDialog*/
{
// Construction
public:
  CLoginDlg(HWND hParent = NULL); // standard constructor

// Dialog Data
  //{{AFX_DATA(CLoginDlg)
  enum { IDD = IDD_LOGON };
  CEdit m_wndDomain;
  CString m_csUserName;
  CString m_csPassword;
  CString m_csDomain;
  //}}AFX_DATA

  
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CLoginDlg)
protected:
  void UpdateData(BOOL bSaveFlag);
  virtual BOOL MessageHandler(UINT, WPARAM, LPARAM);
  //}}AFX_VIRTUAL

// Implementation
protected:
  HICON m_hIcon;

  // Generated message map functions
  //{{AFX_MSG(CLoginDlg)
  virtual BOOL OnInitDialog();
  BOOL OnPaint();
  HCURSOR OnQueryDragIcon();
  virtual void OnCancel();
  virtual void OnOK();
  //}}AFX_MSG
//  DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // _LOGINDLG_H_INCLUDED_
