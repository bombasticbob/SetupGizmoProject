//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//      _   _                ____   _         _                   _         //
//     | \ | |  __ _   __ _ |  _ \ (_)  __ _ | |  ___    __ _    | |__      //
//     |  \| | / _` | / _` || | | || | / _` || | / _ \  / _` |   | '_ \     //
//     | |\  || (_| || (_| || |_| || || (_| || || (_) || (_| | _ | | | |    //
//     |_| \_| \__,_| \__, ||____/ |_| \__,_||_| \___/  \__, |(_)|_| |_|    //
//                    |___/                             |___/               //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//       Copyright (c) 1998-2016 by S.F.T. Inc. - All rights reserved       //
//  Use, copying, and distribution of this software are licensed according  //
//    to the GPLv2, LGPLv2, or BSD license, as appropriate (see COPYING)    //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////


#if !defined(AFX_NAGDIALOG_H__3217BBFE_4BCC_475E_8FB2_FEE4CD6622DA__INCLUDED_)
#define AFX_NAGDIALOG_H__3217BBFE_4BCC_475E_8FB2_FEE4CD6622DA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NagDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNagDialog dialog

class CNagDialog : public CDialog
{
// Construction
public:
  CNagDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
  //{{AFX_DATA(CNagDialog)
  enum { IDD = IDD_NAG };
  CString m_csAuthCode;
  CString m_csAuthCompany;
  //}}AFX_DATA


// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CNagDialog)
  protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  //}}AFX_VIRTUAL

// Implementation
protected:

  // Generated message map functions
  //{{AFX_MSG(CNagDialog)
  virtual void OnOK();
  virtual void OnCancel();
  virtual void OnNo();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NAGDIALOG_H__3217BBFE_4BCC_475E_8FB2_FEE4CD6622DA__INCLUDED_)
