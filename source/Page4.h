//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//                 ____                      _  _      _                    //
//                |  _ \  __ _   __ _   ___ | || |    | |__                 //
//                | |_) |/ _` | / _` | / _ \| || |_   | '_ \                //
//                |  __/| (_| || (_| ||  __/|__   _|_ | | | |               //
//                |_|    \__,_| \__, | \___|   |_| (_)|_| |_|               //
//                              |___/                                       //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//       Copyright (c) 1998-2016 by S.F.T. Inc. - All rights reserved       //
//  Use, copying, and distribution of this software are licensed according  //
//    to the GPLv2, LGPLv2, or BSD license, as appropriate (see COPYING)    //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////


#if !defined(AFX_PAGE4_H__8B06F2C7_0C49_11D2_A115_004033901FF3__INCLUDED_)
#define AFX_PAGE4_H__8B06F2C7_0C49_11D2_A115_004033901FF3__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// Page4.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPage4 dialog

class CPage4 : public CPropertyPage
{
	DECLARE_DYNCREATE(CPage4)

// Construction
public:
	CPage4();
	~CPage4();

// Dialog Data
	//{{AFX_DATA(CPage4)
	enum { IDD = IDD_PAGE_4 };
	CButton	m_btnRevert;
	CEdit	m_wndValue;
	CEdit	m_wndString;
	CEdit	m_wndKey;
	CListBox	m_wndEntryList;
	CString	m_csValue;
	CString	m_csString;
	CString	m_csKey;
	//}}AFX_DATA

  int m_iCurSel;

  void DoClear();

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPage4)
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
	//{{AFX_MSG(CPage4)
	afx_msg void OnAddNew();
	afx_msg void OnSelchangeEntryList();
	afx_msg void OnMiniHelp();
	afx_msg void OnRemove();
	afx_msg void OnRevert();
	afx_msg void OnChangeKey();
	afx_msg void OnChangeString();
	afx_msg void OnChangeValue();
	afx_msg void OnKeyHelp();
	afx_msg void OnAddStandard();
	afx_msg void OnImportReg();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGE4_H__8B06F2C7_0C49_11D2_A115_004033901FF3__INCLUDED_)
