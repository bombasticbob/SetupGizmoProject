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


#if !defined(AFX_LICENSEDLG_H__B3134F02_0D1F_11D2_A115_004033901FF3__INCLUDED_)
#define AFX_LICENSEDLG_H__B3134F02_0D1F_11D2_A115_004033901FF3__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// LicenseDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLicenseDlg dialog

//#include "DialogHelpers.h"

class CLicenseDlg : public CMyDialogBaseClass
{
// Construction
public:
	CLicenseDlg(HWND hParent = NULL);   // standard constructor
  ~CLicenseDlg();

	enum { IDD = IDD_LICENSE };
	CString	m_csLicense;
  DWORD m_dwRTFPos;  // used while reading RTF text for license

  HMODULE m_hLib;  // library handle for RICHED32.DLL (valid on '95 and later)

  virtual BOOL MessageHandler(UINT, WPARAM, LPARAM);

protected:
  static DWORD WINAPI RichTextFormatCallback(DWORD_PTR dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LICENSEDLG_H__B3134F02_0D1F_11D2_A115_004033901FF3__INCLUDED_)
