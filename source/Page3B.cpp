//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//        ____                      _____  ____                             //
//       |  _ \  __ _   __ _   ___ |___ / | __ )    ___  _ __   _ __        //
//       | |_) |/ _` | / _` | / _ \  |_ \ |  _ \   / __|| '_ \ | '_ \       //
//       |  __/| (_| || (_| ||  __/ ___) || |_) |_| (__ | |_) || |_) |      //
//       |_|    \__,_| \__, | \___||____/ |____/(_)\___|| .__/ | .__/       //
//                     |___/                            |_|    |_|          //
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
#include "Page3B.h"
#include "SetupGizDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPage3B property page

IMPLEMENT_DYNCREATE(CPage3B, CPropertyPage)

CPage3B::CPage3B() : CPropertyPage(CPage3B::IDD)
{
  //{{AFX_DATA_INIT(CPage3B)
  m_csDesc = _T("");
  m_csFileName = _T("");
  m_iShow = -1;
  m_csParms = _T("");
  //}}AFX_DATA_INIT

  DoClear();
}

CPage3B::~CPage3B()
{
  ClearImages();
}

void CPage3B::DoClear()
{
  ClearImages();

  m_iCurSel = -1;
  m_iImage = -1;
  m_csImageFileName = "";

  m_csDesc = _T("");
  m_csFileName = _T("");
  m_iShow = 0;
  m_csParms = _T("");
}

void CPage3B::DoDataExchange(CDataExchange* pDX)
{
  CPropertyPage::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CPage3B)
  DDX_Control(pDX, IDC_ICON_FILES, m_wndIconFiles);
  DDX_Control(pDX, IDC_ICON_LIST, m_wndIconList);
  DDX_Control(pDX, IDC_APP_LIST, m_wndAppList);
  DDX_Text(pDX, IDC_DESC, m_csDesc);
  DDX_Text(pDX, IDC_FILENAME, m_csFileName);
  DDX_Radio(pDX, IDC_SHOWNORMAL, m_iShow);
  DDX_Text(pDX, IDC_PARMS, m_csParms);
  //}}AFX_DATA_MAP

  if(pDX->m_bSaveAndValidate)
  {
    m_iCurSel = m_wndAppList.GetCurSel();
    m_iImage = m_wndIconList.GetCurSel();

    int i1 = m_wndIconFiles.GetCurSel();

    if(i1 != CB_ERR && i1 > 0)
    {
      CString csTemp = m_csImageFileName;

      m_wndIconFiles.GetLBText(i1, m_csImageFileName);

      if(!csTemp.GetLength() || csTemp.CompareNoCase(m_csImageFileName))
      {
        // image file name has changed - re-load

        LoadImages();

        if(m_wndIconList.GetCount())
        {
          m_iCurSel = 0;  // since THIS is an 'UpdateData(1)' call, I must re-assign the index
        }
        else
        {
          m_iCurSel = -1;
        }

        m_wndIconList.SetCurSel(m_iCurSel);
      }
    }
    else
    {
      m_csImageFileName = "";

      ClearImages();

      m_wndIconList.ResetContent();
      m_iImage = -1;
    }
  }
  else
  {
    m_wndAppList.SetCurSel(m_iCurSel);

    if(m_csImageFileName.GetLength())
    {
      int i1, iSel, iOldSel = m_wndIconFiles.GetCurSel();
      CString csTemp;

      for(i1=1, iSel = -1; i1 < m_wndIconFiles.GetCount(); i1++)
      {
        m_wndIconFiles.GetLBText(i1, csTemp);

        if(!csTemp.CompareNoCase(m_csImageFileName))
        {
          iSel = i1;
          break;
        }
      }

      m_wndIconFiles.SetCurSel(i1);

      if(iOldSel == LB_ERR || iOldSel != iSel
         || m_wndIconList.GetCount() != m_aImages.GetSize())
      {
        // a mis-match in the image information!

        LoadImages();

        if(m_iImage < 0 || m_iImage > m_wndIconList.GetCount())  // this handles
          m_iImage = 0;
      }
    }
    else
    {
      m_wndIconFiles.SetCurSel(0);  // NONE selected

      ClearImages();

      m_wndIconList.ResetContent();
      m_iImage = -1;
    }

    m_wndIconList.SetCurSel(m_iImage);
  }

}


