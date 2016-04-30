//////////////////////////////////////////////////////////////////////////////////////
//                                                                                  //
//   _      _                               ____   _                                //
//  | |    (_)  ___  ___  _ __   ___   ___ |  _ \ | |  __ _     ___  _ __   _ __    //
//  | |    | | / __|/ _ \| '_ \ / __| / _ \| | | || | / _` |   / __|| '_ \ | '_ \   //
//  | |___ | || (__|  __/| | | |\__ \|  __/| |_| || || (_| | _| (__ | |_) || |_) |  //
//  |_____||_| \___|\___||_| |_||___/ \___||____/ |_| \__, |(_)\___|| .__/ | .__/   //
//                                                    |___/         |_|    |_|      //
//                                                                                  //
//////////////////////////////////////////////////////////////////////////////////////
//                                                                                  //
//           Copyright (c) 1998-2016 by S.F.T. Inc. - All rights reserved           //
//      Use, copying, and distribution of this software are licensed according      //
//        to the GPLv2, LGPLv2, or BSD license, as appropriate (see COPYING)        //
//                                                                                  //
//////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "setup.h"
#include "LicenseDlg.h"
#include "richedit.h" // for richedit control

//#ifdef _DEBUG
//#define new DEBUG_NEW
//#undef THIS_FILE
//static char THIS_FILE[] = __FILE__;
//#endif

/////////////////////////////////////////////////////////////////////////////
// CLicenseDlg dialog


CLicenseDlg::CLicenseDlg(HWND hParent /*=NULL*/)
	: CMyDialogBaseClass(CLicenseDlg::IDD, hParent)
{
  m_dwRTFPos = 0;

  m_hLib = LoadLibrary("RICHED32.DLL");
}

CLicenseDlg::~CLicenseDlg()
{
  if(m_hLib)
    FreeLibrary(m_hLib);
}

DWORD WINAPI CLicenseDlg::RichTextFormatCallback(DWORD_PTR dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb)
{
  if(!cb || IsBadWritePtr(pbBuff, cb) || IsBadWritePtr(pcb, sizeof(*pcb)))
    return(1);  // an error

  CLicenseDlg *pThis = reinterpret_cast<CLicenseDlg *>(dwCookie);

  if(pThis->m_dwRTFPos >= (unsigned int)pThis->m_csLicense.GetLength())
  {
    *pcb = 0;
    return(0);  // not an error
  }

  if(pThis->m_dwRTFPos + cb > (unsigned int)pThis->m_csLicense.GetLength())
    cb = pThis->m_csLicense.GetLength() - pThis->m_dwRTFPos;

  memcpy(pbBuff, ((LPCSTR)pThis->m_csLicense) + pThis->m_dwRTFPos, cb);
  pThis->m_dwRTFPos += cb;

  *pcb = cb;
  return(0);
}

BOOL CLicenseDlg::MessageHandler(UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
  WORD wCtrl = LOWORD(wParam);
  WORD wNotify = HIWORD(wParam);

  switch(uiMsg)
  {
    case WM_INITDIALOG:
      if(m_csLicense.GetLength() > 5 &&
         !_strnicmp(m_csLicense, "{\\rtf", 5))
      {
        // Assume 'Rich Text Format' and assign text accordingly

        EDITSTREAM es;
        es.dwCookie = reinterpret_cast<DWORD_PTR>(this);
        es.dwError = 0;
        es.pfnCallback = RichTextFormatCallback;

        m_dwRTFPos = 0;
        SendDlgItemMessage(m_hWnd, IDC_LICENSE, EM_STREAMIN, (WPARAM)SF_RTF, (LPARAM)&es);
      }
      else
        SetDlgItemText(IDC_LICENSE, m_csLicense);

      return TRUE;

    case WM_COMMAND:
      switch(wCtrl)
      {
        case IDOK:
        case IDCANCEL:
          if(wNotify == BN_CLICKED)
          {
            EndDialog(wCtrl);
            return TRUE;
          }
          break;
      }
  }

  return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CLicenseDlg message handlers
