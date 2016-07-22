//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//     _   _  _   _  ___  _   _  ____  _____  ____   _            _         //
//    | | | || \ | ||_ _|| \ | |/ ___||_   _||  _ \ | |  __ _    | |__      //
//    | | | ||  \| | | | |  \| |\___ \  | |  | | | || | / _` |   | '_ \     //
//    | |_| || |\  | | | | |\  | ___) | | |  | |_| || || (_| | _ | | | |    //
//     \___/ |_| \_||___||_| \_||____/  |_|  |____/ |_| \__, |(_)|_| |_|    //
//                                                      |___/               //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//       Copyright (c) 1998-2016 by S.F.T. Inc. - All rights reserved       //
//  Use, copying, and distribution of this software are licensed according  //
//    to the GPLv2, LGPLv2, or BSD license, as appropriate (see COPYING)    //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////


#if !defined(AFX_UNINSTDLG_H__B3134EF9_0D1F_11D2_A115_004033901FF3__INCLUDED_)
#define AFX_UNINSTDLG_H__B3134EF9_0D1F_11D2_A115_004033901FF3__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

//#include "DialogHelpers.h"

/////////////////////////////////////////////////////////////////////////////
// CUNINSTDlg dialog

class CUNINSTDlg : public CMyDialogBaseClass
{
// Construction
public:
  CUNINSTDlg(HWND hParent = NULL);  // standard constructor

  enum { IDD = IDD_UNINST_DIALOG };

  HWND m_hwndAppTitle, m_hwndTagLine;
  CString m_csCaption;

  CFont m_fontBold, m_fontSmall;


// Implementation
protected:
  HICON m_hIcon;
//  MSG m_msgLast;  // so I know what I'm dealing with

  virtual BOOL MessageHandler(UINT uiMsg, WPARAM wParam, LPARAM lParam);

  static BOOL WINAPI AboutDlgProc(HWND hDlg, UINT uiMsg, WPARAM wParam, LPARAM lParam);

  // Generated message map functions
  //{{AFX_MSG(CUNINSTDlg)
  BOOL OnInitDialog();
  BOOL OnPaint();
  HCURSOR OnQueryDragIcon();
  //}}AFX_MSG
//  DECLARE_MESSAGE_MAP()
public:
  void OnBnClickedRetry();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UNINSTDLG_H__B3134EF9_0D1F_11D2_A115_004033901FF3__INCLUDED_)
