///////////////////////////////////////////////////////////////////////////////////////////
//                                                                                       //
//    ____             _    _   __  _              _         ____   _            _       //
//   / ___| ___  _ __ | |_ (_) / _|(_)  ___  __ _ | |_  ___ |  _ \ | |  __ _    | |__    //
//  | |    / _ \| '__|| __|| || |_ | | / __|/ _` || __|/ _ \| | | || | / _` |   | '_ \   //
//  | |___|  __/| |   | |_ | ||  _|| || (__| (_| || |_|  __/| |_| || || (_| | _ | | | |  //
//   \____|\___||_|    \__||_||_|  |_| \___|\__,_| \__|\___||____/ |_| \__, |(_)|_| |_|  //
//                                                                     |___/             //
//                                                                                       //
///////////////////////////////////////////////////////////////////////////////////////////
//                                                                                       //
//             Copyright (c) 1998-2016 by S.F.T. Inc. - All rights reserved              //
//         Use, copying, and distribution of this software are licensed according        //
//           to the GPLv2, LGPLv2, or BSD license, as appropriate (see COPYING)          //
//                                                                                       //
///////////////////////////////////////////////////////////////////////////////////////////


#if !defined(AFX_CERTIFICATEDLG_H__B3134F02_0D1F_11D2_A115_004033901FF3__INCLUDED_)
#define AFX_CERTIFICATEDLG_H__B3134F02_0D1F_11D2_A115_004033901FF3__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// CertificateDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCertificateDlg dialog

//#include "DialogHelpers.h"

class CCertificateDlg : public CMyDialogBaseClass
{
// Construction
public:
  CCertificateDlg(HWND hParent = NULL);   // standard constructor
  ~CCertificateDlg();

  enum { IDD = IDD_CERTIFICATE };


protected:
  virtual BOOL MessageHandler(UINT uiMsg, WPARAM wParam, LPARAM lParam);

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CERTIFICATEDLG_H__B3134F02_0D1F_11D2_A115_004033901FF3__INCLUDED_)
