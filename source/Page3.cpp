//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//           ____                      _____                                //
//          |  _ \  __ _   __ _   ___ |___ /    ___  _ __   _ __            //
//          | |_) |/ _` | / _` | / _ \  |_ \   / __|| '_ \ | '_ \           //
//          |  __/| (_| || (_| ||  __/ ___) |_| (__ | |_) || |_) |          //
//          |_|    \__,_| \__, | \___||____/(_)\___|| .__/ | .__/           //
//                        |___/                     |_|    |_|              //
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
#include "Page3.h"
#include "SetupGizDlg.h"
#include ".\page3.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPage3 property page

IMPLEMENT_DYNCREATE(CPage3, CPropertyPage)

CPage3::CPage3() : CPropertyPage(CPage3::IDD)
, m_bRegDotNet(FALSE)
{
	//{{AFX_DATA_INIT(CPage3)
	m_bAutoDelete = FALSE;
	m_bExecute = FALSE;
	m_bOptionInstall = FALSE;
	m_csOtherPathName = _T("");
	m_bOverwrite = FALSE;
	m_bSelectOS = FALSE;
	m_bShared = FALSE;
	m_iDestPath = -1;
	m_bNoUninstall = FALSE;
	m_bRegister = FALSE;
	m_bAddShortcut = FALSE;
	m_csExecArgs = _T("");
	m_csUninstArgs = _T("");
	//}}AFX_DATA_INIT

  DoClear();
}

CPage3::~CPage3()
{
}

void CPage3::DoClear()
{
	m_bAutoDelete = FALSE;
	m_bExecute = FALSE;
	m_bOptionInstall = FALSE;
	m_csOtherPathName = _T("");
	m_bOverwrite = FALSE;
	m_bSelectOS = FALSE;
	m_bShared = FALSE;
	m_iDestPath = -1;
	m_bNoUninstall = FALSE;
	m_bRegister = FALSE;
	m_bAddShortcut = FALSE;
	m_csExecArgs = _T("");
	m_csUninstArgs = _T("");

  m_iCurSel = LB_ERR;
}

void CPage3::DoDataExchange(CDataExchange* pDX)
{
  CPropertyPage::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CPage3)
  DDX_Control(pDX, IDC_AUTODELETE, m_wndAutoDelete);
  DDX_Control(pDX, IDC_UNINST_ARGS, m_wndUninstArgs);
  DDX_Control(pDX, IDC_EXEC_ARGS, m_wndExecArgs);
  DDX_Control(pDX, IDC_OTHERPATH_NAME, m_wndOtherPathName);
  DDX_Control(pDX, IDC_OSLIST, m_wndOSList);
  DDX_Control(pDX, IDC_OPTIONS, m_wndOptions);
  DDX_Control(pDX, IDC_FILE_LIST, m_wndFileList);
  DDX_Check(pDX, IDC_AUTODELETE, m_bAutoDelete);
  DDX_Check(pDX, IDC_EXECUTE, m_bExecute);
  DDX_Check(pDX, IDC_OPTION_INSTALL, m_bOptionInstall);
  DDX_Text(pDX, IDC_OTHERPATH_NAME, m_csOtherPathName);
  DDX_Check(pDX, IDC_OVERWRITE, m_bOverwrite);
  DDX_Check(pDX, IDC_SELECT_OS, m_bSelectOS);
  DDX_Check(pDX, IDC_SHARED, m_bShared);
  DDX_Radio(pDX, IDC_SYSTEMDIR, m_iDestPath);
  DDX_Check(pDX, IDC_NO_UNINSTALL, m_bNoUninstall);
  DDX_Check(pDX, IDC_REGISTER, m_bRegister);
  DDX_Check(pDX, IDC_ADD_SHORTCUT, m_bAddShortcut);
  DDX_Text(pDX, IDC_EXEC_ARGS, m_csExecArgs);
  DDX_Text(pDX, IDC_UNINST_ARGS, m_csUninstArgs);
  DDX_Check(pDX, IDC_REG_DOT_NET, m_bRegDotNet);
  DDX_Control(pDX, IDC_REG_DOT_NET, m_btnRegDotNet);
  //}}AFX_DATA_MAP

  if(pDX->m_bSaveAndValidate)
    m_iCurSel = m_wndFileList.GetCurSel();
  else
    m_wndFileList.SetCurSel(m_iCurSel);

  if(m_bRegister)
    m_btnRegDotNet.EnableWindow(1);
  else
    m_btnRegDotNet.EnableWindow(0);

  if(m_iDestPath == 3)
    m_wndOtherPathName.EnableWindow(1);
  else
    m_wndOtherPathName.EnableWindow(0);

  if(m_bOptionInstall)
    m_wndOptions.EnableWindow(1);
  else
    m_wndOptions.EnableWindow(0);


  if(m_bShared)  // system/shared files can't be auto-deleted
    m_wndAutoDelete.EnableWindow(0);
  else
    m_wndAutoDelete.EnableWindow(1);


  if(m_bSelectOS)
    m_wndOSList.EnableWindow(1);
  else
    m_wndOSList.EnableWindow(0);

  if(m_bExecute)
  {
    m_wndExecArgs.EnableWindow(1);

    if(m_bNoUninstall)
    {
      m_wndUninstArgs.EnableWindow(0);
    }
    else
    {
      m_wndUninstArgs.EnableWindow(1);
    }
  }
  else
  {
    m_wndExecArgs.EnableWindow(0);
    m_wndUninstArgs.EnableWindow(0);
  }
}


