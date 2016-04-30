/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                 //
//   ____                      _     _        _                                     _  ____   _            _       //
//  |  _ \  __ _   __ _   ___ / |   / \    __| |__   __ __ _  _ __    ___  ___   __| ||  _ \ | |  __ _    | |__    //
//  | |_) |/ _` | / _` | / _ \| |  / _ \  / _` |\ \ / // _` || '_ \  / __|/ _ \ / _` || | | || | / _` |   | '_ \   //
//  |  __/| (_| || (_| ||  __/| | / ___ \| (_| | \ V /| (_| || | | || (__|  __/| (_| || |_| || || (_| | _ | | | |  //
//  |_|    \__,_| \__, | \___||_|/_/   \_\\__,_|  \_/  \__,_||_| |_| \___|\___| \__,_||____/ |_| \__, |(_)|_| |_|  //
//                |___/                                                                          |___/             //
//                                                                                                                 //
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                 //
//                           Copyright (c) 1998-2016 by S.F.T. Inc. - All rights reserved                          //
//                      Use, copying, and distribution of this software are licensed according                     //
//                        to the GPLv2, LGPLv2, or BSD license, as appropriate (see COPYING)                       //
//                                                                                                                 //
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#if !defined(AFX_PAGE1ADVANCEDDLG_H__ABA2AEF1_828D_11D3_A484_004033901FF3__INCLUDED_)
#define AFX_PAGE1ADVANCEDDLG_H__ABA2AEF1_828D_11D3_A484_004033901FF3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Page1AdvancedDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPage1AdvancedDlg dialog

class CPage1AdvancedDlg : public CDialog
{
// Construction
public:
	CPage1AdvancedDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPage1AdvancedDlg)
	enum { IDD = IDD_PAGE1_ADVANCED };
	CButton	m_btnRemove;
	CListBox	m_wndUpgrade;
	CString	m_csAppCommon;
	CString	m_csAppPath;
	CString	m_csStartMenu;
	//}}AFX_DATA

  CStringArray m_acsUpgrade;  // list of 'upgrade' things

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPage1AdvancedDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPage1AdvancedDlg)
	virtual void OnOK();
	afx_msg void OnAdd();
	afx_msg void OnRemove();
	afx_msg void OnHelp();
	afx_msg void OnSelchangeUpgrade();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGE1ADVANCEDDLG_H__ABA2AEF1_828D_11D3_A484_004033901FF3__INCLUDED_)