BEGIN_MESSAGE_MAP(CPage3B, CPropertyPage)
  //{{AFX_MSG_MAP(CPage3B)
  ON_LBN_SELCHANGE(IDC_APP_LIST, OnSelchangeAppList)
  ON_EN_KILLFOCUS(IDC_FILENAME, OnKillfocusFilename)
  ON_EN_KILLFOCUS(IDC_DESC, OnKillfocusDesc)
  ON_LBN_SELCHANGE(IDC_ICON_LIST, OnSelchangeIconList)
  ON_CBN_CLOSEUP(IDC_ICON_FILES, OnCloseupIconFiles)
  ON_CBN_KILLFOCUS(IDC_ICON_FILES, OnKillfocusIconFiles)
  ON_EN_KILLFOCUS(IDC_PARMS, OnKillfocusParms)
  ON_BN_CLICKED(IDC_SHOWHIDE, OnShowhide)
  ON_BN_CLICKED(IDC_SHOWMAX, OnShowmax)
  ON_BN_CLICKED(IDC_SHOWMIN, OnShowmin)
  ON_BN_CLICKED(IDC_SHOWNORMAL, OnShownormal)
  ON_WM_DRAWITEM()
  ON_WM_MEASUREITEM()
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CPage3B::ClearImages()
{
  int i1;
  for(i1=0; i1 < m_aImages.GetSize(); i1++)
  {
    DestroyIcon(m_aImages[i1]);
  }

  m_aImages.RemoveAll();
}


void CPage3B::LoadImages()
{
  ClearImages();

  CSetupGizDlg *pSheet = CSetupGizDlg::GetPropertySheet(this);

  if(!pSheet)
    return;

  m_wndIconList.ResetContent();

  if(!m_csImageFileName.GetLength())
  {
    m_iImage = -1;
    m_wndIconList.SetCurSel(-1);

    return;
  }

  CString csTemp = m_csImageFileName;

  if(m_csImageFileName[0] == '$')
  {
    csTemp = m_csImageFileName.Mid(1);
  }
  else
  {
    csTemp = pSheet->m_Page1.m_csSourcePath;

    if(csTemp.GetLength() && csTemp[csTemp.GetLength() - 1] != '\\')
      csTemp += '\\';

    csTemp += m_csImageFileName;
  }

  UINT nIcons = ExtractIconEx(csTemp, -1, NULL, NULL, 0);

  if(!nIcons)
    return;

  m_aImages.SetSize(nIcons);

  UINT nIcons2 = ExtractIconEx(csTemp, 0, m_aImages.GetData(), NULL, nIcons);

  ASSERT(nIcons2 == nIcons);

  if(nIcons2 < nIcons)
    m_aImages.SetSize(nIcons2);

  // load the window with image information

  for(nIcons = 0; nIcons < nIcons2; nIcons++)
  {
    csTemp.Format("%lu", nIcons);
    csTemp = m_csImageFileName + ":" + csTemp;  // for reference, mostly

    m_wndIconList.AddString(csTemp);
  }
}


/////////////////////////////////////////////////////////////////////////////
// CPage3B message handlers

