///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                               //
//   ____                                     ____   _                            _  ____   _            _       //
//  |  _ \  ___  _ __ ___    ___ __   __ ___ / ___| | |__    __ _  _ __  ___   __| ||  _ \ | |  __ _    | |__    //
//  | |_) |/ _ \| '_ ` _ \  / _ \\ \ / // _ \\___ \ | '_ \  / _` || '__|/ _ \ / _` || | | || | / _` |   | '_ \   //
//  |  _ <|  __/| | | | | || (_) |\ V /|  __/ ___) || | | || (_| || |  |  __/| (_| || |_| || || (_| | _ | | | |  //
//  |_| \_\\___||_| |_| |_| \___/  \_/  \___||____/ |_| |_| \__,_||_|   \___| \__,_||____/ |_| \__, |(_)|_| |_|  //
//                                                                                             |___/             //
//                                                                                                               //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                               //
//                          Copyright (c) 1998-2016 by S.F.T. Inc. - All rights reserved                         //
//                     Use, copying, and distribution of this software are licensed according                    //
//                       to the GPLv2, LGPLv2, or BSD license, as appropriate (see COPYING)                      //
//                                                                                                               //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_REMOVESHAREDDLG_H__96FF268C_64B2_49B4_A0BE_A74E278A85E6__INCLUDED_)
#define AFX_REMOVESHAREDDLG_H__96FF268C_64B2_49B4_A0BE_A74E278A85E6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RemoveSharedDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRemoveSharedDlg dialog

//#include "DialogHelpers.h"

class CRemoveSharedDlg : public CMyDialogBaseClass
{
// Construction
public:
	CRemoveSharedDlg(HWND hParent = NULL);   // standard constructor

	enum { IDD = IDD_REMOVE_SHARED };
	CString	m_csFileName;
	CString	m_csMessage;

// Implementation
protected:

  virtual BOOL MessageHandler(UINT uiMsg, WPARAM wParam, LPARAM lParam);
  void Update(BOOL bSaveFlag);

	void OnYesToAll();
	virtual void OnCancel();
	void OnNo();
	void OnYes();

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REMOVESHAREDDLG_H__96FF268C_64B2_49B4_A0BE_A74E278A85E6__INCLUDED_)
