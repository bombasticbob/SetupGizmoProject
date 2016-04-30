//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                                      //
//   ____   _                _    __  __                      _____       _      _              ____   _                                //
//  / ___| | |_  __ _  _ __ | |_ |  \/  |  ___  _ __   _   _ |  ___|___  | |  __| |  ___  _ __ |  _ \ | |  __ _     ___  _ __   _ __    //
//  \___ \ | __|/ _` || '__|| __|| |\/| | / _ \| '_ \ | | | || |_  / _ \ | | / _` | / _ \| '__|| | | || | / _` |   / __|| '_ \ | '_ \   //
//   ___) || |_| (_| || |   | |_ | |  | ||  __/| | | || |_| ||  _|| (_) || || (_| ||  __/| |   | |_| || || (_| | _| (__ | |_) || |_) |  //
//  |____/  \__|\__,_||_|    \__||_|  |_| \___||_| |_| \__,_||_|   \___/ |_| \__,_| \___||_|   |____/ |_| \__, |(_)\___|| .__/ | .__/   //
//                                                                                                        |___/         |_|    |_|      //
//                                                                                                                                      //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                                      //
//                                     Copyright (c) 1998-2016 by S.F.T. Inc. - All rights reserved                                     //
//                                Use, copying, and distribution of this software are licensed according                                //
//                                  to the GPLv2, LGPLv2, or BSD license, as appropriate (see COPYING)                                  //
//                                                                                                                                      //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "setup.h"
#include "StartMenuFolderDlg.h"
#include "PathBrowse.h"

//#ifdef _DEBUG
//#define new DEBUG_NEW
//#undef THIS_FILE
//static char THIS_FILE[] = __FILE__;
//#endif

/////////////////////////////////////////////////////////////////////////////
// CStartMenuFolderDlg dialog


CStartMenuFolderDlg::CStartMenuFolderDlg(HWND hParent /*=NULL*/)
: CMyDialogBaseClass(CStartMenuFolderDlg::IDD, hParent)
{
  BOOL bStripBackSlash = FALSE;

  m_bAllUsers = FALSE;
  m_csFolderName = theApp.m_csStartMenuFolderName;

  if(!m_csFolderName.GetLength() ||
     (!theApp.m_bQuietSetup && !theApp.m_bExtraQuietMode))
  {
    if(m_csFolderName.GetLength() &&
       !theApp.ExpandStrings(m_csFolderName)) // theApp.m_csAppName;
    {
      CString csMsg;
      csMsg.Format(theApp.LoadString(IDS_WARNING14), (LPCSTR)theApp.m_csStartMenuFolderName);

      MyMessageBox(csMsg, MB_OK | MB_ICONASTERISK | MB_SETFOREGROUND);

      m_csFolderName = theApp.m_csAppName;
      bStripBackSlash = TRUE;
    }
    else if(!m_csFolderName.GetLength())
    {
      m_csFolderName = theApp.m_csAppName;
      bStripBackSlash = TRUE;
    }

    // strip out any non-file characters

    m_csFolderName = AppNameToPathName(m_csFolderName, bStripBackSlash);
  }


  if(theApp.m_bIsNT)
  {
    m_bAllUsers = TRUE;  // by default, I want "all users"
    m_bHasUserProfile = TRUE;  // and, I want to enable it
  }
  else
  {
    m_bAllUsers = FALSE;       // by default, I do *not* want "all users"
    m_bHasUserProfile = FALSE; // initially, OFF

//    if(theApp.m_dwMajorVer > 4 || theApp.m_dwMinorVer > 0) // win '98 or later
//    {
//      // open HKLM\Network\Logon and check for 'UserProfiles' as a DWORD <> 0
//
//      HKEY hKey;
//
//      if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Network\\Logon",
//                      0, KEY_QUERY_VALUE, &hKey)
//         == ERROR_SUCCESS)
//      {
//        DWORD dw1, dw2 = sizeof(DWORD), dwUserProfiles = 0;
//
//        if(RegQueryValueEx(hKey, "UserProfiles", NULL, &dw1,
//                           (BYTE *)&dwUserProfiles, &dw2)
//           == ERROR_SUCCESS)
//        {
//          if(dw1 == REG_DWORD && dw2 == sizeof(DWORD)
//             && dwUserProfiles)
//          {
//            m_bHasUserProfile = TRUE;
//          }
//        }
//      }
//    }
  }
}