BOOL CPage3B::OnSetActive() 
{
  CSetupGizDlg *pSheet = CSetupGizDlg::GetPropertySheet(this);

  if(pSheet)
  {
    pSheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);
  }

  UpdateData(0);

  // fill list box

  int iOldSel = m_wndAppList.GetCurSel();  // just in case

  m_wndAppList.ResetContent();
  m_wndIconFiles.ResetContent();

  m_wndIconFiles.AddString("$moricons.dll");  // by default, must have this
  m_wndIconFiles.AddString("$shell32.dll");    // by default, must have this

  CString csTemp;
  int i1, i2;
  for(i1=0; i1 < pSheet->m_aFileList.GetSize(); i1++)
  {
    i2 = pSheet->m_aFileList[i1].csFileName.ReverseFind('\\');

    if(i2 >= 0)
      csTemp = pSheet->m_aFileList[i1].csFileName.Mid(i2 + 1);
    else
      csTemp = pSheet->m_aFileList[i1].csFileName;

    i2 = csTemp.ReverseFind('.');

    if(i2 >= 0)
    {
      csTemp = csTemp.Mid(i2);

      if(!csTemp.CompareNoCase(".ICO") ||
//         !csTemp.CompareNoCase(".ANI") ||
//         !csTemp.CompareNoCase(".CUR") ||
         !csTemp.CompareNoCase(".DLL") ||
         !csTemp.CompareNoCase(".OCX") ||
         !csTemp.CompareNoCase(".MOD") ||
         !csTemp.CompareNoCase(".EXE") ||
         !csTemp.CompareNoCase(".VXD"))
      {
        m_wndIconFiles.AddString(pSheet->m_aFileList[i1].csFileName);
      }
    }

    if(pSheet->m_aFileList[i1].bAddShortcut)
    {
      i2 = m_wndAppList.AddString(pSheet->m_aFileList[i1].csFileName);
      m_wndAppList.SetItemData(i2, (DWORD)i1);  // assign data to the correct index
    }
  }

  m_wndIconFiles.InsertString(0, "{NONE}");  // *MUST* be the first one

  UpdateData(0);  // do it again to get data items squared away

  if(iOldSel < 0 && m_wndAppList.GetCount())
    iOldSel = 0;

  m_wndAppList.SetCurSel(iOldSel);

  m_iCurSel = LB_ERR;      // forces a re-load
  m_iImage = -1;           // same here
  m_csImageFileName = "";  // just to be sure

  OnSelchangeAppList();

  return CPropertyPage::OnSetActive();
}

BOOL CPage3B::OnKillActive() 
{
  OnSelchangeAppList();  // this updates data properly

  return CPropertyPage::OnKillActive();
}

LRESULT CPage3B::OnWizardBack() 
{
  OnSelchangeAppList();  // this updates data properly

  CSetupGizDlg *pSheet = CSetupGizDlg::GetPropertySheet(this);

  if(!pSheet)
  {
    AfxMessageBox("Internal error - cannot continue!",
                  MB_OK | MB_ICONHAND | MB_SETFOREGROUND);
    return -1;
  }

  CPropertyPage::OnWizardBack();

  if(!pSheet->m_aGroupList.GetSize())
  {
    return(IDD_PAGE_3);  // go to 'page 3' and not 'page 3A'
  }

  return IDD_PAGE_3A;
}

LRESULT CPage3B::OnWizardNext() 
{
  OnSelchangeAppList();  // this updates data properly

  CSetupGizDlg *pSheet = CSetupGizDlg::GetPropertySheet(this);

  if(!pSheet)
  {
    AfxMessageBox("Internal error - cannot continue!",
                  MB_OK | MB_ICONHAND | MB_SETFOREGROUND);
    return -1;
  }

  CPropertyPage::OnWizardNext();

  if(pSheet->m_acsCertFileNameList.GetSize())
  {
    return IDD_PAGE_3_CERT; // 3 Cert - certificates
  }
  
  return IDD_PAGE_4; // where to go if there are no certs
}

