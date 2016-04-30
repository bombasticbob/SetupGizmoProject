//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//       _           _          ____                                        //
//      / \   _   _ | |_  ___  |  _ \  _   _  _ __      ___  _ __   _ __    //
//     / _ \ | | | || __|/ _ \ | |_) || | | || '_ \    / __|| '_ \ | '_ \   //
//    / ___ \| |_| || |_| (_) ||  _ < | |_| || | | | _| (__ | |_) || |_) |  //
//   /_/   \_\\__,_| \__|\___/ |_| \_\ \__,_||_| |_|(_)\___|| .__/ | .__/   //
//                                                          |_|    |_|      //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//       Copyright (c) 1998-2016 by S.F.T. Inc. - All rights reserved       //
//  Use, copying, and distribution of this software are licensed according  //
//    to the GPLv2, LGPLv2, or BSD license, as appropriate (see COPYING)    //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "AutoRun.h"
#include "AutoRunDlg.h"
#include "LoginDlg.h"

//#pragma lib(shlwapi.dll)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#if 0
// SHLWAPI HELPER functions (so I don't need the DLL)
// This is so that Win '95 applications can load it!
#include "shlwapi.h"
#pragma warning(disable:4273)

extern "C" LPSTR STDAPICALLTYPE PathFindFileNameA(LPCTSTR pPath)
{
  LPCTSTR p1 = strrchr(pPath, '\\');

  if(p1)
    return((LPTSTR)(p1 + 1));
  else
    return((LPTSTR)pPath);
}

extern "C" LPSTR STDAPICALLTYPE PathFindExtensionA(LPCTSTR pPath)
{
  LPCTSTR p1 = strrchr(pPath, '\\');
  if(!p1)
    p1 = pPath;

  LPCTSTR p2 = strrchr(p1, '.');

  if(p2)
    return((LPTSTR)p2);
  else
    return((LPTSTR)(pPath + strlen(pPath)));
}

extern "C" void STDAPICALLTYPE PathRemoveExtension(LPTSTR pszPath)
{
  LPTSTR p1 = strrchr(pszPath, '\\');
  if(!p1)
    p1 = pszPath;

  LPTSTR p2 = strrchr(p1, '.');

  if(p2)
    *p2 = 0;
}

extern "C" BOOL STDAPICALLTYPE PathIsUNC(LPCTSTR pszPath)
{
  if(*pszPath != '\\' || pszPath[1] != '\\')
    return(FALSE);

  LPCTSTR p1 = strchr(pszPath + 2, '\\');
  if(!p1)
    return(FALSE);

  // TODO:  validate server name?  naw....

  return(TRUE);
}

extern "C" BOOL STDAPICALLTYPE PathStripToRoot(LPTSTR pszPath)
{
  if(*pszPath == '\\' && pszPath[1] == '\\')
  {
    // UNC paths - trim to \\server\share\, optionally add trailing '\'

    LPTSTR p1 = strchr(pszPath + 2, '\\');

    if(!p1)
      return(FALSE);

    p1 = strchr(p1 + 1, '\\');
    if(!p1)        // allow p1 to be NULL, though
      p1 = pszPath + strlen(pszPath);

    *p1 = '\\';    // making sure if no backslash before
    p1[1] = 0;  // but if not, include the '\'

    return(TRUE);
  }

  if(((pszPath[0] >= 'A' && pszPath[0] <= 'Z') ||
      (pszPath[0] >= 'a' && pszPath[0] <= 'z')) &&
     pszPath[1] == ':')
  {
    if(pszPath[2] != '\\')  // if not there add backslash
      pszPath[2] = '\\';

    pszPath[3] = 0;
    return(TRUE);
  }

  return(FALSE);
}

#endif // 0


/////////////////////////////////////////////////////////////////////////////
// CAutoRunApp

//BEGIN_MESSAGE_MAP(CAutoRunApp, CWinApp)
//	//{{AFX_MSG_MAP(CAutoRunApp)
//	//}}AFX_MSG
//	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
//END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAutoRunApp construction

CAutoRunApp::CAutoRunApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CAutoRunApp object

CAutoRunApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CAutoRunApp initialization

