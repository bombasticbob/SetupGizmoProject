//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//     ____                      _____   ____             _       _         //
//    |  _ \  __ _   __ _   ___ |___ /  / ___| ___  _ __ | |_    | |__      //
//    | |_) |/ _` | / _` | / _ \  |_ \ | |    / _ \| '__|| __|   | '_ \     //
//    |  __/| (_| || (_| ||  __/ ___) || |___|  __/| |   | |_  _ | | | |    //
//    |_|    \__,_| \__, | \___||____/  \____|\___||_|    \__|(_)|_| |_|    //
//                  |___/                                                   //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//       Copyright (c) 1998-2016 by S.F.T. Inc. - All rights reserved       //
//  Use, copying, and distribution of this software are licensed according  //
//    to the GPLv2, LGPLv2, or BSD license, as appropriate (see COPYING)    //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////


#ifndef _PAGE3CERT_H_INCLUDED_
#define _PAGE3CERT_H_INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// Page3Cert.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPage3A dialog

class CPage3Cert : public CPropertyPage
{
	DECLARE_DYNCREATE(CPage3Cert)

// Construction
public:
	CPage3Cert();
	~CPage3Cert();

// Dialog Data
	//{{AFX_DATA(CPage3A)
	enum { IDD = IDD_PAGE_3_CERT };
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
	afx_msg void OnSelchangeCertList();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	CListBox	m_wndCertList;
  CComboBox m_wndRepository;
  CString m_csRepository;
  afx_msg void OnSelchangeRepository();
  afx_msg void OnEditupdateRepository();
  afx_msg void OnKillfocusRepository();
  afx_msg void OnEditchangeRepository();
  afx_msg void OnCloseupRepository();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // _PAGE3CERT_H_INCLUDED_
