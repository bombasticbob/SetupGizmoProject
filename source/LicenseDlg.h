//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//     _      _                               ____   _            _         //
//    | |    (_)  ___  ___  _ __   ___   ___ |  _ \ | |  __ _    | |__      //
//    | |    | | / __|/ _ \| '_ \ / __| / _ \| | | || | / _` |   | '_ \     //
//    | |___ | || (__|  __/| | | |\__ \|  __/| |_| || || (_| | _ | | | |    //
//    |_____||_| \___|\___||_| |_||___/ \___||____/ |_| \__, |(_)|_| |_|    //
//                                                      |___/               //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//       Copyright (c) 1998-2016 by S.F.T. Inc. - All rights reserved       //
//  Use, copying, and distribution of this software are licensed according  //
//    to the GPLv2, LGPLv2, or BSD license, as appropriate (see COPYING)    //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////


#pragma once
#include "afxcmn.h"

// CLicenseDlg dialog

class CLicenseDlg : public CDialog
{
	DECLARE_DYNAMIC(CLicenseDlg)

public:
	CLicenseDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CLicenseDlg();

// Dialog Data
	enum { IDD = IDD_LICENSE };

  CString m_csLicense;
  HMODULE m_hLib;  // for RICHED32.DLL
  DWORD m_dwRTFPos;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

  static DWORD WINAPI RichTextFormatCallback(DWORD_PTR dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb);

	DECLARE_MESSAGE_MAP()
public:
  virtual BOOL OnInitDialog();
  CRichEditCtrl m_wndLicense;
};