BEGIN_MESSAGE_MAP(CPage3, CPropertyPage)
	//{{AFX_MSG_MAP(CPage3)
	ON_BN_CLICKED(IDC_SYSTEMDIR, OnSystemdir)
	ON_BN_CLICKED(IDC_TEMPDIR, OnTempdir)
	ON_BN_CLICKED(IDC_APPDIR, OnAppdir)
	ON_BN_CLICKED(IDC_OTHERPATH, OnOtherpath)
	ON_BN_CLICKED(IDC_OPTION_INSTALL, OnOptionInstall)
	ON_BN_CLICKED(IDC_SELECT_OS, OnSelectOs)
	ON_LBN_SELCHANGE(IDC_FILE_LIST, OnSelchangeFileList)
	ON_BN_CLICKED(IDC_MINI_HELP, OnMiniHelp)
	ON_BN_CLICKED(IDC_EXECUTE, OnExecute)
	ON_BN_CLICKED(IDC_NO_UNINSTALL, OnNoUninstall)
	ON_BN_CLICKED(IDC_SHARED, OnShared)
	//}}AFX_MSG_MAP
  ON_BN_CLICKED(IDC_REG_DOT_NET, OnClickRegDotNet)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPage3 message handlers

BOOL CPage3::OnSetActive() 
{
  CSetupGizDlg *pSheet = CSetupGizDlg::GetPropertySheet(this);

  if(!pSheet)
  {
    AfxMessageBox("Internal error - can't continue!",
                  MB_OK | MB_ICONHAND | MB_SETFOREGROUND);
    return(FALSE);
  }

  pSheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);

  UpdateData(0);

  // filling left-hand listbox

  int iOldSel = m_wndFileList.GetCurSel();  // just in case

  m_wndFileList.ResetContent();

  int i1;
  for(i1=0; i1 < pSheet->m_aFileList.GetSize(); i1++)
  {
    m_wndFileList.InsertString(i1, pSheet->m_aFileList[i1].csFileName);
  }

  // filling 'groups' combo box

  m_wndOptions.ResetContent();

  for(i1=0; i1 < pSheet->m_aGroupList.GetSize(); i1++)
  {
    m_wndOptions.AddString(pSheet->m_aGroupList[i1].csGroupName);
  }

  // filling 'OS' list box

  m_wndOSList.ResetContent();

  i1 = 0;
  m_wndOSList.InsertString(i1, "Win 95");      // 4.00
  m_wndOSList.SetItemData(i1,(1 << i1));
  i1++;
  m_wndOSList.InsertString(i1, "Win 98");      // 4.10
  m_wndOSList.SetItemData(i1,(1 << i1));
  i1++;
  m_wndOSList.InsertString(i1, "Win NT 4.x");  // 4.00
  m_wndOSList.SetItemData(i1,(1 << i1));
  i1++;
  m_wndOSList.InsertString(i1, "Win 2000");    // 5.00
  m_wndOSList.SetItemData(i1,(1 << i1));
  i1++;
  m_wndOSList.InsertString(i1, "Win ME");      // 4.90
  m_wndOSList.SetItemData(i1,(1 << i1));
  i1++;
  m_wndOSList.InsertString(i1, "Win XP");      // 5.01
  m_wndOSList.SetItemData(i1,(1 << i1));
  i1++;
  m_wndOSList.InsertString(i1, "Win Srv 2003");// 5.02
  m_wndOSList.SetItemData(i1,(1 << i1));
  i1++;
  m_wndOSList.InsertString(i1, "Windows Vista"); // 6.0 (desktop)
  m_wndOSList.SetItemData(i1,(1 << i1));
  i1++;
  m_wndOSList.InsertString(i1, "Win Srv 2008");  // 6.0 (server)
  m_wndOSList.SetItemData(i1,(1 << i1));
  i1++;
  m_wndOSList.InsertString(i1, "Win 7");         // 6.1 (desktop)
  m_wndOSList.SetItemData(i1,(1 << i1));
  i1++;
  m_wndOSList.InsertString(i1, "Win Srv 2008R2");// 6.1 (server)
  m_wndOSList.SetItemData(i1,(1 << i1));
  i1++;
  m_wndOSList.InsertString(i1, "Win 8.0");       // 6.2 (desktop)
  m_wndOSList.SetItemData(i1,(1 << i1));
  i1++;
  m_wndOSList.InsertString(i1, "Win Srv 2012");  // 6.2 (server)
  m_wndOSList.SetItemData(i1,(1 << i1));
  i1++;
  m_wndOSList.InsertString(i1, "Win 8.1");       // 6.3 (desktop)
  m_wndOSList.SetItemData(i1,(1 << i1));
  i1++;
  m_wndOSList.InsertString(i1, "Win Srv 2012R2");// 6.3 (server)
  m_wndOSList.SetItemData(i1,(1 << i1));
  i1++;
  m_wndOSList.InsertString(i1, "Win 10");        // 6.4
  m_wndOSList.SetItemData(i1,(1 << i1));
  i1++;

  // this one must always appear LAST and use the specified flag
  m_wndOSList.InsertString(i1, "{New Win OS}"); // anything bigger than the one above
  m_wndOSList.SetItemData(i1,0x40000000);       // special "NEW WINOS" flag


  UpdateData(0);  // do it again to get data items squared away

  if(iOldSel < 0 && m_wndFileList.GetCount())
    iOldSel = 0;

  m_wndFileList.SetCurSel(iOldSel);

  m_iCurSel = LB_ERR;  // forces a re-load
  OnSelchangeFileList();

	return CPropertyPage::OnSetActive();
}

