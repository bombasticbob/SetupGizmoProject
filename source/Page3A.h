//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//             ____                      _____    _        _                //
//            |  _ \  __ _   __ _   ___ |___ /   / \      | |__             //
//            | |_) |/ _` | / _` | / _ \  |_ \  / _ \     | '_ \            //
//            |  __/| (_| || (_| ||  __/ ___) |/ ___ \  _ | | | |           //
//            |_|    \__,_| \__, | \___||____//_/   \_\(_)|_| |_|           //
//                          |___/                                           //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//       Copyright (c) 1998-2016 by S.F.T. Inc. - All rights reserved       //
//  Use, copying, and distribution of this software are licensed according  //
//    to the GPLv2, LGPLv2, or BSD license, as appropriate (see COPYING)    //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////


#if !defined(AFX_PAGE3A_H__8B06F2C6_0C49_11D2_A115_004033901FF3__INCLUDED_)
#define AFX_PAGE3A_H__8B06F2C6_0C49_11D2_A115_004033901FF3__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// Page3A.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPage3A dialog

class CPage3A : public CPropertyPage
{
	DECLARE_DYNCREATE(CPage3A)

// Construction
public:
	CPage3A();
	~CPage3A();

// Dialog Data
	//{{AFX_DATA(CPage3A)
	enum { IDD = IDD_PAGE_3A };
	CListBox	m_wndOptionList;
	CString	m_csLongDesc;
	CString	m_csDesc;
	//}}AFX_DATA

  int m_iCurSel;

  void DoClear();

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPage3A)
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
	//{{AFX_MSG(CPage3A)
	afx_msg void OnSelchangeOptionList();
	afx_msg void OnKillfocusLongdesc();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGE3A_H__8B06F2C6_0C49_11D2_A115_004033901FF3__INCLUDED_)