void CPage3B::OnSelchangeAppList() 
{
  CSetupGizDlg *pSheet = CSetupGizDlg::GetPropertySheet(this);

  if(!pSheet)  // internal error
  {
    return;  // for now, just bail
  }

  int iOldSel = m_iCurSel;
  m_iCurSel = m_wndAppList.GetCurSel();

  UpdateData(1);  // this refreshes all data members

  if(iOldSel != LB_ERR)
  {
    // ASSIGN DATA MEMBERS

    int iMainIndex = m_wndAppList.GetItemData(iOldSel);

    if(iMainIndex >= 0 && iMainIndex < pSheet->m_aFileList.GetSize())  // it better be...
    {
      CString csFileName = m_csFileName;

      int i1 = csFileName.ReverseFind('.');
      int i2 = csFileName.ReverseFind('\\');

      if(i2 >= i1 || i1 < 0) // no extension
        csFileName += ".LNK";  // make sure it gets one!!

      pSheet->m_aFileList[iMainIndex].csShortcutFileName    = csFileName;  // 'cooked' name

      pSheet->m_aFileList[iMainIndex].csShortcutDescription = m_csDesc;
      pSheet->m_aFileList[iMainIndex].csStartPath           = "";  // for now
      pSheet->m_aFileList[iMainIndex].csArgs                = m_csParms;

      pSheet->m_aFileList[iMainIndex].csIconFile            = m_csImageFileName;
      pSheet->m_aFileList[iMainIndex].uiIconIndex           = m_iImage;

      if(m_iShow == 0)
        pSheet->m_aFileList[iMainIndex].iShow = SW_SHOWNORMAL;
      else if(m_iShow == 1)
        pSheet->m_aFileList[iMainIndex].iShow = SW_SHOWMINIMIZED;
      else if(m_iShow == 2)
        pSheet->m_aFileList[iMainIndex].iShow = SW_SHOWMAXIMIZED;
      else if(m_iShow == 3)
        pSheet->m_aFileList[iMainIndex].iShow = SW_HIDE;
      else
        pSheet->m_aFileList[iMainIndex].iShow = SW_SHOWNA;  // for now...
    }
  }

  if(m_iCurSel == LB_ERR)  // just in case
  {
    ClearImages();
    m_iImage = -1;

  }
  else if(m_iCurSel != iOldSel)
  {
    int iMainIndex = m_wndAppList.GetItemData(m_iCurSel);

    if(iMainIndex >= 0 && iMainIndex < pSheet->m_aFileList.GetSize())  // it better be...
    {
      m_csFileName      = pSheet->m_aFileList[iMainIndex].csShortcutFileName;

      int i1 = m_csFileName.ReverseFind('.');
      int i2 = m_csFileName.ReverseFind('\\');

      if(i2 < i1 && i1 >= 0) // it has an extension
      {
        if(!m_csFileName.Mid(i1).CompareNoCase(".LNK"))  // it's a ".LNK"
        {
          m_csFileName = m_csFileName.Left(i1);  // make sure it goes away!!
        }
      }

      m_csDesc          = pSheet->m_aFileList[iMainIndex].csShortcutDescription;
      m_csParms         = pSheet->m_aFileList[iMainIndex].csArgs;

      m_csImageFileName = pSheet->m_aFileList[iMainIndex].csIconFile;
      m_iImage          = pSheet->m_aFileList[iMainIndex].uiIconIndex;

      if(!m_csImageFileName.GetLength())
      {
        m_csImageFileName = pSheet->m_aFileList[iMainIndex].csFileName;

        CString csTemp = m_csImageFileName;
        int i1 = csTemp.ReverseFind('\\');
        if(i1 >= 0)
          csTemp = csTemp.Mid(i1 + 1);

        i1 = csTemp.ReverseFind('.');
        if(i1 < 0)
        {
          m_csImageFileName = "";  // default is *NONE*
          m_iImage = -1;     // force the image to -1 also
        }
        else
        {
          csTemp = csTemp.Mid(i1);

          if(!csTemp.CompareNoCase(".ICO") ||
//             !csTemp.CompareNoCase(".ANI") ||
//             !csTemp.CompareNoCase(".CUR") ||
             !csTemp.CompareNoCase(".EXE") ||
             !csTemp.CompareNoCase(".DLL") ||
             !csTemp.CompareNoCase(".OCX") ||
             !csTemp.CompareNoCase(".MOD") ||
             !csTemp.CompareNoCase(".VXD"))
          {
            // keep it
          }
          else
          {
            m_csImageFileName = "";  // don't keep it
          }
        }
      }


//      pSheet->m_aFileList[iMainIndex].csStartPath

      if(pSheet->m_aFileList[iMainIndex].iShow == SW_SHOWNORMAL)
        m_iShow = 0;
      else if(pSheet->m_aFileList[iMainIndex].iShow == SW_SHOWMINIMIZED)
        m_iShow = 1;
      else if(pSheet->m_aFileList[iMainIndex].iShow == SW_SHOWMAXIMIZED)
        m_iShow = 2;
      else if(pSheet->m_aFileList[iMainIndex].iShow == SW_HIDE)
        m_iShow = 3;
      else
        m_iShow = -1;  // for now...
    }

    UpdateData(0);

    m_wndIconList.SetCurSel(m_iImage);
  }
}