BOOL CAutoRunApp::InitInstance()
{
	// Standard initialization

//#ifdef _AFXDLL
//	Enable3dControls();			// Call this when using MFC in a shared DLL
//#else
//	Enable3dControlsStatic();	// Call this when linking to MFC statically
//#endif

//  // check for access rights
//
//  if(!(GetVersion() & 0x80000000L))  // windows NT/2k/XP/2k3
//  {
//    // Check user access rights to run SETUP.  If I can't get write access to HKLM, 
//    // the user has "insufficient privileges" and can't run SETUP.
//
//    HKEY hkeyTest = NULL;
//
//    if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE", 0, KEY_POWERUSER_ACCESS, &hkeyTest)
//       != ERROR_SUCCESS
//#ifdef _DEBUG
//       || MyMessageBox("DEBUG:  pretend I have to log in?", MB_YESNO | MB_ICONQUESTION | MB_SETFOREGROUND)
//          == IDYES
//#endif // _DEBUG
//       )
//    {
//      DWORD dwRval = 0xffffffff;
//
//      if((GetVersion() & 0xff) <= 4)
//      {
//        MyMessageBox("ERROR:  You do not have sufficient privileges on this system to run "
//                      "either the SETUP or UNINSTALL applications.\r\n"
//                      "If you want to run SETUP, you must log in as a different user first.\r\n"
//                      "This application will now close.\r\n",
//                      MB_OK | MB_ICONEXCLAMATION | MB_SETFOREGROUND);
//      }
//      else
//      {
//        CLoginDlg dlgLogin;
//
//        if(dlgLogin.DoModal() == IDOK)
//        {
//          CString csAppName, csCurDir;
//          GetModuleFileName(NULL, csAppName.GetBufferSetLength(MAX_PATH * 2), MAX_PATH * 2);
//          csAppName.ReleaseBuffer(-1);
//
//          GetCurrentDirectory(MAX_PATH * 2, csCurDir.GetBufferSetLength(MAX_PATH * 2));
//          csCurDir.ReleaseBuffer(-1);
//
////          csAppName = '"' + csAppName + '"';  // in case it's got spaces in the name
//
//          // convert to wide char using 'BSTR'
//          BSTR bstrUser = NULL, bstrPass = NULL, bstrDomain = NULL,
//               bstrModule = NULL, bstrArgs = NULL, bstrCurDir = NULL;
//
//          bstrUser = dlgLogin.m_csUserName.AllocSysString();
//          bstrPass = dlgLogin.m_csPassword.AllocSysString();
//
//          if(dlgLogin.m_csDomain.GetLength())
//            bstrDomain = dlgLogin.m_csDomain.AllocSysString();
//
//          bstrModule = csAppName.AllocSysString();
//          bstrArgs = (" " + (CString)theApp.m_lpCmdLine).AllocSysString();
//          bstrCurDir = csCurDir.AllocSysString();
//
//          STARTUPINFOW si;
//          memset(&si, 0, sizeof(si));
//          si.cb = sizeof(si);
//
//          PROCESS_INFORMATION pi;
//          memset(&pi, 0, sizeof(pi));
//
//          LPWSTR lpwEnv = GetEnvironmentStringsW();
//
//          if(CreateProcessWithLogonW(bstrUser, bstrDomain, bstrPass,
//                                     0, bstrModule, bstrArgs,
//                                     CREATE_NEW_PROCESS_GROUP | CREATE_UNICODE_ENVIRONMENT,
//                                     lpwEnv, bstrCurDir, &si, &pi))
//          {
//            dwRval = 0;
//
//            CloseHandle(pi.hProcess);
//            CloseHandle(pi.hThread);
//          }
//          else
//          {
//            DWORD dwErr = GetLastError();  // 1326 is 'logon failure'
//
//            TRACE("ERROR in 'CreateProcessWithLogonW' - %ld (%08xH)\r\n", dwErr, dwErr);
//
//            MyMessageBox("The login information you supplied is incorrect.  The application will now close.",
//                          MB_OK | MB_ICONHAND | MB_SETFOREGROUND);
//          }
//
//          if(bstrUser)
//            SysFreeString(bstrUser);
//          if(bstrPass)
//            SysFreeString(bstrPass);
//          if(bstrDomain)
//            SysFreeString(bstrDomain);
//          if(bstrModule)
//            SysFreeString(bstrModule);
//          if(bstrArgs)
//            SysFreeString(bstrArgs);
//          if(bstrCurDir)
//            SysFreeString(bstrCurDir);
//        }
//      }
//
//      MSG *pMsg = AfxGetCurrentMessage();
//      if(pMsg)
//      {
//        pMsg->message = WM_QUIT;   // this is specifically to force return(-1) on error
//        pMsg->wParam = 0xffffffff;
//        pMsg->lParam = 0xffffffff;
//      }
//
//      return(FALSE);
//    }
//
//    RegCloseKey(hkeyTest);  // now that I'm done with it...
//  }

	CAutoRunDlg dlg;

  int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
	}
	else if (nResponse == IDCANCEL)
	{
	}

  m_hWnd = NULL; // make sure

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

