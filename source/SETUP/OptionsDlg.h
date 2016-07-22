//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//     ___          _    _                     ____   _            _        //
//    / _ \  _ __  | |_ (_)  ___   _ __   ___ |  _ \ | |  __ _    | |__     //
//   | | | || '_ \ | __|| | / _ \ | '_ \ / __|| | | || | / _` |   | '_ \    //
//   | |_| || |_) || |_ | || (_) || | | |\__ \| |_| || || (_| | _ | | | |   //
//    \___/ | .__/  \__||_| \___/ |_| |_||___/|____/ |_| \__, |(_)|_| |_|   //
//          |_|                                          |___/              //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//       Copyright (c) 1998-2016 by S.F.T. Inc. - All rights reserved       //
//  Use, copying, and distribution of this software are licensed according  //
//    to the GPLv2, LGPLv2, or BSD license, as appropriate (see COPYING)    //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////


#if !defined(AFX_OPTIONSDLG_H__B98C4061_9AC1_11D3_A488_004033901FF3__INCLUDED_)
#define AFX_OPTIONSDLG_H__B98C4061_9AC1_11D3_A488_004033901FF3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptionsDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COptionsDlg dialog

//#include "DialogHelpers.h"
#include "OptionListBox.h"

class COptionsDlg : public CMyDialogBaseClass
{
// Construction
public:
  COptionsDlg(CStringArray *pList, const CStringArray *pPreSel);   // standard constructor

  enum { IDD = IDD_OPTIONS };
  COptionListBox m_wndOptionList;
  CString m_csOptionDesc;

  DWORD m_dwHeight;
  CStringArray *m_pList, m_acsDesc;
  const CStringArray *m_pPreSel;

// Implementation
protected:

  void UpdateData(BOOL bSaveFlag);
  BOOL MessageHandler(UINT uiMsg, WPARAM wParam, LPARAM lParam);

  void OnOK();
  void OnCancel();
  void OnSelchangeOptionList();
  BOOL OnInitDialog();
  BOOL OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
  BOOL OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTIONSDLG_H__B98C4061_9AC1_11D3_A488_004033901FF3__INCLUDED_)
