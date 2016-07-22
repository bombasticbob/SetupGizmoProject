/////////////////////////////////////////////////////////////////////////////////////
//                                                                                 //
//   _   _                ____   _         _                                       //
//  | \ | |  __ _   __ _ |  _ \ (_)  __ _ | |  ___    __ _     ___  _ __   _ __    //
//  |  \| | / _` | / _` || | | || | / _` || | / _ \  / _` |   / __|| '_ \ | '_ \   //
//  | |\  || (_| || (_| || |_| || || (_| || || (_) || (_| | _| (__ | |_) || |_) |  //
//  |_| \_| \__,_| \__, ||____/ |_| \__,_||_| \___/  \__, |(_)\___|| .__/ | .__/   //
//                 |___/                             |___/         |_|    |_|      //
//                                                                                 //
/////////////////////////////////////////////////////////////////////////////////////
//                                                                                 //
//           Copyright (c) 1998-2016 by S.F.T. Inc. - All rights reserved          //
//      Use, copying, and distribution of this software are licensed according     //
//        to the GPLv2, LGPLv2, or BSD license, as appropriate (see COPYING)       //
//                                                                                 //
/////////////////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "setupgiz.h"
#include "NagDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNagDialog dialog


CNagDialog::CNagDialog(CWnd* pParent /*=NULL*/)
  : CDialog(CNagDialog::IDD, pParent)
{
  //{{AFX_DATA_INIT(CNagDialog)
  m_csAuthCode = _T("");
  m_csAuthCompany = _T("");
  //}}AFX_DATA_INIT
}


void CNagDialog::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CNagDialog)
  DDX_Text(pDX, IDC_AUTHCODE, m_csAuthCode);
  DDX_Text(pDX, IDC_AUTHCOMPANY, m_csAuthCompany);
  //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNagDialog, CDialog)
  //{{AFX_MSG_MAP(CNagDialog)
  ON_BN_CLICKED(IDNO,OnNo)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNagDialog message handlers

void CNagDialog::OnOK()
{
  UpdateData(1);

  m_csAuthCode.TrimRight();
  m_csAuthCode.TrimLeft();

  m_csAuthCode.MakeUpper();

  m_csAuthCompany.TrimRight();
  m_csAuthCompany.TrimLeft();

  UpdateData(0);

  if(!m_csAuthCode.GetLength())
  {
    AfxMessageBox("A blank authorization code is not allowed",
                  MB_OK | MB_ICONHAND | MB_SETFOREGROUND);
    return;
  }
  else if(!m_csAuthCompany.GetLength())
  {
    AfxMessageBox("You must specify a valid company name.  "
                  "If you do not have a company name, you may enter your own name."
                  "  This entry must NOT be blank!",
                  MB_OK | MB_ICONHAND | MB_SETFOREGROUND);

    return;
  }
  else
  {
    if(theApp.ValidateAuthCode(m_csAuthCode, m_csAuthCompany))
    {
      AfxMessageBox("Thank you for registering the SFT Setup Gizmo!",
                    MB_OK | MB_ICONHAND | MB_SETFOREGROUND);

      theApp.WriteProfileInt("BypassRegistration", "Enable", 1); // bypasses registration check

      // TODO:  calculate the serial number, and include it with the above message

      HKEY hKey = theApp.GetRegistryKey(FALSE);

      if(!hKey)
      {
        AfxMessageBox("ERROR:  unable to register!  Please contact Stewart~Frazier Tools Inc. immediately!",
                      MB_OK | MB_ICONHAND | MB_SETFOREGROUND);

        CDialog::OnCancel();
        return;
      }

      if(RegSetValueEx(hKey, "Company", 0, REG_SZ,
                       (const BYTE *)(LPCSTR)m_csAuthCompany, m_csAuthCompany.GetLength())
         != ERROR_SUCCESS ||
         RegSetValueEx(hKey, "Registration", 0, REG_SZ,
                       (const BYTE *)(LPCSTR)m_csAuthCode, m_csAuthCode.GetLength())
         != ERROR_SUCCESS)
      {
        RegCloseKey(hKey);

        AfxMessageBox("WARNING:  unable to register!  Please log in as a user that has write access to the registry",
                      MB_OK | MB_ICONASTERISK | MB_SETFOREGROUND);

        CDialog::OnCancel();
        return;
      }

      RegCloseKey(hKey);

      CDialog::OnOK();
      return;
    }
  }

  AfxMessageBox("The authorization code is not valid.  Please check your spelling, with special care with respect to '0' (zero) and 'O' (letter 'o')",
                MB_OK | MB_ICONEXCLAMATION | MB_SETFOREGROUND);
}

void CNagDialog::OnCancel()
{
  CDialog::OnCancel();
}

void CNagDialog::OnNo()
{
  AfxMessageBox("You have chosen to bypass registration.  You may only use this software for"
                " evaluation or non-commercial use.  If you wish to register at a later time,"
                " you can show this screen by running Setup Gizmo with the '-reg' flag, as follows:\n\n"
                "    SetupGiz -reg", MB_OK | MB_ICONEXCLAMATION);

  theApp.WriteProfileInt("BypassRegistration", "Enable", 1); // bypasses registration

  CDialog::EndDialog(IDNO);
}
