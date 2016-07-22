//////////////////////////////////////////////////////////////////////////////////////
//                                                                                  //
//   ____                      _____   ____             _                           //
//  |  _ \  __ _   __ _   ___ |___ /  / ___| ___  _ __ | |_     ___  _ __   _ __    //
//  | |_) |/ _` | / _` | / _ \  |_ \ | |    / _ \| '__|| __|   / __|| '_ \ | '_ \   //
//  |  __/| (_| || (_| ||  __/ ___) || |___|  __/| |   | |_  _| (__ | |_) || |_) |  //
//  |_|    \__,_| \__, | \___||____/  \____|\___||_|    \__|(_)\___|| .__/ | .__/   //
//                |___/                                             |_|    |_|      //
//                                                                                  //
//////////////////////////////////////////////////////////////////////////////////////
//                                                                                  //
//           Copyright (c) 1998-2016 by S.F.T. Inc. - All rights reserved           //
//      Use, copying, and distribution of this software are licensed according      //
//        to the GPLv2, LGPLv2, or BSD license, as appropriate (see COPYING)        //
//                                                                                  //
//////////////////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "setupgiz.h"
#include "Page3Cert.h"
#include "SetupGizDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPage3Cert property page

IMPLEMENT_DYNCREATE(CPage3Cert, CPropertyPage)

CPage3Cert::CPage3Cert() : CPropertyPage(CPage3Cert::IDD)
{
  //{{AFX_DATA_INIT(CPage3Cert)
  m_csRepository = _T("");
  //}}AFX_DATA_INIT

  DoClear();
}

CPage3Cert::~CPage3Cert()
{
}

void CPage3Cert::DoClear()
{
  m_csRepository = _T("");

  m_iCurSel = LB_ERR;
}

void CPage3Cert::DoDataExchange(CDataExchange* pDX)
{
  CPropertyPage::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CPage3Cert)
  DDX_Control(pDX, IDC_CERT_LIST, m_wndCertList);
  DDX_Control(pDX, IDC_REPOSITORY, m_wndRepository);
  DDX_CBString(pDX, IDC_REPOSITORY, m_csRepository);
  //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPage3Cert, CPropertyPage)
  //{{AFX_MSG_MAP(CPage3Cert)
  ON_LBN_SELCHANGE(IDC_CERT_LIST, OnSelchangeCertList)
  //}}AFX_MSG_MAP
  ON_CBN_SELCHANGE(IDC_REPOSITORY, &CPage3Cert::OnSelchangeRepository)
  ON_CBN_EDITUPDATE(IDC_REPOSITORY, &CPage3Cert::OnEditupdateRepository)
  ON_CBN_KILLFOCUS(IDC_REPOSITORY, &CPage3Cert::OnKillfocusRepository)
  ON_CBN_EDITCHANGE(IDC_REPOSITORY, &CPage3Cert::OnEditchangeRepository)
  ON_CBN_CLOSEUP(IDC_REPOSITORY, &CPage3Cert::OnCloseupRepository)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPage3Cert message handlers

static const char * const aszRepositories[] =
{
  "ROOT","CA","TrustedPublisher","TrustedDevices","AuthRoot"
};

BOOL CPage3Cert::OnSetActive() 
{
  CSetupGizDlg *pSheet = CSetupGizDlg::GetPropertySheet(this);

  if(pSheet)
  {
    pSheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);
  }

  UpdateData(0);

  // fill list box

  int iOldSel = m_wndCertList.GetCurSel();  // just in case

  m_wndCertList.ResetContent();
  m_wndRepository.ResetContent();

  int i1, i2, iExtent, iTab;
  CString csTemp, csTemp2;

  iTab = 60;
  iExtent = (LOWORD(::GetDialogBaseUnits()) * iTab) / 4;

  CDC *pDC = GetDC(); // DC of this window

  if(!pDC)
  {
    iExtent += LOWORD(::GetDialogBaseUnits()) * 100; // use a default of 100 dialog units
  }

  for(i1=0; pSheet && i1 < pSheet->m_acsCertFileNameList.GetSize(); i1++)
  {
    csTemp = pSheet->m_acsCertFileNameList[i1];
    csTemp2 = GetCertFileFriendlyName(csTemp);

    if(!csTemp2.GetLength())
    {
      i2 = csTemp.ReverseFind('\\');
      if(i2 >= 0)
      {
        csTemp2 = csTemp.Mid(i2 + 1);
      }
      else
      {
        csTemp2 = csTemp; // no path
      }
    }

    m_wndCertList.InsertString(i1, csTemp2 + '\t' + csTemp);

    if(pDC)
    {
      CSize siz = pDC->GetTextExtent(csTemp);
      i2 = (LOWORD(::GetDialogBaseUnits()) * iTab) / 4 + siz.cx + 20;
      if(i2 > iExtent)
      {
        iExtent = i2;
      }
    }
  }

  m_wndCertList.SendMessage(LB_SETTABSTOPS, 1, (LPARAM)&iTab);
  m_wndCertList.SendMessage(LB_SETHORIZONTALEXTENT, iExtent, 0);

  UpdateData(0);  // do it again to get data items squared away

  if(iOldSel < 0 && m_wndCertList.GetCount())
  {
    iOldSel = 0;
  }

  m_wndCertList.SetCurSel(iOldSel);

  // load the repository list with possible entries
  for(i1=0; i1 < sizeof(aszRepositories) / sizeof(aszRepositories[0]); i1++)
  {
    // must use 'InsertString' so it shows up in the right place
    m_wndRepository.InsertString(i1,aszRepositories[i1]);
  }

  // load the repository list with any custom entries I might have
  for(i1=0; pSheet && i1 < pSheet->m_acsCertRepositoryList.GetSize(); i1++)
  {
    csTemp = pSheet->m_acsCertRepositoryList[i1];

    for(i2=0; i2 < sizeof(aszRepositories) / sizeof(aszRepositories[0]); i2++)
    {
      // TODO:  I believe this is NOT case sensitive - verify?

      if(!csTemp.CompareNoCase(aszRepositories[i2]))
      {
        break;
      }
    }

    if(i2 >= sizeof(aszRepositories) / sizeof(aszRepositories[0])) // not found
    {
      m_wndRepository.AddString(csTemp);
    }
  }

  m_iCurSel = LB_ERR;  // forces a re-load
  OnSelchangeCertList();

  return CPropertyPage::OnSetActive();
}

