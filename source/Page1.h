//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//                   ____                      _     _                      //
//                  |  _ \  __ _   __ _   ___ / |   | |__                   //
//                  | |_) |/ _` | / _` | / _ \| |   | '_ \                  //
//                  |  __/| (_| || (_| ||  __/| | _ | | | |                 //
//                  |_|    \__,_| \__, | \___||_|(_)|_| |_|                 //
//                                |___/                                     //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//       Copyright (c) 1998-2016 by S.F.T. Inc. - All rights reserved       //
//  Use, copying, and distribution of this software are licensed according  //
//    to the GPLv2, LGPLv2, or BSD license, as appropriate (see COPYING)    //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_PAGE1_H__13778B92_0C34_11D2_A115_004033901FF3__INCLUDED_)
#define AFX_PAGE1_H__13778B92_0C34_11D2_A115_004033901FF3__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// Page1.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPage1 dialog

class CPage1 : public CPropertyPage
{
  DECLARE_DYNCREATE(CPage1)

// Construction
public:
  CPage1();
  ~CPage1();

// Dialog Data
  //{{AFX_DATA(CPage1)
  enum { IDD = IDD_PAGE_1 };
  CString m_csAppName;
  CString m_csCompanyName;
  CString m_csSourcePath;
  CString m_csStartMessage;
  CString m_csDestPath;
  CString m_csLicenseFile;
  CString m_csCertFiles;
  //}}AFX_DATA

  CString m_csAppPath, m_csAppCommon, m_csStartMenu;  // "advanded" settings
  CStringArray m_acsUpgrade;  // list o' things to upgrade

  void DoClear();

// Overrides
  // ClassWizard generate virtual function overrides
  //{{AFX_VIRTUAL(CPage1)
  public:
  virtual LRESULT OnWizardNext();
  virtual BOOL OnSetActive();
  virtual BOOL OnKillActive();
  protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  //}}AFX_VIRTUAL

// Implementation
protected:
  // Generated message map functions
  //{{AFX_MSG(CPage1)
  afx_msg void OnBrowseDest();
  afx_msg void OnBrowse();
  afx_msg void OnBrowseLicense();
  afx_msg void OnBrowseCert();
  afx_msg void OnTest();
  afx_msg void OnOpen();
  afx_msg void OnAdvanced();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()

public:
  afx_msg void OnTestLic();
};


#define MAX_NUM_CERT_FILES 32


//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGE1_H__13778B92_0C34_11D2_A115_004033901FF3__INCLUDED_)
