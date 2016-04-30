//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//       ____   _____  _____  _   _  ____   ____   _            _           //
//      / ___| | ____||_   _|| | | ||  _ \ |  _ \ | |  __ _    | |__        //
//      \___ \ |  _|    | |  | | | || |_) || | | || | / _` |   | '_ \       //
//       ___) || |___   | |  | |_| ||  __/ | |_| || || (_| | _ | | | |      //
//      |____/ |_____|  |_|   \___/ |_|    |____/ |_| \__, |(_)|_| |_|      //
//                                                    |___/                 //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//       Copyright (c) 1998-2016 by S.F.T. Inc. - All rights reserved       //
//  Use, copying, and distribution of this software are licensed according  //
//    to the GPLv2, LGPLv2, or BSD license, as appropriate (see COPYING)    //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_SETUPDLG_H__B3134EEB_0D1F_11D2_A115_004033901FF3__INCLUDED_)
#define AFX_SETUPDLG_H__B3134EEB_0D1F_11D2_A115_004033901FF3__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// CSETUPDlg dialog

//#include "DialogHelpers.h"

class CSETUPDlg : public CMyDialogBaseClass
{
// Construction
public:
	CSETUPDlg(HWND hParent = NULL);	// standard constructor

	enum { IDD = IDD_SETUP_DIALOG };
	CString	m_csWelcome;
  CString m_csTitle;  // dialog box title


// Implementation
protected:
	HICON m_hIcon;

  void UpdateData(BOOL bSaveFlag);
  BOOL MessageHandler(UINT uiMsg, WPARAM wParam, LPARAM lParam);

	BOOL OnInitDialog();
	BOOL OnPaint();
	HCURSOR OnQueryDragIcon();

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETUPDLG_H__B3134EEB_0D1F_11D2_A115_004033901FF3__INCLUDED_)
