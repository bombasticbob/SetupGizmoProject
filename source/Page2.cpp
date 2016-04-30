//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//           ____                      ____                                 //
//          |  _ \  __ _   __ _   ___ |___ \     ___  _ __   _ __           //
//          | |_) |/ _` | / _` | / _ \  __) |   / __|| '_ \ | '_ \          //
//          |  __/| (_| || (_| ||  __/ / __/  _| (__ | |_) || |_) |         //
//          |_|    \__,_| \__, | \___||_____|(_)\___|| .__/ | .__/          //
//                        |___/                      |_|    |_|             //
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
#include "Page2.h"
#include "setupgizdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPage2 property page

IMPLEMENT_DYNCREATE(CPage2, CPropertyPage)

CPage2::CPage2() : CPropertyPage(CPage2::IDD)
{
	//{{AFX_DATA_INIT(CPage2)
	m_bShowDirs = FALSE;
	//}}AFX_DATA_INIT

  DoClear();
}

CPage2::~CPage2()
{
}

void CPage2::DoClear()
{
  m_bShowDirs = FALSE;
}

void CPage2::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPage2)
	DDX_Control(pDX, IDC_MOVE_UP, m_btnMoveUp);
	DDX_Control(pDX, IDC_MOVE_DOWN, m_btnMoveDown);
	DDX_Control(pDX, IDC_SOURCE_LIST, m_wndSourceList);
	DDX_Control(pDX, IDC_DEST_LIST, m_wndDestList);
	DDX_Check(pDX, IDC_SHOWDIRS, m_bShowDirs);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPage2, CPropertyPage)
	//{{AFX_MSG_MAP(CPage2)
	ON_BN_CLICKED(IDC_GOZINTA, OnGozinta)
	ON_BN_CLICKED(IDC_GOZOUTA, OnGozouta)
	ON_BN_CLICKED(IDC_MOVE_DOWN, OnMoveDown)
	ON_BN_CLICKED(IDC_MOVE_UP, OnMoveUp)
	ON_LBN_DBLCLK(IDC_SOURCE_LIST, OnDblclkSourceList)
	ON_BN_CLICKED(IDC_SHOWDIRS, OnShowdirs)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPage2 message handlers

void CPage2::OnGozinta() 
{
  // for each item selected, add it to the left-hand box if it's
  // not already there.

  // BUG:  filename (without path) is compared for uniqueness.  To do this
  //       correctly, only the FULL destination path must be unique.  It
  //       shows up as not putting things like multiple cygwin1.dll files into
  //       multiple directories, even though specified in the 'giz' file

  CWaitCursor wait;

  BOOL bAddedFlag = FALSE;

  int i1;
  CString csTemp, csTemp2;

  for(i1=0; i1 < m_wndSourceList.GetCount(); i1++)
  {
    if(m_wndSourceList.GetSel(i1))  // it's selected?
    {
      m_wndSourceList.GetText(i1, csTemp);

      int i2;

      for(i2=0; i2 < m_wndDestList.GetCount(); i2++)
      {
        m_wndDestList.GetText(i2, csTemp2);

        if(!csTemp.CompareNoCase(csTemp2))
        {
          break;
        }
      }

      if(i2 >= m_wndDestList.GetCount())
      {
        // add string at bottom

        m_wndDestList.InsertString(-1, csTemp);
        bAddedFlag = TRUE;  // to say I did at least one
      }
    }
  }

  if(!bAddedFlag)
    MessageBeep(0);  // to warn user nothing happened
  else
  {
    INT dwS, dwE, dwTemp;
    m_wndDestList.GetScrollRange(SB_HORZ, &dwS, &dwE);

    dwTemp = csTemp.GetLength() * LOWORD(::GetDialogBaseUnits());
    if(dwTemp > dwE - dwS)
    {
      dwE = dwS + dwTemp;
      m_wndDestList.SetScrollRange(SB_HORZ, dwS, dwE);
    }
  }
}

void CPage2::OnGozouta() 
{
  // for each item selected, remove it from the left-hand box

  CWaitCursor wait;

  BOOL bRemovedFlag = FALSE;

  int i1;
  CString csTemp, csTemp2;

  for(i1 = m_wndDestList.GetCount() - 1; i1 >= 0; i1--)
  {
    if(m_wndDestList.GetSel(i1))  // it's selected?
    {
      m_wndDestList.DeleteString(i1);
      bRemovedFlag = TRUE;  // to say I did at least one
    }
  }

  if(!bRemovedFlag)
    MessageBeep(0);  // to warn user nothing happened
}