BOOL CPage3Cert::OnKillActive() 
{
  OnSelchangeCertList();  // this updates data properly
  
  return CPropertyPage::OnKillActive();
}

LRESULT CPage3Cert::OnWizardBack() 
{
  OnSelchangeCertList();  // this updates data properly
  
  int i1;

  CSetupGizDlg *pSheet = CSetupGizDlg::GetPropertySheet(this);

  if(!pSheet)
  {
    AfxMessageBox("Internal error - cannot continue!",
                  MB_OK | MB_ICONHAND | MB_SETFOREGROUND);
    return -1;
  }

  CPropertyPage::OnWizardBack();

  // check for 3B first

  for(i1=0; i1 < pSheet->m_aFileList.GetSize(); i1++)
  {
    if(pSheet->m_aFileList[i1].bAddShortcut)
    {
      return IDD_PAGE_3B; // 3B - shortcuts
    }
  }

  if(pSheet->m_aGroupList.GetSize() > 0)
  {
    return(IDD_PAGE_3A);  // go to 'page 3A'
  }

  return IDD_PAGE_3; // if not 3A or 3B
}

LRESULT CPage3Cert::OnWizardNext() 
{
  OnSelchangeCertList();  // this updates data properly

  CPropertyPage::OnWizardNext();

  return(IDD_PAGE_4);
}

void CPage3Cert::OnSelchangeCertList() 
{
  CSetupGizDlg *pSheet = CSetupGizDlg::GetPropertySheet(this);

  if(!pSheet)
  {
    return;  // for now, just bail
  }

  int iOldSel = m_iCurSel;

  UpdateData(1);

  if(m_iCurSel != LB_ERR)
  {
    // save the current repository name into the appropriate array
    pSheet->m_acsCertRepositoryList.SetAtGrow(m_iCurSel, m_csRepository);
  }

  // now, load new values (when applicable)

  int iSel = m_wndCertList.GetCurSel();

  if(iSel != iOldSel &&
     iSel >= 0 && iSel < pSheet->m_acsCertFileNameList.GetSize())
  {
    if(pSheet->m_acsCertRepositoryList.GetSize() <= iSel ||
       !pSheet->m_acsCertRepositoryList[iSel].GetLength())
    {
      m_csRepository = "ROOT";
    }
    else
    {
      m_csRepository = pSheet->m_acsCertRepositoryList[iSel];
    }

    UpdateData(0);
  }

  m_iCurSel = iSel;  // *MUST* update current selection!
}

void CPage3Cert::OnSelchangeRepository()
{
  UpdateData(1);
}

void CPage3Cert::OnEditupdateRepository()
{
  UpdateData(1);
}

void CPage3Cert::OnKillfocusRepository()
{
int i2;
CString csTemp;


  UpdateData(1);

  CSetupGizDlg *pSheet = CSetupGizDlg::GetPropertySheet(this);

  if(!pSheet)
  {
    return;
  }

  if(!m_csRepository.GetLength())
  {
    m_csRepository = "ROOT";
  }
  else
  {
    // load the repository list with any custom entries I might have
    for(i2=0; i2 < sizeof(aszRepositories) / sizeof(aszRepositories[0]); i2++)
    {
      // TODO:  I believe this is NOT case sensitive - verify?

      if(!m_csRepository.CompareNoCase(aszRepositories[i2]))
      {
        break;
      }
    }

    if(i2 >= sizeof(aszRepositories) / sizeof(aszRepositories[0])) // not found
    {
      m_wndRepository.AddString(m_csRepository);
    }
  }

  UpdateData(0);
}

void CPage3Cert::OnEditchangeRepository()
{
  UpdateData(1);
}

void CPage3Cert::OnCloseupRepository()
{
  UpdateData(1);
}

