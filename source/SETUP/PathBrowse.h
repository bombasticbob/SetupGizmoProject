//////////////////////////////////////////////////////////////////////////////////
//                                                                              //
//   ____         _    _      ____                                      _       //
//  |  _ \  __ _ | |_ | |__  | __ )  _ __  ___ __      __ ___   ___    | |__    //
//  | |_) |/ _` || __|| '_ \ |  _ \ | '__|/ _ \\ \ /\ / // __| / _ \   | '_ \   //
//  |  __/| (_| || |_ | | | || |_) || |  | (_) |\ V  V / \__ \|  __/ _ | | | |  //
//  |_|    \__,_| \__||_| |_||____/ |_|   \___/  \_/\_/  |___/ \___|(_)|_| |_|  //
//                                                                              //
//                                                                              //
//////////////////////////////////////////////////////////////////////////////////
//                                                                              //
//         Copyright (c) 1998-2016 by S.F.T. Inc. - All rights reserved         //
//    Use, copying, and distribution of this software are licensed according    //
//      to the GPLv2, LGPLv2, or BSD license, as appropriate (see COPYING)      //
//                                                                              //
//////////////////////////////////////////////////////////////////////////////////


#if !defined(AFX_PATHBROWSE_H__247C6801_2049_11D2_A115_004033901FF3__INCLUDED_)
#define AFX_PATHBROWSE_H__247C6801_2049_11D2_A115_004033901FF3__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// PathBrowse.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPathBrowse dialog

//#include "DialogHelpers.h"

class CPathBrowse : public CMyDialogBaseClass
{
//  DECLARE_DYNAMIC(CPathBrowse);
public:
  CPathBrowse(LPCTSTR lpszFileName = NULL,
              DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
              HWND hParentWnd = NULL);

  int DoModal(void);

  CString GetPathName() { return(m_csPath); }

  CString m_csTitle, m_csRoot;

protected:

//  CMySubClassWindow *m_pOwner;  not being used
  CString m_csPath;

  virtual BOOL MessageHandler(UINT, WPARAM, LPARAM);
  static int CALLBACK PathBrowseCallback(HWND, UINT, LPARAM, LPARAM);

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PATHBROWSE_H__247C6801_2049_11D2_A115_004033901FF3__INCLUDED_)
