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

class CPathBrowse : public CCommonDialog
{
  DECLARE_DYNAMIC(CPathBrowse)

public:
  CPathBrowse(LPCTSTR lpszFileName = NULL,
              DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
              CWnd* pParentWnd = NULL);

  int DoModal(void);

  CString GetPathName() { return(m_csPath); }

  CString m_csTitle;

protected:

  CWnd *m_pOwner;
  CString m_csPath;

  static int CALLBACK PathBrowseCallback(HWND, UINT, LPARAM, LPARAM);

  //{{AFX_MSG(CPathBrowse)
    // NOTE - the ClassWizard will add and remove member functions here.
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PATHBROWSE_H__247C6801_2049_11D2_A115_004033901FF3__INCLUDED_)
