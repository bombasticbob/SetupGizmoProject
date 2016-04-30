////////////////////////////////////////////////////////////////////////////////////
//                                                                                //
//   ____         _                  ____   _         _                   _       //
//  / ___|   ___ | |_  _   _  _ __  |  _ \ (_)  __ _ | |  ___    __ _    | |__    //
//  \___ \  / _ \| __|| | | || '_ \ | | | || | / _` || | / _ \  / _` |   | '_ \   //
//   ___) ||  __/| |_ | |_| || |_) || |_| || || (_| || || (_) || (_| | _ | | | |  //
//  |____/  \___| \__| \__,_|| .__/ |____/ |_| \__,_||_| \___/  \__, |(_)|_| |_|  //
//                           |_|                                |___/             //
//                                                                                //
////////////////////////////////////////////////////////////////////////////////////
//                                                                                //
//          Copyright (c) 1998-2016 by S.F.T. Inc. - All rights reserved          //
//     Use, copying, and distribution of this software are licensed according     //
//       to the GPLv2, LGPLv2, or BSD license, as appropriate (see COPYING)       //
//                                                                                //
////////////////////////////////////////////////////////////////////////////////////


#if !defined(AFX_SETUPDIALOG_H__6A588603_19BF_11D2_A115_004033901FF3__INCLUDED_)
#define AFX_SETUPDIALOG_H__6A588603_19BF_11D2_A115_004033901FF3__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// SetupDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSetupDialog dialog

class CSetupDialog : public CDialog
{
// Construction
public:
	CSetupDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSetupDialog)
	enum { IDD = IDD_SETUP_DIALOG };
	CString	m_csWelcome;
	//}}AFX_DATA

  CString m_csTitle;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSetupDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSetupDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETUPDIALOG_H__6A588603_19BF_11D2_A115_004033901FF3__INCLUDED_)
