//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//         _   _  _   _  ___  _   _  ____  _____                            //
//        | | | || \ | ||_ _|| \ | |/ ___||_   _|  ___  _ __   _ __         //
//        | | | ||  \| | | | |  \| |\___ \  | |   / __|| '_ \ | '_ \        //
//        | |_| || |\  | | | | |\  | ___) | | | _| (__ | |_) || |_) |       //
//         \___/ |_| \_||___||_| \_||____/  |_|(_)\___|| .__/ | .__/        //
//                                                     |_|    |_|           //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//       Copyright (c) 1998-2016 by S.F.T. Inc. - All rights reserved       //
//  Use, copying, and distribution of this software are licensed according  //
//    to the GPLv2, LGPLv2, or BSD license, as appropriate (see COPYING)    //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "UNINST.h"
#include "UNINSTDlg.h"
#include "ProgDlg.h"
#include "LoginDlg.h"
#include "RemoveSharedDlg.h"

#include "lmcons.h"  // basic type information for 'Net' API functions
#include "lmerr.h"
#include "lmshare.h"



struct share_info_50
{  // from 'svrapi.h' - must *NOT* include it!
  char shi50_netname[LM20_NNLEN+1];    /* share name */
  unsigned char shi50_type;                 /* see below */
  unsigned short shi50_flags;                /* see below */
  char FAR * shi50_remark;                   /* ANSI comment string */
  char FAR * shi50_path;                     /* shared resource */
  char shi50_rw_password[SHPWLEN+1];   /* read-write password (share-level security) */
  char shi50_ro_password[SHPWLEN+1];   /* read-only password (share-level security) */
};  /* share_info_50 */



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// simple utility functions

static int __cdecl StringSortCompareProc(LPCVOID p1, LPCVOID p2)
{
  LPCSTR *lp1 = (LPCSTR *)p1;
  LPCSTR *lp2 = (LPCSTR *)p2;

  return(_stricmp(*lp1, *lp2));  // no-case comparison
}

static int __cdecl KeyInfoSortCompareProc(LPCVOID p1, LPCVOID p2)
{
  MYKEYINFO *lp1 = (MYKEYINFO *)p1;
  MYKEYINFO *lp2 = (MYKEYINFO *)p2;

  if(lp1->hkeyRoot < lp2->hkeyRoot)
    return(-1);
  else if(lp1->hkeyRoot > lp2->hkeyRoot)
    return(1);
  else
    return(_stricmp(lp1->szKey, lp2->szKey));
}


#include ".\uninst.h"
#if 0
// SHLWAPI HELPER functions (so I don't need the DLL)
// This is so that Win '95 applications can load it!
#include "shlwapi.h"
#include ".\uninst.h"
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
// CUNINSTApp

//BEGIN_MESSAGE_MAP(CUNINSTApp, CMyApp)
//  //{{AFX_MSG_MAP(CUNINSTApp)
//    // NOTE - the ClassWizard will add and remove mapping macros here.
//    //    DO NOT EDIT what you see in these blocks of generated code!
//  //}}AFX_MSG
//  ON_COMMAND(ID_HELP, CMyApp::OnHelp)
//END_MESSAGE_MAP()


static const char szSETUP_KEY[]="SOFTWARE\\Stewart~Frazier Tools\\SETUP\\";
static const char szUNINST_KEY[]="SOFTWARE\\Stewart~Frazier Tools\\UNINST\\";
static const char szSHARED_DLLS[]="SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\SharedDLLs";
static const char szHKLM_WINDOWS_CURRENT_VERSION[] = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion";


/////////////////////////////////////////////////////////////////////////////
// CUNINSTApp construction

CUNINSTApp::CUNINSTApp()
: CMyApp("UNINSTALL")  // make sure the app name reflects this...
{
  // TODO: add construction code here,
  // Place all significant initialization in InitInstance

  // OS information - do this during construction

  OSVERSIONINFOEX osv;
  memset(&osv, 0, sizeof(osv));

  osv.dwOSVersionInfoSize = sizeof(osv);

  if(!GetVersionEx((OSVERSIONINFO *)&osv))
  {
    m_bIsNT = !(GetVersion() & 0x80000000L);
    m_dwMajorVer = GetVersion() & 0xff;
    m_dwMinorVer = ((GetVersion() & 0xff00) >> 8) & 0xff;

    m_wSuiteMask = 0;
    m_wSPMajor = 0;
    m_wSPMinor = 0;
  }
  else
  {
    m_bIsNT = osv.dwPlatformId == VER_PLATFORM_WIN32_NT;
    m_dwMajorVer = osv.dwMajorVersion;
    m_dwMinorVer = osv.dwMinorVersion;

    m_wSuiteMask = osv.wSuiteMask;
    m_wSPMajor = osv.wServicePackMajor;
    m_wSPMinor = osv.wServicePackMinor;
  }

  m_csOS = "";
  m_csOSName = "";

  if(!m_bIsNT)
  {
    if(m_dwMajorVer >= 4)
    {
      if(m_dwMinorVer < 10)
      {
        m_csOS = "Win95";
        m_csOSName = "Windows 95";
      }
      else if(m_dwMinorVer < 90)
      {
        m_csOS = "Win98";
        m_csOSName = "Windows 98";
      }
      else // if(m_dwMinorVer >= 90)
      {
        m_csOS = "WinME";
        m_csOSName = "Windows ME";
      }

    }
  }
  else // windows NT/2000/XP/Server 2003
  {
    if(m_dwMajorVer == 4)
    {
      m_csOS = "WinNT4";
      m_csOSName = "Windows NT 4.0";
    }
    else if(m_dwMajorVer == 5 && m_dwMinorVer == 0)
    {
      m_csOS = "Win2000";
      m_csOSName = "Windows 2000";
    }
    else if(m_dwMajorVer == 5 && m_dwMinorVer == 1)
    {
      m_csOS = "WinXP"; // either 'home' or 'pro'
      m_csOSName = "Windows XP";
    }
    else if(m_dwMajorVer == 5 && m_dwMinorVer == 2)
    {
      m_csOS = "WSrv2003";
      m_csOSName = "Win Server 2003";
    }
    else if(m_dwMajorVer > 5 ||
            (m_dwMajorVer == 5 && m_dwMinorVer > 2))
    {
      m_csOS = "NEW_WINOS";
      m_csOSName.Format("Windows %d.%d", m_dwMajorVer, m_dwMinorVer);
    }
  }


  // additional information

  if(m_wSuiteMask & VER_SUITE_BACKOFFICE)
  {
    // back office
  }

  if(m_wSuiteMask & VER_SUITE_BLADE)
  {
    // Server 2003 "web edition"
  }

  if(m_wSuiteMask & VER_SUITE_DATACENTER)
  {
    // Win 2k Datacenter Server
    // Server 2003 "data center edition"
  }

  if(m_wSuiteMask & VER_SUITE_ENTERPRISE)
  {
    // any 'enterprise' server
  }

  if(!m_csOS.CompareNoCase("WinXP"))
  {
    if(m_wSuiteMask & VER_SUITE_PERSONAL)
    {
      m_csOSName += " 'home'";  // Win XP 'home' edition
    }
    else
    {
      m_csOSName += " 'pro'";
    }
  }

  if(m_wSuiteMask & VER_SUITE_TERMINAL)
  {
    // terminal services is installed
  }

}

/////////////////////////////////////////////////////////////////////////////
// The one and only CUNINSTApp object

CUNINSTApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CUNINSTApp initialization