void CPage3B::OnKillfocusFilename() 
{
  UpdateData(1);

  m_csFileName.TrimRight();
  m_csFileName.TrimLeft();

  if(!m_csFileName.GetLength())
  {
    AfxMessageBox("You must specify a shortcut file name.  The default will be now assigned for you.");

    CSetupGizDlg *pSheet = CSetupGizDlg::GetPropertySheet(this);

    if(!pSheet)
    {
      m_csFileName = "INTERNAL_ERROR_1";
    }
    else
    {
      int iMainIndex = m_wndAppList.GetItemData(m_iCurSel);

      if(iMainIndex >= 0 && iMainIndex < pSheet->m_aFileList.GetSize())  // it better be...
      {
        SetupFileInfo sfi;
    
        sfi = pSheet->m_aFileList[iMainIndex];

        GenerateDefaultShortcutInfo(sfi);

        m_csFileName = sfi.csShortcutFileName;
      }
      else
      {
        m_csFileName = "INTERNAL_ERROR_2";
      }
    }

    UpdateData(0);
  }
}

void CPage3B::OnKillfocusDesc() 
{
  UpdateData(1);
}

void CPage3B::OnSelchangeIconList() 
{
  UpdateData(1);
}


void CPage3B::OnCloseupIconFiles() 
{
  OnKillfocusIconFiles();
}

void CPage3B::OnKillfocusIconFiles() 
{
  UpdateData(1);
}

void CPage3B::OnKillfocusParms() 
{
  UpdateData(1);
}

void CPage3B::OnShowhide() 
{
  UpdateData(1);
}

void CPage3B::OnShowmax() 
{
  UpdateData(1);
}

void CPage3B::OnShowmin() 
{
  UpdateData(1);
}

void CPage3B::OnShownormal() 
{
  UpdateData(1);
}

void CPage3B::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
  if(nIDCtl == IDC_ICON_LIST)
  {
    if(lpDrawItemStruct->itemState & ODS_SELECTED)
    {
      CBrush cbr(RGB(128,0,128));

      ::FillRect(lpDrawItemStruct->hDC, &(lpDrawItemStruct->rcItem),
                 (HBRUSH)cbr.m_hObject);
    }
    else
    {
      CBrush cbr(RGB(255,255,255));

      ::FillRect(lpDrawItemStruct->hDC, &(lpDrawItemStruct->rcItem),
                 (HBRUSH)cbr.m_hObject);
    }

    if(lpDrawItemStruct->itemID >= 0 && lpDrawItemStruct->itemID < (UINT)m_aImages.GetSize())
    {
      ::DrawIcon(lpDrawItemStruct->hDC, lpDrawItemStruct->rcItem.left + 2,
                 lpDrawItemStruct->rcItem.top + 2,
                 m_aImages[lpDrawItemStruct->itemID]);
    }

    if((lpDrawItemStruct->itemState & ODS_FOCUS)
       /*&& !(lpDrawItemStruct->itemState & ODS_NOFOCUSRECT)*/)
    {
      DrawFocusRect(lpDrawItemStruct->hDC, &(lpDrawItemStruct->rcItem));
    }

    return;
  }
  
  CPropertyPage::OnDrawItem(nIDCtl, lpDrawItemStruct);
}

void CPage3B::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct) 
{
  if(nIDCtl == IDC_ICON_LIST)
  {
    lpMeasureItemStruct->itemHeight = ::GetSystemMetrics(SM_CYICON) + 4;
    lpMeasureItemStruct->itemWidth = ::GetSystemMetrics(SM_CXICON) + 4;

    return;
  }
  
  CPropertyPage::OnMeasureItem(nIDCtl, lpMeasureItemStruct);
}