void CStartMenuFolderDlg::UpdateData(BOOL bSaveFlag)
{
  if(bSaveFlag)
  {
    GetDlgItemText(IDC_FOLDER_NAME, m_csFolderName);
    m_bAllUsers = ::SendDlgItemMessage(m_hWnd, IDC_ALL_USERS, BM_GETCHECK, 0, 0)
                ? TRUE : FALSE;
  }
  else
  {
//    ::SendDlgItemMessage(m_hWnd, IDC_ALL_USERS, BN_SETCHECK, m_bAllUsers, 0);
    SetDlgItemText(IDC_FOLDER_NAME, m_csFolderName);

    if(!m_bHasUserProfile)
    {
      if(IsWindowEnabled(GetDlgItem(IDC_ALL_USERS)))
      {
        EnableWindow(GetDlgItem(IDC_ALL_USERS), FALSE);
        m_bAllUsers = FALSE;
      }
    }
    else  // in these cases, "all users" is valid
    {
      if(!IsWindowEnabled(GetDlgItem(IDC_ALL_USERS)))
      {
        EnableWindow(GetDlgItem(IDC_ALL_USERS), TRUE);
      }
    }

    ::SendDlgItemMessage(m_hWnd, IDC_ALL_USERS, BM_SETCHECK, m_bAllUsers, 0);
  }
}

BOOL CStartMenuFolderDlg::MessageHandler(UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
  WORD wCtrl = LOWORD(wParam);
  WORD wNotify = HIWORD(wParam);

  switch(uiMsg)
  {
    case WM_INITDIALOG:
      UpdateData(0);

      return TRUE;

    case WM_COMMAND:
      switch(wCtrl)
      {
        case IDOK:
          OnOK();
          return TRUE;

        case IDCANCEL:
          OnCancel();
          return TRUE;

        case IDC_BROWSE:
          if(wNotify == BN_CLICKED)
          {
            OnBrowse();
            return TRUE;
          }
          break;
      }
  }

  return FALSE;
}

void CStartMenuFolderDlg::DoIt() 
{
  int i1, i2;

  // first, see if I have any items marked "create shortcut"
  // (this includes UNINSTALL).
  //
  // NOTE:  if the start menu folder name is *BLANK*, don't do this!
  //        A blank start menu folder name DISABLES shortcuts!

  m_acsEntries.RemoveAll();  // pre-requisite, in this case

  CStringArray acsShortcut, acsTemp;
  theApp.GetInfEntries("AddShortcut", acsShortcut);

  // go through this list o' things, and see which ones I must create
  // (they will be in 'theApp.m_acsFiles' if I must create them)

  for(i1=0; i1 < acsShortcut.GetSize(); i1++)
  {
    DoParseString(acsShortcut[i1], acsTemp);

    if(acsTemp.GetSize() < 3)  // invalid entry (disregard)
      continue;

    CString csTemp = acsTemp[0];  // this is the destination path for the shortcut item

    i2 = csTemp.ReverseFind('\\');
    if(i2 < 0)
    {
      csTemp = AdjustPathName(theApp.m_csAppPath) + csTemp;
    }
    else
    {
      if(!theApp.ExpandStrings(csTemp))
      {
        csTemp.Format(theApp.LoadString(IDS_ERROR63), (LPCSTR)acsTemp[0]);

        MyMessageBox(csTemp, MB_OK | MB_ICONHAND | MB_SETFOREGROUND);
        continue;
      }
    }

    if(csTemp.GetLength()) // just in case
    {
      for(i2=0; i2 < theApp.m_acsFiles.GetSize(); i2++)
      {
        CString csTemp2 = theApp.GetFileEntryDestPath(theApp.m_acsFiles[i2]);

        if(csTemp2.GetLength() && !csTemp.CompareNoCase(csTemp2))
        {
          // FOUND!  I'm installing this, so g'head and add it's entry to my
          //         list o' shortcuts

          m_acsEntries.Add(acsShortcut[i1]);
          break;
        }
      }
    }
  }

  if(!m_acsEntries.GetSize())
  {
    if(theApp.m_bNoUninstall || !theApp.m_csStartMenuFolderName.GetLength())
    {
      return;  // do nothing - no folder name or not uninstalling (and nothing to add)
    }
  }

  if(theApp.m_bQuietSetup)
  {
    CString csFolderName = m_csFolderName;
    CreateShortcuts(csFolderName);  // ignore errors in this case (already reported)

    // TODO:  do I want to display the folder?
  }
  else
  {
    DoModal();  // this creates the shortcuts, also
  }
}