BOOL CPage3::OnKillActive() 
{
  OnSelchangeFileList();  // this updates data properly
	
	return CPropertyPage::OnKillActive();
}

LRESULT CPage3::OnWizardBack() 
{
//  UpdateData(1);
  OnSelchangeFileList();  // this updates data properly
	
	return CPropertyPage::OnWizardBack();
}

LRESULT CPage3::OnWizardNext() 
{
//  UpdateData(1);
  OnSelchangeFileList();  // this updates data properly

  CSetupGizDlg *pSheet = CSetupGizDlg::GetPropertySheet(this);

  if(!pSheet)
  {
    AfxMessageBox("Internal error - cannot continue!",
                  MB_OK | MB_ICONHAND | MB_SETFOREGROUND);
    return -1;
  }


  // using "live" data, generate a list of valid option groups

  CStringArray acsGroups;
  int i1, i2;
  for(i1=0; i1 < pSheet->m_aFileList.GetSize(); i1++)
  {
    if(pSheet->m_aFileList[i1].csGroupName.GetLength())
    {
      for(i2=0; i2 < acsGroups.GetSize(); i2++)
      {
        if(!acsGroups[i2].CompareNoCase(pSheet->m_aFileList[i1].csGroupName))
        {
          break;
        }
      }

      if(i2 >= acsGroups.GetSize())
        acsGroups.Add(pSheet->m_aFileList[i1].csGroupName);
    }
  }

  // For each matching group, keep the entries in 'pSheet->m_aGroupList'
  // but delete all of the rest (this includes supporting data)

  for(i1=pSheet->m_aGroupList.GetSize() - 1; i1 >= 0; i1--)
  {
    for(i2=0; i2 < acsGroups.GetSize(); i2++)
    {
      if(!pSheet->m_aGroupList[i1].csGroupName.CompareNoCase(acsGroups[i2]))
      {
        break;
      }
    }

    if(i2 >= acsGroups.GetSize())  // not there
      pSheet->m_aGroupList.RemoveAt(i1);
  }

  // For each "other" group not already in the list, add it to the list

  for(i2=0; i2 < acsGroups.GetSize(); i2++)
  {
    for(i1=0; i1 < pSheet->m_aGroupList.GetSize(); i1++)
    {
      if(!pSheet->m_aGroupList[i1].csGroupName.CompareNoCase(acsGroups[i2]))
      {
        break;
      }
    }

    if(i1 >= pSheet->m_aGroupList.GetSize())
    {
      SetupGroupInfo sgi;    // not there - add it!

      sgi.csGroupName = acsGroups[i2];
      sgi.csDesc = acsGroups[i2];
      sgi.csLongDesc = "{Insert your long description here}";

      pSheet->m_aGroupList.Add(sgi);
    }
  }

  // if there are no groups, go to 'Page4' or 'Page3B'; else, 'Page3A'

  if(!pSheet->m_aGroupList.GetSize())
  {
    // if there are any shortcuts, go to 'Page3B'

    CPropertyPage::OnWizardNext();

    for(i1=0; i1 < pSheet->m_aFileList.GetSize(); i1++)
    {
      if(pSheet->m_aFileList[i1].bAddShortcut)
      {
        CPropertyPage::OnWizardNext();

        return(IDD_PAGE_3B);
      }
    }

    // if there are certs, go to 'page 3 Cert'

    if(pSheet->m_acsCertFileNameList.GetSize())
    {
      return IDD_PAGE_3_CERT;
    }

    return IDD_PAGE_4;  // go to 'page 4' and not 'page 3B' or cert
  }
	
	CPropertyPage::OnWizardNext();

  return(IDD_PAGE_3A);
}

