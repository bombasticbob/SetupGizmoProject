//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//    _____  _         _       _      ____                         _        //
//   |  ___|(_) _ __  (_) ___ | |__  |  _ \  __ _   __ _   ___    | |__     //
//   | |_   | || '_ \ | |/ __|| '_ \ | |_) |/ _` | / _` | / _ \   | '_ \    //
//   |  _|  | || | | || |\__ \| | | ||  __/| (_| || (_| ||  __/ _ | | | |   //
//   |_|    |_||_| |_||_||___/|_| |_||_|    \__,_| \__, | \___|(_)|_| |_|   //
//                                                 |___/                    //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//       Copyright (c) 1998-2016 by S.F.T. Inc. - All rights reserved       //
//  Use, copying, and distribution of this software are licensed according  //
//    to the GPLv2, LGPLv2, or BSD license, as appropriate (see COPYING)    //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////


#if !defined(AFX_FINISHPAGE_H__8B06F2C4_0C49_11D2_A115_004033901FF3__INCLUDED_)
#define AFX_FINISHPAGE_H__8B06F2C4_0C49_11D2_A115_004033901FF3__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// FinishPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFinishPage dialog

class CSetupGizDlg;

class CFinishPage : public CPropertyPage
{
  DECLARE_DYNCREATE(CFinishPage)

// Construction
public:
  CFinishPage();
  ~CFinishPage();

  BOOL DoWizardFinish(CSetupGizDlg *pSheet);
    // allows me to automate the process by using THIS to write output

// Dialog Data
  //{{AFX_DATA(CFinishPage)
  enum { IDD = IDD_FINISH };
  CEdit m_wndDisk1Reserve;
  CButton m_btnAutoInsert;
  BOOL  m_bAutoInsert;
  int   m_iInstallType;
  BOOL  m_bDoNotClose;
  BOOL  m_bLZCompress;
  BOOL  m_bNoUninstall;
  BOOL  m_bQuietSetup;
  BOOL  m_bDoNotDelWorkFiles;
  BOOL  m_bDoNotRebuildCab;
  int   m_iRebootFlag;
  int   m_iDisk1Reserve;
  //}}AFX_DATA

  void DoClear();

// Overrides
  // ClassWizard generate virtual function overrides
  //{{AFX_VIRTUAL(CFinishPage)
  public:
  virtual BOOL OnSetActive();
  virtual BOOL OnWizardFinish();
  virtual LRESULT OnWizardBack();
  virtual BOOL OnKillActive();
  protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  //}}AFX_VIRTUAL

// Implementation
protected:
  // Generated message map functions
  //{{AFX_MSG(CFinishPage)
  afx_msg void OnSave();
  afx_msg void OnCdrom();
  afx_msg void OnCdromMulti();
  afx_msg void OnDiskette();
  afx_msg void OnSelfExtract();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FINISHPAGE_H__8B06F2C4_0C49_11D2_A115_004033901FF3__INCLUDED_)