/////////////////////////////////////////////////////////////////////////////
// CStartMenuFolderDlg message handlers

void CStartMenuFolderDlg::OnOK() 
{
  UpdateData(1);

  m_csFolderName.TrimRight();
  m_csFolderName.TrimLeft();

  if(!m_csFolderName.GetLength())
  {
    MyMessageBox((IDS_ERROR64), MB_OK | MB_ICONASTERISK | MB_SETFOREGROUND);

    m_csFolderName = AppNameToPathName(theApp.m_csAppName);

    UpdateData(0);
    return;
  }

  CString csFolderName;
  if(!CreateShortcuts(csFolderName))
  {
    return;
  }

  csFolderName = csFolderName.Left(csFolderName.GetLength() - 1);

  // open the folder I just created...

  ShellExecute(NULL, "open", csFolderName,
               NULL, NULL, SW_SHOWNA);

  EndDialog(IDOK);
}

void CStartMenuFolderDlg::OnCancel() 
{
  EndDialog(IDCANCEL);
}

BOOL CStartMenuFolderDlg::GetStartMenuFolderName(BOOL bAllUsersFlag, CString &csFolderName,
                                                 CString &csCommonFolderName,
                                                 BOOL bMessageBoxFlag /* = TRUE */)
{
  CMyWaitCursor wait;

  // get the user's shell folder name using *THIS* key
  // HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Explorer\Shell Folders
  // 'Start Menu' and 'Programs' do what they say, say what they do
  // the one I want is the 'programs' folder

  // TODO:  consider using SHGetKnownFolderPath for vista and later
  //        [this may break in windows 10, apparently]

  HKEY hKey;
  DWORD dw1, dw2;

  if(RegOpenKeyEx(HKEY_CURRENT_USER,
                  "Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders",
                  0, KEY_QUERY_VALUE, &hKey)
     == ERROR_SUCCESS)
  {
    if(RegQueryValueEx(hKey, "Programs", NULL, &dw1,
                       (BYTE *)csFolderName.GetBufferSetLength(dw2 = MAX_PATH), &dw2)
       == ERROR_SUCCESS)
    {
      csFolderName.ReleaseBuffer(dw2);
      csFolderName.ReleaseBuffer(-1);

      if(dw1 == REG_EXPAND_SZ)
      {
        CString csTemp99 = csFolderName;
        csFolderName = "";

        DWORD cbSize = csTemp99.GetLength() * 16 + MAX_PATH;
        dw1 = ExpandEnvironmentStrings(csTemp99, csFolderName.GetBufferSetLength(cbSize), cbSize);

        while(dw1 >= cbSize)
        {
          csFolderName.ReleaseBuffer(0);

          cbSize = dw1 * 2;
          dw1 = ExpandEnvironmentStrings(csTemp99, csFolderName.GetBufferSetLength(cbSize), cbSize);
        }

        if(!dw1)
        {
          csFolderName.ReleaseBuffer(0);
          csFolderName = csTemp99;
        }
        else
        {
          csFolderName.ReleaseBuffer(dw1);
        }
      }
    }
    else
    {
      csFolderName.ReleaseBuffer(0);
    }

    RegCloseKey(hKey);
  }
  
  if(!csFolderName.GetLength())  // the above thingy didn't work
  {
//    if(SHGetSpecialFolderPath(NULL, csFolderName.GetBufferSetLength(MAX_PATH),
//                              CSIDL_STARTMENU, FALSE)
//    {
//      csFolderName.ReleaseBuffer(-1);
//    }

    CString csUser;
    DWORD cbUserName = 4096;
    if(!GetUserName(csUser.GetBufferSetLength(cbUserName), &cbUserName))
    {
      csUser.ReleaseBuffer(0);
    }
    else
    {
      csUser.ReleaseBuffer(cbUserName);
    }

    csUser.ReleaseBuffer(-1);  // just 'cause

    if(!csUser.GetLength())
    {
      if(bMessageBoxFlag)
        MyMessageBox((IDS_ERROR65), MB_OK | MB_ICONHAND | MB_SETFOREGROUND);

      return FALSE;
    }

    csFolderName  = theApp.m_csWinPath
                  + "Profiles\\"
                  + csUser
                  + "\\Start Menu\\Programs";
  }

  if(bAllUsersFlag)
  {
    // get the 'All Users' shell folder name using *THIS* key
    // HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Explorer\Shell Folders
    // 'Common Start Menu' and 'Common Programs' do what they say, say what they do
    // the one I want is the 'Common Programs' folder

    // TODO:  consider using SHGetKnownFolderPath for vista and later
    //        [this may break in windows 10, apparently]

    if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                    "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders",
                    0, KEY_QUERY_VALUE, &hKey)
       == ERROR_SUCCESS)
    {
      if(RegQueryValueEx(hKey, "Common Programs", NULL, &dw1,
                         (BYTE *)csCommonFolderName.GetBufferSetLength(dw2 = MAX_PATH), &dw2)
         == ERROR_SUCCESS)
      {
        csCommonFolderName.ReleaseBuffer(dw2);
        csCommonFolderName.ReleaseBuffer(-1);

        if(dw1 == REG_EXPAND_SZ)
        {
          CString csTemp99 = csCommonFolderName;
          csCommonFolderName = "";

          DWORD cbSize = csTemp99.GetLength() * 16 + MAX_PATH;
          dw1 = ExpandEnvironmentStrings(csTemp99, csCommonFolderName.GetBufferSetLength(cbSize), cbSize);

          while(dw1 >= cbSize)
          {
            csCommonFolderName.ReleaseBuffer(0);

            cbSize = dw1 * 2;
            dw1 = ExpandEnvironmentStrings(csTemp99, csCommonFolderName.GetBufferSetLength(cbSize), cbSize);
          }

          if(!dw1)
          {
            csCommonFolderName.ReleaseBuffer(0);
            csCommonFolderName = csTemp99;
          }
          else
          {
            csCommonFolderName.ReleaseBuffer(dw1);
          }
        }
      }
      else
      {
        csCommonFolderName.ReleaseBuffer(0);
      }

      RegCloseKey(hKey);
    }
  
    if(!csCommonFolderName.GetLength())  // the above thingy didn't work
    {
//      if(SHGetSpecialFolderPath(NULL, csCommonFolderName.GetBufferSetLength(MAX_PATH),
//                                CSIDL_COMMON_STARTMENU, FALSE)
//      {
//        csCommonFolderName.ReleaseBuffer(-1);
//      }

      csCommonFolderName = theApp.m_csWinPath
                   + "Profiles\\All Users\\Start Menu\\Programs";
    }
  }

  if(csFolderName.GetLength())
    csFolderName = AdjustPathName2(csFolderName);

  if(csCommonFolderName.GetLength())
    csCommonFolderName = AdjustPathName2(csCommonFolderName);

  return TRUE;
}

