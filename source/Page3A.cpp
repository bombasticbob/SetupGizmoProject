//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//       ____                      _____    _                               //
//      |  _ \  __ _   __ _   ___ |___ /   / \       ___  _ __   _ __       //
//      | |_) |/ _` | / _` | / _ \  |_ \  / _ \     / __|| '_ \ | '_ \      //
//      |  __/| (_| || (_| ||  __/ ___) |/ ___ \  _| (__ | |_) || |_) |     //
//      |_|    \__,_| \__, | \___||____//_/   \_\(_)\___|| .__/ | .__/      //
//                    |___/                              |_|    |_|         //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//       Copyright (c) 1998-2016 by S.F.T. Inc. - All rights reserved       //
//  Use, copying, and distribution of this software are licensed according  //
//    to the GPLv2, LGPLv2, or BSD license, as appropriate (see COPYING)    //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "setupgiz.h"
#include "Page3A.h"
#include "SetupGizDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPage3A property page

IMPLEMENT_DYNCREATE(CPage3A, CPropertyPage)

CPage3A::CPage3A() : CPropertyPage(CPage3A::IDD)
{
  //{{AFX_DATA_INIT(CPage3A)
  m_csLongDesc = _T("");
  m_csDesc = _T("");
  //}}AFX_DATA_INIT

  DoClear();
}

CPage3A::~CPage3A()
{
}

void CPage3A::DoClear()
{
  m_csLongDesc = _T("");
  m_csDesc = _T("");

  m_iCurSel = LB_ERR;
}

void CPage3A::DoDataExchange(CDataExchange* pDX)
{
  CPropertyPage::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CPage3A)
  DDX_Control(pDX, IDC_OPTION_LIST, m_wndOptionList);
  DDX_Text(pDX, IDC_LONGDESC, m_csLongDesc);
  DDX_Text(pDX, IDC_DESC, m_csDesc);
  //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPage3A, CPropertyPage)
  //{{AFX_MSG_MAP(CPage3A)
  ON_LBN_SELCHANGE(IDC_OPTION_LIST, OnSelchangeOptionList)
  ON_EN_KILLFOCUS(IDC_LONGDESC, OnKillfocusLongdesc)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPage3A message handlers

BOOL CPage3A::OnSetActive() 
{
  CSetupGizDlg *pSheet = CSetupGizDlg::GetPropertySheet(this);

  if(pSheet)
  {
    pSheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);
  }

  UpdateData(0);

  // fill list box

  int iOldSel = m_wndOptionList.GetCurSel();  // just in case

  m_wndOptionList.ResetContent();

  int i1;
  for(i1=0; i1 < pSheet->m_aGroupList.GetSize(); i1++)
  {
    m_wndOptionList.InsertString(i1, pSheet->m_aGroupList[i1].csGroupName);
  }

  UpdateData(0);  // do it again to get data items squared away

  if(iOldSel < 0 && m_wndOptionList.GetCount())
    iOldSel = 0;

  m_wndOptionList.SetCurSel(iOldSel);

  m_iCurSel = LB_ERR;  // forces a re-load
  OnSelchangeOptionList();




  return CPropertyPage::OnSetActive();
}

BOOL CPage3A::OnKillActive() 
{
  OnSelchangeOptionList();  // this updates data properly
  
  return CPropertyPage::OnKillActive();
}

LRESULT CPage3A::OnWizardBack() 
{
  OnSelchangeOptionList();  // this updates data properly
  
  return CPropertyPage::OnWizardBack();
}

LRESULT CPage3A::OnWizardNext() 
{
  OnSelchangeOptionList();  // this updates data properly

  int i1;

  CSetupGizDlg *pSheet = CSetupGizDlg::GetPropertySheet(this);

  CPropertyPage::OnWizardNext();

  if(pSheet)
  {
    for(i1=0; i1 < pSheet->m_aFileList.GetSize(); i1++)
    {
      if(pSheet->m_aFileList[i1].bAddShortcut)
      {
        return IDD_PAGE_3B; // 3B - shortcuts
      }
    }

    if(pSheet->m_acsCertFileNameList.GetSize())
    {
      return IDD_PAGE_3_CERT; // 3 Cert - certificates
    }
  }

  return IDD_PAGE_4;  // skipping 3B and cert

}

void CPage3A::OnSelchangeOptionList() 
{
  CSetupGizDlg *pSheet = CSetupGizDlg::GetPropertySheet(this);

  if(!pSheet)
  {
    return;  // for now, just bail
  }

  int iOldSel = m_iCurSel;

  UpdateData(1);

  if(iOldSel >= 0 && iOldSel < pSheet->m_aGroupList.GetSize())
  {
    pSheet->m_aGroupList[iOldSel].csDesc = m_csDesc;
    pSheet->m_aGroupList[iOldSel].csLongDesc = m_csLongDesc;
  }
    
  // now, load new values (when applicable)

  int iSel = m_wndOptionList.GetCurSel();

  if(iSel != iOldSel &&
     iSel >= 0 && iSel < pSheet->m_aGroupList.GetSize())
  {
    m_csDesc = pSheet->m_aGroupList[iSel].csDesc;
    m_csLongDesc = pSheet->m_aGroupList[iSel].csLongDesc;

    UpdateData(0);
  }

  m_iCurSel = iSel;  // *MUST* update current selection!
}

void CPage3A::OnKillfocusLongdesc() 
{
  UpdateData(1);
}
