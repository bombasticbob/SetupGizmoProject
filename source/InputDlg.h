//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//         ___                       _    ____   _            _             //
//        |_ _| _ __   _ __   _   _ | |_ |  _ \ | |  __ _    | |__          //
//         | | | '_ \ | '_ \ | | | || __|| | | || | / _` |   | '_ \         //
//         | | | | | || |_) || |_| || |_ | |_| || || (_| | _ | | | |        //
//        |___||_| |_|| .__/  \__,_| \__||____/ |_| \__, |(_)|_| |_|        //
//                    |_|                           |___/                   //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//       Copyright (c) 1998-2016 by S.F.T. Inc. - All rights reserved       //
//  Use, copying, and distribution of this software are licensed according  //
//    to the GPLv2, LGPLv2, or BSD license, as appropriate (see COPYING)    //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////


#if !defined(AFX_INPUTDLG_H__C1B94526_A661_4F06_95C5_5C1102258952__INCLUDED_)
#define AFX_INPUTDLG_H__C1B94526_A661_4F06_95C5_5C1102258952__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// InputDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CInputDlg dialog

class CInputDlg : public CDialog
{
// Construction
public:
	CInputDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CInputDlg)
	enum { IDD = IDD_INPUT };
	CString	m_csCaption;
	CString	m_csInput;
	//}}AFX_DATA

  CString m_csTitle;  // string that appears in the title bar

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInputDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CInputDlg)
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnHelp();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INPUTDLG_H__C1B94526_A661_4F06_95C5_5C1102258952__INCLUDED_)