CString CStartMenuFolderDlg::GetFolderName(BOOL &bAllUsersFlag)
{
  if(m_csFolderName.GetLength() >= 3 &&
     ((m_csFolderName[0] == '\\' && m_csFolderName[1] == '\\') ||
      m_csFolderName[1] == ':'))
  {
    m_csFolderName = AdjustPathName(m_csFolderName);

    // assume that it is a fully qualified path and return it 'as-is'

    if(CheckForAndCreateDirectory(m_csFolderName))
      return(m_csFolderName);

    // if the above thingy fails, then I flow through and prompt for it
  }

  CString csFolderName, csCommonFolderName;

  if(!GetStartMenuFolderName(bAllUsersFlag, csFolderName, csCommonFolderName))
    return("");

  csFolderName = AdjustPathName(AdjustPathName(csFolderName) + m_csFolderName);

  if(bAllUsersFlag && csCommonFolderName.GetLength())
  {
    csCommonFolderName = AdjustPathName(AdjustPathName(csCommonFolderName) + m_csFolderName);
  }

  CMyWaitCursor wait;

  if(bAllUsersFlag && csCommonFolderName.GetLength())
  {
    if(CheckForAndCreateDirectory(csCommonFolderName))
      return(csCommonFolderName);

    if(MyMessageBox((IDS_ERROR66),
                     MB_YESNO | MB_ICONQUESTION | MB_SETFOREGROUND)
       != IDYES)
    {
      return("");
    }

    wait.Restore();

    bAllUsersFlag = FALSE;

    // flow through...
  }

  if(CheckForAndCreateDirectory(csFolderName))
    return(csFolderName);

  MyMessageBox((IDS_ERROR67), MB_OK | MB_ICONHAND | MB_SETFOREGROUND);

  return("");
}

