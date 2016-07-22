//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//            ____                      ____   _            _               //
//           |  _ \  _ __  ___    __ _ |  _ \ | |  __ _    | |__            //
//           | |_) || '__|/ _ \  / _` || | | || | / _` |   | '_ \           //
//           |  __/ | |  | (_) || (_| || |_| || || (_| | _ | | | |          //
//           |_|    |_|   \___/  \__, ||____/ |_| \__, |(_)|_| |_|          //
//                               |___/            |___/                     //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//       Copyright (c) 1998-2016 by S.F.T. Inc. - All rights reserved       //
//  Use, copying, and distribution of this software are licensed according  //
//    to the GPLv2, LGPLv2, or BSD license, as appropriate (see COPYING)    //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// CProgressDialog dialog

//#include "DialogHelpers.h"
#include "Thermom.h"

class CProgressDialog : public CMyDialogBaseClass
{
// Construction
public:
  CProgressDialog(HWND hParent = NULL);

  enum { IDD = IDD_PROGRESS };
  CThermometer m_Progress;
  CString m_csProgressText;
  CString m_csProgress;


  void SetProgressText(UINT uiID)
  {
    CString csText;
    csText.LoadString(uiID);
    SetProgressText((LPCSTR)csText);
  }

  void SetProgressText(LPCSTR szText)
  {
    ::SendMessage(m_hWnd, WM_COMMAND, (WPARAM)0x1234, (LPARAM)szText);
  }

  void SetProgress(UINT uiPercent)
  {
    CString csProgress;
    csProgress.Format("%d", uiPercent); 
    ::SendMessage(m_hWnd, WM_COMMAND, (WPARAM)0x4321, (LPARAM)(LPCSTR)csProgress);
  }


protected:
  void UpdateData(BOOL);

  virtual BOOL MessageHandler(UINT uiMsg, WPARAM wParam, LPARAM lParam);

  BOOL OnCommand(WPARAM wParam, LPARAM lParam);
  BOOL OnInitDialog();

};