BOOL CUNINSTApp::InitInstance()
{
  if(!m_csOS.GetLength())
  {
    MyMessageBox(IDS_UNSUP_OS, MB_OK | MB_ICONASTERISK | MB_SETFOREGROUND);
  }

  CMyWaitCursor wait;

  int i1, i2, i3;

  CoInitialize(NULL);  // initialize COM (but not OLE) and ignore return

  // initialize DDE
  if(!CMyApp::InitInstance())
  {
    return(FALSE);
  }

  // Initialize OLE libraries
//  if(!AfxOleInit())
//  {
//    MyMessageBox("UNINSTALL - OLE Init Failed");
//    return FALSE;
//  }
//
//  AfxEnableControlContainer();
//  Enable3dControlsStatic(); // Call this when linking to MFC statically


  // check for access rights

  if(m_bIsNT)
  {
    // Check user access rights to run SETUP.  If I can't get write access to HKLM, 
    // the user has "insufficient privileges" and can't run SETUP.

    HMODULE hAdvAPI32 = GetModuleHandle("ADVAPI32.DLL");
    HMODULE hKernel32 = GetModuleHandle("KERNEL32.DLL");

    LPVOID (WINAPI *pGetEnvironmentStringsW)(void) = NULL;
    BOOL (WINAPI *pCreateProcessWithLogonW)(LPCWSTR,LPCWSTR,LPCWSTR,DWORD,LPCWSTR,
                                            LPWSTR,DWORD,LPVOID,LPCWSTR,LPSTARTUPINFOW,
                                            LPPROCESS_INFORMATION) = NULL;

    if(hAdvAPI32)
      (FARPROC &)pCreateProcessWithLogonW = GetProcAddress(hAdvAPI32, "CreateProcessWithLogonW");

    if(hKernel32)
      (FARPROC &)pGetEnvironmentStringsW = GetProcAddress(hKernel32, "GetEnvironmentStringsW");


    HKEY hkeyTest = NULL;

    if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE", 0, KEY_POWERUSER_ACCESS, &hkeyTest)
       != ERROR_SUCCESS
#ifdef _DEBUG
       || MyMessageBox("DEBUG:  pretend I have to log in?", MB_YESNO | MB_ICONQUESTION | MB_SETFOREGROUND)
          == IDYES
#endif // _DEBUG
       )
    {
      // NT versions prior to or equal to 4 will have trouble with this so bail

      if((GetVersion() & 0xff) <= 4)
      {
        MyMessageBox("You do not have sufficient privileges on this system to run SETUP.\r\n"
                      "SETUP will now close.",
                      MB_OK | MB_ICONEXCLAMATION | MB_SETFOREGROUND);

        MSG *pMsg = MyGetCurrentMessage();
        if(pMsg)
        {
          pMsg->message = WM_QUIT;   // this is specifically to force return(-1) on error
          pMsg->wParam = 0xffffffff;
          pMsg->lParam = 0xffffffff;
        }
      }
      else
      {
        CLoginDlg dlgLogin;

        if(dlgLogin.DoModal() != IDOK)
        {
#ifdef _DEBUG
          if(MyMessageBox("Do you want to continue anyway?",
                          MB_YESNO | MB_ICONQUESTION | MB_SETFOREGROUND)
             == IDYES)
          {
            goto I_have_the_permissions; // makes program flow more 'sane' to use 'goto' for a "but if" condition
          }
#endif // _DEBUG
        }
        else
        {
          CString csAppName, csCurDir;
          GetModuleFileName(NULL, csAppName.GetBufferSetLength(MAX_PATH * 2), MAX_PATH * 2);
          csAppName.ReleaseBuffer(-1);

          GetCurrentDirectory(MAX_PATH * 2, csCurDir.GetBufferSetLength(MAX_PATH * 2));
          csCurDir.ReleaseBuffer(-1);


          // convert to wide char using 'BSTR'
          BSTR bstrUser = NULL, bstrPass = NULL, bstrDomain = NULL,
              bstrModule = NULL, bstrArgs = NULL, bstrCurDir = NULL;

          bstrUser = dlgLogin.m_csUserName.AllocSysString();
          bstrPass = dlgLogin.m_csPassword.AllocSysString();

          if(dlgLogin.m_csDomain.GetLength())
            bstrDomain = dlgLogin.m_csDomain.AllocSysString();

          bstrModule = csAppName.AllocSysString();
          bstrArgs = (" " + (CString)theApp.m_lpCmdLine).AllocSysString();
          bstrCurDir = csCurDir.AllocSysString();

          STARTUPINFOW si;
          memset(&si, 0, sizeof(si));
          si.cb = sizeof(si);

          PROCESS_INFORMATION pi;
          memset(&pi, 0, sizeof(pi));

          LPWSTR lpwEnv = pGetEnvironmentStringsW ? (LPWSTR)pGetEnvironmentStringsW()
                                                  : NULL;

          BOOL bSuccess = FALSE;

          if(pCreateProcessWithLogonW(bstrUser, bstrDomain, bstrPass,
                                      0, bstrModule, bstrArgs,
                                      (lpwEnv ? CREATE_NEW_PROCESS_GROUP | CREATE_UNICODE_ENVIRONMENT
                                              : CREATE_NEW_PROCESS_GROUP),
                                      lpwEnv, bstrCurDir, &si, &pi))
          {
            bSuccess = TRUE;

            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
          }
          else
          {
            DWORD dwErr = GetLastError();  // 1326 is 'logon failure'

            TRACE("ERROR in 'CreateProcessWithLogonW' - %ld (%08xH)\r\n", dwErr, dwErr);

            MyMessageBox("Unable to log in with the specifed user/password",
                          MB_OK | MB_ICONHAND | MB_SETFOREGROUND);
          }

          if(bstrUser)
            SysFreeString(bstrUser);
          if(bstrPass)
            SysFreeString(bstrPass);
          if(bstrDomain)
            SysFreeString(bstrDomain);
          if(bstrModule)
            SysFreeString(bstrModule);
          if(bstrArgs)
            SysFreeString(bstrArgs);
          if(bstrCurDir)
            SysFreeString(bstrCurDir);

          MSG *pMsg = MyGetCurrentMessage();
          if(pMsg)
          {
            pMsg->message = WM_QUIT;   // this is specifically to force return(-1) on error
            if(bSuccess)
            {
              pMsg->wParam = 0;
              pMsg->lParam = 0;
            }
            else
            {
              pMsg->wParam = 0xffffffff;
              pMsg->lParam = 0xffffffff;
            }
          }
          return(FALSE);
        }
      }

      MSG *pMsg = MyGetCurrentMessage();
      if(pMsg)
      {
        pMsg->message = WM_QUIT;   // this is specifically to force return(-1) on error
        pMsg->wParam = 0xffffffff;
        pMsg->lParam = 0xffffffff;
      }

      return(FALSE);
    }

#ifdef _DEBUG
I_have_the_permissions:
#endif _DEBUG

    if(hkeyTest)
    {
      RegCloseKey(hkeyTest);  // now that I'm done with it...
    }
  }

  // initialize common path information

  // NOTE:  if I'm windows 2000 or later, I must call 'GetSystemWindowsDirectory'
  //        Otherwise, call 'GetWindowsDirectory'

  if(m_bIsNT && m_dwMajorVer >= 5)   // Win 2000 or later...
  {
    UINT (WINAPI *pGetSystemWindowsDirectory)(LPSTR, UINT) = NULL;

    HMODULE hK = GetModuleHandle("KERNEL32.DLL");

    if(hK)
    {
      (FARPROC &)pGetSystemWindowsDirectory = GetProcAddress(hK, "GetSystemWindowsDirectoryA");
    }

    // THIS IS REQUIRED BECAUSE OF TERMINAL SERVICES!

    if(pGetSystemWindowsDirectory)
    {
      pGetSystemWindowsDirectory(m_csWinPath.GetBufferSetLength(MAX_PATH), MAX_PATH);
    }
    else
    {
      GetWindowsDirectory(m_csWinPath.GetBufferSetLength(MAX_PATH), MAX_PATH);
    }
  }
  else
  {
    GetWindowsDirectory(m_csWinPath.GetBufferSetLength(MAX_PATH), MAX_PATH);
  }

  m_csWinPath.ReleaseBuffer(-1);

  m_csWinPath = AdjustPathName(m_csWinPath);

  GetSystemDirectory(m_csSysPath.GetBufferSetLength(MAX_PATH), MAX_PATH);
  m_csSysPath.ReleaseBuffer(-1);

  m_csSysPath = AdjustPathName(m_csSysPath);


  // If this program was launched directly, make a copy of it and
  // put the copy into a temporary location, then run the copy with
  // the command line "/UNINST" and point the file directory
  // here.

  CString csTemp, csTemp2, csTemp3, csMsg;

  if(!m_lpCmdLine || !*m_lpCmdLine ||
     _strnicmp(m_lpCmdLine, "/UNINST", 7))
  {
#ifdef _DEBUG
    if(!_strnicmp(m_lpCmdLine, "/TEST", 5) &&
       m_lpCmdLine[5] <= ' ')
    {
      CString csTemp = m_lpCmdLine + 5;

      csTemp.TrimRight();
      csTemp.TrimLeft();

      if(csTemp.GetLength())
      {
        if(PathHasFiles(csTemp))
          MyMessageBox("HAS FILES: " + csTemp);

        if(MyMessageBox("Nuke " + csTemp,
                         MB_YESNO | MB_ICONHAND)
           == IDYES)
        {
          NukeDirectory(csTemp);
        }
      }

      return(FALSE);
    }
#endif // _DEBUG

    // make a copy of myself and run it.....

    csTemp = "";
    csTemp2 = "";

    GetModuleFileName(NULL, csTemp2.GetBufferSetLength(MAX_PATH), MAX_PATH);
    csTemp2.ReleaseBuffer(-1);

    if(!csTemp2.GetLength())
    {
      MyMessageBox(IDS_ERROR01, MB_OK | MB_ICONHAND | MB_SETFOREGROUND);
      return(FALSE);
    }

    GetShortPathName(csTemp2, csTemp.GetBufferSetLength(MAX_PATH), MAX_PATH);
    csTemp.ReleaseBuffer(-1);

    if(!csTemp.GetLength())
      csTemp = csTemp2;  // just in case (may screw up later)

    csTemp2 = "";

    // create a temporary file name
    CString csTempPath;

    GetTempPath(MAX_PATH, csTempPath.GetBufferSetLength(MAX_PATH));
    csTempPath.ReleaseBuffer(-1);

    if(!csTempPath.GetLength())
    {
      csTempPath = m_csWinPath + "TEMP";
      CreateDirectory(csTempPath, NULL);  // ignore errors
    }

    if(!GetTempFileName(csTempPath, "~UN", 0,
                        csTemp2.GetBufferSetLength(MAX_PATH)))
    {
      csTemp2.ReleaseBuffer(0);
      MyMessageBox(IDS_ERROR02, MB_OK | MB_ICONHAND | MB_SETFOREGROUND);
      return(FALSE);
    }

    csTemp2.ReleaseBuffer(-1);

    csTemp2.TrimRight();
    csTemp2.TrimLeft();

    if(!csTemp2.GetLength())
    {
      MyMessageBox(IDS_ERROR03, MB_OK | MB_ICONHAND | MB_SETFOREGROUND);
      return(FALSE);
    }

    if(!CopyFile(csTemp, csTemp2, FALSE))
    {
      MyMessageBox(IDS_ERROR04, MB_OK | MB_ICONHAND | MB_SETFOREGROUND);

      DeleteFile(csTemp2);
      return(FALSE);
    }

    csTemp3.Format("%ld,", GetCurrentThreadId()); // thread ID is first parameter

    // NOW, execute it....
    csTemp = "\""
           + csTemp2         // the program to run (always quote it)
           + "\" /UNINST="   // the '/UNINST=' options
           + csTemp3         // the main thread ID followed by a ','
           + csTemp;         // the original EXE path for UNINST (to find SETUP.INF)

    if(m_lpCmdLine && *m_lpCmdLine)
    {
      csTemp += " "
             + (CString)m_lpCmdLine;
    }

//    if((UINT)WinExec(csTemp, SW_SHOWNORMAL)
//       <= 32)
//    {

    STARTUPINFO si;
    memset(&si, 0, sizeof(si));
    si.cb = sizeof(si);

    PROCESS_INFORMATION pi;
    memset(&pi, 0, sizeof(pi));

    if(!CreateProcess(NULL, (LPSTR)(LPCSTR)csTemp, NULL, NULL, FALSE,
                      CREATE_DEFAULT_ERROR_MODE | NORMAL_PRIORITY_CLASS,
                      NULL, (LPCSTR)csTempPath, &si, &pi))
    {
      MyMessageBox(IDS_ERROR05, MB_OK | MB_ICONHAND | MB_SETFOREGROUND);

      DeleteFile(csTemp2);
      return(FALSE);
    }

    ::PeekMessage(MyGetCurrentMessage(), 0, 0, 0, PM_NOREMOVE);  // forces a message queue to be created

    // wait for one of two things:  a) a WM_QUIT message, or b) the process to end

    while(1)
    {
      DWORD dwRval = ::MsgWaitForMultipleObjects(1, &pi.hProcess, FALSE, 100, QS_ALLEVENTS);

      if(dwRval == WAIT_OBJECT_0)  // the process exited
      {
        // exit with the same code

        GetExitCodeProcess(pi.hProcess, &dwRval);

        MSG *pMsg = MyGetCurrentMessage();
        if(pMsg)
        {
          pMsg->message = WM_QUIT;
          pMsg->wParam = dwRval;  // this sort of does what I want
          pMsg->lParam = 0;
        }

        break;
      }
      else // if(dwRval == WAIT_OBJECT_0 + 1)  just ignore other states, just b'cause
      {
        MSG *pMsg = MyGetCurrentMessage();
        if(pMsg)
        {
          if(::PeekMessage(pMsg, 0, 0, 0, PM_REMOVE))
          {
            if(pMsg->message == WM_QUIT ||
               pMsg->message == WM_QUERYENDSESSION ||
               pMsg->message == WM_ENDSESSION)
            {
              break;
            }

            // eat all other messages
          }
        }
      }
    }

    return(FALSE);  // this one must now finish
  }


  // now, 'csCmdLine' must contain everything AFTER the '/UNINST'

  CString csCmdLine;
  
  if(m_lpCmdLine && !_strnicmp(m_lpCmdLine, "/UNINST", 7))
    csCmdLine = m_lpCmdLine + 7;  // points to the '='
  else
    csCmdLine = "=0," + (CString)m_lpCmdLine;  // build an appropriate command line

  csCmdLine.TrimRight();

  DWORD dwStartThreadID = 0;

  if(csCmdLine.GetLength() && csCmdLine[0] == '=')
  {
    csCmdLine = csCmdLine.Mid(1);

    if(csCmdLine.GetLength() && csCmdLine[0] > ' ')  // not "white space"
    {
      i1 = csCmdLine.Find(',');  // find the first comma
      if(i1 >=0 || i1 < 11)  // no more than 11 digits allowed - assume it's an arg if >= 11
      {
        for(i2=0; i2 < i1; i2++)
        {
          if((csCmdLine[i2] < '0' || csCmdLine[i2] > '9') &&
             csCmdLine[i2] != '-')  // allow a '-' in it
          {
            break;
          }
        }

        if(i2 >= i1)
        {
          dwStartThreadID = atol(csCmdLine.Left(i1));
           // NOTE:  this may be zero!

          csCmdLine = csCmdLine.Mid(i1 + 1);  // eliminate all text through the ','
        }
      }
//    }
//
//    if(csCmdLine.GetLength() && csCmdLine[0] > ' ')  // not "white space"
//    {
      if(csCmdLine.GetLength() && csCmdLine[0] == '"')
      {
        csCmdLine = csCmdLine.Mid(1);
        for(i1=0; i1 < csCmdLine.GetLength(); i1++)
        {
          if(csCmdLine[i1] == '"')
            break;
        }

        csCmdLine = csCmdLine.Left(i1)
                  + csCmdLine.Mid(i1 + 1);  // edit out the quote
      }
      else
      {
        for(i1=0; i1 < csCmdLine.GetLength(); i1++)
        {
          if(csCmdLine[i1] <= ' ')
            break;
        }
      }

      csTemp = csCmdLine.Left(i1);
      csCmdLine = csCmdLine.Mid(i1);

      // get EXE path, where I expect to find SETUP.INF as well...

      if(csTemp.GetLength())
      {
        LPSTR lp1;
        GetFullPathName(csTemp, MAX_PATH, 
                        m_csEXEPath.GetBufferSetLength(MAX_PATH), &lp1);

        m_csEXEPath.ReleaseBuffer(-1);
      }
    }
  }
  else
  {
    m_csEXEPath = "";
  }

  csCmdLine.TrimLeft();

  if(!m_csEXEPath.GetLength())
  {
    GetModuleFileName(NULL, m_csEXEPath.GetBufferSetLength(MAX_PATH), MAX_PATH);
    m_csEXEPath.ReleaseBuffer(-1);
  }

  i1 = m_csEXEPath.ReverseFind('\\');
  m_csEXEPath = m_csEXEPath.Left(i1 + 1);  // include the backslash

  m_csEXEPath = AdjustPathName(m_csEXEPath);  // make sure


  // open the INF file, read into 'm_csINFFile'

  HANDLE hFile = CreateFile(m_csEXEPath + "SETUP.INF",
                            GENERIC_READ,
                            0, NULL, OPEN_EXISTING,
                            FILE_ATTRIBUTE_NORMAL, NULL);

  if(hFile == INVALID_HANDLE_VALUE)
  {
    // one more try - see if it's in the 'DISK1' sub-directory...

    hFile = CreateFile(m_csEXEPath + "DISK1\\SETUP.INF",
                       GENERIC_READ,
                       0, NULL, OPEN_EXISTING,
                       FILE_ATTRIBUTE_NORMAL, NULL);

    if(hFile == INVALID_HANDLE_VALUE)
    {
      // still one more try - see if it's in the 'SETUP' sub-directory...

      hFile = CreateFile(m_csEXEPath + "SETUP\\SETUP.INF",
                         GENERIC_READ,
                         0, NULL, OPEN_EXISTING,
                         FILE_ATTRIBUTE_NORMAL, NULL);

      if(hFile == INVALID_HANDLE_VALUE)
      {
        MyMessageBox(IDS_ERROR06, MB_OK | MB_ICONHAND | MB_SETFOREGROUND);
        return(FALSE);
      }
      else
      {
        m_csEXEPath += "SETUP\\";  // 'udpated path' for EXE file
                                   // allows SETUP to be in root of CD
      }
    }
    else
    {
      m_csEXEPath += "DISK1\\";  // 'udpated path' for EXE file
                                 // allows SETUP to be in root of CD
    }
  }


  DWORD dw1, cb1, cbFile = GetFileSize(hFile, NULL);

  if(!ReadFile(hFile, m_csINFFile.GetBufferSetLength(cbFile), cbFile, &cb1, NULL)
     || cb1 != cbFile)
  {
    m_csINFFile.ReleaseBuffer(0);
    CloseHandle(hFile);

    MyMessageBox(IDS_ERROR07, MB_OK | MB_ICONHAND | MB_SETFOREGROUND);
    return(FALSE);
  }
              
  m_csINFFile.ReleaseBuffer(cbFile);
  CloseHandle(hFile);


  m_csAppName = GetInfEntry("SetupOptions", "AppName");
  m_csCompanyName = GetInfEntry("SetupOptions", "CompanyName");

  BOOL bNoUninstall = GetInfEntry("SetupOptions", "NoUninstall") == 1;


  // get 'FILES' entries using same method as before
  // (I may need some info from there)

  GetInfEntries("Files", m_acsFiles);

  CStringArray acsTemp;
  GetInfEntries("Files." + m_csOS, acsTemp);

  for(i1=0; i1 < acsTemp.GetSize(); i1++)
  {
    m_acsFiles.Add(acsTemp[i1]);  // add these to the list of files
  }

  acsTemp.RemoveAll();

  // Get 'options' (all of them, for THIS CPU)

  CStringArray acsOptions;
  GetInfEntries("Options", acsOptions);

  for(i2=0; i2 < acsOptions.GetSize(); i2++)
  {
    DoParseString(acsOptions[i2], acsTemp);

    if(acsTemp.GetSize() < 3)
      continue;  // not valid

    CString csOption = acsTemp[0];

    GetInfEntries("Option." + csOption, acsTemp);
    for(i1=0; i1 < acsTemp.GetSize(); i1++)
    {
      m_acsFiles.Add(acsTemp[i1]);  // add these to the list of files
    }

    GetInfEntries("Option." + csOption + "." + m_csOS, acsTemp);
    for(i1=0; i1 < acsTemp.GetSize(); i1++)
    {
      m_acsFiles.Add(acsTemp[i1]);  // add these to the list of files
    }
  }

  // at THIS point, 'm_acsFiles' contains all of the entries
  // that could have possibly been installed on THIS machine...


  // NEXT, get the 'uninstall' entries.

  GetInfEntries("UnInstall", m_acsUnInstall);
  GetInfEntries("UnInstall.Shared", m_acsUnInstallShared);

  if(!m_acsUnInstall.GetSize() && !m_acsUnInstallShared.GetSize())
  {
    if(bNoUninstall)
      MyMessageBox(IDS_WARNING01,
                    MB_OK | MB_ICONHAND | MB_SETFOREGROUND);
    else
      MyMessageBox(IDS_WARNING02,
                    MB_OK | MB_ICONHAND | MB_SETFOREGROUND);

    return(FALSE);
  }
  else if(bNoUninstall)
  {
    MyMessageBox(IDS_WARNING03,
                  MB_OK | MB_ICONHAND | MB_SETFOREGROUND);
  }

  GetInfEntries("SharedComponents", m_acsShared);

  // READ THE REGISTRY, AND GET INFORMATION ABOUT THIS UNINSTALL

  SetRegistryKey("Stewart~Frazier Tools");  // I'll be using my own, here

  // REGISTRY KEYS:of interest are....
  //
  // HKLM\\SOFTWARE\\Stewart~Frazier Tools\\UNINST\\{app name}
  // HKLM\\SOFTWARE\\Stewart~Frazier Tools\\UNINST\\{app name}\\AppCommon
  // HKLM\\SOFTWARE\\Stewart~Frazier Tools\\UNINST\\{app name}\\SharedDLLs
  // HKLM\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{app name}
  // HKLM\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\SharedDLLs

  HKEY hkeyUninst=NULL, hKey;
  CString csUninstKey = szUNINST_KEY + AppNameToRegKey(m_csAppName);
  CString csSetupKey = szSETUP_KEY + AppNameToRegKey(m_csAppName);

                         // "SOFTWARE\\Microsoft\\Windows\\CurrentVersion"
  CString csSysUninstKey = AdjustPathName(szHKLM_WINDOWS_CURRENT_VERSION) // I cheated
                         + "Uninstall\\"
                         + AppNameToRegKey(m_csAppName);

  m_csStartMenuFolderName = "";  // initial value (make sure)

  if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, (LPCSTR)csUninstKey,
                  0, KEY_ALL_ACCESS,
                  &hkeyUninst)
     != ERROR_SUCCESS)
  {
    // problem

    MyMessageBox(IDS_ERROR08,
                  MB_OK | MB_ICONHAND | MB_SETFOREGROUND);

    // TODO - do I fail outright, or try anyway?

    hkeyUninst = NULL;
  }
  else
  {
    // get app path

    cb1 = 4096;
    if(RegQueryValueEx(hkeyUninst, "", 0, &dw1,
                       (LPBYTE)m_csAppPath.GetBufferSetLength(cb1),
                       &cb1)
       != ERROR_SUCCESS)
    {
      m_csAppPath.ReleaseBuffer(0);
    }
    else
    {
      m_csAppPath.ReleaseBuffer(cb1);
      m_csAppPath.ReleaseBuffer(-1);
    }

    // get start menu folder name while I'm at it
    cb1 = 4096;
    if(RegQueryValueEx(hkeyUninst, "StartMenuFolderName", 0, &dw1,
                       (LPBYTE)m_csStartMenuFolderName.GetBufferSetLength(cb1),
                       &cb1)
       != ERROR_SUCCESS)
    {
      m_csStartMenuFolderName.ReleaseBuffer(0);
    }
    else
    {
      m_csStartMenuFolderName.ReleaseBuffer(cb1);
      m_csStartMenuFolderName.ReleaseBuffer(-1);

      m_csStartMenuFolderName = AdjustPathName(m_csStartMenuFolderName);
    }

    // get 'app common' path (if it exists)

    if(RegOpenKeyEx(hkeyUninst, "AppCommon", 0, KEY_ALL_ACCESS, &hKey)
       == ERROR_SUCCESS)
    {
      cb1 = 4096;
      if(RegQueryValueEx(hKey, "", 0, &dw1,
                         (LPBYTE)m_csAppCommon.GetBufferSetLength(cb1),
                         &cb1)
         != ERROR_SUCCESS)
      {
        m_csAppCommon.ReleaseBuffer(0);
      }
      else
      {
        m_csAppCommon.ReleaseBuffer(cb1);
        m_csAppCommon.ReleaseBuffer(-1);
      }

      RegCloseKey(hKey);  // I can do this now...
    }

    // open "shared DLL's" key, make sure each entry exists.
    // (remove those that aren't here)
    // TODO:  do I still want the reference counts???

    // TODO:  implement this part

  }


  // TODO:  Do I need more information?  Location of shortcut folder?
  //        add anything ELSE that's useful to SETUP.EXE ...



  // TODO:  open the 'CurrentVersion' key

  // TODO:  open the 'HKLM\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion' key,
  //        check for and remove "app path" for EXE's that I installed


  CUNINSTDlg dlg;

//  static CWnd wndNULL;  // temporary parent window with NULL handle

//  m_pMainWnd = &dlg;
//  m_pMainWnd = &wndNULL;  // hopefully this prevents sending any 'WM_QUIT' messages

  int nResponse = dlg.DoModal();