BOOL CStartMenuFolderDlg::CreateShortcuts(CString &csFolderName)
{
  CMyWaitCursor wait;

  // create the folder, first

  csFolderName = GetFolderName(m_bAllUsers);


  // at this time 'csFolderName' is correct and ends in a '\'
  // BEGIN by creating some shortcuts in it...

  // use 'IShellLink' and sample code 'CreateLink' from
  // MSVC 4.0 help file to create shortcuts for each EXE and
  // '.HLP' file that I find.

  // TODO:  add a 'create shortcut' section to the INF file ?

  HRESULT hres;
  CStringArray acsLine;

  // 'm_acsEntries' contains result from 'theApp.GetInfEntries("AddShortcut", m_acsEntries)'
  // [AddShortcut]
  // destfilename,Description,ShortcutName[,StartPath,Args,IconFile,IconIndex]

  BOOL bBogusUninstLnkFlag = FALSE;

  for(int i1=0; i1 < m_acsEntries.GetSize(); i1++)
  {
    // <destination path>,<display name>,<shortcut file name>
    DoParseString(m_acsEntries[i1], acsLine);

    if(acsLine.GetSize() < 3)  // invalid entry (disregard)
      continue;

    CString csTemp = acsLine[0];

    int i2 = csTemp.ReverseFind('\\');
    if(i2 < 0)
    {
      csTemp = AdjustPathName(theApp.m_csAppPath) + csTemp;
    }
    else
    {
      if(!theApp.ExpandStrings(csTemp))
      {
        csTemp.Format(theApp.LoadString(IDS_ERROR63), (LPCSTR)acsLine[0]);

        MyMessageBox(csTemp, MB_OK | MB_ICONHAND | MB_SETFOREGROUND);
        continue;
      }
    }

    if(csTemp.GetLength()) // just in case
    {
      LPCSTR lpszPath = NULL;
      LPCSTR lpszArgs = NULL;
      LPCSTR lpszIconFile = NULL;
      UINT uiIcon = (UINT)-1;

      CString csIconFile = "";

      if(acsLine.GetSize() > 3 && acsLine[3].GetLength())
        lpszPath = acsLine[3];

      if(acsLine.GetSize() > 4 && acsLine[4].GetLength())
        lpszArgs = acsLine[4];

      if(acsLine.GetSize() > 5 && acsLine[5].GetLength())
      {
        csIconFile = acsLine[5];

        if(csIconFile[0] == '$')  // indicating a system file with no path
        {
          csIconFile = csIconFile.Mid(1);
        }
        else
        {
          csIconFile = theApp.GetFileEntryDestPath(acsLine[5]);
        }

        if(csIconFile.GetLength())  // just in case
          lpszIconFile = csIconFile;
      }

      if(acsLine.GetSize() > 6 && acsLine[6].GetLength())
        uiIcon = atoi(acsLine[6]);

      int iCmdShow = SW_SHOWNORMAL;

      if(acsLine.GetSize() > 7 && acsLine[7].GetLength())
        iCmdShow = atoi(acsLine[7]);


      // check to see if I'm overwriting 'Un-Install.LNK' and if I am, don't
      // do a DAMN thing to it later!

      if(!csTemp.CompareNoCase(csFolderName + "Un-Install.LNK"))
        bBogusUninstLnkFlag = TRUE;

      hres = CreateLink(csTemp, 
                        csFolderName + acsLine[2],
                        acsLine[1],
                        lpszPath,
                        lpszArgs,
                        uiIcon,
                        lpszIconFile,
                        iCmdShow);

      if(hres != NOERROR)
      {
        csTemp = theApp.LoadString(IDS_ERROR68);

        MyMessageBox(csTemp + acsLine[2],
                      MB_OK | MB_ICONHAND | MB_SETFOREGROUND);
        return FALSE;
      }
    }
  }

  // create shortcut for uninstall..... (if I have already no big deal)
  // if it's an upgrade I must also delete the OLD shortcut (with a different name)

  if(!theApp.m_bNoUninstall)
  {
    // NOTE:  if I'm UPGRADING, delete any existing 'Un-Install.LNK' from this folder
    //        and assume that it used to point to *MY* uninstaller!

    if(theApp.m_bUpgrading && !bBogusUninstLnkFlag)
    {
      DeleteFile(csFolderName + "Un-Install.LNK");  // the old file (regardless)
    }

    hres = CreateLink(theApp.m_csUninstPath + "UNINST.EXE",
                      csFolderName
                      + "Un-Install "
                      + AppNameToPathName(theApp.m_csAppName)
                      + ".LNK",
                      "Un-Install " + theApp.m_csAppName + " and/or its optional components");

    if(hres != NOERROR)
    {
      MyMessageBox((IDS_ERROR69),
                    MB_OK | MB_ICONHAND | MB_SETFOREGROUND);
      return FALSE;
    }
  }

  theApp.m_csStartMenuFolderName = csFolderName;  // keep a record of it

  return TRUE;
}