CString GetRegistryString(HKEY hkeyRoot, LPCSTR szKeyName, LPCSTR szString)
{
  // 99% of all registry queries are like this one

  HKEY hKey;

  if(RegOpenKeyEx(hkeyRoot, szKeyName, 0, KEY_QUERY_VALUE, &hKey)
     != ERROR_SUCCESS)
  {
    return("");
  }

  CString csRval;

  DWORD dwType, cbBuf=4096;

  if(RegQueryValueEx(hKey, szString, NULL, &dwType,
                     (LPBYTE)csRval.GetBufferSetLength(cbBuf), &cbBuf)
     == ERROR_SUCCESS)
  {
    csRval.ReleaseBuffer(cbBuf);
    csRval.ReleaseBuffer(-1);

    if(dwType == REG_EXPAND_SZ)
    {
      CString csTemp = csRval;
      csRval = "";

      DWORD cbSize = csTemp.GetLength() * 16 + MAX_PATH;
      DWORD dw1 = ExpandEnvironmentStrings(csTemp, csRval.GetBufferSetLength(cbSize), cbSize);

      while(dw1 >= cbSize)
      {
        csRval.ReleaseBuffer(0);

        cbSize = dw1 * 2;
        dw1 = ExpandEnvironmentStrings(csTemp, csRval.GetBufferSetLength(cbSize), cbSize);
      }

      if(!dw1)
      {
        csRval.ReleaseBuffer(0);
        csRval = csTemp;
      }
      else
      {
        csRval.ReleaseBuffer(dw1);
      }
    }
  }
  else
  {
    csRval.ReleaseBuffer(0);
  }

  RegCloseKey(hKey);

  return(csRval);
}

CString GetBugoSoftNotYetVersion()
{
  // Gets the installed version for 'BugoSoft Not Yet' aka 'Microsoft Dot Net'

  // To check for .net installation
  // [HKLM\SOFTWARE\Microsoft\.NETFramework]
  //   "InstallRoot"="c:\winnt\Microsoft.NET\Framework\"


  CString csTemp, csRval, csInstallRoot;

  HKEY hKey = NULL;
#if 0
  if(RegOpenKey(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\.NetFramework",
                &hKey)
     == ERROR_SUCCESS)
  {
    csInstallRoot = GetRegistryString(hKey, "", "InstallRoot");

    if(csInstallRoot.GetLength())
    {
      // get version information next (this is a hack)

      csTemp = GetRegistryString(HKEY_CLASSES_ROOT,
                                   "System.Version\\CLSID",
                                   "");

      if(!csTemp.GetLength())
        csTemp = GetRegistryString(HKEY_CLASSES_ROOT,
                                     "Microsoft.CLRAdmin.CAbout\\CLSID",
                                      "");

      if(csTemp.GetLength())
      {
        csRval = GetRegistryString(HKEY_CLASSES_ROOT,
                                     "CLSID\\" + csTemp
                                     + "\\InProcServer32",
                                     "RuntimeVersion");
      }
    }

    RegCloseKey(hKey);
  }
#endif // 0

  return(csRval);
}