//  m_pMainWnd = &wndNULL;

  if(nResponse == IDOK)
  {
    // here's where I do the work.....

    CProgressDialog dlgProgress;
    dlgProgress.Create(CProgressDialog::IDD, NULL); //&wndNULL);

    dlgProgress.ShowWindow(SW_SHOWNORMAL);
    dlgProgress.SetForegroundWindow();
    dlgProgress.SetProgressText(IDS_PROGRESS01);
    dlgProgress.SetProgress(0);

    CStringArray acsDeleteFiles;  // files to be deleted

    // FIRST STEP:  programs that I must EXECUTE on UNINSTALL that
    //              were not deleted during SETUP

    CStringArray acsExecuteOnUninstall, acsAutoDelete, acsRegister,
                 acsRegAsm, acsDecrement;
    CArray<DWORD,DWORD> adwDecrement;

    GetInfEntries("ExecuteOnUninstall", acsExecuteOnUninstall);
    GetInfEntries("AutoDelete", acsAutoDelete);
    GetInfEntries("RegisterControl", acsRegister);
    GetInfEntries("RegAsmControl", acsRegAsm);


    // FIRST STEP:  decrement count on registered "COMMON" DLL's.
    //              and build a list of "things" to delete.

    // NOTE:  if there are shared components, and the reference count is *NOT*
    //        going to be zero, then I want to EXCLUDE them from the list
    //        of 'ExecuteOnUninstall' and 'RegisterControl'.  I'll do this by
    //        updating the "true path" within the 'm_acsUnInstall' and 
    //        'm_acsUnInstallShared' arrays, and compare each name against
    //        these two arrays and the 'acsDeleteFiles' array.  Anything
    //        that's NOT in 'm_acsUnInstall' or m_acsUnInstallShared' and is
    //        NOT in 'acsDeleteFiles' will NOT be executed, unregistered, or deleted.


    // PART 1:  put "adjusted" (real) path into 'm_acsUnInstall'

    for(i1=0; i1 < m_acsUnInstall.GetSize(); i1++)
    {
      csTemp = m_acsUnInstall[i1];
      if(csTemp.GetLength())
      {
        i2 = csTemp.Find('\\');

        if(i2 < 0)
        {
          csTemp = AdjustPathName(m_csAppPath) + csTemp;
        }
        else if(!theApp.ExpandStrings(csTemp))
        {
          csTemp = "";
        }
      }

      if(csTemp.GetLength())
        m_acsUnInstall.SetAt(i1, csTemp);  // the *ADJUSTED* path
    }


    // PART 2:  put  "adjusted" (real) path into 'm_acsUnInstallShared'
    //          and make damn sure that it's one of the "shared DLL's"
    //          mentioned in the registry under 'UNINST'

    if(m_acsShared.GetSize())
    {
      dlgProgress.SetProgressText(IDS_PROGRESS02);
      dlgProgress.SetProgress(0);

      hKey = NULL;

      if(RegCreateKey(HKEY_LOCAL_MACHINE, szSHARED_DLLS, &hKey)
         != ERROR_SUCCESS)
      {
        csMsg.Format(IDS_ERROR09, (LPCSTR)szSHARED_DLLS);

        MyMessageBox(csMsg, MB_OK | MB_ICONHAND | MB_SETFOREGROUND);
        return(FALSE);  // fail it NOW
      }

      HKEY hkeyUninst;
      if(RegOpenKey(HKEY_LOCAL_MACHINE, 
                    szUNINST_KEY
                    + AppNameToRegKey(m_csAppName)
                    + "\\SharedDLLs",
                    &hkeyUninst)
         != ERROR_SUCCESS)
      {
        // for now, just ignore it - UNINSTAL won't decrement usage counts
        // if it doesn't find this entry in the registry.

        hkeyUninst = NULL;
      }

      // GO THROUGH THE ARRAY OF 'UnInstallShared' stuff, and update the
      // path information, check/update the usage count, and prompt the user to
      // uninstall things.  After this loop, the values in 'm_acsUnInstallShared'
      // will be the actual file paths so I can check this list later on.

      BOOL bRemoveAllShared = FALSE;

      for(i1=0; i1 < m_acsUnInstallShared.GetSize(); i1++)
      {
        csTemp = m_acsUnInstallShared[i1];

        if(csTemp.GetLength())
        {
          i3 = csTemp.ReverseFind('\\');

          // NOTE:  if there is no path info, prepend the path info using the
          //        application's path

          if(i3 < 0)
          {
            csTemp = AdjustPathName(theApp.m_csAppPath) + csTemp;
          }
          else if(!theApp.ExpandStrings(csTemp))
          {
            csTemp = "";
          }
        }

        if(csTemp.GetLength())
        {
          m_acsUnInstallShared.SetAt(i1, csTemp);  // make sure it's the ACTUAL PATH now!

          DWORD dwType, dwVal, cbVal = sizeof(dwVal);
          if(RegQueryValueEx(hkeyUninst, csTemp, NULL, &dwType,
                             (BYTE *)&dwVal, &cbVal)
             != ERROR_SUCCESS ||
             (dwType != REG_DWORD && dwType != REG_DWORD_LITTLE_ENDIAN))
          {
            // this component wasn't installed - don't uninstall it!
            continue;
          }

          // Get the shared DLL usage count from the registry key
          // HKLM\SOFTWARE\Microsoft\Windows\CurrentVersion\SharedDLLs
          //
          // The value name will be 'csTemp'.

          cbVal = sizeof(dwVal);
          if(RegQueryValueEx(hKey, csTemp, NULL, &dwType,
                             (BYTE *)&dwVal, &cbVal)
             != ERROR_SUCCESS)
          {
            continue;
          }
          else if(dwType != REG_DWORD &&
                  dwType != REG_DWORD_LITTLE_ENDIAN)
          {
            continue;
          }

          BOOL bRemoveItFlag = FALSE;

          if(!dwVal)
          {
            csMsg.Format(IDS_REF_COUNT_ZERO, (LPCSTR)csTemp);

            bRemoveItFlag = MyMessageBox(csMsg, MB_YESNO | MB_ICONEXCLAMATION | MB_SETFOREGROUND)
                          == IDYES;
          }
          else
          {
            dwVal--;

            if(!dwVal)
            {
              if(bRemoveAllShared)
              {
                bRemoveItFlag = TRUE;
              }
              else
              {
                CRemoveSharedDlg dlg;
                dlg.m_csFileName = csTemp;

                i2 = dlg.DoModal();

                if(i2 == IDCANCEL)
                {
                  MyMessageBox(IDS_MESSAGE01, MB_OK | MB_ICONINFORMATION | MB_SETFOREGROUND);

                  return(FALSE);
                }

                bRemoveItFlag = (i2 == IDYES || i2 == IDC_YES_TO_ALL);
                bRemoveAllShared = (i2 == IDC_YES_TO_ALL);
              }
            }
          }

          // NEXT, add this key to the list o' things to "decrement" from shared components

          i2 = acsDecrement.Add(csTemp);
          adwDecrement.SetAtGrow(i2, dwVal);  // the value to which I assign it

          if(!dwVal && (bRemoveItFlag || bRemoveAllShared))
          {
            acsDeleteFiles.Add(csTemp);
          }

//          if(RegSetValueEx(hKey, csTemp, NULL, REG_DWORD,
//                           (BYTE *)&dwVal, sizeof(dwVal))
//             != ERROR_SUCCESS)
//          {
//            MyMessageBox("WARNING:  unable to update reference count for " + csTemp,
//                          MB_OK | MB_ICONHAND | MB_SETFOREGROUND);
//          }
//
//          if(!dwVal && bRemoveItFlag)
//          {
//            RegDeleteValue(hKey, csTemp);  // remove it
//
//            acsDeleteFiles.Add(csTemp);
//          }
        }
      }

      if(hkeyUninst)
        RegCloseKey(hkeyUninst);

      RegCloseKey(hKey);

      dlgProgress.SetProgressText(IDS_PROGRESS03);
      dlgProgress.SetProgress(100);

      Sleep(500);
    }


    dlgProgress.SetProgressText(IDS_PROGRESS04);
    dlgProgress.SetProgress(0);

    // SECOND STEP - 'Execute After Copy' applications that were not deleted during install
    // these use the DESTINATION name in the INF file
    //
    // NOTE:  these get uninstalled in *REVERSE* order!!!

    for(i1 = acsExecuteOnUninstall.GetSize() - 1; i1 >= 0; i1--)
    {
      // exclude files that are in the 'AutoDelete' section.
      // (these include '%tmpdir%' files also)

      for(i2=0; i2 < acsAutoDelete.GetSize(); i2++)
      {
        if(!acsAutoDelete[i2].CompareNoCase(acsExecuteOnUninstall[i1]))
        {
          break;
        }
      }

      if(i2 < acsAutoDelete.GetSize()) // was "auto delete"
        continue;

      csTemp = acsExecuteOnUninstall[i1];

      // find the ',' if there is one
      i3 = csTemp.Find(',');
      if(i3 >= 0)
      {
        csTemp2 = csTemp.Mid(i3 + 1);
        csTemp2.TrimLeft();

        csTemp = csTemp.Left(i3);
        csTemp.TrimRight();
      }

      if(csTemp.GetLength() > 2 && csTemp[0] == '\"')
      {
        csTemp.TrimRight();
        if(csTemp.GetLength() > 1 && 
           csTemp[csTemp.GetLength() - 1] == '\"')
        {
          csTemp = csTemp.Mid(1, csTemp.GetLength() - 2);
        }
        else
        {
          csTemp = csTemp.Mid(1);  // just in case
        }
      }

      csMsg.Format(IDS_PROGRESS05, (LPCSTR)csTemp);

      // convert 'csTemp' to the actual file's path

      i3 = csTemp.ReverseFind('\\');
      if(i3 < 0)
      {
        csTemp = AdjustPathName(theApp.m_csAppPath) + csTemp;
      }
      else
      {
        if(!theApp.ExpandStrings(csTemp))
        {
          // whoops - I got a problem
          csMsg.Format(IDS_CANNOT_EXEC, (LPCSTR)csTemp);

          MyMessageBox(csMsg, MB_OK | MB_ICONASTERISK | MB_SETFOREGROUND);
          continue;
        }
      }


      // If the file was "shared", and it's NOT in the 'delete files'
      // section, I must skip it.

      BOOL bUninstallFlag = FALSE;

      for(i2=0; i2 < m_acsUnInstall.GetSize(); i2++)
      {
        if(!csTemp.CompareNoCase(m_acsUnInstall[i2]))
        {
          bUninstallFlag = TRUE;
          break;
        }
      }

      for(i2=0; !bUninstallFlag && i2 < m_acsUnInstallShared.GetSize(); i2++)
      {
        if(!csTemp.CompareNoCase(m_acsUnInstallShared[i2]))
        {
          bUninstallFlag = FALSE;
          for(i3=0; i3 < acsDeleteFiles.GetSize(); i3++)
          {
            if(!csTemp.CompareNoCase(acsDeleteFiles[i3]))
            {
              bUninstallFlag = TRUE;

              break;
            }
          }

          break;  // always break out if I get here
        }
      }

      if(!bUninstallFlag)
      {
        // in other words, I found the file in my list o' shared components,
        // but I'm not supposed to uninstall it yet

        continue;  // do *NOT* uninstall this file - it's not in the 'delete files' list!
      }


      dlgProgress.SetProgressText(csMsg);

      // TODO:  validate against 'installed options' before trying to execute
      //        For now,if the file does NOT exist, don't execute it!

// TEMPORARY
      WIN32_FIND_DATA fd;
      HANDLE hFF = FindFirstFile(csTemp, &fd);

      if(hFF != INVALID_HANDLE_VALUE)
        FindClose(hFF);

      if(hFF != INVALID_HANDLE_VALUE &&
         !(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
// TEMPORARY
      {

        SHELLEXECUTEINFO sei;
        memset(&sei, 0, sizeof(sei));
        sei.cbSize = sizeof(sei);
        sei.fMask = SEE_MASK_FLAG_NO_UI |  SEE_MASK_NOCLOSEPROCESS;
        sei.lpFile = (LPCSTR)csTemp;
        sei.lpParameters = (LPCSTR)csTemp2;
        sei.nShow = SW_SHOWNORMAL;

#ifdef _DEBUG
        TRACE("WOULD BE EXECUTING \"%s\" %s\r\n", (LPCSTR)csTemp, (LPCSTR)csTemp2);
#else // _DEBUG          
        if(!ShellExecuteEx(&sei))
        {
          csMsg.Format(IDS_CANNOT_EXEC2, (LPCSTR)csTemp);
          if(MyMessageBox(csMsg, MB_YESNO | MB_ICONHAND | MB_SETFOREGROUND)
             != IDYES)
          {
            return(FALSE);  // this is really a FATAL error...
          }
        }
        else
        {
          WaitForSingleObject(sei.hProcess, INFINITE);
        }

        DWORD dwExitCode;
        if(!GetExitCodeProcess(sei.hProcess, &dwExitCode))
        {
          csMsg.Format(IDS_WARNING04, (LPCSTR)csTemp);
          MyMessageBox(csMsg, MB_OK | MB_ICONHAND | MB_SETFOREGROUND);
        }
        else if(dwExitCode == STILL_ACTIVE)
        {
          csMsg.Format(IDS_WARNING05, (LPCSTR)csTemp);
          MyMessageBox(csMsg, MB_OK | MB_ICONHAND | MB_SETFOREGROUND);
        }
        else if(dwExitCode)
        {
          csMsg.Format(IDS_WARNING06, (LPCSTR)csTemp, dwExitCode, dwExitCode);

          MyMessageBox(csMsg, MB_OK | MB_ICONHAND | MB_SETFOREGROUND);
        }
#endif // _DEBUG
      }

      dlgProgress.SetProgress(100 * (acsExecuteOnUninstall.GetSize() - i1)
                              / acsExecuteOnUninstall.GetSize());
    }

    dlgProgress.SetProgressText(IDS_PROGRESS06);
    dlgProgress.SetProgress(100);

    Sleep(250);


    // SECOND STEP:  registered controls - call DllUnregisterServer()
    //
    // DO THIS IN **REVERSE** ORDER!

    dlgProgress.SetProgressText(IDS_PROGRESS07);
    dlgProgress.SetProgress(0);

    for(i1 = acsRegister.GetSize() - 1; i1 >= 0; i1--)
    {
      dlgProgress.SetProgress(100 * (acsRegister.GetSize() - i1 - 1)
                              / acsRegister.GetSize());

      csTemp = acsRegister[i1];

      BOOL bDotNetFlag = FALSE;

      for(i2=0; i2 < acsRegAsm.GetSize(); i2++)
      {
        if(!csTemp.CompareNoCase(acsRegAsm[i2]))
        {
          bDotNetFlag = TRUE;
          break;
        }
      }

      i2 = csTemp.ReverseFind('\\');
      if(i2 < 0)
      {
        csTemp = AdjustPathName(theApp.m_csAppPath) + csTemp;
      }
      else
      {
        if(!ExpandStrings(csTemp))
        {
          csMsg.Format(IDS_ERROR10, (LPCSTR)acsRegister[i1]);

          MyMessageBox(csMsg, MB_OK | MB_ICONHAND | MB_SETFOREGROUND);
          continue;
        }
      }

      // If the file was "shared", and it's NOT in the 'delete files'
      // section, I must skip it.

      BOOL bUninstallFlag = FALSE;

      for(i2=0; i2 < m_acsUnInstall.GetSize(); i2++)
      {
        if(!csTemp.CompareNoCase(m_acsUnInstall[i2]))
        {
          bUninstallFlag = TRUE;
          break;
        }
      }

      for(i2=0; !bUninstallFlag && i2 < m_acsUnInstallShared.GetSize(); i2++)
      {
        if(!csTemp.CompareNoCase(m_acsUnInstallShared[i2]))
        {
          for(i3=0; i3 < acsDeleteFiles.GetSize(); i3++)
          {
            if(!csTemp.CompareNoCase(acsDeleteFiles[i3]))
            {
              bUninstallFlag = TRUE;

              break;
            }
          }

          break;  // always break out if I get here
                  // if I am to uninstall, i2 will be > the size of 'm_acsUnInstallShared'
        }
      }

      if(!bUninstallFlag)
      {
        // in other words, I found the file in my list o' shared components,
        // but I'm not supposed to uninstall it yet

        continue;  // do *NOT* uninstall this file - it's not in the 'delete files' list!
      }


      csMsg.Format(IDS_PROGRESS08, (LPCSTR)csTemp);
      dlgProgress.SetProgressText(csMsg);

#ifdef _DEBUG
      TRACE0("WOULD BE UN-REGISTERING " + csTemp + "\r\n");
#else // _DEBUG
      if(bDotNetFlag && IsDotNetAvailable())
      {
        CString csRegAsm = LocateBoguSoftNotYetRegistrationApp();

        if(csRegAsm.GetLength())
        {
          if(!ShellExecApp(csRegAsm, " /u " + csTemp, SW_HIDE))
          {
            // TODO:  error?  warning?
          }
        }
      }
      else if(!SafeCallDllUnregisterServer(csTemp, csMsg))
      {
        return(FALSE);
      }
#endif // _DEBUG
    }

    dlgProgress.SetProgressText(IDS_PROGRESS09);
    dlgProgress.SetProgress(100);
    Sleep(250);


    dlgProgress.SetProgressText(IDS_PROGRESS10);
    dlgProgress.SetProgress(0);

    if(acsDecrement.GetSize())
    {
      hKey = NULL;  // make sure

      if(RegOpenKey(HKEY_LOCAL_MACHINE, szSHARED_DLLS, &hKey)
         != ERROR_SUCCESS)
      {
        csMsg.Format(IDS_WARNING07, (LPCSTR)szSHARED_DLLS);

        MyMessageBox(csMsg, MB_OK | MB_ICONHAND | MB_SETFOREGROUND);
      }
      else
      {
        for(i1=0; i1 < acsDecrement.GetSize(); i1++)
        {
          dlgProgress.SetProgress(100 * i1 / acsDecrement.GetSize());

          DWORD dwVal = adwDecrement[i1];

          if(dwVal)
          {
#ifdef _DEBUG
            TRACE("WOULD BE SETTING VALUE FOR %s TO %d (%08xH)\r\n",
                  (LPCSTR)acsDecrement[i1], dwVal, dwVal);
#else // _DEBUG
            if(RegSetValueEx(hKey, acsDecrement[i1], NULL, REG_DWORD,
                             (BYTE *)&dwVal, sizeof(dwVal))
               != ERROR_SUCCESS)
            {
              csMsg.Format(IDS_WARNING08, (LPCSTR)acsDecrement[i1]);
              MyMessageBox(csMsg, MB_OK | MB_ICONHAND | MB_SETFOREGROUND);
            }
#endif // _DEBUG
          }
          else
          {
#ifdef _DEBUG
            TRACE("WOULD BE DELETING VALUE FOR %s\r\n",
                  (LPCSTR)acsDecrement[i1]);
#else // _DEBUG
            if(RegDeleteValue(hKey, acsDecrement[i1])
               != ERROR_SUCCESS)
            {
              csMsg.Format(IDS_WARNING08a, (LPCSTR)acsDecrement[i1]);
              MyMessageBox(csMsg, MB_OK | MB_ICONHAND | MB_SETFOREGROUND);
            }
#endif // _DEBUG
          }
        }

        RegCloseKey(hKey);
        hKey = NULL;  // by convention
      }
    }

    dlgProgress.SetProgressText(IDS_PROGRESS11);
    dlgProgress.SetProgress(100);
    Sleep(250);


    dlgProgress.SetProgressText(IDS_PROGRESS12);
    dlgProgress.SetProgress(0);

    // NOW, the most difficult part of this may be that the user has created
    // his own key and I must decide whether or not I want to delete that key
    // along with these.  In Windows '95, deleting a key deletes subkeys.
    // Under NT it does not.  The fastest method then would be to recursively
    // call 'RegDeleteKey()' for each key, on all its parent keys, deleting
    // it *only* when there are no subkeys left to delete.  The safest method
    // would be to delete these in reverse order, *ONLY* when there are no
    // sub-keys, and not delete any un-specified parent keys.

    // I opt for 'safe'.

    CStringArray acsRegKeys, acsRegKeys0;
    theApp.GetInfEntries("RegistryKeys", acsRegKeys);

    CArray<MYKEYINFO,MYKEYINFO &> aKeyInfo;

    for(i1=0; i1 < acsRegKeys.GetSize(); i1++)
    {
      dlgProgress.SetProgress(50 * i1 / acsRegKeys.GetSize());

      // parse the entry (very strict adherence to 'the rules'

      CString csKey, csString, csValue;
      csTemp = acsRegKeys[i1];

      if(!csTemp.GetLength())
        continue;

      // find the first '=' (no quotes, either)
      i2 = csTemp.Find('=');
      if(i2 <= 0)
      {
        csMsg.Format(IDS_WARNING09, (LPCSTR)csTemp);
        MyMessageBox(csMsg, MB_OK | MB_ICONHAND | MB_SETFOREGROUND);

        continue;
      }
      else
      {
        csKey = csTemp.Left(i2);
        csTemp = csTemp.Mid(i2 + 1);

        // parse through quote marks to find '='

        if(csTemp[0] == '\"')  // most likely
        {
          for(i2=1; i2 < csTemp.GetLength(); i2++)
          {
            if(csTemp[i2] == '\"' &&
               ((i2 + 1) >= csTemp.GetLength() ||
                csTemp[i2 + 1] != '\"'))
            {
              break;  // found the ending quote mark
            }
          }

          csString = csTemp.Mid(1, i2 - 1);

          // un-double quote marks

          for(i3 = 0; (i3 + 1) < csString.GetLength(); i3++)
          {
            if(csString[i3] == '\"' &&
               csString[i3 + 1] == '\"')
            {
              csString = csString.Left(i3)
                       + csString.Mid(i3 + 1);
            }
          }

          if(i2 < csTemp.GetLength())
            i2++;
        }
        else
        {
          i2 = csTemp.Find('=');

          csString = csTemp.Left(i2);
          csString.TrimLeft();
          csString.TrimRight();

          if(csString[0] == '@')
          {
            csString = "";
          }
        }

        if(i2 < csTemp.GetLength() &&
           csTemp[i2] == '=')  // only if it's working properly
        {
          csTemp = csTemp.Mid(i2 + 1);
          csTemp.TrimRight();
          csTemp.TrimLeft();  // just in case

          // strip outer quotes off (if any)

          if(csTemp[0] == '\"' && csTemp[csTemp.GetLength()-1] == '\"')
          {
            csValue = csTemp.Mid(1, csTemp.GetLength() - 2);

            // un-double quote marks

            for(i3 = 0; (i3 + 1) < csValue.GetLength(); i3++)
            {
              if(csValue[i3] == '\"' &&
                 csValue[i3 + 1] == '\"')
              {
                csValue = csValue.Left(i3)
                        + csValue.Mid(i3 + 1);
              }
            }
          }
          else
          {
            csValue = csTemp;  // just because
          }
        }
        else
        {
          csValue = "";  // blank value otherwise
        }
      }

      // NOTE:  'csString' 'csKey' and 'csValue' could *ALL*
      //        have embedded thingies, so I need to do a string
      //        expansion on each.

      if(!theApp.ExpandStrings(csKey) ||
         !theApp.ExpandStrings(csValue) ||
         !theApp.ExpandStrings(csString))
      {
        csMsg.Format(IDS_WARNING10, (LPCSTR)acsRegKeys[i1]);
        MyMessageBox(csMsg, MB_OK | MB_ICONHAND | MB_SETFOREGROUND);

        continue;
      }

      MYKEYINFO mki;
      mki.szKey = NULL;  // for now - will adjust next pass
      mki.hkeyRoot = HKEY_CLASSES_ROOT;  // the default

      // time to create the key and set the value
      // NOTE:  if 'csString' and 'csValue' are both blank, I delete
      //        the key.  If the value is blank, I delete the value only

      static const char szHKLM[]="HKLM\\";
      static const char szHKEY_LOCAL_MACHINE[]="HKEY_LOCAL_MACHINE\\";
      static const char szHKU[]="HKU\\";
      static const char szHKEY_USERS[]="HKEY_USERS\\";
      static const char szHKCR[]="HKCR\\";
      static const char szHKEY_CLASSES_ROOT[]="HKEY_CLASSES_ROOT\\";
      static const char szHKCU[]="HKCU\\";
      static const char szHKEY_CURRENT_USER[]="HKEY_CURRENT_USER\\";

#define KEYCHECK(X) (!csKey.Left(lstrlen(X)).CompareNoCase(X))

      if(KEYCHECK(szHKLM) || KEYCHECK(szHKEY_LOCAL_MACHINE))
      {
        mki.hkeyRoot = HKEY_LOCAL_MACHINE;
        i2 = csKey.Find('\\');
        csKey = csKey.Mid(i2 + 1);
      }
      else if(KEYCHECK(szHKCU) || KEYCHECK(szHKEY_CURRENT_USER))
      {
        mki.hkeyRoot = HKEY_LOCAL_MACHINE;
        i2 = csKey.Find('\\');
        csKey = csKey.Mid(i2 + 1);
      }
      else if(KEYCHECK(szHKU) || KEYCHECK(szHKEY_USERS))
      {
        mki.hkeyRoot = HKEY_USERS;
        i2 = csKey.Find('\\');
        csKey = csKey.Mid(i2 + 1);
      }
      else if(KEYCHECK(szHKCR) || KEYCHECK(szHKEY_CLASSES_ROOT))
      {
        mki.hkeyRoot = HKEY_CLASSES_ROOT;
        i2 = csKey.Find('\\');
        csKey = csKey.Mid(i2 + 1);
      }
      else
      {
        csMsg.Format(IDS_WARNING11, (LPCSTR)csKey);
        MyMessageBox(csMsg, MB_OK | MB_ICONASTERISK | MB_SETFOREGROUND);

        mki.hkeyRoot = HKEY_CLASSES_ROOT;
      }

#undef KEYCHECK

      i2 = acsRegKeys0.Add(csKey);

      aKeyInfo.SetAtGrow(i2, mki);
    }

    // now, do another pass and fix up the string pointers in 'aKeyInfo'

    for(i1=0; i1 < acsRegKeys0.GetSize(); i1++)
    {
      aKeyInfo[i1].szKey = (LPCSTR)acsRegKeys0[i1];
    }

    // NOW, sort the 'aKeyInfo' array

    qsort(aKeyInfo.GetData(), aKeyInfo.GetSize(), sizeof(aKeyInfo[0]),
          KeyInfoSortCompareProc);

    for(i1 = aKeyInfo.GetSize() - 1; i1 >= 0; i1--)
    {
      dlgProgress.SetProgress(50 + 50 * i1 / aKeyInfo.GetSize());

      // attempt to remove keys.  In Win '95, must check to see that there
      // are no additional keys, so I'll do it really nice-like...

      MyDeleteRegistryKey0(aKeyInfo.GetAt(i1).hkeyRoot,
                           aKeyInfo.GetAt(i1).szKey);
    }

    dlgProgress.SetProgress(99);


    // NOW, delete the 'SETUP' key (so I don't prompt if I re-install)

    MyDeleteRegistryKey(HKEY_LOCAL_MACHINE, csSetupKey);


    // LAST STEP:  remove HKLM\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{app name}
    MyDeleteRegistryKey(HKEY_LOCAL_MACHINE, csSysUninstKey);

    // ignore result.  Assume it works.


    dlgProgress.SetProgressText(IDS_PROGRESS13);
    dlgProgress.SetProgress(100);

    Sleep(250);

    dlgProgress.SetProgressText(IDS_PROGRESS14);
    dlgProgress.SetProgress(0);

    // NOW, add the rest of the stuff to 'acsDeleteFiles'

    CStringArray acsDirectories;  // will automatically remove files from directories
                                  // that are empty after I finish...

    for(i1=0; i1 < m_acsUnInstall.GetSize(); i1++)
    {
      csTemp = m_acsUnInstall[i1];

      for(i2=0; i2 < acsDeleteFiles.GetSize(); i2++)
      {
        if(!acsDeleteFiles[i2].CompareNoCase(csTemp))
          break;
      }

      if(i2 >= acsDeleteFiles.GetSize())
      {
        // see if this is in 'm_acsUnInstallShared' - if it's there, I do *NOT*
        // want to remove it!  It would mean that the user did *NOT* want to 
        // uninstall the component, or that the component's share count was not zero

        for(i2=0; i2 < m_acsUnInstallShared.GetSize(); i2++)
        {
          if(!m_acsUnInstallShared[i2].CompareNoCase(csTemp))
            break;
        }

        if(!m_acsUnInstallShared.GetSize() ||
           i2 >= m_acsUnInstallShared.GetSize())  // not in the list!
        {
          acsDeleteFiles.Add(csTemp);
          if(csTemp.GetLength() > 4 &&
             !csTemp.Right(4).CompareNoCase(".HLP"))
          {
            acsDeleteFiles.Add(csTemp.Left(csTemp.GetLength() - 4) + ".GID");
              // deleting ".GID" files associated with ".HLP" files, because
              // Windows creates them, and I want them GONE as well...
          }

          i3 = csTemp.ReverseFind('\\');

          if(i3 < 0) // I want the PATH, dammit!
          {
            csTemp = AdjustPathName(theApp.m_csAppPath);
          }
          else
          {
            if(!theApp.ExpandStrings(csTemp))
            {
              continue;
            }

            i3 = csTemp.ReverseFind('\\');

            csTemp = csTemp.Left(i3 + 1);  // keep the trailing backslash
          }

          if(csTemp.GetLength())  // will be blank if no path...
          {
            for(i2=0; i2 < acsDirectories.GetSize(); i2++)
            {
              if(!acsDirectories[i2].CompareNoCase(csTemp))
                break;
            }

            if(i2 >= acsDirectories.GetSize())
              acsDirectories.Add(csTemp);
          }
        }
      }
    }

    // add the UNINSTALL program and UNINST.INF to the list of files to remove
    // These will normally be located in the 'UNINST\{AppName}' directory.
    // Since there's an entry for "SETUPGIZ\uninst.exe" in the INF file, I'll
    // rely on THAT one to lead me in the right direction...

    csTemp = theApp.GetFileEntryFromName("SETUPGIZ\\uninst.exe");

    if(csTemp.GetLength())
      csTemp = theApp.GetFileEntryDestPath(csTemp);

    if(!csTemp.GetLength())
    {
      csTemp = "%uninst%\\uninst.exe"; // the default
      theApp.ExpandStrings(csTemp);
    }

    acsDeleteFiles.Add(csTemp);

    i3 = csTemp.ReverseFind('\\');

    if(i3 < 0)
    {
      acsDeleteFiles.Add("SETUP.INF");
      csTemp = AdjustPathName(theApp.m_csAppPath);
    }
    else
    {
      acsDeleteFiles.Add(csTemp.Left(i3 + 1) + "SETUP.INF");

      if(theApp.ExpandStrings(csTemp))
      {
        i3 = csTemp.ReverseFind('\\');

        csTemp = csTemp.Left(i3 + 1);  // keep the trailing backslash
      }
      else
      {
        csTemp = "";  // a flag for below
      }
    }

    if(csTemp.GetLength())  // will be blank if no path...
    {
      for(i2=0; i2 < acsDirectories.GetSize(); i2++)
      {
        if(!acsDirectories[i2].CompareNoCase(csTemp))
          break;
      }

      if(i2 >= acsDirectories.GetSize())
        acsDirectories.Add(csTemp);

      // check for 'UNINST' directory as part of this one, and
      // add it in there so that it's deleted properly

      if(csTemp.GetLength() >= (AdjustPathName(theApp.m_csAppPath).GetLength() + 7))
      {
        if(!csTemp.Left(AdjustPathName(theApp.m_csAppPath).GetLength() + 7)
           .CompareNoCase(AdjustPathName(theApp.m_csAppPath) + "UNINST\\"))
        {
          acsDirectories.Add(AdjustPathName(theApp.m_csAppPath) + "UNINST\\");
        }
      }
    }

    // before I delete anything, post a WM_QUIT message to the "calling app"
    // and give it 1/2 second to die before proceeding.

    if(dwStartThreadID)
    {
      ::PostThreadMessage(dwStartThreadID, WM_QUIT, 0, 0);
      ::PostThreadMessage(dwStartThreadID, WM_QUIT, 0, 0);
      ::PostThreadMessage(dwStartThreadID, WM_QUIT, 0, 0);
      ::PostThreadMessage(dwStartThreadID, WM_QUIT, 0, 0);  // do this 4 times for insurance

      Sleep(500);  // wait 1/2 second.  caller should "die". this is for Windows Control Panel
                   // so that it behaves rationally.
    }


    // FINAL STEP:  delete shit - files in previous step plus
    //              those not marked as "system/shared" plus
    //              the shortcuts and the shortcut folder
    //
    // All shared items to delete will have their full paths in
    // 'acsDeleteFiles' already.

    CStringArray acsAddShortcut; // the "add shortcut" entries - need it for below...
    theApp.GetInfEntries("AddShortcut", acsAddShortcut);

    // filling 'acsDeletedFiles' with file names

    for(i1=0; i1 < acsDeleteFiles.GetSize(); i1++)
    {
      dlgProgress.SetProgress(i1 * 100 / (acsDeleteFiles.GetSize() + acsAddShortcut.GetSize() + 1));

      csTemp = acsDeleteFiles[i1];

      i3 = csTemp.ReverseFind('\\');
      if(i3 < 0)
      {
        csTemp = AdjustPathName(theApp.m_csAppPath) + csTemp;
      }
      else
      {
        if(!theApp.ExpandStrings(csTemp))
        {
          // whoops - I got a problem

          csMsg.Format(IDS_ERROR12, (LPCSTR)csTemp);

          MyMessageBox(csMsg, MB_OK | MB_ICONHAND | MB_SETFOREGROUND);
          continue;
        }
      }

#ifndef _DEBUG

      SetFileAttributes(csTemp, 0);  // turn off *ALL* attributes in case it's read-only
      DeleteFile(csTemp);            // ignore failures in this case

#endif // _DEBUG

      TRACE("WOULD BE DELETING %s\r\n", (LPCSTR)csTemp);

      // get the file's path and place it into 'acsDirectories', if not already there

      i3 = csTemp.ReverseFind('\\');
      if(i3 > 0)
      {
        csTemp = csTemp.Left(i3 + 1);

        for(i2=0; i2 < acsDirectories.GetSize(); i2++)
        {
          if(!acsDirectories[i2].CompareNoCase(csTemp))
            break;
        }

        if(i2 >= acsDirectories.GetSize())
          acsDirectories.Add(csTemp);
      }
    }


    // SHORTCUT LISTING (and the shortcut directory)

    if(m_csStartMenuFolderName.GetLength())  // ah, HAH!  I have a shortcut...
    {
      for(i1=0; i1 < acsAddShortcut.GetSize(); i1++)
      {
        dlgProgress.SetProgress((i1 + acsDeleteFiles.GetSize()) * 100
                                / (acsDeleteFiles.GetSize() + acsAddShortcut.GetSize() + 1));

        // OLD:  <destination path>,<display name>,<shortcut file name>
        // NEW:  destfilename,Description,ShortcutName[,StartPath,Args,IconFile,IconIndex]

        DoParseString(acsAddShortcut[i1], acsTemp);

        if(acsTemp.GetSize() < 3)  // invalid entry (disregard)
          continue;

        if(acsTemp[2].GetLength())
        {
          csTemp = acsTemp[2];

          while(csTemp.GetLength())  // recursing through the directories specified for the shortcut
          {
            i3 = csTemp.ReverseFind('\\');

            if(i3 > 0)
            {
              csTemp = csTemp.Left(i3 + 1);

              for(i2=0; i2 < acsDirectories.GetSize(); i2++)
              {
                if(!acsDirectories[i2].CompareNoCase(m_csStartMenuFolderName + csTemp))
                  break;
              }

              if(i2 >= acsDirectories.GetSize())
                acsDirectories.Add(m_csStartMenuFolderName + csTemp);

              csTemp = csTemp.Left(i3);  // get rid of backslash now
            }
            else
            {
              csTemp = "";
            }
          }

          // force extension on acsTemp[2] if there isn't one

          if(acsTemp[2].GetLength() < 4 ||
             (acsTemp[2].Right(4).CompareNoCase(".LNK") &&
              acsTemp[2].Right(4).CompareNoCase(".PIF")))
          {
            acsTemp.SetAt(2,acsTemp[2] + ".LNK");  // force it (for now just do this)
          }

#ifndef _DEBUG
          DeleteFile(m_csStartMenuFolderName + acsTemp[2]);
#endif // _DEBUG

          TRACE("WOULD BE DELETING %s\r\n", (LPCSTR)(m_csStartMenuFolderName + acsTemp[2]));
        }


        // Because MS-DOS shortcuts may be given a '.PIF' extension, I shall 
        // delete any '.PIF' files that have matching file names to the '.LNK' files...

        if(!acsTemp[2].Right(4).CompareNoCase(".LNK"))
        {
  #ifndef _DEBUG
          DeleteFile(m_csStartMenuFolderName
                     + acsTemp[2].Left(acsTemp[2].GetLength() - 3) + "PIF");
  #endif // _DEBUG

          TRACE("WOULD BE DELETING %s\r\n", (LPCSTR)(m_csStartMenuFolderName
                                                     + acsTemp[2].Left(acsTemp[2].GetLength() - 3)
                                                     + "PIF"));
        }
      }

      // last one - 'uninstall' shortcut

#ifndef _DEBUG
      DeleteFile(m_csStartMenuFolderName + "Un-Install " + AppNameToPathName(m_csAppName) + ".LNK");
      DeleteFile(m_csStartMenuFolderName + "Un-Install.LNK");  // backward compatibility
#endif // _DEBUG

      TRACE("WOULD BE DELETING %s\r\n", (LPCSTR)(m_csStartMenuFolderName + "Un-Install " + AppNameToPathName(m_csAppName) + ".LNK"));
      TRACE("WOULD BE DELETING %s\r\n", (LPCSTR)(m_csStartMenuFolderName + "Un-Install.LNK"));

      // removing the start menu folder itself

      acsDirectories.Add(m_csStartMenuFolderName); // just add it to the list
                                                   // if the user made changes, oh well...

      // NOTE:  if the start menu folder name contains a sub-directory name, I must also
      //        delete THAT as well.  So I shall locate the term "\Start Menu\Programs\"
      //        within the file name.  If I do *not* find it, I stop here.  If I *do*
      //        find it, I'll delete everything down to 'that' point.

      csTemp = AdjustPathName2(m_csStartMenuFolderName);  // remove trailing backslash

      i1 = csTemp.ReverseFind('\\');

      if(i1 > 0)
        csTemp = csTemp.Left(i1);  // trim it, and do not include the '\\'
      else
        csTemp = "";

      while(csTemp.Find("\\Start Menu\\Programs\\") > 0)
      {
        acsDirectories.Add(csTemp);  // backslash re-added so it sorts properly

        i1 = csTemp.ReverseFind('\\');

        if(i1 < 0)
          break;

        csTemp = csTemp.Left(i1);
      }
    }

    // make sure that the "apppath" has been added to my list o' dirs
    // This is likely to have subdirs under it without any actual files

    csTemp = m_csAppPath;
    if(!ExpandStrings(csTemp))
      csTemp = "";

    if(m_csAppPath.GetLength() && csTemp.GetLength() &&
        !m_csAppPath.CompareNoCase(csTemp))
    {
      csTemp = AdjustPathName(theApp.m_csAppPath);

      for(i2=0; i2 < acsDirectories.GetSize(); i2++)
      {
        if(!acsDirectories[i2].CompareNoCase(csTemp))
          break;
      }

      if(i2 >= acsDirectories.GetSize())
        acsDirectories.Add(csTemp);
    }

    // also check 'appcommon'.  Don't attempt to remove it
    // unless I've installed something in this path.
    // If this is the case, there won't be any expansions in
    // it.

    csTemp = m_csAppCommon;
    if(!ExpandStrings(csTemp))
      csTemp = "";

    if(m_csAppCommon.GetLength() && csTemp.GetLength() &&
       !m_csAppCommon.CompareNoCase(csTemp))
    {
      csTemp = AdjustPathName(theApp.m_csAppCommon);

      for(i2=0; i2 < acsDirectories.GetSize(); i2++)
      {
        if(!acsDirectories[i2].CompareNoCase(csTemp))
          break;
      }

      if(i2 >= acsDirectories.GetSize())
        acsDirectories.Add(csTemp);
    }


    dlgProgress.SetProgressText(IDS_PROGRESS15);
    dlgProgress.SetProgress(100);

    Sleep(1000);

    dlgProgress.SetProgressText(IDS_PROGRESS16);
    dlgProgress.SetProgress(0);

    Sleep(1000);  // allow additional time to ensure that the file system "recovers"

    CString csWinDir = AdjustPathName2(m_csWinPath);  // strip trailing backslash (if any)

    if(csWinDir.GetLength())
      SetCurrentDirectory(csWinDir);  // change to WINDOWS directory, just in case

    // SORT the DIRECTORY ENTRIES BACKWARDS (so that I remove the
    // child directories FIRST... hint hint hint).

    CArray<LPCSTR,LPCSTR> aszDirs;
    acsTemp.RemoveAll();

    for(i1=0; i1 < acsDirectories.GetSize(); i1++)
    {
//      TRACE0("TEMPORARY - adding " + acsDirectories[i1] + "\r\n");

      acsTemp.SetAtGrow(i1, acsDirectories[i1]);
    }

    acsDirectories.RemoveAll();

    for(i1=0; i1 < acsTemp.GetSize(); i1++)
    {
//      TRACE0("TEMPORARY - adding ptr to " + acsTemp[i1] + "\r\n");

      aszDirs.SetAtGrow(i1, (LPCSTR)acsTemp[i1]);  // assume they don't move
    }

    qsort(aszDirs.GetData(), aszDirs.GetSize(), sizeof(aszDirs[0]),
          StringSortCompareProc);

    for(i2=0, i1=aszDirs.GetSize() - 1; i1 >= 0; i1--, i2++)
    {
//      TRACE0("TEMPORARY - re-adding " + (CString)aszDirs[i1] + "\r\n");

      acsDirectories.SetAtGrow(i2, aszDirs[i1]);  // add 'em backwards
    }


    // NOW, REMOVE THE EMPTY DIRECTORIES

    Sleep(1000);  // allow the file system to "catch up" with me

    for(i1=0; i1 < acsDirectories.GetSize(); i1++)
    {
      dlgProgress.SetProgress(100 * i1 / acsDirectories.GetSize());

      csTemp = AdjustPathName2(acsDirectories[i1]);  // get path, strip trailing '\'

      // NOTE:  neither 'AppCommon' nor 'AppPath' may be removed if
      // *ANY* files exist within them.

      BOOL bDon_tAsk = !csTemp.CompareNoCase(AdjustPathName2(m_csAppPath))
                    || !csTemp.CompareNoCase(AdjustPathName2(m_csAppCommon));

      if(!PathHasFiles(csTemp) ||
         (!bDon_tAsk &&
          MyMessageBox("The directory \""
                        + csTemp
                        + "\" contains files that have not "
                          "been deleted.  This directory was "
                          "originally created when the application "
                          "that you are removing was first installed.\r\n"
                          "Do you want to remove this directory anyway?",
                          MB_YESNO | MB_ICONEXCLAMATION)
          == IDYES))
      {
#ifndef _DEBUG

        // NOTE:  if there are *ANY* shares that use this directory,
        //        I must remove THEM as well.

        NukeDirectory(csTemp);
        Sleep(100);  // 100 msecs per directory, eh?

#else
        DeleteSharesOnDirectory(csTemp);
          // in DEBUG mode it won't actually delete them...

#endif // _DEBUG

        TRACE("WOULD BE REMOVING %s\r\n", (LPCSTR)csTemp);
      }
    }

    dlgProgress.SetProgressText(IDS_PROGRESS17);
    dlgProgress.SetProgress(100);

    Sleep(250);

    dlgProgress.SetProgressText(IDS_PROGRESS18);
    dlgProgress.SetProgress(0);

    // CLOSE AND DELETE REGISTRY KEY HERE SO PROGRESS DIALOG CAN SHOW IT

    if(hkeyUninst)
    {
      RegCloseKey(hkeyUninst);
      hkeyUninst = NULL;  // for later
    }

    // remove the entire registry key set for 'UNINST' for this application
    // (if not already deleted) and then I'm done, eh???

    MyDeleteRegistryKey(HKEY_LOCAL_MACHINE, csUninstKey);
    csUninstKey = ""; // for later

    dlgProgress.SetProgress(100);

    Sleep(1000);  // a full second!

    dlgProgress.DestroyWindow();
    m_hWnd = NULL;  // just because
  }

  // close the registry key handle (if not closed already)

  if(hkeyUninst)
  {
    RegCloseKey(hkeyUninst);
    hkeyUninst = NULL;  // by convention
  }


  m_hWnd = NULL;  // just because

  // delete myself!  yay!

  GetModuleFileName(NULL, csTemp.GetBufferSetLength(MAX_PATH), MAX_PATH);
  csTemp.ReleaseBuffer(-1);

  if(csTemp.GetLength())
  {
    // TIME TO ERASE MYSELF NOW!!!

    TRACE0("ERASING " + csTemp + "\r\n");

    // set this process priority at MAXIMUM so I go away

    SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
    SetThreadPriority(THREAD_PRIORITY_HIGHEST); // THREAD_PRIORITY_TIME_CRITICAL);

    if(m_bIsNT)
    {
      TRACE("MoveFileEx(\"%s\", NULL, MOVEFILE_DELAY_UNTIL_REBOOT)\r\n",
            (LPCSTR)csTemp);

#ifndef _DEBUG
      CString csTemp99, csComSpec;
      int i99 = csTemp.ReverseFind('\\');
      if(i99 >= 0)
        csTemp99 = csTemp.Mid(i99 + 1);
      else
        csTemp99.Format("%08x", GetTickCount() ^ GetCurrentProcessId());

      csTemp99 = "ZAP_" + csTemp99;  // the key name

      csComSpec = "";
      if(!ExpandEnvironmentStrings("%comspec%", csComSpec.GetBufferSetLength(MAX_PATH), MAX_PATH))
      {
        csComSpec.ReleaseBuffer(0);
      }
      else
      {
        csComSpec.ReleaseBuffer(-1);
      }

      if(!csComSpec.GetLength())
        csComSpec = "COMMAND";

      csComSpec = '"' + csComSpec + "\" /C DEL \"" + csTemp + '"';

      HKEY hKey = NULL;

      if(RegCreateKey(HKEY_LOCAL_MACHINE,
                      (LPCSTR)((CString)szHKLM_WINDOWS_CURRENT_VERSION + "\\RunOnce"),
                      &hKey)
         != ERROR_SUCCESS ||
         RegSetValueEx(hKey, csTemp99, 0, REG_SZ,
                       (const BYTE *)(LPCSTR)csComSpec, csComSpec.GetLength() + 1)
         != ERROR_SUCCESS)
      {
        // by NOT putting it in the 'delay until reboot' part, I avoid the
        // situation where I think I ought to be rebooting.  ALSO, since it's in
        // the 'RunOnce' section, and *NOT* the 'RunOnce\SETUP' section, I shouldn't
        // flag a reboot from any of the SETUP thingies...

        MoveFileEx(csTemp, NULL, MOVEFILE_DELAY_UNTIL_REBOOT);
      }

      if(hKey)
        RegCloseKey(hKey);
#endif // _DEBUG
    }
    else
    {
      TRACE("WritePrivateProfileString(\"Rename\", \"NUL\", \"%s\", \"%s\")\r\n",
            (LPCSTR)csTemp, (LPCSTR)(m_csWinPath + "WININIT.INI"));


#ifndef _DEBUG
      WritePrivateProfileString("Rename", "NUL", csTemp, m_csWinPath + "WININIT.INI");
#endif // _DEBUG
    }
  }

#ifdef _DEBUG
  Sleep(500);  // to allow debug messages to finish
#endif // _DEBUG

  return(FALSE);  // so I dont' do the "message pump"
}

int CUNINSTApp::Run()
{
//  if (m_pMainWnd == NULL && AfxOleGetUserCtrl())
//  {
//    // Not launched /Embedding or /Automation, but has no main window!
//    TRACE(traceAppMsg, 0, "Warning: m_pMainWnd is NULL in CMyApp::Run - quitting application.\n");
//    AfxPostQuitMessage(0);
//  }
  return(CMyThread::Run());
}

int CUNINSTApp::ExitInstance()
{
  m_hWnd = NULL;  // just because

  CoUninitialize();

  return CMyApp::ExitInstance();
}


//
//BOOL CUNINSTApp::DoEvents()
//{
//static LONG lIdleCount = 0;
//DWORD dwStartTime = GetTickCount();
//
//
//   // phase1: check to see if we can do idle work
//
//   if(!::PeekMessage(&m_msgCur, NULL, NULL, NULL, PM_NOREMOVE))
//   {
//       // call OnIdle unless in bNoIdle state
//      if(OnIdle(lIdleCount++))
//        return(FALSE);
//
//      // at this point, no more idle work to do
//
//      return(FALSE);
//   }
//
//
//   // phase2: pump messages
//
//   do
//   {
//      // pump message, but quit on WM_QUIT
//
//      if(!PumpMessage())
//        return(TRUE);
//
//
//      // reset "no idle" state after pumping "normal" message
//      // (exit on 'Idle' message)
//
//      if(IsIdleMessage(&m_msgCur))
//      {
//         break;
//      }
//
//   } while(::PeekMessage(&m_msgCur, NULL, NULL, NULL, PM_NOREMOVE)
//           && (GetTickCount() - dwStartTime) < 100);
//
//   return(FALSE);
//}

CString CUNINSTApp::GetFileEntryFromName(LPCSTR szInternalName)
{
  int i1;
  CStringArray acsTemp;

  for(i1=0; i1 < m_acsFiles.GetSize(); i1++)
  {
    DoParseString(m_acsFiles[i1], acsTemp);

    if(acsTemp.GetSize() > 2 &&
       !acsTemp[2].CompareNoCase(szInternalName))
    {
      return(m_acsFiles[i1]);
    }
  }

  return("");
}

CString CUNINSTApp::GetFileEntryDestPath(LPCSTR szFileEntry)
{
  CString csFileEntry = szFileEntry;
  CStringArray acsTemp;

  DoParseString(csFileEntry, acsTemp);

  if(acsTemp.GetSize() < 4)
    return("");

  if(acsTemp[3].GetLength())
  {
    CString csTemp = acsTemp[3];

    if(!ExpandStrings(csTemp))
      return("");

    // see if there's an actual path, and if there isn't
    // (or if it's relative), prepend the application path

    // NOTE:  fully qualified path name MUST begin with 'd:\' or '\\'

    if(csTemp.GetLength() < 3 ||
       ((csTemp[0] != '\\' || csTemp[1] != '\\') && csTemp[1] != ':'))
    {
      csTemp = AdjustPathName(m_csAppPath) + csTemp;
    }

    return(csTemp);
  }
  else
  {
    return(AdjustPathName(m_csAppPath) + acsTemp[2]);
  }
}


void CUNINSTApp::GetInfEntries(LPCSTR szKey, CStringArray &acsEntries)
{
  acsEntries.RemoveAll();  // begin by initializing array

  // scan each line, look for '[' + szKey + ']' at the beginning

  CString csTemp, csSearch = '[' + (CString)szKey + ']';

  LPCSTR lpc1, lpc2, lpc3;

  for(lpc1 = (LPCSTR)m_csINFFile; *lpc1; lpc1 = lpc2)
  {
    lpc2 = strchr(lpc1, '\n');  // I can do this because I made the file

    if(!lpc2)
      lpc2 = lpc1 + lstrlen(lpc1);
    else
      lpc2++;  // point PAST it

    lpc3 = lpc1;
    while(*lpc3 <= ' ' && lpc3 < lpc2) // skip white space
      lpc3++;

    if((lpc2 - lpc3) < csSearch.GetLength())
      continue;

    if(!_strnicmp(lpc3, csSearch, csSearch.GetLength()))
    {
      break;  // found!
    }
  }

  if(!*lpc1)
    return;

  for(lpc1 = lpc2; *lpc1; lpc1 = lpc2)  // starts on next line
  {
    lpc2 = strchr(lpc1, '\n');  // I can do this because I made the file

    if(!lpc2)
      lpc2 = lpc1 + lstrlen(lpc1);
    else
      lpc2++;  // point PAST it

    lpc3 = lpc1;
    while(*lpc3 <= ' ' && lpc3 < lpc2) // skip white space
      lpc3++;

    if(*lpc3 == '[')
      break;  // another group

    if(*lpc3 == ';')
      continue;  // a comment line

    // assume that valid strings won't have comments at the end of the line

    csTemp = ((CString)lpc3).Left(lpc2 - lpc3);

    csTemp.TrimRight();
    csTemp.TrimLeft();

    if(csTemp.GetLength())
      acsEntries.Add(csTemp);
  }

}

CString CUNINSTApp::GetInfEntry(LPCSTR szKey, LPCSTR szString)
{
  CStringArray acsEntries;

  GetInfEntries(szKey, acsEntries);

  CString csString = (CString)szString + '=';

  int i1;
  for(i1=0; i1 < acsEntries.GetSize(); i1++)
  {
    if(!acsEntries[i1].Left(csString.GetLength()).CompareNoCase(csString))
    {
      CString csRval = acsEntries[i1].Mid(csString.GetLength());
      csRval.TrimRight();
      csRval.TrimLeft();

      // return it "as-is" with quotes, when applicable

      return(csRval);
    }
  }

  return("");  // not found
}


static CString DriveFromPath(LPCSTR szPath)
{
CString csRval, csPath = szPath;

  // if 'csPath' is UNC, return \\server\share\
  // otherwise, return 'D:\' where 'D' is the drive letter

  if(csPath.GetLength() <= 2)
    return("");  // this is an error

  if(csPath[0] == '\\' && csPath[1] == '\\')
  {
    // UNC path
    int i1;
    for(i1=2; i1 < csPath.GetLength(); i1++)
    {
      if(csPath[i1] == '\\')
      {
        i1++;
        break;
      }
    }

    for(i1=2; i1 < csPath.GetLength(); i1++)
    {
      if(csPath[i1] == '\\')
      {
        i1++;
        break;
      }
    }

    csRval = csPath.Left(i1);
  }
  else
  {
    if(csPath[1] == ':')
    {
      // assume it's a drive

      csRval = csPath.Left(2);
    }
  }

  if(!csPath.GetLength() ||
     csPath[csPath.GetLength() - 1] != '\\')
  {
    csPath += '\\';  // ensure it ends in a backslash!!!
  }

  return(csRval);
}

BOOL CUNINSTApp::ExpandStrings(CString &csStringVal)
{
  CString csTemp;

  // parse out '%apppath%' '%appcommon%' '%sysdir%' '%windir%' '%themedir%',
  // '%tmpdir%', '%appdrive%', '%windrive%', '%sysdrive%', '%bootdrive%',
  // '%tmpdrive%', '%programfiles%, '%appname%', %uninst%

  int i1;
  for(i1=0; i1 < csStringVal.GetLength(); i1++)
  {
    if(csStringVal[i1] == '%')
    {
      if((i1 + 1) < csStringVal.GetLength())
      {
        if(csStringVal[i1 + 1] == '%')
        {
          // double '%' - skip the 2nd '%'
          csStringVal = csStringVal.Left(i1) + csStringVal.Mid(i1 + 1);
        }
        else
        {
          // find next '%' and take segment of string
          int i2;
          for(i2=i1 + 1; i2 < csStringVal.GetLength(); i2++)
          {
            if(csStringVal[i2] == '%')
              break;
          }

          if(i2 < csStringVal.GetLength())
            i2++;  // point PAST the '%'

          csTemp = csStringVal.Mid(i1 + 1, i2 - i1 - 2);

          if(!csTemp.CompareNoCase("apppath"))
          {
            csTemp = AdjustPathName2(m_csAppPath);
          }
          else if(!csTemp.CompareNoCase("appcommon"))
          {
            csTemp = AdjustPathName2(m_csAppCommon);
          }
          else if(!csTemp.CompareNoCase("uninst"))
          {
            // THIS CODE IS COPIED FROM WHAT SETUP DOES WHEN IT ADDS UNINSTALL

            // if I ever ask for this directory, I'll need to delete it
            // so I must add it to my list o' directories to nuke

            csTemp = AdjustPathName(theApp.m_csAppPath)
                   + "UNINST\\"
                   + AppNameToPathName(m_csAppName);
          }
          else if(!csTemp.CompareNoCase("windir"))
          {
            csTemp = AdjustPathName2(m_csWinPath);
          }
          else if(!csTemp.CompareNoCase("sysdir"))
          {
            csTemp = AdjustPathName2(m_csSysPath);
          }
          else if(!csTemp.CompareNoCase("themedir"))
          {
            // TODO:  what is the *real* path for this???

            csTemp = GetRegistryString(HKEY_LOCAL_MACHINE,
                                       szHKLM_WINDOWS_CURRENT_VERSION,
                                       "ProgramFilesDir");

            if(!csTemp.GetLength())
              csTemp = GetRegistryString(HKEY_LOCAL_MACHINE,
                                         szHKLM_WINDOWS_CURRENT_VERSION,
                                         "ProgramFilesPath");  // try both

            if(!csTemp.GetLength())
            {
              // use windows directory

              csTemp = m_csWinPath;
            }

            csTemp = AdjustPathName(csTemp)
                   + "Plus!\\Themes";
          }
          else if(!csTemp.CompareNoCase("tmpdir"))
          {
            csTemp = "";
            GetTempPath(MAX_PATH, csTemp.GetBufferSetLength(MAX_PATH));
            csTemp.ReleaseBuffer(-1);

            if(!csTemp.GetLength())
            {
              csTemp = AdjustPathName(m_csWinPath) + "TEMP";
            }
            else
            {
              csTemp = AdjustPathName2(csTemp);
            }
          }
          else if(!csTemp.CompareNoCase("appdrive"))
          {
            csTemp = DriveFromPath(m_csAppPath);

            if(!csTemp.GetLength())
              return(FALSE);
          }
          else if(!csTemp.CompareNoCase("windrive"))
          {
            csTemp = DriveFromPath(m_csWinPath);

            if(!csTemp.GetLength())
              return(FALSE);
          }
          else if(!csTemp.CompareNoCase("sysdrive"))
          {
            csTemp = DriveFromPath(m_csSysPath);

            if(!csTemp.GetLength())
              return(FALSE);
          }
          else if(!csTemp.CompareNoCase("bootdrive"))
          {
            csTemp = DriveFromPath(m_csWinPath);  // for now - later fix

            if(!csTemp.GetLength())
              return(FALSE);
          }
          else if(!csTemp.CompareNoCase("tmpdrive"))
          {
            csTemp = "";
            GetTempPath(MAX_PATH, csTemp.GetBufferSetLength(MAX_PATH));
            csTemp.ReleaseBuffer(-1);

            if(!csTemp.GetLength())
            {
              csTemp = m_csWinPath; // + "TEMP";
            }

            csTemp = DriveFromPath(csTemp);

            if(!csTemp.GetLength())
              return(FALSE);
          }
          else if(!csTemp.CompareNoCase("programfiles"))
          {
            csTemp = GetRegistryString(HKEY_LOCAL_MACHINE,
                                       szHKLM_WINDOWS_CURRENT_VERSION,
                                       "ProgramFilesDir");

            if(!csTemp.GetLength())
              csTemp = GetRegistryString(HKEY_LOCAL_MACHINE,
                                         szHKLM_WINDOWS_CURRENT_VERSION,
                                         "ProgramFilesPath");  // try both

            if(!csTemp.GetLength())
            {
              // use windows directory to generate it

              csTemp = DriveFromPath(m_csWinPath);
              if(csTemp.GetLength() && csTemp[csTemp.GetLength() - 1] != '\\')
                csTemp += '\\';

              csTemp += "Program Files";
            }

            csTemp = AdjustPathName2(csTemp);  // removes trailing '\'
          }
          else if(!csTemp.CompareNoCase("appname"))
          {
            csTemp = AppNameToPathName(m_csAppName);  // no backslash will be here
          }
          else if(!csTemp.CompareNoCase("srcpath"))
          {
            csTemp = AdjustPathName2(m_csEXEPath);

            if(!csTemp.GetLength())
              return(FALSE);
          }
          else if(!csTemp.CompareNoCase("srcdrive"))
          {
            csTemp = DriveFromPath(m_csEXEPath);

            if(!csTemp.GetLength())
              return(FALSE);
          }
          else
          {
            // on ERROR, return FALSE!!!!!

            return(FALSE);
          }

//          if(csTemp.GetLength() > 3 && 
//             csTemp[csTemp.GetLength() - 1] == '\\')
//            csTemp = csTemp.Left(csTemp.GetLength() - 1);

          csStringVal = csStringVal.Left(i1) + csTemp + csStringVal.Mid(i2);

          i1 += csTemp.GetLength() - 1;  // point at last char of sub'ed string
        }
      }
    }
  }

  return(TRUE);
}




// ** GLOBAL UTILITIES **

void DoParseString(CString &csLine, CStringArray &acsColumns)
{
  acsColumns.RemoveAll();

  // parse into columns, trimming any surrounding quote marks

  int i1=0, i2;

  while(i1 < csLine.GetLength())
  {
    while(i1 < csLine.GetLength() && csLine[i1] <= ' ')
      i1++;

    if(csLine[i1] == '"' ||
       csLine[i1] == '\'')
    {
      i2 = i1++;

      while(i1 < csLine.GetLength() && csLine[i1] != csLine[i2])
        i1++;

      acsColumns.Add(csLine.Mid(i2 + 1, i1 - i2 - 1));

      if(i1 < csLine.GetLength())
        i1++;

      while(i1 < csLine.GetLength() && csLine[i1] != ',')
      {
        if(csLine[i1] > ' ')
        {
          TRACE("Found bad character '%c' in line\r\n", 
                (char)csLine[i1]);
        }

        i1++;
      }
    }
    else
    {
      i2 = i1;
      while(i1 < csLine.GetLength() && csLine[i1] != ',')
        i1++;

      acsColumns.Add(csLine.Mid(i2, i1 - i2));
    }

    if(i1 < csLine.GetLength() && csLine[i1] == ',')
      i1++;

    while(i1 < csLine.GetLength() && csLine[i1] <= ' ')
      i1++;
  }
}


BOOL IsValidDirectory(LPCSTR szDirName)
{
  CString csPath = szDirName;

  if(csPath.GetLength() == 3 &&
     toupper(csPath[0]) >= 'A' &&
     toupper(csPath[0]) <= 'Z' &&
     csPath[1] == ':' &&
     csPath[2] == '\\')
  {
    UINT uiType = GetDriveType(csPath);  // see if it's valid
    if(uiType == DRIVE_UNKNOWN || uiType == DRIVE_NO_ROOT_DIR)
    {
      return(FALSE);
    }

    return(TRUE);  // assume valid
  }

  // if it's not a root directory, and it ends in a backslash,
  // we want to get RID of the ending backslash.

  if(csPath.GetLength() && csPath[csPath.GetLength() - 1] == '\\')
    csPath = csPath.Left(csPath.GetLength() - 1);


  WIN32_FIND_DATA fd;
  HANDLE hFF = FindFirstFile(csPath, &fd);

  if(hFF == INVALID_HANDLE_VALUE)
  {
    return(FALSE);  // not valid
  }

  BOOL bIsDir = fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;

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

  return(bIsDir);
}

int FileAlreadyExists(LPCSTR szFileName)
{
  // validate file name and check for existence

  LPSTR lp1;
  CString csFile;

  if(!GetFullPathName(szFileName, MAX_PATH, csFile.GetBufferSetLength(MAX_PATH), &lp1))
  {
    csFile.ReleaseBuffer(0);
    return(-1);
  }

  csFile.ReleaseBuffer(-1);
  int i1 = csFile.ReverseFind('\\');

  CString csPathOnly = csFile.Left(i1 + 1);  // include backslash

  if(!IsValidDirectory(csPathOnly))
    return(-1);  // not a valid directory

  WIN32_FIND_DATA fd;
  HANDLE hFF = FindFirstFile(csFile, &fd);

  if(hFF == INVALID_HANDLE_VALUE)
  {
    return(0);  // file does not exist
  }

  BOOL bIsDir = fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;

  FindClose(hFF);

  if(bIsDir)
  {
    return(-1);  // not allowed
  }
  else
  {
    return(1);  // file was found and is not a directory name
  }
}


CString TranslateEscapeCodes(CString &csSource)
{
  CString csRval = "";

  int i1;
  for(i1=0; i1 < csSource.GetLength(); i1++)
  {
    char c1 = csSource[i1];

    if(c1 == '\\' && 
       (i1 + 1) < csSource.GetLength())
    {
      i1++;
      c1 = toupper(csSource[i1]);
      switch(c1)
      {
        case 'A':
          c1 = '\a';
          break;
        case 'B':
          c1 = '\b';
          break;
        case 'F':
          c1 = '\f';
          break;
        case 'N':
          c1 = '\n';
          break;
        case 'R':
          c1 = '\r';
          break;
        case 'T':
          c1 = '\t';
          break;
        case 'V':
          c1 = '\v';
          break;
        case '?':
          c1 = '?';
          break;
        case '\'':
          c1 = '\'';
          break;
        case '"':
          c1 = '"';
          break;
        case '\\':
          c1 = '\\';
          break;
        case '0':
          c1 = '\0';
          break;
        default:
          if(c1 == 'X')
          {
            c1 = 0;

            if((i1 + 1) < csSource.GetLength())
            {
              char c2 = toupper(csSource[i1 + 1]);
              if((c2 >= '0' && c2 <= '9') ||
                 (c2 >= 'A' && c2 <= 'F'))
              {
                i1++;

                if(c2 >= 'A' && c2 <= 'F')
                {
                  c1 += 10 + c2 - 'A';
                }
                else
                {
                  c1 += c2 - '0';
                }

                if((i1 + 1) < csSource.GetLength())
                {
                  c2 = toupper(csSource[i1 + 1]);

                  if((c2 >= '0' && c2 <= '9') ||
                     (c2 >= 'A' && c2 <= 'F'))
                  {
                    i1++;

                    c1 *= 16;

                    if(c2 >= 'A' && c2 <= 'F')
                    {
                      c1 += 10 + c2 - 'A';
                    }
                    else
                    {
                      c1 += c2 - '0';
                    }
                  }
                }
              }
            }
          }
          else
          {
            c1 = 0;

            if((i1 + 1) < csSource.GetLength())
            {
              char c2 = toupper(csSource[i1 + 1]);
              if(c2 >= '0' && c2 <= '7')
              {
                i1++;
                c1 += c2 - '0';

                if((i1 + 1) < csSource.GetLength())
                {
                  c2 = toupper(csSource[i1 + 1]);

                  if(c2 >= '0' && c2 <= '7')
                  {
                    i1++;

                    c1 *= 8;
                    c1 += c2 - '0';

                    if((i1 + 1) < csSource.GetLength())
                    {
                      c2 = toupper(csSource[i1 + 1]);

                      if(c2 >= '0' && c2 <= '7')
                      {
                        i1++;

                        c1 *= 8;
                        c1 += c2 - '0';
                      }
                    }
                  }
                }
              }
            }
          }
      }
    }

    csRval += c1;
  }

  return(csRval);
}  



// REGISTRY KEY HELPERS

CString GetRegistryString(HKEY hkeyRoot, LPCSTR szKeyName, LPCSTR szString)
{
  HKEY hKey;

  if(RegOpenKeyEx(hkeyRoot, szKeyName, 0, KEY_READ, &hKey)
     != ERROR_SUCCESS)
  {
    return("");
  }

  CString csRval;
  DWORD dw1, cb1 = 65535;

  if(RegQueryValueEx(hKey, szString, NULL, &dw1,
                     (LPBYTE)(csRval.GetBufferSetLength(cb1 + 1)),
                     &cb1) != ERROR_SUCCESS)
  {
    csRval.ReleaseBuffer(0);
  }
  else
  {
    csRval.ReleaseBuffer(cb1);
    csRval.ReleaseBuffer(-1);

    if(dw1 == REG_EXPAND_SZ)
    {
      CString csTemp = csRval;
      csRval = "";

      DWORD cbSize = csTemp.GetLength() * 16 + MAX_PATH;
      dw1 = ExpandEnvironmentStrings(csTemp, csRval.GetBufferSetLength(cbSize), cbSize);

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

  RegCloseKey(hKey);

  return(csRval);
}

BOOL CheckKeyDeleteOK(HKEY hkeyRoot, LPCSTR szKeyName)
{
#define IT_MATCHES(X,Y) (!_strnicmp(X,Y,sizeof(Y)-1) ? X += sizeof(Y) - 1, TRUE : FALSE)

// I check for the following and prevent deletion:
//
// HKLM|HKCU SOFTWARE
// HKLM|HKCU SOFTWARE\MICROSOFT
// HKLM|HKCU SOFTWARE\MICROSOFT\WINDOWS
// HKLM|HKCU SOFTWARE\MICROSOFT\WINDOWS NT
// HKLM|HKCU SOFTWARE\MICROSOFT\WINDOWS\CURRENTVERSION [\ANY]
// HKLM|HKCU SOFTWARE\MICROSOFT\WINDOWS NT\CURRENTVERSION [\ANY]
// HKLM|HKCU SOFTWARE\MICROSOFT\WINDOWS NT\CURRENTVERSION\INIFILEMAPPING [\ANY]
// HKLM      SYSTEM\CURRENTCONTROLSET
// HKLM      SYSTEM\CURRENTCONTROLSET\[CONTROL | ENUM | HARDWARE PROFILES | SERVICES] [\ANY]
// HKLM      SYSTEM\SETUP
// HKLM      SYSTEM\SETUP\ALLOWSTART [\ANY]


  static const char szSOFTWARE[]="SOFTWARE",
                    szMICROSOFT[]="MICROSOFT",
                    szWINDOWS[]="WINDOWS",
                    sz_NT[]=" NT",
                    szCURRENTVERSION[]="CURRENTVERSION",
                    szSYSTEM[]="SYSTEM",
                    szSETUP[]="SETUP",
                    szCURRENTCONTROLSET[]="CURRENTCONTROLSET",
                    szINIFILEMAPPING[]="INIFILEMAPPING",
                    szUNINSTALL[]="UNINSTALL",
                    szCONTROL[]="CONTROL",
                    szENUM[]="ENUM",
                    szHARDWARE_PROFILES[]="HARDWARE PROFILES",
                    szSERVICES[]="SERVICES",
                    szALLOWSTART[]="ALLOWSTART";

  BOOL bIgnoreFlag = FALSE;

  // TODO:  how do I check for aliasing???


  if(hkeyRoot == HKEY_LOCAL_MACHINE ||
     hkeyRoot == HKEY_CURRENT_USER)
  {
    LPCSTR lpc1 = szKeyName;

    if(IT_MATCHES(lpc1, szSOFTWARE))
    {
      if(!*lpc1)
      {
        bIgnoreFlag = TRUE;
      }
      else if(*lpc1 == '\\')
      {
        lpc1++;
        if(IT_MATCHES(lpc1, szMICROSOFT))
        {
          if(!*lpc1)
          {
            bIgnoreFlag = TRUE;
          }
          else if(*lpc1 == '\\')
          {
            lpc1++;

            if(IT_MATCHES(lpc1, szWINDOWS))
            {
              BOOL bNT = FALSE;

              if(IT_MATCHES(lpc1, sz_NT))
              {
                bNT = TRUE;
              }

              if(!*lpc1)
              {
                bIgnoreFlag = TRUE;
              }
              else if(*lpc1 == '\\')
              {
                lpc1++;

                if(IT_MATCHES(lpc1, szCURRENTVERSION))
                {
                  if(!*lpc1)
                  {
                    bIgnoreFlag = TRUE;
                  }
                  else if(*lpc1 == '\\')
                  {
                    lpc1++;

                    // any single-level key must NOT be deleted here

                    if(!strchr(lpc1, '\\'))
                    {
                      bIgnoreFlag = TRUE;  // single-level key
                    }
                    else
                    {
                      if(bNT && IT_MATCHES(lpc1, szINIFILEMAPPING))  // for the NT key only
                      {
                        if(!*lpc1)
                        {
                          bIgnoreFlag = TRUE;  // single-level key
                        }
                        else if(*lpc1 == '\\')
                        {
                          lpc1++;

                          // if it's one of the system's INI files I must ignore it
                          // TODO:  check for these

                          bIgnoreFlag = TRUE;  // for now, don't remove ANY of them!
                        }
                      }
                      else if(!bNT && IT_MATCHES(lpc1, szUNINSTALL))
                      {
                        if(!*lpc1)
                        {
                          bIgnoreFlag = TRUE;
                        }
                        else if(*lpc1 == '\\')
                        {
                          lpc1++;

                          // TODO:  do I want to do anything here???
                        }
                        else
                        {
                          // TODO:  check for other "no no" values (see below)

                          bIgnoreFlag = TRUE;  // for now, just don't allow key delete!
                        }
                      }
                      else
                      {
                        // TODO:  check for other "no no" values

                        bIgnoreFlag = TRUE;  // for now, just don't allow key delete!
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
    else if(hkeyRoot == HKEY_LOCAL_MACHINE  &&
            IT_MATCHES(lpc1, szSYSTEM))
    {
      if(!*lpc1)
      {
        bIgnoreFlag = TRUE;
      }
      else if(*lpc1 == '\\')
      {
        lpc1++;

        if(IT_MATCHES(lpc1, szCURRENTCONTROLSET))
        {
          if(!*lpc1)
          {
            bIgnoreFlag = TRUE;
          }
          else if(*lpc1 == '\\')
          {
            lpc1++;

            if(IT_MATCHES(lpc1, szCONTROL) ||
               IT_MATCHES(lpc1, szHARDWARE_PROFILES) ||
               IT_MATCHES(lpc1, szENUM) ||
               IT_MATCHES(lpc1, szSERVICES))
            {
              if(!*lpc1 || *lpc1 == '\\')
                bIgnoreFlag = TRUE;  // allow NO auto-delete in this area!
            }

            if(!bIgnoreFlag)
            {
              // any single-level sub-key in this area must NOT be deleted!

              if(!strchr(lpc1, '\\'))
              {
                bIgnoreFlag = TRUE;  // single-level key
              }
              else
              {
                // TODO:  something???
              }
            }
          }              
        }
        else if(IT_MATCHES(lpc1, szSETUP))
        {
          if(!*lpc1)
          {
            bIgnoreFlag = TRUE;
          }
          else if(*lpc1 == '\\')
          {
            lpc1++;

            if(IT_MATCHES(lpc1, szALLOWSTART))
            {
              if(!*lpc1)
              {
                bIgnoreFlag = TRUE;
              }
              else if(*lpc1 == '\\')
              {
                lpc1++;

                // any single-level sub-key in this area must NOT be deleted!

                if(!strchr(lpc1, '\\'))
                {
                  bIgnoreFlag = TRUE;  // single-level key
                }
                else
                {
                  // TODO:  something???
                }
              }
            }
          }
        }
      }
    }
  }

  return(!bIgnoreFlag);
}

BOOL WriteRegistryString(HKEY hkeyRoot, LPCSTR szKeyName, LPCSTR szString, LPCSTR szValue)
{
  HKEY hKey = NULL;

  LONG lr = RegOpenKeyEx(hkeyRoot, szKeyName, 0, KEY_ALL_ACCESS, &hKey);
  if(lr != ERROR_SUCCESS)
  {
//    if(!szString && !szValue)
//    {
//      return(TRUE);  // this is ok (not found)
//    }

    // FOR NOW - re-create key if it's not there, then delete it again, below

    DWORD dwDisp;

    if(RegCreateKeyEx(hkeyRoot, szKeyName, 0, NULL,
                      REG_OPTION_NON_VOLATILE,
                      KEY_ALL_ACCESS, NULL, &hKey, &dwDisp)
       != ERROR_SUCCESS)
    {
      return(FALSE);
    }
  }

  BOOL bRval = TRUE;

  if(!szValue)  // deletes the value
  {
    if(!szString)  // deletes the key
    {
      if(!CheckKeyDeleteOK(hkeyRoot, szKeyName))
      {
        RegCloseKey(hKey);

        return(TRUE);  // exit without removing the key
      }

      // enum all sub-keys and delete them (for NT)

      CStringArray acsKeys;

      if(!EnumRegistryKeys(hKey, "", acsKeys))
      {
        bRval = FALSE;
      }
      else
      {
        int i1;
        for(i1=0; i1 < acsKeys.GetSize(); i1++)
        {
          if(!WriteRegistryString(hKey, acsKeys[i1], NULL, NULL))
          {
            bRval = FALSE;
            break;
          }
        }

        if(bRval)
        {
          RegCloseKey(hKey);
          hKey = NULL;

          bRval = (RegDeleteKey(hkeyRoot, szKeyName) == ERROR_SUCCESS);
        }
      }
    }
    else // deletes the value
    {
      bRval = (RegDeleteValue(hKey, szString) == ERROR_SUCCESS);
    }
  }
  else if(RegSetValueEx(hKey, szString, 0, REG_SZ,
                        (const BYTE *)szValue, lstrlen(szValue) + 1)
     != ERROR_SUCCESS)
  {
    bRval = FALSE;
  }

  if(hKey)
  {
    RegFlushKey(hKey);
    RegCloseKey(hKey);
  }

  RegFlushKey(hkeyRoot);

  return(bRval);
}

BOOL EnumRegistryKeys(HKEY hkeyRoot, LPCSTR szKeyName, CStringArray &acsKeys)
{
  acsKeys.RemoveAll();

  HKEY hKey;

  if(RegOpenKeyEx(hkeyRoot, szKeyName, 0, KEY_READ, &hKey)
     != ERROR_SUCCESS)
  {
    return(FALSE);
  }

  CString csTemp;

  LPSTR lp1 = csTemp.GetBufferSetLength(MAX_PATH * 4 + 4);
  memset(lp1, 0, MAX_PATH * 4 + 4);

  DWORD dw1;

  for(dw1=0; RegEnumKey(hKey, dw1, lp1, MAX_PATH * 4 + 4)
             == ERROR_SUCCESS; dw1++)
  {
    acsKeys.Add(lp1);
    memset(lp1, 0, MAX_PATH * 4 + 4);
  }

  csTemp.ReleaseBuffer(0);

  RegCloseKey(hKey);

  return(TRUE);
}

// delete all keys within the specified key ('95 default)

BOOL MyDeleteRegistryKey(HKEY hkeyRoot, LPCSTR szKeyName)
{
  CStringArray acsKeys;

  if(!CheckKeyDeleteOK(hkeyRoot, szKeyName))
  {
    TRACE("IGNORING REMOVAL OF REGISTRY KEY %08xH\\%s\r\n", hkeyRoot, szKeyName);
    return(TRUE);  // assume that it worked so I can continue without error
  }

  if(!EnumRegistryKeys(hkeyRoot, szKeyName, acsKeys))
  {
    return(FALSE);
  }

  int i1;
  for(i1=0; i1 < acsKeys.GetSize(); i1++)
  {
    if(!MyDeleteRegistryKey(hkeyRoot, (CString)szKeyName + "\\" + acsKeys[i1]))
      return(FALSE);
  }

  
  TRACE("WOULD BE REMOVING REGISTRY KEY %08xH\\%s\r\n", hkeyRoot, szKeyName);

#ifndef _DEBUG
  return(RegDeleteKey(hkeyRoot, szKeyName) == ERROR_SUCCESS);
#else  // _DEBUG
  return(TRUE);
#endif // _DEBUG
}

// delete key ONLY if no sub-keys (NT default)

BOOL MyDeleteRegistryKey0(HKEY hkeyRoot, LPCSTR szKeyName)
{
  CStringArray acsKeys;

  if(!CheckKeyDeleteOK(hkeyRoot, szKeyName))
  {
    TRACE("IGNORING REMOVAL OF REGISTRY KEY %08xH\\%s\r\n", hkeyRoot, szKeyName);
    return(TRUE);  // assume that it worked so I can continue without error
  }

  if(!EnumRegistryKeys(hkeyRoot, szKeyName, acsKeys) ||
     acsKeys.GetSize())
  {
    return(FALSE);
  }


  TRACE("WOULD BE REMOVING REGISTRY KEY %08xH\\%s\r\n", hkeyRoot, szKeyName);

#ifndef _DEBUG
  return(RegDeleteKey(hkeyRoot, szKeyName) == ERROR_SUCCESS);
#else  // _DEBUG
  return(TRUE);
#endif // _DEBUG
}


CString AppNameToPathName(LPCSTR szAppName, BOOL bStripBackSlash /* = TRUE */)
{
  int i1;
  CString csRval = szAppName;

  for(i1=0; i1 < csRval.GetLength(); i1++)
  {
    char c1 = csRval[i1];

    if((c1 >= 'a' && c1 <= 'z') || // valid
       (c1 >= 'A' && c1 <= 'Z') ||
       (c1 >= '0' && c1 <= '9') ||
       c1 == ' ' || c1 == '%' || c1 == '$' ||
       c1 == '#' || c1 == '^' || c1 == '-' ||
       c1 == '@' || c1 == '!' || c1 == '~' ||
       c1 == '(' || c1 == ')' || c1 == '[' ||
       c1 == ']' || c1 == '{' || c1 == '}' ||
       c1 == '_' || c1 == '.' || c1 == '\'' ||  // assume single quote ok
       (c1 == '\\' && !bStripBackSlash))        // backslash allowed
    {
      // these characters *ARE* allowed
    }
    else
    {
      csRval.SetAt(i1, '_');  // change invalid characters to underscores
    }
  }

  return(csRval);
}

CString AppNameToRegKey(LPCSTR szAppName)
{
  int i1;
  CString csRval = szAppName;

  for(i1=0; i1 < csRval.GetLength(); i1++)
  {
    char c1 = csRval[i1];

    if(c1 == '\\' || c1 < ' ' || c1 > 0x7f)  // for now, just these
    {
      csRval.SetAt(i1, '_');  // change invalid characters to underscores
    }
  }

  return(csRval);
}

CString AdjustPathName(LPCSTR szPathName)
{
  CString csRval = szPathName;

  if(csRval.GetLength() && csRval[csRval.GetLength() - 1] != '\\')
    csRval += '\\';

  return(csRval);
}

CString AdjustPathName2(LPCSTR szPathName)
{
  CString csRval = szPathName;

  if(csRval.GetLength() /*> 3*/ && csRval[csRval.GetLength() - 1] == '\\')
    csRval = csRval.Left(csRval.GetLength() - 1);  // no ending backslash

  return(csRval);
}

BOOL IsDotNetAvailable()
{
  CString csTemp = LocateBoguSoftNotYetRegistrationApp();
  return(csTemp.GetLength() > 0);
}

CString LocateBoguSoftNotYetRegistrationApp()
{
  // locates 'or^H^Hregasm'

  // To check for .net installation
  // [HKLM\SOFTWARE\Microsoft\.NETFramework]
  //   "InstallRoot"="c:\winnt\Microsoft.NET\Framework\"

  // easiest method to use is to search this directory tree for
  // 'regasm.exe' and invoke it with the DLL as the 1st parameter.
  // the location of 'regasm.exe' should be to find 'mscormmc.dll'
  // (which seems to have multiple versions) and use the NEWEST
  // version of it.  The 'Microsoft.CLRAdmin' classes seem to be
  // the "best bet" for locating it.
  // 'System.Version' points to 'mscoree.dll' which resides in
  // the system32 directory.  The CLSID has a string value
  // 'RuntimeVersion' that matches the sub-directory name.

  CString csTemp, csTemp2, csRval;

  HKEY hKey = NULL;

  if(RegOpenKey(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\.NetFramework",
                &hKey)
     == ERROR_SUCCESS)
  {
    csRval = GetRegistryString(hKey, "", "InstallRoot");
    if(csRval.GetLength())
    {
      if(csRval[csRval.GetLength() - 1] != '\\')
        csRval += '\\';

      // get version information next

      csTemp = GetRegistryString(HKEY_CLASSES_ROOT,
                                 "System.Version\\CLSID",
                                 "");

      if(!csTemp.GetLength())
        csTemp = GetRegistryString(HKEY_CLASSES_ROOT,
                                   "Microsoft.CLRAdmin.CAbout\\CLSID",
                                    "");

      if(csTemp.GetLength())
      {
        csTemp2 = GetRegistryString(HKEY_CLASSES_ROOT,
                                    "CLSID\\" + csTemp
                                    + "\\InProcServer32",
                                    "RuntimeVersion");

        if(csTemp2.GetLength())
        {
          csTemp2 = csRval + csTemp2 + '\\';

          if(FileAlreadyExists(csTemp2 + "regasm.exe") > 0)
            return(csTemp2 + "regasm.exe");
        }
      }
    }

    RegCloseKey(hKey);
  }

  if(csRval.GetLength())
  {
    // TODO:  search for 'regasm.exe' within this tree
  }

  return("");  // temporary (just return error)
}

BOOL ShellExecApp(LPCSTR szApp, LPCSTR szParms, UINT nCmdShow /*= SW_SHOWNORMAL*/)
{
  BOOL bRval = TRUE;

  SHELLEXECUTEINFO sei;
  memset(&sei, 0, sizeof(sei));
  sei.cbSize = sizeof(sei);
  sei.fMask = SEE_MASK_FLAG_NO_UI |  SEE_MASK_NOCLOSEPROCESS;
  sei.lpFile = szApp;
  sei.lpParameters = szParms;
  sei.nShow = nCmdShow;

  CString csMsg;

  if(!ShellExecuteEx(&sei))
  {
    csMsg.Format(IDS_ERROR13, szApp);

    MyMessageBox(csMsg, MB_OK | MB_ICONHAND | MB_SETFOREGROUND);

    return(FALSE);  // this is really a FATAL error...
  }
  else
  {
    WaitForSingleObject(sei.hProcess, INFINITE);

    DWORD dwExitCode;
    if(!GetExitCodeProcess(sei.hProcess, &dwExitCode))
    {
      csMsg.Format(IDS_WARNING12, szApp);

      MyMessageBox(csMsg, MB_OK | MB_ICONASTERISK | MB_SETFOREGROUND);

      bRval = FALSE;
    }
    else if(dwExitCode == STILL_ACTIVE)
    {
      csMsg.Format(IDS_WARNING13, szApp);

      MyMessageBox(csMsg, MB_OK | MB_ICONASTERISK | MB_SETFOREGROUND);

      bRval = FALSE;
    }
    else if(dwExitCode)
    {
      csMsg.Format(IDS_WARNING14, szApp, dwExitCode);

      MyMessageBox(csMsg, MB_OK | MB_ICONASTERISK | MB_SETFOREGROUND);

      bRval = FALSE;
    }
  }

  return(bRval);
}


BOOL SafeCallDllUnregisterServer(LPCSTR szLibrary, CString &csMsg)
{
  HINSTANCE h1 = NULL;

  __try
  {
    h1 = LoadLibrary(szLibrary);
  }
  __except(EXCEPTION_EXECUTE_HANDLER)
  {
    h1 = NULL;
  }

  if(!h1)
  {
    csMsg.Format(IDS_UNREG_FAILED, szLibrary);

    MyMessageBox(csMsg, MB_OK | MB_ICONHAND | MB_SETFOREGROUND);

    return(TRUE);  // this is *NOT* a fatal error
  }

  BOOL bRval = FALSE;

  HRESULT (STDAPICALLTYPE *pDllUnregisterServer)(void);

  (FARPROC &)pDllUnregisterServer = GetProcAddress(h1, "DllUnregisterServer");

  if(pDllUnregisterServer)  // it worked
  {
    HRESULT hr = E_UNEXPECTED;

    __try
    {
      hr = pDllUnregisterServer();
    }
    __except(EXCEPTION_EXECUTE_HANDLER)
    {
      hr = E_UNEXPECTED;
    }

    if(hr == NO_ERROR)
      bRval = TRUE;
  }

  __try
  {
    FreeLibrary(h1);
  }
  __except(EXCEPTION_EXECUTE_HANDLER)
  {
  }

  if(!bRval)
  {
    csMsg.Format(IDS_ERROR11, szLibrary);

    if(MyMessageBox(csMsg, MB_YESNO | MB_ICONHAND | MB_SETFOREGROUND)
       == IDYES)
    {
      bRval = TRUE;  // so that it continues with the next one
    }
  }

  return(bRval);
}

BOOL InternalEnumNetResources(LPNETRESOURCE lpNS, CStringArray &acsShares)
{
  // build a list of things on the network

  // I could call 'gethostname' for this...

  CString csTemp, csHost;
  DWORD dw1, dwRval;
  HANDLE hEnum = NULL;

  if(!lpNS)
  {
    GetComputerName(csHost.GetBufferSetLength(dw1 = MAX_PATH), &dw1);
    csHost.ReleaseBuffer(dw1);
    csHost.ReleaseBuffer(-1);

    if(csHost.GetLength() > 2 && (csHost[0] != '\\' || csHost[1] != '\\'))
      csHost = "\\\\" + csHost;  // must start with '\\'
  }

  DWORD dwScope = RESOURCE_GLOBALNET;

  if(!lpNS)
  {
    acsShares.RemoveAll();

    if(!csHost.GetLength())
      return(FALSE);

    TCHAR tbuf[MAX_PATH * 4];
    strcpy_s(tbuf, csHost);

    NETRESOURCE ns;
    ns.dwScope = RESOURCE_GLOBALNET;
    ns.dwType = RESOURCETYPE_ANY;
    ns.dwDisplayType = RESOURCEDISPLAYTYPE_SERVER;
    ns.dwUsage = RESOURCEUSAGE_CONTAINER;
    ns.lpComment = NULL;
    ns.lpLocalName = tbuf;
    ns.lpRemoteName = tbuf;
    ns.lpProvider = NULL;

    return(InternalEnumNetResources(&ns, acsShares));
  }

  if((dwRval = WNetOpenEnum(dwScope, RESOURCETYPE_DISK,
                            RESOURCEUSAGE_CONTAINER | RESOURCEUSAGE_CONNECTABLE,
                            lpNS, &hEnum))
     != NO_ERROR)
  {
    return(FALSE);  // some kind of error there was
  }


  BOOL bRval = TRUE, bFirstTime = !lpNS;
  DWORD dwCount, dwBufSize;

  LPNETRESOURCEA lpCurNS, lpBuf;  // force it to ASCII version, on purpose


  lpBuf = (LPNETRESOURCE)GlobalAlloc(GPTR, sizeof(*lpBuf) * 1024);  // 1024 max per buffer

  // enumerate all resources, recursing for any container resources found
  while(lpBuf &&
        ((dwCount=1, dwBufSize = 1024*sizeof(*lpBuf),  // 1 at a time - else not reliable
          memset(lpBuf, 0, dwBufSize),
          dwRval = WNetEnumResource(hEnum,&dwCount,lpBuf,&dwBufSize))
         == NO_ERROR || dwRval == ERROR_MORE_DATA))
  {
    bFirstTime = FALSE;

    for(dw1=0, lpCurNS = lpBuf; dw1 < dwCount; dw1++, lpCurNS++)
    {
      if(lpCurNS->dwScope == RESOURCE_CONNECTED ||
         lpCurNS->dwScope == RESOURCE_REMEMBERED)
      {
#ifdef _DEBUG
        if(!lpCurNS->lpLocalName)  // no local drive letter assigned
        {
          TRACE("ADD (1):  %s, %s\r\n", (LPCSTR)lpNS->lpRemoteName, (LPCSTR)lpCurNS->lpRemoteName);
        }
#endif // _DEBUG
      }
      else if(lpCurNS->dwScope == RESOURCE_GLOBALNET &&
              lpCurNS->dwUsage & RESOURCEUSAGE_CONTAINER)
      {
        if(lpCurNS->dwDisplayType == RESOURCEDISPLAYTYPE_SERVER)  // a network machine name
        {
          if(!InternalEnumNetResources(lpCurNS, acsShares))
          {
            bRval = FALSE;  // make sure it returns 'error'
            break;
          }
        }
        else
        {
          // RECURSE RECURSE RECURSE!
          if(lpCurNS->lpRemoteName)  // exclude 'NULL'
          {
            TRACE("RECURSE:  %s\r\n", (LPCSTR)lpCurNS->lpRemoteName);

            if(!InternalEnumNetResources(lpCurNS, acsShares))
            {
              bRval = FALSE;  // make sure it returns 'error'
              break;
            }
          }
        }
      }
      else if(lpCurNS->dwScope == RESOURCE_GLOBALNET)  // shares
      {
#ifdef _DEBUG
        TRACE("SHARED RESOURCE:  %s, %s\r\n", (LPCSTR)lpNS->lpRemoteName, (LPCSTR)lpCurNS->lpRemoteName);
#endif // _DEBUG

        if(lpCurNS->dwType == RESOURCETYPE_DISK)  // shares only
        {
          acsShares.Add(lpCurNS->lpRemoteName);
        }
      }
    }
  }

  if(lpBuf)
    GlobalFree(lpBuf);

  if(!lpBuf)  // not enough memory
  {
    MyMessageBox("Not enough memory to enumerate all network resources");

    bRval = FALSE;
  }
  else if(dwRval != NO_ERROR && dwRval != ERROR_NO_MORE_ITEMS)
  {
    // error trapping

    if(bFirstTime)  // first time thorough
      MyMessageBox("Unable to enumerate network resources due to an error on the network.");
    else
      MyMessageBox("Unable to enumerate all of the network resources due to an error on the network.");

    bRval = FALSE;  // indicate an error here
  }

  // close the enumeration

  WNetCloseEnum(hEnum);

  return(bRval);
}

BOOL DeleteSharesOnDirectory(LPCSTR szDirectory)
{
  WCHAR wszTemp[16384];  // 32k of "stuff" 

  HMODULE hMod = LoadLibrary("NETAPI32.DLL");

  if(!hMod)
  {
    TRACE0("WARNING:  DeleteSharesOnDirectory() returns FALSE\r\n");
    return(FALSE);
  }

  if(!szDirectory)
  {
    FreeLibrary(hMod);
    TRACE0("WARNING:  DeleteSharesOnDirectory() returns FALSE\r\n");
    return(FALSE);
  }

  if(szDirectory[0] == '\\' && szDirectory[1] == '\\')
  {
    FreeLibrary(hMod);
    return(TRUE);
  }

  // TODO:  check drive type *NOT* 'REMOTE' ?


  CString csTemp, csDirectory = AdjustPathName2(szDirectory);

  if(!csDirectory.GetLength())
  {
    FreeLibrary(hMod);
    TRACE0("WARNING:  DeleteSharesOnDirectory() returns FALSE\r\n");
    return(FALSE);
  }

  CStringArray acsShares, acsLocal;
  if(!InternalEnumNetResources(NULL, acsShares))
  {
    FreeLibrary(hMod);
    TRACE0("WARNING:  DeleteSharesOnDirectory() returns FALSE\r\n");
    return(FALSE);
  }

  // delete all shares with matching path
  int i1, i2;

  if(GetVersion() & 0x80000000)  // win '9x/ME ?
  {
    API_RET_TYPE (APIENTRY *pNetShareGetInfo)(const char FAR * pszServer,        
                                              const char FAR * pszNetName,       
                                              short sLevel,                      
                                              char FAR * pbBuffer,               
                                              unsigned short  cbBuffer,          
                                              unsigned short FAR * pcbTotalAvail) = NULL;

    (FARPROC &)pNetShareGetInfo = GetProcAddress(hMod, "NetShareGetInfo");

    for(i1=0; pNetShareGetInfo && i1 < acsShares.GetSize(); i1++)
    {
      CString csShare = acsShares[i1];

      if(csShare.GetLength())
      {
        i2 = csShare.ReverseFind('\\');
        if(i2 > 0)
          csShare = csShare.Mid(i2 + 1);  // after backslash

        WORD cb1 = sizeof(wszTemp);  // convenient
        memset(wszTemp, 0, sizeof(wszTemp));  // just in case

        LPSTR lp1 = (LPSTR)wszTemp;

        if(pNetShareGetInfo(NULL, (LPCSTR)csShare, 50,
                            lp1, sizeof(wszTemp), &cb1)
           == NERR_Success)
        {
          share_info_50 *pS = (share_info_50 *)lp1;

          if(pS->shi50_path && strlen(pS->shi50_path))
            acsLocal.SetAtGrow(i1, pS->shi50_path);
        }
      }
    }
  }
  else
  {
    NET_API_STATUS (NET_API_FUNCTION *pNetShareGetInfo)(LPWSTR servername,  
                                                        LPWSTR netname,     
                                                        DWORD level,        
                                                        LPBYTE *bufptr) = NULL;
    NET_API_STATUS (NET_API_FUNCTION *pNetApiBufferFree)(LPVOID) = NULL;

    (FARPROC &)pNetShareGetInfo = GetProcAddress(hMod, "NetShareGetInfo");
    (FARPROC &)pNetApiBufferFree = GetProcAddress(hMod, "NetApiBufferFree");

    if(pNetShareGetInfo && pNetApiBufferFree)
    {
      for(i1=0; i1 < acsShares.GetSize(); i1++)
      {
        CString csShare = acsShares[i1];

        if(csShare.GetLength())
        {
          i2 = csShare.ReverseFind('\\');
          if(i2 > 0)
            csShare = csShare.Mid(i2 + 1);  // after backslash

          memset(wszTemp, 0, sizeof(wszTemp));

          MultiByteToWideChar(CP_ACP, 0, csShare, csShare.GetLength() + 1,
                              wszTemp, sizeof(wszTemp)/sizeof(*wszTemp));

          SHARE_INFO_2 *pSI2 = NULL;
          NET_API_STATUS stat = pNetShareGetInfo(NULL, wszTemp, 2, (BYTE **)&pSI2);

          if(stat == NERR_Success && pSI2)
          {
            DWORD cb1 = lstrlenW((WCHAR *)pSI2->shi2_path);
            LPSTR lp1 = csTemp.GetBufferSetLength(cb1 * 2);

            WideCharToMultiByte(CP_ACP, 0,
                                (WCHAR *)pSI2->shi2_path, cb1 + 1,
                                lp1, cb1 * 2, NULL, NULL);

            csTemp.ReleaseBuffer(-1);  // that oughta fix it
            acsLocal.SetAtGrow(i1, csTemp);
          }

          if(pSI2)
          {
            pNetApiBufferFree(pSI2);
            pSI2 = NULL;  // by convention
          }
        }
      }
    }
  }

  BOOL bRval = TRUE;

  for(i1=0; i1 < acsLocal.GetSize(); i1++)
  {
    if(!acsLocal[i1].CompareNoCase(csDirectory) ||
       !acsLocal[i1].CompareNoCase(csDirectory + '\\'))
    {
      CString csShare = acsShares[i1];

      if(csShare.GetLength())
      {
        i2 = csShare.ReverseFind('\\');
        if(i2 > 0)
          csShare = csShare.Mid(i2 + 1);  // after backslash

        // an EXACT match - remove the share
        if(GetVersion() & 0x80000000)  // win '9x/ME ?
        {
          API_RET_TYPE (APIENTRY *pNetShareDel)(const char FAR * pszServer,       
                                                const char FAR * pszNetName,      
                                                unsigned short  usReserved) = NULL;

          (FARPROC &)pNetShareDel = GetProcAddress(hMod, "NetShareDel");

          if(!pNetShareDel)
          {
            bRval = FALSE;
          }
          else
          {
#ifdef _DEBUG
            TRACE0("Would be deleting " + csShare + "\r\n");
#else // _DEBUG
            if(pNetShareDel(NULL, (LPCSTR)csShare, 0)
               != NERR_Success)
            {
              bRval = FALSE;
            }
#endif // _DEBUG
          }
        }
        else
        {
          NET_API_STATUS (NET_API_FUNCTION *pNetShareDel)(LPWSTR servername,
                                                          LPWSTR netname,     
                                                          DWORD reserved) = NULL;

          (FARPROC &)pNetShareDel = GetProcAddress(hMod, "NetShareDel");

          if(!pNetShareDel)
          {
            bRval = FALSE;
          }
          else
          {
            memset(wszTemp, 0, sizeof(wszTemp));

            MultiByteToWideChar(CP_ACP, 0, csShare, csShare.GetLength() + 1,
                                wszTemp, sizeof(wszTemp)/sizeof(*wszTemp));

#ifdef _DEBUG
            TRACE0("Would be deleting " + csShare + "\r\n");
#else // _DEBUG
            if(pNetShareDel(NULL, wszTemp, 0)
               != NERR_Success)
            {
              bRval = FALSE;
            }
#endif // _DEBUG
          }
        }
      }
    }
  }

  FreeLibrary(hMod);

  if(!bRval)
  {
    TRACE0("WARNING:  DeleteSharesOnDirectory() returns FALSE\r\n");
  }

  return(bRval);
}

BOOL PathHasFiles(LPCSTR szPath)
{
  CStringArray acsDirs;
  CString csPath = AdjustPathName(szPath);

  WIN32_FIND_DATA fd;
  BOOL bFiles = FALSE;

  HANDLE hFF = FindFirstFile(csPath + "*.*", &fd);
  if(hFF != INVALID_HANDLE_VALUE)
  {
    do
    {
      if(fd.cFileName[0] != '.' ||
          (fd.cFileName[1] &&
          (fd.cFileName[1] != '.' || fd.cFileName[2])))
      {
        if(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
          acsDirs.Add(csPath + fd.cFileName);
        else
          bFiles = TRUE;  // something other than '.' or '..'
      }
    } while(!bFiles && FindNextFile(hFF, &fd));

    FindClose(hFF);
  }

  if(!bFiles)
  {
    int i1;
    for(i1=0; i1 < acsDirs.GetSize(); i1++)
    {
      if(PathHasFiles(acsDirs[i1]))
      {
        bFiles = TRUE;
        break;
      }
    }
  }

  return(bFiles);
}

BOOL NukeDirectory(LPCSTR szPath)
{
// ***********************************************************
// DO NOT CALL THIS FUNCTION UNLESS YOU REALLY REALLY MEAN IT!
// ***********************************************************

  CString csPath = AdjustPathName(szPath);

  WIN32_FIND_DATA fd;
  HANDLE hFF = FindFirstFile(csPath + "*.*", &fd);

  CStringArray acsDir, acsFile;

  if(hFF != INVALID_HANDLE_VALUE)
  {
    do
    {
      if(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
      {
        // check for '.' and '..' and exclude THEM

        if(fd.cFileName[0] == '.' &&
           (!fd.cFileName[1] ||
            (fd.cFileName[1] == '.' && !fd.cFileName[2])))
        {
          // skip me
        }
        else if(fd.cFileName[0])  // why not check it just in case?
        {
          acsDir.Add(fd.cFileName);
        }
      }
      else
      {
        acsFile.Add(fd.cFileName);
      }

    } while(FindNextFile(hFF, &fd));

    FindClose(hFF);
  }

  int i1;
  for(i1=0; i1 < acsFile.GetSize(); i1++)
  {
    if(acsFile[i1].GetLength())  // just in case
      DeleteFile(csPath + acsFile[i1]);
  }

  for(i1=0; i1 < acsDir.GetSize(); i1++)
  {
    if(acsDir[i1].GetLength())  // just in case
      NukeDirectory(csPath + acsDir[i1]);
  }

  csPath = AdjustPathName2(szPath);

  Sleep(25);  // slow it down just a *little*

  DeleteSharesOnDirectory(csPath);
  
  Sleep(25);

  return(RemoveDirectory(csPath));
}
