///////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                   //
//    ____             _    _   __  _              _         ____   _                                //
//   / ___| ___  _ __ | |_ (_) / _|(_)  ___  __ _ | |_  ___ |  _ \ | |  __ _     ___  _ __   _ __    //
//  | |    / _ \| '__|| __|| || |_ | | / __|/ _` || __|/ _ \| | | || | / _` |   / __|| '_ \ | '_ \   //
//  | |___|  __/| |   | |_ | ||  _|| || (__| (_| || |_|  __/| |_| || || (_| | _| (__ | |_) || |_) |  //
//   \____|\___||_|    \__||_||_|  |_| \___|\__,_| \__|\___||____/ |_| \__, |(_)\___|| .__/ | .__/   //
//                                                                     |___/         |_|    |_|      //
//                                                                                                   //
///////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                   //
//                   Copyright (c) 1998-2016 by S.F.T. Inc. - All rights reserved                    //
//               Use, copying, and distribution of this software are licensed according              //
//                 to the GPLv2, LGPLv2, or BSD license, as appropriate (see COPYING)                //
//                                                                                                   //
///////////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "setup.h"
#include "CertificateDlg.h"

//#ifdef _DEBUG
//#define new DEBUG_NEW
//#undef THIS_FILE
//static char THIS_FILE[] = __FILE__;
//#endif

/////////////////////////////////////////////////////////////////////////////
// CCertificateDlg dialog


CCertificateDlg::CCertificateDlg(HWND hParent /*=NULL*/)
  : CMyDialogBaseClass(CCertificateDlg::IDD, hParent)
{
}

CCertificateDlg::~CCertificateDlg()
{
}

static CString CreateDialogTextFromCertInfo(CString &csCertInfo)
{
CStringArray acsTemp;
CString csTemp2;
int i1;

  DoParseString(csCertInfo, acsTemp); // parse into columns

  if(acsTemp.GetSize() <= 0)
  {
    return "";
  }

  if(acsTemp.GetSize() < 3)
  {
    if(acsTemp.GetSize() < 2)
    {
      acsTemp.SetAtGrow(1, acsTemp[0]);
    }

    acsTemp.SetAtGrow(2, "0");
  }

  csTemp2 = acsTemp[2];
  csTemp2.TrimRight();
  csTemp2.TrimLeft();

  if(csTemp2.GetLength() &&
      csTemp2[0] >= '0' &&
      csTemp2[0] <= '9')
  {
    i1 = atoi(csTemp2);
  }
  else
  {
    i1 = -1;
  }

  if(i1 == 0)
  {
    csTemp2 = "ROOT CA";
  }
  else if(i1 == 1)
  {
    csTemp2 = "Trusted Publ.";
  }

  return acsTemp[1] + '\t' + csTemp2;
}

BOOL CCertificateDlg::MessageHandler(UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
WORD wCtrl = LOWORD(wParam);
WORD wNotify = HIWORD(wParam);
CString csTemp, csTemp2;
int i1, i2;

  switch(uiMsg)
  {
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

      break;

    case WM_INITDIALOG:
      // build list of certs
      i1 = 110; // control is 220 dialog units wide; this is ~1/2 of the way across
      SendDlgItemMessage(m_hWnd, IDC_CERTLIST, LB_SETTABSTOPS, 1, (LPARAM)&i1);

      i2 = theApp.GetCertFileCount();
      for(i1=0; i1 < i2; i1++)
      {
        csTemp = theApp.GetCertFileEntry(i1);
        if(csTemp.GetLength())
        {
          csTemp2 = CreateDialogTextFromCertInfo(csTemp);

          SendDlgItemMessage(m_hWnd, IDC_CERTLIST, LB_ADDSTRING, 0,
                             (LPARAM)((LPCSTR)csTemp2));
        }
      }

      return TRUE;
  }

  return FALSE;
}