void CStartMenuFolderDlg::OnBrowse()
{
  UpdateData(1);

  CString csFolderName, csCommonFolderName;

  if(!GetStartMenuFolderName(m_bAllUsers, csFolderName, csCommonFolderName, FALSE))
  {
    MessageBeep(0);
    return;
  }

  if(m_bAllUsers && csCommonFolderName.GetLength()
     && CheckForAndCreateDirectory(csCommonFolderName))
  {
    csFolderName = csCommonFolderName;
  }
  else
  {
    if(!csFolderName.GetLength() ||
       !CheckForAndCreateDirectory(csFolderName))
    {
      MessageBeep(0);
      return;
    }
    else if(m_bAllUsers)
      m_bAllUsers = FALSE;  // adjust it NOW - on 'UpdateData()' will re-adjust itself
  }

  csFolderName = AdjustPathName2(csFolderName);

  CPathBrowse dlg(csFolderName + '\\' + m_csFolderName);
  dlg.m_csRoot = csFolderName;

  if(m_bAllUsers)
    dlg.m_csTitle = "Start Menu Folder (All Users)";
  else
    dlg.m_csTitle = "Start Menu Folder";

  if(dlg.DoModal() == IDOK)
  {
    m_csFolderName = dlg.GetPathName();
    csFolderName = AdjustPathName(csFolderName);

    if(m_csFolderName.GetLength() &&
       !_strnicmp(csFolderName, m_csFolderName, csFolderName.GetLength()))
    {
      m_csFolderName = m_csFolderName.Mid(csFolderName.GetLength());
    }
  }

  UpdateData(0);
}
