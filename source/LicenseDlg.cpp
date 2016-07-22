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
#include "SetupGiz.h"
#include "LicenseDlg.h"


// CLicenseDlg dialog

IMPLEMENT_DYNAMIC(CLicenseDlg, CDialog)
CLicenseDlg::CLicenseDlg(CWnd* pParent /*=NULL*/)
  : CDialog(CLicenseDlg::IDD, pParent)
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

void CLicenseDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CLicenseDlg, CDialog)
END_MESSAGE_MAP()


// CLicenseDlg message handlers

BOOL CLicenseDlg::OnInitDialog()
{
  CDialog::OnInitDialog();

  if(m_csLicense.GetLength() > 5 &&
      !_strnicmp(m_csLicense, "{\\rtf", 5))
  {
    // Assume 'Rich Text Format' and assign text accordingly

    EDITSTREAM es;
    es.dwCookie = reinterpret_cast<DWORD_PTR>(this);
    es.dwError = 0;
    es.pfnCallback = RichTextFormatCallback;

    m_dwRTFPos = 0;
    SendDlgItemMessage(IDC_LICENSE, EM_STREAMIN, (WPARAM)SF_RTF, (LPARAM)&es);
  }
  else
    SetDlgItemText(IDC_LICENSE, m_csLicense);

  return TRUE;  // return TRUE unless you set the focus to a control
  // EXCEPTION: OCX Property Pages should return FALSE
}
