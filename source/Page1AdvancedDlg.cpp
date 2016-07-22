/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                             //
//   ____                      _     _        _                                     _  ____   _                                //
//  |  _ \  __ _   __ _   ___ / |   / \    __| |__   __ __ _  _ __    ___  ___   __| ||  _ \ | |  __ _     ___  _ __   _ __    //
//  | |_) |/ _` | / _` | / _ \| |  / _ \  / _` |\ \ / // _` || '_ \  / __|/ _ \ / _` || | | || | / _` |   / __|| '_ \ | '_ \   //
//  |  __/| (_| || (_| ||  __/| | / ___ \| (_| | \ V /| (_| || | | || (__|  __/| (_| || |_| || || (_| | _| (__ | |_) || |_) |  //
//  |_|    \__,_| \__, | \___||_|/_/   \_\\__,_|  \_/  \__,_||_| |_| \___|\___| \__,_||____/ |_| \__, |(_)\___|| .__/ | .__/   //
//                |___/                                                                          |___/         |_|    |_|      //
//                                                                                                                             //
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                             //
//                                 Copyright (c) 1998-2016 by S.F.T. Inc. - All rights reserved                                //
//                            Use, copying, and distribution of this software are licensed according                           //
//                              to the GPLv2, LGPLv2, or BSD license, as appropriate (see COPYING)                             //
//                                                                                                                             //
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "setupgiz.h"
#include "Page1AdvancedDlg.h"
#include "InputDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPage1AdvancedDlg dialog


CPage1AdvancedDlg::CPage1AdvancedDlg(CWnd* pParent /*=NULL*/)
  : CDialog(CPage1AdvancedDlg::IDD, pParent)
{
  //{{AFX_DATA_INIT(CPage1AdvancedDlg)
  m_csAppCommon = _T("");
  m_csAppPath = _T("");
  m_csStartMenu = _T("");
  //}}AFX_DATA_INIT
}


void CPage1AdvancedDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CPage1AdvancedDlg)
  DDX_Control(pDX, IDC_REMOVE, m_btnRemove);
  DDX_Control(pDX, IDC_UPGRADE, m_wndUpgrade);
  DDX_Text(pDX, IDC_APPCOMMON, m_csAppCommon);
  DDX_Text(pDX, IDC_APPPATH, m_csAppPath);
  DDX_Text(pDX, IDC_STARTMENU, m_csStartMenu);
  //}}AFX_DATA_MAP

  m_btnRemove.EnableWindow(m_wndUpgrade.GetCurSel() != LB_ERR);
}


BEGIN_MESSAGE_MAP(CPage1AdvancedDlg, CDialog)
  //{{AFX_MSG_MAP(CPage1AdvancedDlg)
  ON_BN_CLICKED(IDC_ADD, OnAdd)
  ON_BN_CLICKED(IDC_REMOVE, OnRemove)
  ON_BN_CLICKED(IDHELP, OnHelp)
  ON_LBN_SELCHANGE(IDC_UPGRADE, OnSelchangeUpgrade)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPage1AdvancedDlg message handlers

BOOL CPage1AdvancedDlg::OnInitDialog() 
{
  CDialog::OnInitDialog();

  int i1;
  for(i1=0; i1 < m_acsUpgrade.GetSize(); i1++)
  {
    m_wndUpgrade.AddString(m_acsUpgrade[i1]);
  }

  m_wndUpgrade.SetCurSel(0);  // just do it and ignore errors

  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
}

void CPage1AdvancedDlg::OnOK() 
{
  UpdateData(1);

  CString csUpper1 = m_csAppCommon;
  csUpper1.MakeUpper();
  CString csUpper2 = m_csAppPath;
  csUpper2.MakeUpper();

  if(strstr(csUpper1, "%APPCOMMON%"))
  {
    AfxMessageBox("ERROR:  'App Common' cannot contain a reference to itself",
                  MB_OK | MB_ICONHAND | MB_SETFOREGROUND);
    return;
  }
  else if(strstr(csUpper2,"%APPPATH%") ||
          strstr(csUpper2,"%APPCOMMON%"))
  {
    AfxMessageBox("ERROR:  'App Path' cannot contain a reference to itself nor 'App Common'",
                  MB_OK | MB_ICONHAND | MB_SETFOREGROUND);
    return;
  }


  m_acsUpgrade.RemoveAll();

  int i1, i2 = m_wndUpgrade.GetCount();
  CString csTemp;

  for(i1=0; i1 < i2; i1++)
  {
    m_wndUpgrade.GetText(i1, csTemp);

    if(csTemp.GetLength())
      m_acsUpgrade.Add(csTemp);
  }


  CDialog::OnOK();
}

void CPage1AdvancedDlg::OnAdd() 
{
  UpdateData(1);

  CInputDlg dlg(this);

  dlg.m_csCaption = "Enter the name of the application to upgrade from";
  dlg.m_csTitle = "Upgrading from a previously installed application";

  if(dlg.DoModal() == IDOK)
  {
    CString csTemp;

    dlg.m_csInput.TrimRight();
    dlg.m_csInput.TrimLeft();  // make sure, no lead/trail spaces

    if(!dlg.m_csInput.GetLength())
    {
      AfxMessageBox("The specified value was BLANK!  A blank value is not allowed.");
      return;
    }

    int i1;
    for(i1=0; i1 < m_wndUpgrade.GetCount(); i1++)
    {
      m_wndUpgrade.GetText(i1, csTemp);

      if(!csTemp.CompareNoCase(dlg.m_csInput))
      {
        if(csTemp.CompareNoCase(dlg.m_csInput))
          AfxMessageBox("The specified value already exists in the list box.  Please observe that these values are NOT case-sensitive.");
        else
          AfxMessageBox("The specified value already exists in the list box.");

        return;
      }
    }

    i1 = m_wndUpgrade.AddString(dlg.m_csInput);
    m_wndUpgrade.SetCurSel(i1);

    UpdateData(1);
  }

  UpdateData(0);  // by convention
}

void CPage1AdvancedDlg::OnRemove() 
{
  UpdateData(1);

  int i1 = m_wndUpgrade.GetCurSel();

  if(i1 != LB_ERR)
  {
    m_wndUpgrade.DeleteString(i1);

    if(i1 > 0)
      m_wndUpgrade.SetCurSel(i1 - 1);
    else
      m_wndUpgrade.SetCurSel(0);
  }

  UpdateData(0);
}

void CPage1AdvancedDlg::OnHelp() 
{
  UpdateData(1);

  CDialog::OnHelp();
}

void CPage1AdvancedDlg::OnSelchangeUpgrade() 
{
  UpdateData(1);
}

