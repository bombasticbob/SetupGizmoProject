//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//              ____                      _____  ____     _                 //
//             |  _ \  __ _   __ _   ___ |___ / | __ )   | |__              //
//             | |_) |/ _` | / _` | / _ \  |_ \ |  _ \   | '_ \             //
//             |  __/| (_| || (_| ||  __/ ___) || |_) |_ | | | |            //
//             |_|    \__,_| \__, | \___||____/ |____/(_)|_| |_|            //
//                           |___/                                          //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//       Copyright (c) 1998-2016 by S.F.T. Inc. - All rights reserved       //
//  Use, copying, and distribution of this software are licensed according  //
//    to the GPLv2, LGPLv2, or BSD license, as appropriate (see COPYING)    //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////


#if !defined(AFX_PAGE3B_H__1514C3AC_ECD7_40F8_BC34_88B131809164__INCLUDED_)
#define AFX_PAGE3B_H__1514C3AC_ECD7_40F8_BC34_88B131809164__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Page3B.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPage3B dialog

class CPage3B : public CPropertyPage
{
  DECLARE_DYNCREATE(CPage3B)

// Construction
public:
  CPage3B();
  ~CPage3B();

  void DoClear();


// Dialog Data
  //{{AFX_DATA(CPage3B)
  enum { IDD = IDD_PAGE_3B };
  CComboBox m_wndIconFiles;
  CListBox  m_wndIconList;
  CListBox  m_wndAppList;
  CString m_csDesc;
  CString m_csFileName;
  int   m_iShow;
  CString m_csParms;
  //}}AFX_DATA

  int m_iCurSel;             // current selection from 'App List'

  int m_iImage;              // curent icon index in 'Icon List' (same as image file)
  CString m_csImageFileName; // name of file from which to extract icon image


  CArray<HICON,HICON> m_aImages;

  void ClearImages();
  void LoadImages();


// Overrides
  // ClassWizard generate virtual function overrides
  //{{AFX_VIRTUAL(CPage3B)
  public:
  virtual BOOL OnKillActive();
  virtual BOOL OnSetActive();
  virtual LRESULT OnWizardBack();
  virtual LRESULT OnWizardNext();
  protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  //}}AFX_VIRTUAL

// Implementation
protected:
  // Generated message map functions
  //{{AFX_MSG(CPage3B)
  afx_msg void OnSelchangeAppList();
  afx_msg void OnKillfocusFilename();
  afx_msg void OnKillfocusDesc();
  afx_msg void OnSelchangeIconList();
  afx_msg void OnCloseupIconFiles();
  afx_msg void OnKillfocusIconFiles();
  afx_msg void OnKillfocusParms();
  afx_msg void OnShowhide();
  afx_msg void OnShowmax();
  afx_msg void OnShowmin();
  afx_msg void OnShownormal();
  afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
  afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()

public:
  afx_msg void OnEnChangeFilename();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGE3B_H__1514C3AC_ECD7_40F8_BC34_88B131809164__INCLUDED_)
