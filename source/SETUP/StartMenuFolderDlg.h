//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                          //
//   ____   _                _    __  __                      _____       _      _              ____   _            _       //
//  / ___| | |_  __ _  _ __ | |_ |  \/  |  ___  _ __   _   _ |  ___|___  | |  __| |  ___  _ __ |  _ \ | |  __ _    | |__    //
//  \___ \ | __|/ _` || '__|| __|| |\/| | / _ \| '_ \ | | | || |_  / _ \ | | / _` | / _ \| '__|| | | || | / _` |   | '_ \   //
//   ___) || |_| (_| || |   | |_ | |  | ||  __/| | | || |_| ||  _|| (_) || || (_| ||  __/| |   | |_| || || (_| | _ | | | |  //
//  |____/  \__|\__,_||_|    \__||_|  |_| \___||_| |_| \__,_||_|   \___/ |_| \__,_| \___||_|   |____/ |_| \__, |(_)|_| |_|  //
//                                                                                                        |___/             //
//                                                                                                                          //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                          //
//                               Copyright (c) 1998-2016 by S.F.T. Inc. - All rights reserved                               //
//                          Use, copying, and distribution of this software are licensed according                          //
//                            to the GPLv2, LGPLv2, or BSD license, as appropriate (see COPYING)                            //
//                                                                                                                          //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#if !defined(AFX_STARTMENUFOLDERDLG_H__A8A60D81_1599_11D2_A115_004033901FF3__INCLUDED_)
#define AFX_STARTMENUFOLDERDLG_H__A8A60D81_1599_11D2_A115_004033901FF3__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// StartMenuFolderDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CStartMenuFolderDlg dialog

//#include "DialogHelpers.h"

class CStartMenuFolderDlg : public CMyDialogBaseClass
{
// Construction
public:
  CStartMenuFolderDlg(HWND hParent = NULL);   // standard constructor

  enum { IDD = IDD_STARTMENU_FOLDER };
  BOOL  m_bAllUsers;
  CString m_csFolderName;

  BOOL m_bHasUserProfile; // for '98 must check registry first
                          // always true for 'NT.
                          // Always FALSE for '95 [even though '95 can have profiles]

  void DoIt(void);  // this makes it all happen...
  BOOL CreateShortcuts(CString &);

  CStringArray m_acsEntries;  // list o' shortcuts to create


// Implementation
protected:
  CString GetFolderName(BOOL &bAllUsersFlag);

  BOOL GetStartMenuFolderName(BOOL bAllUsersFlag, CString &csFolderName,
                              CString &csCommonFolderName, BOOL bMessageBoxFlag = TRUE);

  void UpdateData(BOOL bSaveFlag);
  virtual BOOL MessageHandler(UINT uiMsg, WPARAM wParam, LPARAM lParam);

  void OnOK();
  void OnCancel();
  void OnBrowse();

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STARTMENUFOLDERDLG_H__A8A60D81_1599_11D2_A115_004033901FF3__INCLUDED_)