void CPage2::OnMoveDown() 
{
  CString csTemp;

  int i1;
  BOOL bMoved = FALSE;

  // begin with last "un-selected" item (the rest won't move)

  for(i1 = m_wndDestList.GetCount() - 1; i1 >= 0; i1--)
  {
    if(!m_wndDestList.GetSel(i1))  // it's NOT selected?
      break;
  }

  for(i1--; i1 >= 0; i1--)
  {
    if(m_wndDestList.GetSel(i1))  // it's selected?
    {
      m_wndDestList.GetText(i1, csTemp);
      m_wndDestList.DeleteString(i1);
      m_wndDestList.InsertString(i1 + 1, csTemp);

      m_wndDestList.SetSel(i1 + 1, 1);

      bMoved = TRUE;
    }
  }

  if(!bMoved)
  {
    MessageBeep(0);
  }
}

void CPage2::OnMoveUp() 
{
  CString csTemp;

  int i1;
  BOOL bMoved = FALSE;

  // begin with 1st "un-selected" item (the rest won't move)

  for(i1=0; i1 < m_wndDestList.GetCount(); i1++)
  {
    if(!m_wndDestList.GetSel(i1))  // it's NOT selected?
      break;
  }

  for(i1++; i1 < m_wndDestList.GetCount(); i1++)
  {
    if(m_wndDestList.GetSel(i1))  // it's selected?
    {
      m_wndDestList.GetText(i1, csTemp);
      m_wndDestList.DeleteString(i1);
      m_wndDestList.InsertString(i1 - 1, csTemp);

      m_wndDestList.SetSel(i1 - 1, 1);

      bMoved = TRUE;
    }
  }

  if(!bMoved)
  {
    MessageBeep(0);
  }
}

void CPage2::OnDblclkSourceList() 
{
  OnGozinta();
}

BOOL CPage2::SynchronizeDestFiles()
{
  // here is where I synchronize the contents with the left-hand listbox

  CSetupGizDlg *pSheet = CSetupGizDlg::GetPropertySheet(this);

  if(!pSheet)
  {
    AfxMessageBox("Internal error - can't continue!",
                  MB_OK | MB_ICONHAND | MB_SETFOREGROUND);
    return(FALSE);
  }

  // first, make a copy of 'pSheet->m_aFileList'

  CArray<SetupFileInfo,SetupFileInfo> aFileList;

  int i1, i2, i3;
  for(i1=0, i2=0; i1 < pSheet->m_aFileList.GetSize(); i1++)
  {
    i3 = pSheet->m_aFileList[i1].csFileName.GetLength();
    if(i2 < i3)
    {
      i2 = i3; // max length
    }
    
    aFileList.Add(pSheet->m_aFileList[i1]);
  }

  i2 *= LOWORD(::GetDialogBaseUnits());

  m_wndDestList.SetScrollRange(SB_HORZ, 0, i2);

  // second, clear out 'pSheet->m_aFileList' and start adding
  // entries back in based on the following criteria:
  // a) if the entry existed before, use the old entry
  // b) if the entry did not exist before, create a default entry

  pSheet->m_aFileList.RemoveAll();

  CString csTemp;
  for(i1=0; i1 < m_wndDestList.GetCount(); i1++)
  {
    m_wndDestList.GetText(i1, csTemp);

    for(i2=0; i2 < aFileList.GetSize(); i2++)
    {
      if(!csTemp.CompareNoCase(aFileList[i2].csFileName))
      {
        pSheet->m_aFileList.Add(aFileList[i2]);
        break;
      }
    }

    if(i2 >= aFileList.GetSize())
    {
      // need to create a new 'default' entry
      SetupFileInfo sfi;

      sfi.csFileName = csTemp;  // the file name
      sfi.bAutoDelete = FALSE;  // by default, off
      sfi.bExecute = FALSE;     // by default, off
      sfi.bOverwrite = FALSE;   // by default, off
      sfi.bShared = FALSE;      // by default, off
      sfi.bNoUninstall = FALSE; // by default, off

      sfi.iDestPath = 1;        // default is 'app path'
      sfi.csGroupName = "";     // no option group
      sfi.csOtherPathName = ""; // no special install path

      sfi.dwOSFlag = 0;         // ALL operating systems

      pSheet->m_aFileList.Add(sfi);
    }
  }

	return(TRUE);
}

