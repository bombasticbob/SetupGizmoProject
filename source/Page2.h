//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//                 ____                      ____      _                    //
//                |  _ \  __ _   __ _   ___ |___ \    | |__                 //
//                | |_) |/ _` | / _` | / _ \  __) |   | '_ \                //
//                |  __/| (_| || (_| ||  __/ / __/  _ | | | |               //
//                |_|    \__,_| \__, | \___||_____|(_)|_| |_|               //
//                              |___/                                       //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//       Copyright (c) 1998-2016 by S.F.T. Inc. - All rights reserved       //
//  Use, copying, and distribution of this software are licensed according  //
//    to the GPLv2, LGPLv2, or BSD license, as appropriate (see COPYING)    //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_PAGE2_H__8B06F2C3_0C49_11D2_A115_004033901FF3__INCLUDED_)
#define AFX_PAGE2_H__8B06F2C3_0C49_11D2_A115_004033901FF3__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// Page2.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPage2 dialog

class CSetupGizDlg;

#include "RepeatButton.h"

class CPage2 : public CPropertyPage
{
	DECLARE_DYNCREATE(CPage2)

// Construction
public:
	CPage2();
	~CPage2();

// Dialog Data
	//{{AFX_DATA(CPage2)
	enum { IDD = IDD_PAGE_2 };
	CRepeatButton	m_btnMoveUp;
	CRepeatButton	m_btnMoveDown;
	CListBox	m_wndSourceList;
	CListBox	m_wndDestList;
	BOOL	m_bShowDirs;
	//}}AFX_DATA

  void DoClear();

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPage2)
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

  void FillSourceListBox(CSetupGizDlg *pSheet, LPCSTR szPath);
  BOOL SynchronizeDestFiles();

	// Generated message map functions
	//{{AFX_MSG(CPage2)
	afx_msg void OnGozinta();
	afx_msg void OnGozouta();
	afx_msg void OnMoveDown();
	afx_msg void OnMoveUp();
	afx_msg void OnDblclkSourceList();
	afx_msg void OnShowdirs();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGE2_H__8B06F2C3_0C49_11D2_A115_004033901FF3__INCLUDED_)
