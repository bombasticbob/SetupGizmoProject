//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//                 ____                      _____    _                     //
//                |  _ \  __ _   __ _   ___ |___ /   | |__                  //
//                | |_) |/ _` | / _` | / _ \  |_ \   | '_ \                 //
//                |  __/| (_| || (_| ||  __/ ___) |_ | | | |                //
//                |_|    \__,_| \__, | \___||____/(_)|_| |_|                //
//                              |___/                                       //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//       Copyright (c) 1998-2016 by S.F.T. Inc. - All rights reserved       //
//  Use, copying, and distribution of this software are licensed according  //
//    to the GPLv2, LGPLv2, or BSD license, as appropriate (see COPYING)    //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_PAGE3_H__8B06F2C5_0C49_11D2_A115_004033901FF3__INCLUDED_)
#define AFX_PAGE3_H__8B06F2C5_0C49_11D2_A115_004033901FF3__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// Page3.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPage3 dialog

class CPage3 : public CPropertyPage
{
  DECLARE_DYNCREATE(CPage3)

// Construction
public:
  CPage3();
  ~CPage3();

// Dialog Data
  //{{AFX_DATA(CPage3)
  enum { IDD = IDD_PAGE_3 };
  CButton m_wndAutoDelete;
  CEdit m_wndUninstArgs;
  CEdit m_wndExecArgs;
  CEdit m_wndOtherPathName;
  CListBox  m_wndOSList;
  CComboBox m_wndOptions;
  CListBox  m_wndFileList;
  BOOL  m_bAutoDelete;
  BOOL  m_bExecute;
  BOOL  m_bOptionInstall;
  CString m_csOtherPathName;
  BOOL  m_bOverwrite;
  BOOL  m_bSelectOS;
  BOOL  m_bShared;
  int   m_iDestPath;
  BOOL  m_bNoUninstall;
  BOOL  m_bRegister;
  BOOL  m_bAddShortcut;
  CString m_csExecArgs;
  CString m_csUninstArgs;
  //}}AFX_DATA


  int m_iCurSel;

  void DoClear();

// Overrides
  // ClassWizard generate virtual function overrides
  //{{AFX_VIRTUAL(CPage3)
  public:
  virtual BOOL OnSetActive();
  virtual BOOL OnKillActive();
  virtual LRESULT OnWizardBack();
  virtual LRESULT OnWizardNext();
  protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  //}}AFX_VIRTUAL

// Implementation
protected:
  // Generated message map functions
  //{{AFX_MSG(CPage3)
  afx_msg void OnSystemdir();
  afx_msg void OnTempdir();
  afx_msg void OnAppdir();
  afx_msg void OnOtherpath();
  afx_msg void OnOptionInstall();
  afx_msg void OnSelectOs();
  afx_msg void OnSelchangeFileList();
  afx_msg void OnMiniHelp();
  afx_msg void OnExecute();
  afx_msg void OnNoUninstall();
  afx_msg void OnShared();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()

public:
  // when TRUE, uses 'regasm' instead of 'regsvr32' or 'Dll[Un]RegisterServer'
  BOOL m_bRegDotNet;
  afx_msg void OnClickRegDotNet();
  // disable this when 'Register DLL' not checked or not enabled
  CButton m_btnRegDotNet;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGE3_H__8B06F2C5_0C49_11D2_A115_004033901FF3__INCLUDED_)