void CPage2::FillSourceListBox(CSetupGizDlg *pSheet, LPCSTR szPath)
{
  // recursive function to add files to listbox

  CString csFullPath = pSheet->m_Page1.m_csSourcePath;

  if(!szPath || !*szPath) // first time through
  {
    // set working directory to match that of the 'source path' - need it for later

    if(!csFullPath.GetLength() || csFullPath[csFullPath.GetLength() - 1] != '\\')
    {
      SetCurrentDirectory(csFullPath);
      csFullPath += '\\';
    }
    else
    {
      if(csFullPath.GetLength() > 3)
        SetCurrentDirectory(csFullPath.Left(csFullPath.GetLength() - 1));
      else
        SetCurrentDirectory(csFullPath);
    }
  }
  else if(csFullPath.GetLength() && csFullPath[csFullPath.GetLength() - 1] != '\\')
  {
    csFullPath += '\\';
  }

  CString csPath = szPath;

  if(csPath.GetLength() && csPath[csPath.GetLength() - 1] != '\\')
    csPath += '\\';

  csFullPath += csPath;

  WIN32_FIND_DATA fd;
  HANDLE hFF = FindFirstFile(csFullPath + "*.*", &fd);

  if(hFF == INVALID_HANDLE_VALUE)
  {
    return;
  }

  CStringArray acsDir;
  int i1 = 0, iMax = 0;

  do
  {
    if(fd.cFileName[0] == '.' &&
       (fd.cFileName[1] == 0 ||
        (fd.cFileName[1] == '.' && fd.cFileName[2] == 0)))
    {
      continue;  // skip it - it's a '.' or '..' filename
    }

    if(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
    {
      if(m_bShowDirs)
        acsDir.Add(fd.cFileName);
    }
    else
    {
      // prepend relative path to file name when I add it

      CString csTemp = csPath + fd.cFileName;

      i1 = csTemp.GetLength();
      if(i1 > iMax)
      {
        iMax = i1;
      }
      m_wndSourceList.AddString(csTemp);
    }

  } while(FindNextFile(hFF, &fd));

  FindClose(hFF);

  iMax *= LOWORD(::GetDialogBaseUnits());

  INT dwS, dwE;
  m_wndSourceList.GetScrollRange(SB_HORZ, &dwS, &dwE);
  if(iMax > (dwE - dwS))
  {
    dwE = iMax + dwS;

    m_wndSourceList.SetScrollRange(SB_HORZ, dwS, dwE);
  }

  // NOW, we need to recurse any sub-directories, but only pass the
  // relative path of the sub-directory when I recurse

  if(m_bShowDirs)
  {
    for(i1=0; i1 < acsDir.GetSize(); i1++)
    {
      FillSourceListBox(pSheet, csPath + acsDir[i1]);
    }
  }
}

BOOL CPage2::OnKillActive() 
{
  return(CPropertyPage::OnKillActive());
}

BOOL CPage2::OnSetActive() 
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


  // get information from page 1 and fill right-hand listbox

  m_wndSourceList.ResetContent();

  FillSourceListBox(pSheet, NULL);

  // get information from parent and fill left-hand listbox

  m_wndDestList.ResetContent();

  int i1, i2, iMax;
  for(i1=0, iMax=0; i1 < pSheet->m_aFileList.GetSize(); i1++)
  {
    i2 = pSheet->m_aFileList[i1].csFileName.GetLength();
    if(i2 > iMax)
    {
      iMax = i2;
    }
    m_wndDestList.InsertString(i1, pSheet->m_aFileList[i1].csFileName);
  }

  iMax *= LOWORD(::GetDialogBaseUnits());

  m_wndDestList.SetScrollRange(SB_HORZ, 0, iMax);

  UpdateData(0);  // do it again to get any selections right...

	return CPropertyPage::OnSetActive();
}

LRESULT CPage2::OnWizardBack() 
{
  SynchronizeDestFiles();
	
	return CPropertyPage::OnWizardBack();
}

LRESULT CPage2::OnWizardNext() 
{
  if(!SynchronizeDestFiles())
    return -1;

  CSetupGizDlg *pSheet = CSetupGizDlg::GetPropertySheet(this);

  if(!pSheet)
  {
    AfxMessageBox("Internal error - can't continue!",
                  MB_OK | MB_ICONHAND | MB_SETFOREGROUND);
    return -1;
  }

  if(!pSheet->m_aFileList.GetSize())
  {
    AfxMessageBox("There are no files selected!  Please select at least one file to install before proceeding",
                  MB_OK | MB_ICONHAND | MB_SETFOREGROUND);
    return -1;
  }
	
	return(CPropertyPage::OnWizardNext());
}

void CPage2::OnShowdirs() 
{
  UpdateData(1);

  CSetupGizDlg *pSheet = CSetupGizDlg::GetPropertySheet(this);

  if(pSheet) // if valid, reset content and re-fill the right-hand listbox
  {
    m_wndSourceList.ResetContent();

    FillSourceListBox(pSheet, NULL);
  }
}