void CPage3::OnSystemdir() 
{
  UpdateData(1);
}

void CPage3::OnTempdir() 
{
  UpdateData(1);
}

void CPage3::OnAppdir() 
{
  UpdateData(1);
}

void CPage3::OnOtherpath() 
{
  UpdateData(1);
}

void CPage3::OnOptionInstall() 
{
  UpdateData(1);
}

void CPage3::OnSelectOs() 
{
  UpdateData(1);
}

void CPage3::OnSelchangeFileList()
{
  int i1, i2;

  // see if I'm "control-clicking" this entry.  If this is the case,
  // copy the settings of the PREVIOUS ITEM onto THIS one!

  BOOL bCopy = (GetKeyState(VK_CONTROL) & 0x80000000);
  bCopy = bCopy && !(GetKeyState(VK_SHIFT) & 0x80000000);
  bCopy = bCopy && !(GetKeyState(VK_MENU) & 0x80000000);
//  bCopy = bCopy && (GetKeyState(VK_LBUTTON) & 0x80000000);

  // call this not only on sel change, but to load/update values

  CString csGroup;
  CSetupGizDlg *pSheet = CSetupGizDlg::GetPropertySheet(this);

  if(!pSheet)
  {
    return;  // for now, just bail
  }

  int iOldSel = m_iCurSel;

  UpdateData(1);

  // DO SOME MASSAGING

  if(m_bShared && m_bAutoDelete) // shared files can't be "auto-deleted"
  {
    m_bAutoDelete = FALSE;       // force this to be the case, ALWAYS!!!
    UpdateData(0);               // make sure everything remains 'in-sync'
  }

  if(iOldSel >= 0 && iOldSel < pSheet->m_aFileList.GetSize())
  {
    pSheet->m_aFileList[iOldSel].bAutoDelete = m_bAutoDelete;
    pSheet->m_aFileList[iOldSel].bExecute = m_bExecute;
    pSheet->m_aFileList[iOldSel].bRegister = m_bRegister;
    pSheet->m_aFileList[iOldSel].bRegDotNet = m_bRegDotNet;
    pSheet->m_aFileList[iOldSel].bOverwrite = m_bOverwrite;
    pSheet->m_aFileList[iOldSel].bShared = m_bShared;
    pSheet->m_aFileList[iOldSel].bNoUninstall = m_bNoUninstall;
    pSheet->m_aFileList[iOldSel].bAddShortcut = m_bAddShortcut;

    pSheet->m_aFileList[iOldSel].iDestPath = m_iDestPath;

    if(m_iDestPath == 3)
      pSheet->m_aFileList[iOldSel].csOtherPathName = m_csOtherPathName;
    else
      pSheet->m_aFileList[iOldSel].csOtherPathName = "";

    if(m_bExecute)
    {
      pSheet->m_aFileList[iOldSel].csExecArgs = m_csExecArgs;

      if(m_bNoUninstall)
        pSheet->m_aFileList[iOldSel].csUninstArgs = "";
      else
        pSheet->m_aFileList[iOldSel].csUninstArgs = m_csUninstArgs;
    }
    else
    {
      pSheet->m_aFileList[iOldSel].csExecArgs = "";
      pSheet->m_aFileList[iOldSel].csUninstArgs = "";
    }

    if(m_bOptionInstall)
    {
      m_wndOptions.GetWindowText(csGroup);

      pSheet->m_aFileList[iOldSel].csGroupName = csGroup;
    }
    else
    {
      pSheet->m_aFileList[iOldSel].csGroupName = "";  // must force to blank!
    }

    // find out which OS's are selected (if applicable)

    pSheet->m_aFileList[iOldSel].dwOSFlag = 0;

    if(m_bSelectOS)
    {
      int i1;
      for(i1=0; i1 < m_wndOSList.GetCount(); i1++)
      {
        if(m_wndOSList.GetSel(i1))
        {
          DWORD dwMask = m_wndOSList.GetItemData(i1);
          pSheet->m_aFileList[iOldSel].dwOSFlag |= dwMask;
        }
      }
    }
  }


  // validate GROUP list in combo box

  CStringArray acsGroups;

  for(i1=0; i1 < pSheet->m_aFileList.GetSize(); i1++)
  {
    if(pSheet->m_aFileList[i1].csGroupName.GetLength())
    {
      for(i2=0; i2 < acsGroups.GetSize(); i2++)
      {
        if(!acsGroups[i2].CompareNoCase(pSheet->m_aFileList[i1].csGroupName))
        {
          break;
        }
      }

      if(i2 >= acsGroups.GetSize())
        acsGroups.Add(pSheet->m_aFileList[i1].csGroupName);
    }
  }

  // at this point 'csGroup' will contain the correct window text
  // for the combo box, either a blank or the actual group...

  m_wndOptions.ResetContent();
  for(i1=0; i1 < acsGroups.GetSize(); i1++)
  {
    m_wndOptions.AddString(acsGroups[i1]);
  }

  m_wndOptions.SetWindowText(csGroup);  // to prevent certain problems



  // now, load new values (when applicable)

  int iSel = m_wndFileList.GetCurSel();

  if(iSel != iOldSel && !bCopy &&
     iSel >= 0 && iSel < pSheet->m_aFileList.GetSize())
  {
    m_bAutoDelete = pSheet->m_aFileList[iSel].bAutoDelete;
    m_bExecute = pSheet->m_aFileList[iSel].bExecute;
    m_bRegister = pSheet->m_aFileList[iSel].bRegister;
    m_bRegDotNet = pSheet->m_aFileList[iSel].bRegDotNet;
    m_bOverwrite = pSheet->m_aFileList[iSel].bOverwrite;
    m_bShared = pSheet->m_aFileList[iSel].bShared;
    m_bNoUninstall = pSheet->m_aFileList[iSel].bNoUninstall;
    m_bAddShortcut = pSheet->m_aFileList[iSel].bAddShortcut;

    ASSERT(!m_bAutoDelete || !m_bShared);  // both can't be TRUE at the same time

    m_iDestPath = pSheet->m_aFileList[iSel].iDestPath;

    if(m_iDestPath == 3)  // i.e. "other"
      m_csOtherPathName = pSheet->m_aFileList[iSel].csOtherPathName;
    else
      m_csOtherPathName = "";

    if(m_bExecute)
    {
      m_csExecArgs = pSheet->m_aFileList[iSel].csExecArgs;

      if(m_bNoUninstall)
        m_csUninstArgs = "";
      else
        m_csUninstArgs = pSheet->m_aFileList[iSel].csUninstArgs;
    }
    else
    {
      m_csExecArgs = "";
      m_csUninstArgs = "";
    }

    if(pSheet->m_aFileList[iSel].csGroupName.GetLength())
    {
      m_bOptionInstall = TRUE;
      m_wndOptions.SetWindowText(pSheet->m_aFileList[iSel].csGroupName);
    }
    else
    {
      m_bOptionInstall = FALSE;
      m_wndOptions.SetWindowText("");
    }

    if(pSheet->m_aFileList[iSel].dwOSFlag)
    {
      m_bSelectOS = TRUE;
    }
    else
    {
      m_bSelectOS = FALSE;
    }

    for(i1=0; i1 < m_wndOSList.GetCount(); i1++)
    {
      DWORD dwMask = m_wndOSList.GetItemData(i1);

      if(pSheet->m_aFileList[iSel].dwOSFlag & dwMask)
      {
        m_wndOSList.SetSel(i1, TRUE);
      }
      else
      {
        m_wndOSList.SetSel(i1, FALSE);
      }
    }

    UpdateData(0);
  }

}

void CPage3::OnMiniHelp() 
{
  AfxMessageBox("The following string substitions are valid at install time:\r\n"
                "%windir%\t\tWINDOWS directory\r\n"
                "%sysdir%\t\tSYSTEM directory\r\n"
                "%apppath%\tApplication (root) path\r\n"
                "%tmpdir%\t\tTemporary Files Directory\r\n"
                "%themedir%\t\tThe directory where 'Themes' are located\r\n"
                "%appcommon%\tApplication 'Common' files\r\n"
                "%%\t\tdoubled '%' indicates a literal '%'\r\n"
                "'apppath' and 'appcommon' will be prompted for when used\r\n"
                "NOTE:  you can install to multiple paths by separating "
                "each of them with a semicolon ';'",
                MB_OK | MB_ICONINFORMATION | MB_SETFOREGROUND);
}

void CPage3::OnExecute() 
{
  UpdateData(1);
}

void CPage3::OnNoUninstall() 
{
  UpdateData(1);
}

void CPage3::OnShared() 
{
  UpdateData(1);
}

void CPage3::OnClickRegDotNet()
{
  UpdateData(1);
}