BOOL ShellExecApp(LPCSTR szApp, LPCSTR szParms)
{
  BOOL bRval = TRUE;

  SHELLEXECUTEINFO sei;
  memset(&sei, 0, sizeof(sei));
  sei.cbSize = sizeof(sei);
  sei.fMask = SEE_MASK_FLAG_NO_UI |  SEE_MASK_NOCLOSEPROCESS;
  sei.lpFile = szApp;
  sei.lpParameters = szParms;
  sei.nShow = SW_SHOWNORMAL;

  CString csMsg;

  if(!ShellExecuteEx(&sei))
  {
    csMsg.Format(IDS_ERROR07, szApp);

    MyMessageBox(csMsg, MB_OK | MB_ICONHAND | MB_SETFOREGROUND);

    return(FALSE);  // this is really a FATAL error...
  }
  else
  {
    WaitForSingleObject(sei.hProcess, INFINITE);

    DWORD dwExitCode;
    if(!GetExitCodeProcess(sei.hProcess, &dwExitCode))
    {
      csMsg.Format(IDS_WARNING07, szApp);

      MyMessageBox(csMsg, MB_OK | MB_ICONASTERISK | MB_SETFOREGROUND);

      bRval = FALSE;
    }
    else if(dwExitCode == STILL_ACTIVE)
    {
      csMsg.Format(IDS_WARNING08, szApp);

      MyMessageBox(csMsg, MB_OK | MB_ICONASTERISK | MB_SETFOREGROUND);

      bRval = FALSE;
    }
    else if(dwExitCode)
    {
      csMsg.Format(IDS_WARNING09, szApp, dwExitCode);

      MyMessageBox(csMsg, MB_OK | MB_ICONASTERISK | MB_SETFOREGROUND);

      bRval = FALSE;
    }
  }

  return(bRval);
}

CString AdjustPathName2(LPCSTR szPathName)
{
  CString csRval = szPathName;

  if(csRval.GetLength() /*> 3*/ && csRval[csRval.GetLength() - 1] == '\\')
    csRval = csRval.Left(csRval.GetLength() - 1);  // no ending backslash

  return(csRval);
}

BOOL CheckForExistence(LPCSTR szPath, BOOL &bIsDir, BOOL &bIsDrive)
{
  CString csPath = szPath;

  bIsDrive = 0;
  bIsDir = 0;

  if(csPath.GetLength() == 3 &&
     toupper(csPath[0]) >= 'A' &&
     toupper(csPath[0]) <= 'Z' &&
     csPath[1] == ':' &&
     csPath[2] == '\\')
  {
    UINT uiDriveType = GetDriveType(csPath);  // see if it's valid
    bIsDrive = TRUE;

    if(uiDriveType == DRIVE_UNKNOWN || uiDriveType == DRIVE_NO_ROOT_DIR)
    {
      return(FALSE);
    }

    bIsDir = TRUE;  // because a drive and a directory are kinda the same
    return(TRUE);  // assume valid
  }

  // if it's not a root directory, and it ends in a backslash,
  // we want to get RID of the ending backslash.

  csPath = AdjustPathName2(csPath);


  WIN32_FIND_DATA fd;
  HANDLE hFF = FindFirstFile(csPath, &fd);

  if(hFF != INVALID_HANDLE_VALUE)
  {
    bIsDir = (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;

    FindClose(hFF);

    if(!bIsDir)
    {
      // don't give up, it might be a 'share'

      if(csPath.GetLength() > 2 &&
         csPath[0] == '\\' && csPath[1] == '\\')
      {
        // so far so good... see if there's a share name

        LPCSTR lp1 = strchr((LPCSTR)csPath + 2, '\\');

        if(lp1)  // has a share name
        {
          lp1 = strchr(lp1 + 1, '\\');  // find next one

          if(!lp1 || !*(lp1 + 1))  // end of string or no more '\'
          {
            bIsDir = TRUE;
          }
        }
      }
    }

    return(TRUE); // it *does* exist!!
  }

  return(FALSE);  // it does *NOT* exist
}

BOOL FileExists(LPCSTR szFileName)
{
  BOOL bIsDir = FALSE, bIsDrive = FALSE;

  return(CheckForExistence(szFileName, bIsDir, bIsDrive)
         && !bIsDir && !bIsDrive);
}

