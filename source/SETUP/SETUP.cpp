//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//            ____         _                                                //
//           / ___|   ___ | |_  _   _  _ __     ___  _ __   _ __            //
//           \___ \  / _ \| __|| | | || '_ \   / __|| '_ \ | '_ \           //
//            ___) ||  __/| |_ | |_| || |_) |_| (__ | |_) || |_) |          //
//           |____/  \___| \__| \__,_|| .__/(_)\___|| .__/ | .__/           //
//                                    |_|           |_|    |_|              //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//       Copyright (c) 1998-2016 by S.F.T. Inc. - All rights reserved       //
//  Use, copying, and distribution of this software are licensed according  //
//    to the GPLv2, LGPLv2, or BSD license, as appropriate (see COPYING)    //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SETUP.h"
#include "SETUPDlg.h"
#include "ExtractThread.h"
#include "mainframe.h"
#include "progdlg.h"
#include "LicenseDlg.h"
#include "PathDlg.h"
#include "OptionsDlg.h"

#include "PEFile.h"

#include "LoginDlg.h"

//#ifdef _DEBUG
//#define new DEBUG_NEW
//#undef THIS_FILE
//static char THIS_FILE[] = __FILE__;
//#endif


// This program uses the Microsoft Cabinet SDK
//
// documentation:
//   http://msdn.microsoft.com/en-us/library/bb417343.aspx
//   http://msdn.microsoft.com/en-us/library/bb432569.aspx
//
// download may not be possible any more.  windows SDK may have it
//   http://download.microsoft.com/download/F/1/0/F10113F5-B750-4969-A255-274341AC6BCE/GRMSDKX_EN_DVD.iso
//


//static CWnd wndNULL;  // use THIS when windows have no parent...

const char szSETUP_KEY[] = "SOFTWARE\\Stewart~Frazier Tools\\SETUP\\";
const char szUNINST_KEY[] = "SOFTWARE\\Stewart~Frazier Tools\\UNINST\\";
const char szHKLM_WINDOWS_CURRENT_VERSION[] = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion";

#include ".\setup.h"

#if 0
// SHLWAPI HELPER functions (so I don't need the DLL)
// This is so that Win '95 applications can load it!
// Newer Visual Studio needs more functions, so this is commented out

#include "shlwapi.h"
#include ".\setup.h"
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
    return FALSE;

  LPCTSTR p1 = strchr(pszPath + 2, '\\');
  if(!p1)
    return FALSE;

  // TODO:  validate server name?  naw....

  return TRUE;
}

extern "C" BOOL STDAPICALLTYPE PathStripToRoot(LPTSTR pszPath)
{
  if(*pszPath == '\\' && pszPath[1] == '\\')
  {
    // UNC paths - trim to \\server\share\, optionally add trailing '\'

    LPTSTR p1 = strchr(pszPath + 2, '\\');

    if(!p1)
      return FALSE;

    p1 = strchr(p1 + 1, '\\');
    if(!p1)        // allow p1 to be NULL, though
      p1 = pszPath + strlen(pszPath);

    *p1 = '\\';    // making sure if no backslash before
    p1[1] = 0;  // but if not, include the '\'

    return TRUE;
  }

  if(((pszPath[0] >= 'A' && pszPath[0] <= 'Z') ||
      (pszPath[0] >= 'a' && pszPath[0] <= 'z')) &&
     pszPath[1] == ':')
  {
    if(pszPath[2] != '\\')  // if not there add backslash
      pszPath[2] = '\\';

    pszPath[3] = 0;
    return TRUE;
  }

  return FALSE;
}

#endif // 0


/////////////////////////////////////////////////////////////////////////////
// CSETUPApp

//BEGIN_MESSAGE_MAP(CSETUPApp, CMyApp/*CWinApp*/)
//  //{{AFX_MSG_MAP(CSETUPApp)
//    // NOTE - the ClassWizard will add and remove mapping macros here.
//    //    DO NOT EDIT what you see in these blocks of generated code!
//  //}}AFX_MSG
////  ON_COMMAND(ID_HELP, CSETUPApp::OnHelp/*CWinApp::OnHelp*/)
//END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSETUPApp construction

CSETUPApp::CSETUPApp()
  : CMyApp("S.F.T. Setup Application")  // assign default application name
{
  // TODO: add construction code here,
  // Place all significant initialization in InitInstance

  m_pMyFrameWnd = NULL;

  m_dwTotalFileSize = 0;
  m_dwTotalFileSizeX = 0;
  m_bRebootFlag = FALSE;  // don't reboot unless I have to

  m_bSelfExtract = FALSE;  // self-extracting file flag

  m_bUpgrading = FALSE;  // initially - will be set later if user 'upgrades'

  m_bNoUninstall = FALSE;
  m_bQuietSetup = FALSE;
  m_bExtraQuietMode = FALSE;

  m_bNoReboot = FALSE;
  m_bForceReboot = FALSE;   // additional flags to force/disallow reboot

  m_bNoLogo = FALSE;

  m_pCopyThread = NULL;
  m_pProgressDlg = NULL;


  // OS information - do this during construction
  // see http://msdn.microsoft.com/en-us/library/windows/desktop/ms724833%28v=vs.85%29.aspx
  // Windows 8.1. and Windows 10 will ONLY admit their real version if you program's manifest claims to be compatible. Otherwise they claim to be Windows 8.

  OSVERSIONINFOEX osv;
  memset(&osv, 0, sizeof(osv));

  osv.dwOSVersionInfoSize = sizeof(osv);

  if(!GetVersionEx((OSVERSIONINFO *)&osv)) // will fail '95
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
    else if(m_dwMajorVer == 6 && m_dwMinorVer == 0)
    {
      if(osv.wProductType != VER_NT_WORKSTATION)
      {
        m_csOS = "WSrv2008";
        m_csOSName = "Win Server 2008";
      }
      else
      {
        m_csOS = "Vista";
        m_csOSName = "Windows Vista";
      }
    }
    else if(m_dwMajorVer == 6 && m_dwMinorVer == 1)
    {
      if(osv.wProductType != VER_NT_WORKSTATION)
      {
        m_csOS = "WSrv2008R2";
        m_csOSName = "Win Srv 2008 R2";
      }
      else
      {
        m_csOS = "Win7";
        m_csOSName = "Windows 7";
      }
    }
    else if(m_dwMajorVer == 6 && m_dwMinorVer == 2)
    {
      if(osv.wProductType != VER_NT_WORKSTATION)
      {
        m_csOS = "WSrv2012";
        m_csOSName = "Win Srv 2012";
      }
      else
      {
        m_csOS = "Win8.0";
        m_csOSName = "Windows 8";
      }
    }
    else if(m_dwMajorVer == 6 && m_dwMinorVer == 3)
    {
      if(osv.wProductType != VER_NT_WORKSTATION)
      {
        m_csOS = "WSrv2012R2";
        m_csOSName = "Win Srv 2012 R2";
      }
      else
      {
        m_csOS = "Win8.1";
        m_csOSName = "Windows 8.1";
      }
    }
    else if(m_dwMajorVer == 6 && m_dwMinorVer == 4)
    {
      m_csOS = "Win10";
      m_csOSName = "Windows 10";
    }
    else if(m_dwMajorVer > 6 ||
            (m_dwMajorVer == 6 && m_dwMinorVer > 4))
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
// The one and only CSETUPApp object

CSETUPApp theApp;



int _cdecl MySpecialSortCompare(LPCVOID p1, LPCVOID p2)
{
  int i1 = *(int *)p1;
  int i2 = *(int *)p2;

  CStringArray acs1, acs2;

  DoParseString(theApp.m_acsFiles[i1], acs1);
  DoParseString(theApp.m_acsFiles[i2], acs2);

  if(acs1.GetSize() > 2 && acs2.GetSize() > 2)
  {
    int i3 = acs1[0].CompareNoCase(acs2[0]);

    if(!i3)
      i3 = acs1[1].CompareNoCase(acs2[1]);

    if(i3)
      return(i3);
  }

  // EQUAL!  sort by physical position in original

  if(i1 < i2)
    return(-1);
  else if(i1 > i2)
    return(1);
  else
    return(0);  // won't be equal...
}


/////////////////////////////////////////////////////////////////////////////
// CSETUPApp initialization

BOOL CSETUPApp::InitInstance()
{
  CMyWaitCursor wait;

  CStringArray acsTemp;
  int i1, i2;
  CString csTemp, csTemp2;

  if(!m_csOS.GetLength())
  {
    MyMessageBox((IDS_ERROR27), MB_OK | MB_ICONHAND | MB_SETFOREGROUND);

    MSG *pMsg = MyGetCurrentMessage();
    if(pMsg)
    {
      pMsg->message = WM_QUIT;   // this is specifically to force return(-1) on error
      pMsg->wParam = 0xffffffff;
      pMsg->lParam = 0xffffffff;
    }

    return FALSE;
  }


  // COMMAND LINE FORMAT:
  // setup [/apppath="pathname"] [/appcommon="pathname"] [/startmenu="pathname"] INF_FILE.INF

  CStringArray acsCommandLineSwitches; // the switches on the command line

  CString csCmdLine;  // the rest of the command line
  // parse command line arguments.  Place them into the array above.

  if(m_lpCmdLine && *m_lpCmdLine)
  {
    LPCSTR lpc1 = m_lpCmdLine;

    while(*lpc1)
    {
      while(*lpc1 && *lpc1 <= ' ')  // skip white space
        lpc1++;

      if(!*lpc1 || (*lpc1 != '/' && *lpc1 != '-'))
        break;  // not a switch, or end of line

      lpc1++;  // point just past the '-' or '/'

      LPCSTR lpc2 = lpc1;

      while(*lpc1 > ' ')
      {
        if(*lpc1 == '"')
        {
          lpc1++;
          while(*lpc1 &&
                (*lpc1 != '"' || lpc1[1] == '"'))
          {
            lpc1++;
          }

          if(*lpc1 == '"')
            lpc1++;
        }
        else
          lpc1++;
      }

      csTemp = ((CString)lpc2).Left(lpc1 - lpc2);
      acsCommandLineSwitches.Add(csTemp);

      // check for certain ones at this time

      if(csTemp.GetLength() >= 8 &&
         !csTemp.Left(8).CompareNoCase("APPPATH="))
      {
        csTemp = csTemp.Mid(8);
        if(csTemp.GetLength() && csTemp[0] == '"')
        {
          // quoted - strip quote marks
          csTemp = csTemp.Mid(1);

          if(csTemp.GetLength() && csTemp[csTemp.GetLength() - 1] == '"')
            csTemp = csTemp.Left(csTemp.GetLength() - 1);
        }

        m_csAppPath = AdjustPathName2(csTemp);
      }
      else if(csTemp.GetLength() >= 10 &&
              !csTemp.Left(10).CompareNoCase("APPCOMMON="))
      {
        csTemp = csTemp.Mid(10);
        if(csTemp.GetLength() && csTemp[0] == '"')
        {
          // quoted - strip quote marks
          csTemp = csTemp.Mid(1);

          if(csTemp.GetLength() && csTemp[csTemp.GetLength() - 1] == '"')
            csTemp = csTemp.Left(csTemp.GetLength() - 1);
        }

        m_csAppCommon = AdjustPathName2(csTemp);
      }
      else if(csTemp.GetLength() >= 10 &&
              !csTemp.Left(10).CompareNoCase("STARTMENU="))
      {
        csTemp = csTemp.Mid(10);
        if(csTemp.GetLength() && csTemp[0] == '"')
        {
          // quoted - strip quote marks
          csTemp = csTemp.Mid(1);

          if(csTemp.GetLength() && csTemp[csTemp.GetLength() - 1] == '"')
            csTemp = csTemp.Left(csTemp.GetLength() - 1);
        }

        m_csStartMenuFolderName = AdjustPathName2(csTemp);  // this gets cleaned up later
      }
      else if(csTemp.GetLength() == 1 &&
              !csTemp.CompareNoCase("Q"))
      {
        m_bExtraQuietMode = TRUE;
        m_bQuietSetup = TRUE;  // forces this also
      }
      else if(csTemp.GetLength() == 5 &&
              !csTemp.CompareNoCase("QUIET"))
      {
        m_bExtraQuietMode = TRUE;
        m_bQuietSetup = TRUE;  // forces this also
      }
      else if(csTemp.GetLength() >= 3 &&  // /NOL[OGO]
              !_strnicmp(csTemp, "NOLOGO", csTemp.GetLength()))
      {
        m_bNoLogo = TRUE;
      }
      else if(csTemp.GetLength() >= 3 &&  // /NOR[EBOOT]
              !_strnicmp(csTemp, "NOREBOOT", csTemp.GetLength()))
      {
        m_bNoReboot = TRUE;
        m_bForceReboot = FALSE;
      }
      else if(csTemp.GetLength() >= 6 &&  // /FORCER[EBOOT]
              !_strnicmp(csTemp, "FORCEREBOOT", csTemp.GetLength()))
      {
        m_bNoReboot = FALSE;
        m_bForceReboot = TRUE;
      }
      else if(csTemp.GetLength() > 4 &&  // /OPT:option_name
              !_strnicmp("OPT:", csTemp, 4))
      {
        // add the remainder of the string to 'm_acsOptions' and
        // pre-select my options...

        csTemp = csTemp.Mid(4);  // past the 'OPT:'

        if(csTemp[0] == '"')  // if quoted...
        {
          if(csTemp.GetLength() > 1 &&
             csTemp[csTemp.GetLength() - 1] == '"')
          {
            csTemp = csTemp.Mid(1, csTemp.GetLength() - 2);
          }
          else
          {
            csTemp = csTemp.Mid(1);  // ignore trailing '"'
          }
        }

        int iOptIndex;
        for(iOptIndex=0; iOptIndex < m_acsOptions.GetSize(); iOptIndex++)
        {
          if(!m_acsOptions[iOptIndex].CompareNoCase(csTemp))
            break;
        }

        if(iOptIndex >= m_acsOptions.GetSize())
          m_acsOptions.Add(csTemp);
      }
    }

    if(*lpc1)
      csCmdLine = lpc1;  // "the rest" of the command line
  }

// ** DO NOT CALL AfxOleInit() !!! **
//
  if(CoInitialize(NULL) != S_OK) // CoInitializeEx(NULL, COINIT_APARTMENTTHREADED) != S_OK)
  {
    MyMessageBox((IDS_ERROR01), MB_OK | MB_ICONHAND | MB_SETFOREGROUND);

    MSG *pMsg = MyGetCurrentMessage();
    if(pMsg)
    {
      pMsg->message = WM_QUIT;   // this is specifically to force return(-1) on error
      pMsg->wParam = 0xffffffff;
      pMsg->lParam = 0xffffffff;
    }

    return FALSE;
  }

  // initialize everything else (like DDE maybe?)

//  if(!CWinApp::InitInstance())
//  {
//    MSG *pMsg = MyGetCurrentMessage();
//    if(pMsg)
//    {
//      pMsg->message = WM_QUIT;   // this is specifically to force return(-1) on error
//      pMsg->wParam = 0xffffffff;
//      pMsg->lParam = 0xffffffff;
//    }
//
//    return FALSE;
//  }

//  Enable3dControlsStatic();  // don't be a control container, though...


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

        if(dlgLogin.DoModal() == IDOK)
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
          return FALSE;
        }
        else
        {
          if(MyMessageBox("Continue Anyway?", MB_YESNO | MB_ICONQUESTION | MB_SETFOREGROUND)
             == IDYES)
          {
            goto jump_point;
          }
        }
      }

      MSG *pMsg = MyGetCurrentMessage();
      if(pMsg)
      {
        pMsg->message = WM_QUIT;   // this is specifically to force return(-1) on error
        pMsg->wParam = 0xffffffff;
        pMsg->lParam = 0xffffffff;
      }

      return FALSE;
    }
jump_point:
    RegCloseKey(hkeyTest);  // now that I'm done with it...
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


  // get path for EXE (not including file name).  So that I
  // can debug this thing, allow me to specify a path name
  // on the command line.  AND, if I'm self-extracting, point
  // it to a temporary path

  m_csEXEPath = "";

  // Before I see if I'm self-extracting, open myself and do
  // a "map view of file" on myself so that I can perform this
  // little operation....

  CString csSelf;

#ifdef _DEBUG
  csSelf = "%tmpdir%\\SFTSETUP\\";
  ExpandStrings(csSelf);
  MyMessageBox("TEMP DIRECTORY:  " + csSelf + "\r\n");
  csSelf = "";
#endif // _DEBUG

  GetModuleFileName(NULL, csSelf.GetBufferSetLength(MAX_PATH), MAX_PATH);
  csSelf.ReleaseBuffer(-1);

  HANDLE hMapping = NULL;
  LPCVOID lpSelf = NULL;
  DWORD cbSelf = 0;
  HANDLE hSelf = CreateFile(csSelf,
                            GENERIC_READ,
                            FILE_SHARE_READ | FILE_SHARE_WRITE,
                            NULL, OPEN_EXISTING,
                            FILE_ATTRIBUTE_NORMAL, NULL);

  if(hSelf != INVALID_HANDLE_VALUE)
  {
    cbSelf = GetFileSize(hSelf, NULL);

    hMapping = CreateFileMapping(hSelf, NULL, PAGE_READONLY, 0, 0, NULL);

    if(hMapping)
    {
      lpSelf = MapViewOfFile(hMapping, FILE_MAP_READ, 0, 0, 0);
    }
  }

  LPCVOID lpEnd = IsSelfExtracting(lpSelf,cbSelf);
    // IsSelfExtracting() will fail whenever others (above) fail...

  if(lpEnd)
  {
    m_bSelfExtract = TRUE;  // so I can clean up later

    if(!DoSelfExtract((const BYTE *)lpEnd, (const BYTE *)lpSelf, cbSelf))
    {
      UnmapViewOfFile(lpSelf);
      CloseHandle(hMapping);
      CloseHandle(hSelf);

      MSG *pMsg = MyGetCurrentMessage();
      if(pMsg)
      {
        pMsg->message = WM_QUIT;   // this is specifically to force return(-1) on error
        pMsg->wParam = 0xffffffff;
        pMsg->lParam = 0xffffffff;
      }

      return FALSE;
    }
  }
  else
  {
    if(csCmdLine.GetLength())
    {
      LPSTR lp1;
      GetFullPathName(csCmdLine, MAX_PATH, 
                      m_csEXEPath.GetBufferSetLength(MAX_PATH), &lp1);

      m_csEXEPath.ReleaseBuffer(-1);

      m_csEXEPath.TrimRight();
      m_csEXEPath.TrimLeft();
    }

    if(!m_csEXEPath.GetLength())
    {
      GetModuleFileName(NULL, m_csEXEPath.GetBufferSetLength(MAX_PATH), MAX_PATH);
      m_csEXEPath.ReleaseBuffer(-1);

      i1 = m_csEXEPath.ReverseFind('\\');
      m_csEXEPath = m_csEXEPath.Left(i1 + 1);  // include the backslash
    }
  }

  m_csEXEPath = AdjustPathName(m_csEXEPath);

  if(lpSelf)
    UnmapViewOfFile(lpSelf);

  if(hMapping)
    CloseHandle(hMapping);

  if(hSelf != INVALID_HANDLE_VALUE)
    CloseHandle(hSelf);


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
        MyMessageBox((IDS_ERROR28), MB_OK | MB_ICONHAND | MB_SETFOREGROUND);

        MSG *pMsg = MyGetCurrentMessage();
        if(pMsg)
        {
          pMsg->message = WM_QUIT;   // this is specifically to force return(-1) on error
          pMsg->wParam = 0xffffffff;
          pMsg->lParam = 0xffffffff;
        }

        return FALSE;
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

  // find out size of file, then read the whole thing into 'm_csINFFile'

  DWORD cb1, cbFile = GetFileSize(hFile, NULL);

  LPSTR lp1 = m_csINFFile.GetBufferSetLength(cbFile + 1);

  if(!lp1 ||
     !ReadFile(hFile, lp1, cbFile, &cb1, NULL) ||
     cb1 != cbFile)
  {
    m_csINFFile.ReleaseBuffer(0);
    CloseHandle(hFile);

    MyMessageBox((IDS_ERROR29), MB_OK | MB_ICONHAND | MB_SETFOREGROUND);

    MSG *pMsg = MyGetCurrentMessage();
    if(pMsg)
    {
      pMsg->message = WM_QUIT;   // this is specifically to force return(-1) on error
      pMsg->wParam = 0xffffffff;
      pMsg->lParam = 0xffffffff;
    }

    return FALSE;
  }
              
  m_csINFFile.ReleaseBuffer(cbFile);
  CloseHandle(hFile);

  // get settings for app name, company name, default app directory, etc.

  m_csAppName = GetInfEntry("SetupOptions", "AppName");
  m_csCompanyName = GetInfEntry("SetupOptions", "CompanyName");
  m_csStartMessage = GetInfEntry("SetupOptions", "StartMessage");

  if(m_csAppName.GetLength())
  {
    m_csApplicationName = m_csAppName + " SETUP";
  }

  GetAndValidateAuthCode(); // converts encrypted auth code and stores in m_csAuthCode.
                            // If not valid, m_csAuthCode will contain an empty string

  if(!m_csAppPath.GetLength())
    m_csAppPath = GetInfEntry("SetupOptions", "AppPath");

  if(!m_csAppCommon.GetLength())
    m_csAppCommon = GetInfEntry("SetupOptions", "AppCommon");

  if(!m_csStartMenuFolderName.GetLength())
    m_csStartMenuFolderName = GetInfEntry("SetupOptions", "StartMenuFolderName");

  if(!m_csStartMenuFolderName.GetLength())
  {
    GetInfEntries("AddShortcut", acsTemp);  // when blank, check for any shortcuts first
    // NOTE:  this does NOT apply if only 'uninstall' will be added to the start menu!

    if(acsTemp.GetSize())
    {
      m_csStartMenuFolderName = AppNameToPathName(m_csAppName);  // the 'default' (for backward compatibility)

      // because I'm grabbing the name from the application name, I must strip
      // out *ALL* backslashes, asterisks, and other questionable characters

      for(i1=0; i1 < m_csStartMenuFolderName.GetLength(); i1++)
      {
        if(m_csStartMenuFolderName[i1] == '\\' ||
           m_csStartMenuFolderName[i1] == '*' ||
           m_csStartMenuFolderName[i1] == '?' ||
           m_csStartMenuFolderName[i1] == ';' ||
           m_csStartMenuFolderName[i1] == ',' ||
           m_csStartMenuFolderName[i1] == '&' ||
           m_csStartMenuFolderName[i1] == ':' ||
           m_csStartMenuFolderName[i1] == '"' ||
           m_csStartMenuFolderName[i1] == '+')
        {
          if(!i1)
          {
            m_csStartMenuFolderName = m_csStartMenuFolderName.Mid(1);  // strip off left-hand character
          }
          else if((i1 + 1) >= m_csStartMenuFolderName.GetLength())
          {
            m_csStartMenuFolderName = m_csStartMenuFolderName.Left(i1);  // trim off rest of string
          }
          else
          {
            m_csStartMenuFolderName = m_csStartMenuFolderName.Left(i1)
                                + m_csStartMenuFolderName.Mid(i1 + 1);  // character 'i1' stripped away
          }

          i1--;  // must re-check this position again, since I shortened the overall string length
        }
      }
    }
  }


  // these next two must be exactly equal to 1 to activate

  m_bNoUninstall = atoi(GetInfEntry("SetupOptions", "NoUninstall")) == 1;

  if(!m_bQuietSetup)  // if not already assigned
    m_bQuietSetup = atoi(GetInfEntry("SetupOptions", "QuietSetup")) == 1;

  if(!m_csAuthCode.GetLength())  // not an authorized version - disable quiet mode!
  {
    m_bQuietSetup = FALSE;
    m_bExtraQuietMode = FALSE;
    m_bNoLogo = FALSE;         // make sure these are DIS-abled!
  }

  if(!m_bForceReboot && !m_bNoReboot)  // never override the command line!!!
  {
    i1 = atoi(GetInfEntry("SetupOptions", "RebootFlag"));

    if(i1 == 0)  // this is the normal situation
    {
      m_bNoReboot = FALSE;
      m_bForceReboot = FALSE;
    }
    else if(i1 < 0)
    {
      m_bNoReboot = TRUE;
      m_bForceReboot = FALSE;
    }
    else // if(i1 > 0)
    {
      m_bNoReboot = FALSE;
      m_bForceReboot = TRUE;
    }
  }

  SetRegistryKey(m_csCompanyName);  // default registry key (why not!)



  // Get critical information from the SETUP.INF file, and cache it
  // in the application class.  Member functions will allow me to
  // access this information from elsewhere (will be thread safe)

  // <disk number>,<cabinet number>,<filename>,<destname>,<date>,<size>
  GetInfEntries("Files", m_acsFiles);

  if(!m_acsFiles.GetSize())
  {
    MyMessageBox((IDS_ERROR30), MB_OK | MB_ICONHAND | MB_SETFOREGROUND);

    MSG *pMsg = MyGetCurrentMessage();
    if(pMsg)
    {
      pMsg->message = WM_QUIT;   // this is specifically to force return(-1) on error
      pMsg->wParam = 0xffffffff;
      pMsg->lParam = 0xffffffff;
    }

    return FALSE;
  }


  GetInfEntries("Files." + m_csOS, acsTemp);

  for(i1=0; i1 < acsTemp.GetSize(); i1++)
  {
    m_acsFiles.Add(acsTemp[i1]);  // add these to the list of files
  }

  acsTemp.RemoveAll();



  // <cabinet number>,<disk number>,<cabinet file name>
  GetInfEntries("cabinet list", m_acsCabs);

  if(!m_acsCabs.GetSize())
  {
    MyMessageBox((IDS_ERROR31),
                  MB_OK | MB_ICONHAND | MB_SETFOREGROUND);

    MSG *pMsg = MyGetCurrentMessage();
    if(pMsg)
    {
      pMsg->message = WM_QUIT;   // this is specifically to force return(-1) on error
      pMsg->wParam = 0xffffffff;
      pMsg->lParam = 0xffffffff;
    }

    return FALSE;
  }

  // <disk number>,<disk label>
  GetInfEntries("disk list", m_acsDisks);

  if(!m_acsDisks.GetSize())
  {
    MyMessageBox((IDS_ERROR32),
                  MB_OK | MB_ICONHAND | MB_SETFOREGROUND);

    MSG *pMsg = MyGetCurrentMessage();
    if(pMsg)
    {
      pMsg->message = WM_QUIT;   // this is specifically to force return(-1) on error
      pMsg->wParam = 0xffffffff;
      pMsg->lParam = 0xffffffff;
    }

    return FALSE;
  }


  // other 'non-critical' entries that may contain nothing

  GetInfEntries("SharedComponents", m_acsShared);
  GetInfEntries("OverwriteAlways", m_acsOverwrite);



  // ** WINDOW CREATION AND DIALOG BOXES **

  // create a new frame window without a document
  // (frame window handled by 'sftsetup' code)

//  static CWnd wndNULL;
//  m_pMainWnd = &wndNULL;
//  m_pMainWnd = NULL;

  m_pMyFrameWnd = new CMainFrame();

  // if there is NO LOGO, then the window won't be created (NULL handle)

  if(!m_bNoLogo &&
     (!m_pMyFrameWnd ||
      !m_pMyFrameWnd->Create(NULL, NULL, 0, 
                             CMainFrame::rectDefault, NULL, 0)))
  {
    MyMessageBox((IDS_ERROR33), MB_OK | MB_ICONHAND | MB_SETFOREGROUND);

    MSG *pMsg = MyGetCurrentMessage();
    if(pMsg)
    {
      pMsg->message = WM_QUIT;   // this is specifically to force return(-1) on error
      pMsg->wParam = 0xffffffff;
      pMsg->lParam = 0xffffffff;
    }

    return FALSE;
  }

  m_hWnd = m_pMyFrameWnd->m_hWnd;  // assign now so that loop will spin

  // create the 'opening' dialog box and allow user to bail out

  CSETUPDlg dlg;

  dlg.m_csTitle = m_csAppName + " SETUP";

  // translate escape sequences for 'm_csStartMessage'
  dlg.m_csWelcome = TranslateEscapeCodes(m_csStartMessage);

  if(!m_bQuietSetup && dlg.DoModal() != IDOK)
  {
    PostQuitMessage(1);  // user cancels - return 1

    return TRUE;
  }

  // if there's a 'LICENSE.TXT' file in the same directory as the
  // SETUP program, display the license dialog.

  CLicenseDlg dlgLicense;

  hFile = CreateFile(m_csEXEPath + "LICENSE.RTF",
                     GENERIC_READ, 0, NULL, OPEN_EXISTING,
                     FILE_ATTRIBUTE_NORMAL, NULL);

  if(hFile == INVALID_HANDLE_VALUE)
    hFile = CreateFile(m_csEXEPath + "LICENSE.TXT",
                       GENERIC_READ, 0, NULL, OPEN_EXISTING,
                       FILE_ATTRIBUTE_NORMAL, NULL);

  if(hFile != INVALID_HANDLE_VALUE)
  {
    // find out size of file, then read the whole thing into 'm_csINFFile'
    // NOTE:  empty file is the same as non-existent file

    cbFile = GetFileSize(hFile, NULL);

    if(cbFile &&
       !ReadFile(hFile, dlgLicense.m_csLicense.GetBufferSetLength(cbFile),
                 cbFile, &cb1, NULL)
       || cb1 != cbFile)
    {
      dlgLicense.m_csLicense.ReleaseBuffer(0);
      CloseHandle(hFile);

      MyMessageBox((IDS_ERROR34), MB_OK | MB_ICONHAND | MB_SETFOREGROUND);

      PostQuitMessage(2);

      return TRUE;
    }
              
    dlgLicense.m_csLicense.ReleaseBuffer(cbFile);
    CloseHandle(hFile);

    if(cbFile && dlgLicense.DoModal() != IDOK)
    {
      PostQuitMessage(2);

      return TRUE;
    }
  }

  // get options.  'Nuff said

  // if this is an *UPGRADE* then I must pre-select the *SAME*
  // options as *LAST* time.

  if(!UpgradeCheck())
  {
    PostQuitMessage(3);
    return FALSE;
  }

  CStringArray acsOptions;
  GetInfEntries("Options", acsOptions);

  // eliminate options not applicable to *THIS* operating system
  for(i1=acsOptions.GetSize() - 1; i1 >= 0; i1--)
  {
    DoParseString(acsOptions[i1], acsTemp);

    if(acsTemp.GetSize() < 3)
    {
      acsOptions.RemoveAt(i1);
      continue;  // not valid
    }

    // before I continue, make sure there ARE files to install
    // for my operating system...

    CString csOption = acsTemp[0]; // must make copy - I'm re-using 'acsTemp'

    GetInfEntries("Option." + csOption, acsTemp);
    if(!acsTemp.GetSize())  // none so far...
    {
      GetInfEntries("Option." + csOption + "." + m_csOS, acsTemp);
      if(!acsTemp.GetSize())
      {
        acsOptions.RemoveAt(i1);
        continue;  // skip this entry
      }
    }
  }

  // if there's only one, display a "YESNOCANCEL" box.  If there are
  // more than one, display a dialog box with some kind of listbox

  if(!m_acsOptions.GetSize() || !m_bExtraQuietMode)
  {
    if(acsOptions.GetSize() == 1)
    {
      DoParseString(acsOptions[0], acsTemp);

      if(acsTemp.GetSize() < 3)
      {
        acsOptions.RemoveAll();  // not valid
      }
      else
      {
        CString csMsg;
        csMsg.Format(theApp.LoadString(IDS_ERROR35),
                     (LPCSTR)acsTemp[1],    // csShortDesc
                     (LPCSTR)acsTemp[2]);   // csLongDesc

        i2 = MyMessageBox(csMsg, MB_YESNOCANCEL | MB_ICONQUESTION | MB_SETFOREGROUND);

        if(i2 == IDCANCEL)
        {
          PostQuitMessage(1);

          return TRUE;  // byby
        }
        else if(i2 != IDYES)
        {
          acsOptions.RemoveAll();  // don't install it
        }
      }
    }
    else if(acsOptions.GetSize() > 1) // only display the dlg if it's not empty
    {
      COptionsDlg dlg(&acsOptions, &m_acsOptions);

      if(dlg.DoModal() != IDOK)
      {
        PostQuitMessage(1);

        return TRUE;
      }
    }

    m_acsOptions.RemoveAll();  // must do this before I load 'acsOptions' into it

    // at this point, if there are any options to add, 'acsOptions' will
    // not be empty.  And, I want to add them *ALL*!

    for(i1=0; i1 < acsOptions.GetSize(); i1++)
    {
      DoParseString(acsOptions[i1], acsTemp);
//
//      if(acsTemp.GetSize() < 3)
//        continue;  // not valid
//
//      // before I continue, make sure there ARE files to install
//      // for my operating system...
//
//      GetInfEntries("Option." + acsTemp[0] /*csOption*/, acsTemp);
//      if(!acsTemp.GetSize())  // none so far...
//      {
//        GetInfEntries("Option." + acsTemp[0] /*csOption*/ + "." + m_csOS, acsTemp);
//        if(!acsTemp.GetSize())
//          continue;  // skip this entry
//      }
//
      m_acsOptions.Add(acsTemp[0] /*csOption*/);
    }

    // *NOW* verify that everything I installed *LAST TIME* will be 'upgraded' whether
    // I selected it or not.  This will avoid problems with SETUP 'not upgrading' when
    // it SHOULD upgrade.

    BOOL bReAddFlag = FALSE;

    for(i1=0; i1 < m_acsUpgradeInstalledOptions.GetSize(); i1++)
    {
      for(i2=0; i2 < m_acsOptions.GetSize(); i2++)
      {
        if(!m_acsUpgradeInstalledOptions[i1].CompareNoCase(m_acsOptions[i2]))
          break;
      }

      if(i2 >= m_acsOptions.GetSize())
      {
        m_acsOptions.Add(m_acsUpgradeInstalledOptions[i1]);
        bReAddFlag = TRUE;
      }
    }

    if(bReAddFlag)
    {
      MyMessageBox((IDS_WARNING18), MB_OK | MB_ICONASTERISK | MB_SETFOREGROUND);
    }
  }
  else
  {
    // as a matter of 'cleanup' for non-applicable options, remove
    // *ANYTHING* from 'm_acsOptions' that's not referenced by 'acsOptions'.

    for(i2=m_acsOptions.GetSize() - 1; i2 >= 0; i2--)
    {
      for(i1=0; i1 < acsOptions.GetSize(); i1++)
      {
        DoParseString(acsOptions[i1], acsTemp);

        if(!m_acsOptions[i2].CompareNoCase(acsTemp[0]))
          break;
      }

      if(i1 >= acsOptions.GetSize())  // wasn't in the 'acsOptions' list...
      {
        m_acsOptions.RemoveAt(i2);
      }
    }
  }


  wait.Restore();

  // Take the selected options and add their files to the
  // 'm_acsFiles' array so that everything will simply work ok.
  // TODO:  do I want to use a separate array for them???

  for(i1=0; i1 < m_acsOptions.GetSize(); i1++)
  {
    GetInfEntries("Option." + m_acsOptions[i1], acsTemp);

    for(i2=0; i2 < acsTemp.GetSize(); i2++)
      m_acsFiles.Add(acsTemp[i2]);  // add raw entries here

    GetInfEntries("Option." + m_acsOptions[i1] + "." + m_csOS, acsTemp);

    for(i2=0; i2 < acsTemp.GetSize(); i2++)
      m_acsFiles.Add(acsTemp[i2]);  // add raw entries for OS here
  }

  // ensure that m_acsFiles is sorted by disk # and cab # and original position
  // (the rest are insignificant)

  if(m_acsFiles.GetSize() > 1)
  {
    acsTemp.RemoveAll();

    CArray<int,int> aiFiles;

    for(i1=0; i1 < m_acsFiles.GetSize(); i1++)
    {
      aiFiles.SetAtGrow(i1, i1);
      acsTemp.SetAtGrow(i1, m_acsFiles[i1]);
    }

    qsort(aiFiles.GetData(), aiFiles.GetSize(), sizeof(int),
          MySpecialSortCompare);

    for(i1=0; i1 < m_acsFiles.GetSize(); i1++)
    {
      m_acsFiles.SetAt(i1, acsTemp[aiFiles[i1]]);

      TRACE("FILE #%d:  %s\n", i1 + 1, (LPCSTR)m_acsFiles[i1]);
    }
  }


  // calculate total file size to extract and corresponding 
  // percentages for the status box.

  // add up total sizes of files for status dialog

  m_dwTotalFileSize = 0, m_dwTotalFileSizeX = 0;

  for(i1=0; i1 < m_acsFiles.GetSize(); i1++)
  {
    DoParseString(m_acsFiles[i1], acsTemp);

    if(acsTemp.GetSize() < 6)  // not enough columns
    {
      m_aFileSize.SetAtGrow(i1, 0);
    }
    else
    {
      i2 = atol(acsTemp[5]);
      m_aFileSize.SetAtGrow(i1, i2);

      m_dwTotalFileSize += i2;

      m_dwTotalFileSizeX += ((DWORD)i2 + 0xffffL) & 0xffff0000L;
        // file size rounded up to 64k bytes, the 'worst case' cluster size
    }
  }


  // prompt for and optionally create the 'apppath' directory

  if(!PromptForAndCreateAppPath())
  {
    PostQuitMessage(3);

    return TRUE;
  }

  wait.Restore();

  // ** IF I AM UPGRADING **
  // move the old path to the new path
  // and if this fails, exit SETUP at this time.

  if(m_bUpgrading &&
     !MoveApplication(AppNameToPathName(m_csUpgradeApp),
                      AppNameToPathName(m_csAppName)))
  {
    PostQuitMessage(4);

    return TRUE;
  }



  // create PROGRESS DIALOG

  m_pProgressDlg = new CProgressDialog;

//  if(m_pMainWnd->GetSafeHwnd() && m_pMainWnd->IsWindowVisible())
//    m_pProgressDlg->Create(CProgressDialog::IDD, m_pMainWnd->GetSafeHwnd());
//  else
//    m_pProgressDlg->Create(CProgressDialog::IDD, NULL);

  if(m_pMyFrameWnd && m_pMyFrameWnd->m_hWnd && ::IsWindowVisible(m_pMyFrameWnd->m_hWnd))
    m_pProgressDlg->Create(CProgressDialog::IDD, m_pMyFrameWnd->m_hWnd);
  else
    m_pProgressDlg->Create(CProgressDialog::IDD, NULL);


  // place bottom of progress dlg in middle of screen, and
  // horizontally center it on the screen.

  CRect rctWindow, rctDlg;

//  m_pMainWnd->GetWindowRect(&rctWindow);

  rctWindow.left = 0;
  rctWindow.top = 0;
  rctWindow.right = GetSystemMetrics(SM_CXSCREEN);
  rctWindow.bottom = GetSystemMetrics(SM_CYSCREEN);

  ::GetWindowRect(m_pProgressDlg->m_hWnd, &rctDlg);

  ::SetWindowPos(m_pProgressDlg->m_hWnd, HWND_TOP,
                 (rctWindow.left + rctWindow.right
                  + rctDlg.left - rctDlg.right) / 2,
                 (rctWindow.top + rctWindow.bottom) / 2
                  - (rctDlg.bottom - rctDlg.top),
                 0, 0, SWP_NOSIZE);

  m_pProgressDlg->ShowWindow(SW_SHOWNORMAL);

  // TODO:  start the setup process as an independent thread.

  m_pCopyThread = CExtractThread::BeginThread();

  if(!m_pCopyThread)
  {
    MyMessageBox((IDS_ERROR36),
                  MB_OK | MB_ICONHAND | MB_SETFOREGROUND);

    PostQuitMessage(4);

    return TRUE;
  }

  return( TRUE );
}

BOOL CSETUPApp::IsRebootPending()
{
  if(m_bRebootFlag)
    return TRUE;  // always do THIS (to be consistent)

  BOOL bRebootFlag = FALSE;
  CString csTemp;

  if(m_bIsNT || m_dwMajorVer > 4 ||
     (m_dwMajorVer == 4 && m_dwMinorVer >= 10))
  {
    // WIN NT, Win '98

    // look in HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\Session Manager\PendingFileRenameOperations
    // NOTE:  this will be a REG_MULTI_SZ key.  For more, look under 'MoveFileEx' documentation

    // If I find anything here, then I must reboot!

    static const char szKeyName[]="SYSTEM\\CurrentControlSet\\Control\\Session Manager";
    BOOL bKeyFound = FALSE;

    HKEY hKey;
    if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, szKeyName,
                    0, KEY_QUERY_VALUE,
                    &hKey)
       == ERROR_SUCCESS)
    {
      DWORD dwType, cbBuf=65535;

      if(RegQueryValueEx(hKey, "PendingFileRenameOperations", NULL, &dwType,
                         (LPBYTE)csTemp.GetBufferSetLength(cbBuf),
                         &cbBuf)
         == ERROR_SUCCESS)
      {
        csTemp.ReleaseBuffer(cbBuf);
      }
      else
      {
        csTemp.ReleaseBuffer(0);
      }

      RegCloseKey(hKey);

      if(dwType == REG_MULTI_SZ && cbBuf > 2)
      {
        if(csTemp[0] > 0)
        {
          bKeyFound = TRUE;  // one item found - that's enough!
        }
      }
    }

    if(bKeyFound)
    {
      bRebootFlag = TRUE;
    }
    else if(!m_bIsNT)  // just in case, check 'WININIT.INI' for '98, too
    {
      csTemp = "";

      int iRet = GetPrivateProfileString("Rename", NULL, "",
                                         csTemp.GetBufferSetLength(32767),
                                         32767,
                                         m_csWinPath + "WININIT.INI");
      if(iRet > 0)
      {
        csTemp.ReleaseBuffer(iRet);

        // verify it....

        if(csTemp.GetLength())
        {
          bRebootFlag = TRUE;
        }
      }
      else
      {
        csTemp.ReleaseBuffer(0);
      }
    }
  }
  else  // Win '95
  {
    // check 'WININIT.INI' to see if there are any entries

    csTemp = "";
    int iRet = GetPrivateProfileString("Rename", NULL, "",
                                       csTemp.GetBufferSetLength(32767),
                                       32767,
                                       m_csWinPath + "WININIT.INI");
    if(iRet > 0)
    {
      csTemp.ReleaseBuffer(iRet);

      // verify it....

      if(csTemp.GetLength())
      {
        bRebootFlag = TRUE;
      }
    }
    else
    {
      csTemp.ReleaseBuffer(0);
    }
  }

  return(bRebootFlag);
}

int CSETUPApp::Run()  // from CWinApp::Run()
{
//  if (m_pMainWnd == NULL && AfxOleGetUserCtrl())
//  {
//    // Not launched /Embedding or /Automation, but has no main window!
//    TRACE(traceAppMsg, 0, "Warning: m_pMainWnd is NULL in CWinApp::Run - quitting application.\n");
//    AfxPostQuitMessage(0);
//  }
  return(CMyApp/*CWinThread*/::Run());
}

int CSETUPApp::ExitInstance() 
{
  CString csTemp;

  if(m_pProgressDlg)
  {
    m_pProgressDlg->DestroyWindow();  // pointer deletes itself
    m_pProgressDlg = NULL;
  }
  
  if(m_pCopyThread)
  {
    // TODO:  put something here to end the thread safely

    if(WaitForSingleObject(m_pCopyThread->m_hThread, INFINITE)
       != WAIT_OBJECT_0)
    {
      TerminateThread(m_pCopyThread->m_hThread, 0);  // just in case
    }

    delete m_pCopyThread;
    m_pCopyThread = 0;
  }

  if(!m_bRebootFlag && !m_bNoReboot && !m_bForceReboot)  // more basic checking here...
    m_bRebootFlag = IsRebootPending();


  if(m_bSelfExtract)
  {
    DeleteFile(m_csEXEPath + "SETUP.INF");
    DeleteFile(m_csEXEPath + "LICENSE.TXT");
    DeleteFile(m_csEXEPath + "SFTCAB.1");

    // remove the directory, but ignore errors while doing it

    RemoveDirectory(m_csEXEPath.Left(m_csEXEPath.GetLength() - 1));
  }

  // if 'm_csTempBatFile' is NOT blank, I must put an entry into 'RunOnce\SETUP' to
  // execute it on the next reboot.  Regardless.

  if(m_csTempBatFile.GetLength())
  {
    // First, I need a temp file name

    CString csTempFile, csTemp2, csComSpec;
    csTemp = "%tmpdir%";

    if(!ExpandStrings(csTemp) ||
       !GetTempFileName(csTemp, "~b", 0, csTempFile.GetBufferSetLength(MAX_PATH)))
    {
      MyMessageBox((IDS_ERROR79));
      csTempFile.ReleaseBuffer(0);
    }
    else
    {
      csTempFile.ReleaseBuffer(-1);
      csTemp = csTempFile;

      int i1 = csTempFile.ReverseFind('.');

      if(i1 < 0)
        csTempFile += ".bat";
      else
        csTempFile = csTempFile.Left(i1) + ".bat";


      // add the command 'EXIT' to the end of the batch file, just to make sure,
      // and the command '@ECHO OFF' to the beginning of it.
      // (also add something to delete the temp file I was messing with)

      m_csTempBatFile = "@ECHO OFF\r\n"
                      + m_csTempBatFile
                      + "DEL \"" + csTemp + "\"\r\n"
                        "EXIT\r\n";

      DWORD cb1;
      HANDLE hTempFile = CreateFile(csTempFile, GENERIC_READ | GENERIC_WRITE, 0, NULL,
                                    CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);

      if(hTempFile == INVALID_HANDLE_VALUE ||
         !WriteFile(hTempFile, m_csTempBatFile, m_csTempBatFile.GetLength(), &cb1, NULL) ||
         cb1 != (DWORD)m_csTempBatFile.GetLength())
      {
        if(hTempFile != INVALID_HANDLE_VALUE)
          CloseHandle(hTempFile);

        MyMessageBox((IDS_ERROR79));
      }
      else
      {
        CloseHandle(hTempFile);  // ready to rock & roll!


        // NEXT, derive an entry based upon the temp file name.  Since the SETUP registry
        // processes in ALPHABETICAL ORDER (or at least seems to) I'll have to make sure
        // that it runs this thing properly, IN ALPHABETICAL ORDER.

        csTemp2 = csTempFile;
        i1 = csTemp2.ReverseFind('\\');

        if(i1 >= 0)
          csTemp2 = csTemp2.Mid(i1 + 1);  // get rid of the path

        if(!csTemp2.GetLength())
          csTemp2.Format("%08x", GetTickCount() ^ GetCurrentProcessId());  // a fudge

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

        csTemp = '"' + csComSpec + "\" /C \"" + csTempFile + '"';
        csTempFile = '"' + csComSpec + "\" /C DEL \"" + csTempFile + '"';


        HKEY hKey = NULL;

        if(RegCreateKeyEx(HKEY_LOCAL_MACHINE,
                          (LPCSTR)((CString)szHKLM_WINDOWS_CURRENT_VERSION + "\\RunOnce\\SETUP"),
                          0, NULL, 0, KEY_POWERUSER_ACCESS,NULL,
                          &hKey, NULL)
           != ERROR_SUCCESS ||
           RegSetValueEx(hKey, "DO_" + csTemp2, 0, REG_SZ,
                         (const BYTE *)(LPCSTR)csTemp, csTemp.GetLength() + 1)
           != ERROR_SUCCESS ||
           RegSetValueEx(hKey, "ZAP_" + csTemp2, 0, REG_SZ,
                         (const BYTE *)(LPCSTR)csTempFile, csTempFile.GetLength() + 1)
           != ERROR_SUCCESS)
        {
          MyMessageBox((IDS_ERROR79));
        }

        if(hKey)
          RegCloseKey(hKey);
      }
    }
  }


  if((m_bRebootFlag && !m_bExtraQuietMode && !m_bNoReboot)
     || m_bForceReboot)  // regardless, if the 'force' flag is on, always reboot
  {
    CString csMsg;
    csMsg.Format(theApp.LoadString(IDS_ERROR70), m_csAppName);

    if(::MessageBox(NULL, csMsg, MyGetAppName()/*m_pszAppName*/,
                    MB_YESNO | MB_ICONASTERISK | MB_SETFOREGROUND | MB_TOPMOST | MB_TASKMODAL)
       == IDYES)
    {
      if(m_bIsNT)
      {
        TRACE("Running under Windows 'NT, fool!\r\n");

        HANDLE hToken = INVALID_HANDLE_VALUE;
        
        if(OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES,
                            &hToken))
        {
          TOKEN_PRIVILEGES *pTP;
          DWORD dw1[sizeof(LUID_AND_ATTRIBUTES) * 4 + sizeof(TOKEN_PRIVILEGES)];
            // that ought to be sufficient buffer length for a few of them

          pTP = (TOKEN_PRIVILEGES *)dw1;

          pTP->PrivilegeCount = 1;
          LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, 
                               &(pTP->Privileges[0].Luid));
                              
          pTP->Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

          if(!AdjustTokenPrivileges(hToken, FALSE, pTP, 0, NULL, 0))
          {
            TRACE("Unable to adjust token privileges!!!!\r\n");
          }

          CloseHandle(hToken);
        }
      }

      ExitWindowsEx(EWX_REBOOT, 0);
    }
  }

  if(m_pMyFrameWnd)
  {
    m_pMyFrameWnd->DestroyWindow();
    delete m_pMyFrameWnd;
    m_pMyFrameWnd = NULL;
  }

  m_hWnd = NULL;  // by convention

  int iRval = 0;
//  int iRval = CWinApp::ExitInstance();
  if(m_msgCur.message == WM_QUIT)
  {
    iRval = static_cast<int>(m_msgCur.wParam);
  }

  CoUninitialize();

  PostQuitMessage(iRval);

  return(iRval);
}


BOOL CSETUPApp::DoSelfExtract(const BYTE *lpEnd, const BYTE *lpSelf, DWORD cbSelf)
{
  CString csTemp;
  const BYTE *lpc1 = lpEnd;
  DWORD cb1, cbRes;

  // get temporary directory, and make THAT 'm_csExePath'

  m_csEXEPath = "%tmpdir%\\SFTSETUP\\";
  ExpandStrings(m_csEXEPath);  // initialization was done, so this should work

  CheckForAndCreateDirectory(m_csEXEPath, FALSE);  // just make it

  DeleteFile(m_csEXEPath + "SETUP.INF");   // make sure they're not there
  DeleteFile(m_csEXEPath + "LICENSE.TXT");
  DeleteFile(m_csEXEPath + "SFTCAB.1");


  // EXE format:
  // [program space]
  // [PROGRAM's NORMAL EOF]
  // 4 byte low endian section size
  // SETUP.INF (0-n bytes)
  // 4 byte low endian section size
  // LICENSE.TXT (0-n bytes)
  // 4 byte low endian section size
  // SFTCAB.1 (0-n bytes)
  // [NEW PROGRAM EOF]

  // section order is:
  // SETUP.INF, LICENSE.TXT, SFTCAB.1

  // these will be extracted into the 'temporary files' directory

  // ** SETUP.INF **

  cbRes = *((const DWORD *)lpc1);
  lpc1 += sizeof(DWORD);
  
  HANDLE hFile = CreateFile(m_csEXEPath + "SETUP.INF",
                            GENERIC_READ | GENERIC_WRITE,
                            0, NULL, CREATE_ALWAYS,
                            FILE_ATTRIBUTE_NORMAL, NULL);

  if(hFile == INVALID_HANDLE_VALUE)
  {
    MyMessageBox((IDS_ERROR37), MB_OK | MB_ICONHAND | MB_SETFOREGROUND);

    return FALSE;
  }

  if(!WriteFile(hFile, lpc1, cbRes, &cb1, NULL) ||
     cb1 != cbRes)
  {
    CloseHandle(hFile);

    MyMessageBox((IDS_ERROR38), MB_OK | MB_ICONHAND | MB_SETFOREGROUND);

#ifndef _DEBUG
    DeleteFile(m_csEXEPath + "SETUP.INF");   // make sure they're not there
#endif // _DEBUG

    return FALSE;
  }

  CloseHandle(hFile);

  lpc1 += cbRes;  // points to next section



  // ** LICENSE.TXT **

  cbRes = *((const DWORD *)lpc1);
  lpc1 += sizeof(DWORD);

  if(cbRes)  // only if something's there
  {
    hFile = CreateFile(m_csEXEPath + "LICENSE.TXT",
                       GENERIC_READ | GENERIC_WRITE,
                       0, NULL, CREATE_ALWAYS,
                       FILE_ATTRIBUTE_NORMAL, NULL);

    if(hFile == INVALID_HANDLE_VALUE)
    {
      MyMessageBox((IDS_ERROR39),
                    MB_OK | MB_ICONHAND | MB_SETFOREGROUND);

#ifndef _DEBUG
      DeleteFile(m_csEXEPath + "SETUP.INF");   // make sure they're not there
#endif // _DEBUG

      return FALSE;
    }

    if(!WriteFile(hFile, lpc1, cbRes, &cb1, NULL) ||
       cb1 != cbRes)
    {
      CloseHandle(hFile);

      MyMessageBox((IDS_ERROR40), MB_OK | MB_ICONHAND | MB_SETFOREGROUND);

#ifndef _DEBUG
      DeleteFile(m_csEXEPath + "SETUP.INF");   // make sure they're not there
      DeleteFile(m_csEXEPath + "LICENSE.TXT");
#endif // _DEBUG

      return FALSE;
    }

    CloseHandle(hFile);

    lpc1 += cbRes;  // points to next section
  }


  // ** SFTCAB.1 **

  // calculate # of bytes remaining in the file from this point...

  cbRes = (DWORD)lpSelf + cbSelf - (DWORD)lpc1;

  hFile = CreateFile(m_csEXEPath + "SFTCAB.1",
                     GENERIC_READ | GENERIC_WRITE,
                     0, NULL, CREATE_ALWAYS,
                     FILE_ATTRIBUTE_NORMAL, NULL);

  if(hFile == INVALID_HANDLE_VALUE)
  {
    MyMessageBox((IDS_ERROR41), MB_OK | MB_ICONHAND | MB_SETFOREGROUND);

#ifndef _DEBUG
    DeleteFile(m_csEXEPath + "SETUP.INF");   // make sure they're not there
    DeleteFile(m_csEXEPath + "LICENSE.TXT");
#endif // _DEBUG

    return FALSE;
  }

  // if there is more than 1Mb of stuff, put up a progress dialog box

  if(cbRes < 0x100000)
  {
    if(!WriteFile(hFile, lpc1, cbRes, &cb1, NULL) ||
       cb1 != cbRes)
    {
      CloseHandle(hFile);

      MyMessageBox((IDS_ERROR42), MB_OK | MB_ICONHAND | MB_SETFOREGROUND);

#ifndef _DEBUG
      DeleteFile(m_csEXEPath + "SETUP.INF");   // make sure they're not there
      DeleteFile(m_csEXEPath + "LICENSE.TXT");
      DeleteFile(m_csEXEPath + "SFTCAB.1");
#endif // _DEBUG

      return FALSE;
    }
  }
  else
  {
    DWORD cbTotal = 0;
    CProgressDialog dlgProgress;

    m_hWnd = dlgProgress.m_hWnd; // so message loop will work

    if(!dlgProgress.Create(CProgressDialog::IDD, NULL))
    {
      MyMessageBox((IDS_ERROR43),
                    MB_OK | MB_ICONHAND | MB_SETFOREGROUND);
    }

    dlgProgress.SendMessage(WM_COMMAND, 0x1234, (LPARAM)"Initializing Setup...");
    dlgProgress.SendMessage(WM_COMMAND, 0x4321, (LPARAM)"0");

    dlgProgress.ShowWindow(SW_SHOWNORMAL);
    dlgProgress.SetForegroundWindow();
    SetWindowPos(dlgProgress.m_hWnd, HWND_TOPMOST, 0, 0, 0, 0,
                 SWP_NOSIZE | SWP_NOMOVE | SWP_SHOWWINDOW);
    InvalidateRect(dlgProgress.m_hWnd, NULL, TRUE);  // force paint *NOW*
    dlgProgress.UpdateWindow();

    CMyWaitCursor wait;

    while(cbTotal < cbRes)
    {
      csTemp.Format("%d",(int)(100.0 * cbTotal / cbRes));
      dlgProgress.SendMessage(WM_COMMAND, 0x4321, (LPARAM)(LPCSTR)csTemp);
      dlgProgress.UpdateWindow();

      DWORD cbBuf = min(0x100000, cbRes - cbTotal);

      if(!WriteFile(hFile, lpc1 + cbTotal, cbBuf, &cb1, NULL) ||
         cb1 != cbBuf)
      {
        CloseHandle(hFile);

        MyMessageBox((IDS_ERROR44), MB_OK | MB_ICONHAND | MB_SETFOREGROUND);

//        m_pMainWnd = NULL;
        dlgProgress.DestroyWindow();

#ifndef _DEBUG
        DeleteFile(m_csEXEPath + "SETUP.INF");   // make sure they're not there
        DeleteFile(m_csEXEPath + "LICENSE.TXT");
        DeleteFile(m_csEXEPath + "SFTCAB.1");
#endif // _DEBUG

        return FALSE;
      }

      cbTotal += cbBuf;
    }

//    m_pMainWnd = NULL;
    dlgProgress.DestroyWindow();
  }


  CloseHandle(hFile);

  m_hWnd = NULL;  // make sure!

  return TRUE;  // we're ok!
}


#ifdef _DEBUG
static const char radix_str[] = "012345678ABCDEFGHJKMNPRSTUVWXYZ";        // base 31 character set
#endif // _DEBUG

static const char charset_str[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"; // entire character set

static void CreateRadix(char szRadix[32])
{
  int i1, i2;

  // to make reverse engineering a pain in the ass, build 'radix_str' in a loop.

  for(i1=0, i2=0; i1 <= 8; i1++)
    szRadix[i2++] = (char)('0' + i1);

  for(i1='A'; i1 <= 'H'; i1++)
    szRadix[i2++] = (char)i1;

  szRadix[i2++] = 'J';
  szRadix[i2++] = 'K';
  szRadix[i2++] = 'M';
  szRadix[i2++] = 'N';
  szRadix[i2++] = 'P';

  for(i1='R'; i1 <= 'Z'; i1++)
    szRadix[i2++] = (char)i1;

  szRadix[i2] = 0;

  ASSERT(i2 == 31);
  ASSERT(!_stricmp(radix_str, szRadix));
}

static CString AuthCodeEncrypt(LPCSTR szCompany, LPCSTR szRadix, const GUID &riid)
{
  CString csTemp;
  char szCharset[37];

  CString csCompany = szCompany;
  csCompany.MakeUpper();

  int i1 = 0;
  while(i1 < csCompany.GetLength())
  {
    if(csCompany[i1] >= '0' && csCompany[i1] <= '9')
    {
      i1++;
    }
    else if(csCompany[i1] >= 'A' && csCompany[i1] <= 'Z')
    {
      i1++;
    }
    else
    {
      if(i1 > 0)
        csCompany = csCompany.Left(i1) + csCompany.Mid(i1 + 1);
      else
        csCompany = csCompany.Mid(1);
    }
  }

  csTemp.Format("%08X%04X%04X%02X%02X%02X%02X%02X%02X%02X%02X",
                riid.Data1, riid.Data2, riid.Data3,
                (BYTE)riid.Data4[0],
                (BYTE)riid.Data4[1],
                (BYTE)riid.Data4[2],
                (BYTE)riid.Data4[3],
                (BYTE)riid.Data4[4],
                (BYTE)riid.Data4[5],
                (BYTE)riid.Data4[6],
                (BYTE)riid.Data4[7]);

  CString csB = csTemp + csCompany;
  CString csC = csB.Left(7);

  int i2 = 0;

  for(i1='0', i2=0; i1 <= '9'; i1++)
    szCharset[i2++] = (char)i1;

  for(i1='A'; i1 <= 'Z'; i1++)
    szCharset[i2++] = (char)i1;

  szCharset[i2] = 0;
  int nRadix = strlen(szRadix);

  ASSERT(i2 == 36);

  for(i1=7; i1 < csB.GetLength(); i1++)
  {
    UINT c8 = ((CString)szCharset).Find(csC[i1 - 7]) + 1;
    UINT c7 = ((CString)szCharset).Find(csC[i1 - 6]) + 1;
    UINT c6 = ((CString)szCharset).Find(csC[i1 - 5]) + 1;
    UINT c5 = ((CString)szCharset).Find(csC[i1 - 4]) + 1;
    UINT c4 = ((CString)szCharset).Find(csC[i1 - 3]) + 1;
    UINT c3 = ((CString)szCharset).Find(csC[i1 - 2]) + 1;
    UINT c2 = ((CString)szCharset).Find(csC[i1 - 1]) + 1;

    UINT c1 = ((CString)szCharset).Find(csB[i1]) + 1;

    // the formula

    c1 = c1 * c2 + c3 * c4;
    c2 = c5 * c7 + c6 * c8;

    c1 = c1 ^ c2;

    c1 = c1 % nRadix;

    ASSERT(c1 >= 0 && c1 < (UINT)nRadix);

    csC += szRadix[c1];
  }

  return(csC.Right(8));
}

static DWORD Base31(LPCSTR szSource, LPCSTR szRadix)
{
  int i1, i2;
  DWORD dwRval= 0;

  ASSERT(strlen(szRadix) == 31);

  for(i1=0; szSource[i1]; i1++)
  {
    for(i2=0; i2 < 31; i2++)
    {
      if(szSource[i1] == szRadix[i2])
      {
        dwRval = dwRval * 31 + i2;
        break;
      }
    }
  }

  return(dwRval);
}

void CSETUPApp::GetAndValidateAuthCode()
{
char szRadix[32];

  CreateRadix(szRadix); // 31 characters long

  // convert the encrypted authorization code into the actual authorization code
  // if it's valid, I display the serial number portion.  If it's *NOT* valid,
  // I go ahead and work, but portray it as an "unlicensed copy" in nice big text.

  m_csAuthCode = "";  // default value (and initial value)
  CString csTemp2, csTemp = GetInfEntry("SetupOptions", "AuthCode");

  if(csTemp.GetLength())
  {
    csTemp2 = m_csCompanyName;
    while(csTemp2.GetLength() * 2 < csTemp.GetLength())
      csTemp2 += ' ';  // same equivalent as that which created it

    csTemp2 = csTemp2.Left(csTemp.GetLength() / 2);

    int i1;
    for(i1=0; i1 < csTemp2.GetLength(); i1++)
    {
      char c1 = csTemp[i1 * 2];
      char c2 = csTemp[i1 * 2 + 1];

      if(c1 >= 'A' && c1 <= 'F')
        c1 = '\xa' + c1 - 'A';
      else
        c1 -= '0';

      if(c2 >= 'A' && c2 <= 'F')
        c2 = '\xa' + c2 - 'A';
      else
        c2 -= '0';

      BYTE b1 = (BYTE)c2 | ((BYTE)c1 << 4);  // I got my hex digit back

      // 'xor' with previous entry

      if(i1)
      {
        b1 ^= csTemp2[i1 - 1];    // XOR with previous character
      }

      if(i1 & 7)  // rotation using the index
      {
        b1 = (b1 >> (i1 & 7)) | (b1 << (8 - (i1 & 7)));
      }
      
      b1 = b1 ^ (BYTE)csTemp2[i1];

      m_csAuthCode += (char)b1;  // that oughta do it!
    }

    // validate authorization code now.  If not valid, make it blank.

    // {234711A4-7010-4c81-B1A2-AA01C429C741}
    static const GUID beta = 
    { 0x234711a4, 0x7010, 0x4c81, { 0xb1, 0xa2, 0xaa, 0x1, 0xc4, 0x29, 0xc7, 0x41 } };

    csTemp.Format("%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x",
                  beta.Data1, beta.Data2, beta.Data3,
                  (BYTE)beta.Data4[0],
                  (BYTE)beta.Data4[1],
                  (BYTE)beta.Data4[2],
                  (BYTE)beta.Data4[3],
                  (BYTE)beta.Data4[4],
                  (BYTE)beta.Data4[5],
                  (BYTE)beta.Data4[6],
                  (BYTE)beta.Data4[7]);

    if(!m_csAuthCode.CompareNoCase(csTemp))
    {
      // this is ok - it's a "beta" code
      m_csAuthCode = "{BETA RELEASE}";
    }
    else // TODO:  check for valid serial number, place it directly into 'm_csAuthCode'
    {
      csTemp = m_csAuthCode;

      m_csAuthCode = "";  // initially, the authorization code MUST be blank

      // NOTE:  this uses a simple hash with about 39 bits of encryption.  It's
      //        not very strong and this is open source anyway.  Please, do NOT
      //        alter this code, or the related code that validates the SETUP.INF
      //        file as being a 'licensed' copy.  Think of this as 'shareware' as
      //        far as the license terms go.  NOT disabling this is like
      //        'supporting' the development.

      // the 1st 2 bytes of 'csTemp' should be the length as Base 31.  If this
      // is not the case, it is *NOT* valid!

      if(Base31(csTemp.Left(2), szRadix) == (DWORD)csTemp.GetLength()) // this should be valid
      {
        csTemp = csTemp.Mid(2);

        // ok, first step passed - now get the company name out of the authorization code
        // This will be all but the last 8 digits.

        m_csAuthCode = csTemp.Right(8);
        m_csAuthCodeCompany = csTemp.Left(csTemp.GetLength() - 8);

        m_csAuthCodeCompany.TrimRight();
        m_csAuthCodeCompany.TrimLeft();  // this will be displayed along with the serial #

        csTemp = csTemp.Left(csTemp.GetLength() - 8);
        csTemp.MakeUpper();

        // reduce this down to its 'authcode' form

        i1 = 0;
        while(i1 < csTemp.GetLength())
        {
          if(csTemp[i1] >= '0' && csTemp[i1] <= '9')
          {
            i1++;
          }
          else if(csTemp[i1] >= 'A' && csTemp[i1] <= 'Z')
          {
            i1++;
          }
          else
          {
            if(i1 > 0)
              csTemp = csTemp.Left(i1) + csTemp.Mid(i1 + 1);
            else
              csTemp = csTemp.Mid(1);
          }
        }

        // running encryption

        // yet another "private key" - well, being 'open source' it really doesn't matter, does it?
        // but please leave this alone anyway.  Yeah, it's a pathetically weak algorith, which is
        // why I don't care much about it.  But if someone WANTS to license it, I should be able
        // to give that person or entity a license, right?  Changing this 'breaks' that.
        static const GUID iid = {0x234711a4,0x7010,0x4c81,{0xb1,0xa2,0xaa,0x01,0xc4,0x29,0xc7,0x41}};

        if(AuthCodeEncrypt(csTemp, szRadix, iid) != m_csAuthCode)
          m_csAuthCode = "";  // to flag that it's not valid
      }
    }
  }
}


void CSETUPApp::GetInfEntries(LPCSTR szKey, CStringArray &acsEntries)
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

CString CSETUPApp::GetInfEntry(LPCSTR szKey, LPCSTR szString)
{
CStringArray acsEntries;
CString csString, csTemp, csRval;

  GetInfEntries(szKey, acsEntries);

  csString = (CString)szString + '=';

  int i1;
  for(i1=0; i1 < acsEntries.GetSize(); i1++)
  {
    csTemp = acsEntries[i1];

    if(!csTemp.Left(csString.GetLength()).CompareNoCase(csString))
    {
      csRval = acsEntries[i1].Mid(csString.GetLength());
      csRval.TrimRight();
      csRval.TrimLeft();

      // return it "as-is" with quotes, when applicable

      return(csRval);
    }
  }

  return("");  // not found
}


void CSETUPApp::SetProgress(UINT uiPercent)
{
static char szText[32];

  wsprintf(szText, "%d", uiPercent);

  if(m_pProgressDlg)
  {
    m_pProgressDlg->SendMessage(WM_COMMAND, 0x4321, (LPARAM)szText);
  }
}

void CSETUPApp::SetProgressText(LPCSTR szText)
{
static char szMessage[1024];

  int cb1 = strlen(szText);
  cb1 = min(sizeof(szMessage) - 1, cb1);

  memcpy(szMessage, szText, cb1);
  szMessage[cb1] = 0;

  if(m_pProgressDlg)
  {
    m_pProgressDlg->SendMessage(WM_COMMAND, 0x1234, (LPARAM)szMessage);
  }
}


BOOL CSETUPApp::UpgradeCheck(void)
{
  CString csMsg, csTemp, csTemp2;

  m_bUpgrading = FALSE;  // initial value
  m_csUpgradeApp = m_csAppName;  // default value

  // see if the application is already installed in a particular place,
  // and if so, set my default application path(s) to THAT location...

  CStringArray acsAppName;

  csTemp = GetInfEntry("SetupOptions","Upgrade");

  DoParseString(csTemp, acsAppName);
  acsAppName.InsertAt(0,m_csAppName);  // this one first, always

  int i1, i2;
  for(i2=0; !m_bUpgrading && i2 < acsAppName.GetSize(); i2++)
  {
    CString csExistingSetupPath = GetRegistryString(HKEY_LOCAL_MACHINE,
                                                    szSETUP_KEY
                                                    + AppNameToRegKey(acsAppName[i2]), "");

    if(csExistingSetupPath.GetLength())
    {
      // see if this path still exists and if there are files there

      csExistingSetupPath = AdjustPathName2(csExistingSetupPath);

      csTemp = csExistingSetupPath + "\\*.*";

      WIN32_FIND_DATA fd;
      HANDLE hFF = FindFirstFile(csTemp, &fd);

      if(hFF == INVALID_HANDLE_VALUE)
      {
        csExistingSetupPath = "";  // assume that it's *NOT* installed any more
      }
      else
      {
        FindClose(hFF);  // at least one file exists in this directory, so it's valid

        m_csAppPath = csExistingSetupPath;  // this *ALWAYS* overrides!

        if(!m_bQuietSetup)
        {
          if(!acsAppName[i2].CompareNoCase(m_csAppName))  // they match??
            csMsg.Format(theApp.LoadString(IDS_WARNING19), (LPCSTR)acsAppName[i2]);
          else
            csMsg.Format(theApp.LoadString(IDS_WARNING20), (LPCSTR)acsAppName[i2], (LPCSTR)m_csAppName);

          if(MyMessageBox(csMsg, MB_OKCANCEL | MB_ICONEXCLAMATION | MB_SETFOREGROUND)
             != IDOK)
          {
            return FALSE;  // this ends setup immediately
          }
        }

        m_bUpgrading = TRUE;  // YES, I am upgrading!
        m_csUpgradeApp = acsAppName[i2];


        // load up AppCommon with the specified 'AppCommon' path (if in the registry)

        csTemp = GetRegistryString(HKEY_LOCAL_MACHINE,
                                   szSETUP_KEY
                                   + AppNameToRegKey(m_csUpgradeApp)
                                   + "\\AppCommon", "");

        if(csTemp.GetLength())
        {
          m_csAppCommon = AdjustPathName2(csTemp);
        }


        // do the same for the startup folder name - thereby keeping the LAST selections intact

        csTemp = GetRegistryString(HKEY_LOCAL_MACHINE,
                                   szSETUP_KEY
                                   + AppNameToRegKey(m_csUpgradeApp),
                                   "StartMenuFolderName");

        if(csTemp.GetLength())
          csTemp = AdjustPathName2(csTemp);

        if(m_bQuietSetup)
        {
          if(csTemp.GetLength())
            m_csStartMenuFolderName = csTemp;  // in quiet setup, keep the ABSOLUTE path
        }
        else if(csTemp.GetLength())
        {
          static const char szStartMenuPrograms[] = "\\START MENU\\PROGRAMS\\";
          csTemp2 = csTemp;

          csTemp2.MakeUpper();
          i1 = csTemp2.Find(szStartMenuPrograms);  // compare UC to UC (just in case)

          if(i1 >= 0)
          {
            i1 += strlen(szStartMenuPrograms);

            m_csStartMenuFolderName = csTemp.Mid(i1);  // everything following ^ that ^
          }
          else
          {
            i1 = csTemp.ReverseFind('\\');
            if(i1 >= 0)
              csTemp = csTemp.Mid(i1 + 1);  // get the path AFTER the backslash [a hack, really]

            // NOTE:  this may not work properly if the user specifies a name with a backslash in it

            m_csStartMenuFolderName = csTemp;
          }
        }
      }
    }
  }


  if(m_bUpgrading)
  {
    // build a list o' previously installed options from its own special registry key
    // this *ALWAYS* adds to the pre-selected options from the command line for things
    // that are already installed.  Items NOT already installed that were added to the
    // command line will be left as-is.

    csTemp = GetRegistryString(HKEY_LOCAL_MACHINE,
                               szSETUP_KEY
                               + AppNameToRegKey(m_csUpgradeApp)
                               + "\\InstalledOptions", "");

    DoParseString(csTemp, m_acsUpgradeInstalledOptions);

    int i1, i2;
    for(i1=0; i1 < m_acsUpgradeInstalledOptions.GetSize(); i1++)
    {
      for(i2=0; i2 < m_acsOptions.GetSize(); i2++)
      {
        if(!m_acsUpgradeInstalledOptions[i1].CompareNoCase(m_acsOptions[i2]))
          break;
      }

      if(i2 >= m_acsOptions.GetSize())
        m_acsOptions.Add(m_acsUpgradeInstalledOptions[i1]);
    }

    if(m_acsOptions.GetSize() != m_acsUpgradeInstalledOptions.GetSize() &&
       m_bExtraQuietMode)
    {
      MyMessageBox((IDS_WARNING17), MB_OK | MB_ICONASTERISK);  // warn about option selection
    }
  }

  return TRUE;
}

int CSETUPApp::DriveSpaceCheck(LPCSTR szPath, BOOL bNoRetry /* = FALSE */)
{
  // return -1 if SETUP should close, 0 if reprompt path, 1 if "keep path as-is"

  CString csTemp, csPath = AdjustPathName2(szPath);
  
  if(!GetFullPathName(csPath, MAX_PATH * 4,
                      csTemp.GetBufferSetLength(MAX_PATH * 4), NULL))
  {
    if(bNoRetry)
    {
      csTemp.Format(theApp.LoadString(IDS_ERROR77), szPath);
      MyMessageBox(csTemp, MB_OK | MB_ICONHAND | MB_SETFOREGROUND);

      return(-1);
    }
    else
    {
      csTemp.Format(theApp.LoadString(IDS_ERROR73), szPath);

      return(MyMessageBox(csTemp, MB_OKCANCEL | MB_ICONHAND | MB_SETFOREGROUND)
             == IDOK ? 0 : -1);
    }
  }

  DWORD dwSPC=0, dwBPS=0, dwFree=0, dwTotal=0;

  // get the drive letter from the path

  if(csTemp.GetLength() > 2 && csTemp[0] == '\\' && csTemp[1] == '\\')
  {
    int iBS = csTemp.Find('\\', 2);

    if(iBS > 0)
      iBS = csTemp.Find('\\', iBS + 1);

    if(iBS > 0)
      csTemp = csTemp.Left(iBS);
  }
  else
  {
    int iBS = csTemp.Find('\\');

    if(iBS >= 0)
      csTemp = csTemp.Left(iBS + 1);  // include the backslash

    // assume at THIS point that 'csTemp' points to a valid drive letter
  }

  if(!GetDiskFreeSpace(csTemp, &dwSPC, &dwBPS, &dwFree, &dwTotal))
  {
    if(bNoRetry)
    {
      csTemp.Format(theApp.LoadString(IDS_ERROR78), szPath);
      MyMessageBox(csTemp, MB_ICONHAND | MB_OK | MB_SETFOREGROUND);

      return(-1);
    }
    else
    {
      csTemp.Format(theApp.LoadString(IDS_ERROR76), szPath, m_dwTotalFileSizeX);
      return(MyMessageBox(csTemp, MB_ICONHAND | MB_OKCANCEL | MB_SETFOREGROUND)
             == IDOK ? 1 : -1);
    }
  }

  if(m_dwTotalFileSizeX >= 0x7fffffffL)
  {
    // TODO:  should I try to use 'GetDiskFreeSpaceEx' ?
  }

  unsigned __int64 ldwClusterSize = (unsigned __int64)dwBPS * (unsigned __int64)dwSPC;
  unsigned __int64 ldwNeed = (((unsigned __int64)m_dwTotalFileSizeX + ldwClusterSize)
                              / ldwClusterSize + 32)
                           * ldwClusterSize;
  unsigned __int64 ldwFree = (unsigned __int64)dwFree * ldwClusterSize;

  if(m_dwTotalFileSizeX && ldwNeed >= ldwFree)
  {
    if(m_bUpgrading && !csPath.CompareNoCase(AdjustPathName2(m_csAppPath)))
      csTemp.Format(theApp.LoadString(IDS_ERROR74), szPath, ldwNeed, ldwFree);
    else
      csTemp.Format(theApp.LoadString(IDS_ERROR75), szPath, ldwNeed, ldwFree);

    return(MyMessageBox(csTemp, MB_ICONEXCLAMATION | MB_OKCANCEL | MB_SETFOREGROUND)
           == IDOK ? 1 : 0);
  }

  return(1);  // must assume that it's ok, here
}

BOOL CSETUPApp::PromptForAndCreateAppPath(void)
{
  CString csMsg;

  if(m_bQuietSetup && !m_csAppPath.GetLength())
  {
    m_csAppPath = m_csWinPath;

    return(DriveSpaceCheck(m_csAppPath) > 0);  // windows directory is assumed here
  }

  CString csAppPath = m_csAppPath;

  if(!csAppPath.GetLength())
  {
    ASSERT(!m_bUpgrading);  // this should *NEVER* happen...

    // INITIALIZATION:  default app path

    csAppPath = GetRegistryString(HKEY_LOCAL_MACHINE,
                                  szHKLM_WINDOWS_CURRENT_VERSION,
                                  "ProgramFilesDir");

    if(!csAppPath.GetLength())
      csAppPath = GetRegistryString(HKEY_LOCAL_MACHINE,
                                    szHKLM_WINDOWS_CURRENT_VERSION,
                                    "ProgramFilesPath");  // try both

    if(!csAppPath.GetLength())
    {
      // use windows directory

      csAppPath = m_csWinPath;
    }

    csAppPath = AdjustPathName(csAppPath)
              + AppNameToPathName(theApp.m_csAppName);
  }

  if(!ExpandStrings(csAppPath))
  {
    csMsg.Format(theApp.LoadString(IDS_ERROR51), (LPCSTR)csAppPath);

    MyMessageBox(csMsg, MB_OK | MB_ICONHAND | MB_SETFOREGROUND);

    return FALSE;  // end SETUP
  }

  if(m_bQuietSetup ||  // in "quiet setup" mode I don't want to prompt unless I must...
     m_bExtraQuietMode)
  {
    if(DriveSpaceCheck(csAppPath) <= 0)
    {
      if(m_bUpgrading && m_bExtraQuietMode)
        return FALSE;   // extra quiet setup with upgrade does *NOT* prompt for a path, ever!
    }
    else if(CheckForAndCreateDirectory(csAppPath, FALSE))
    {
      m_csAppPath = csAppPath;  // make sure I assign it now...

      return TRUE;  // it works!
    }
  }


  // GET THE MAIN APPLICATION PATH!

  CPathDialog dlgPath;
  dlgPath.m_csPathName = csAppPath;
  dlgPath.m_csTitle = m_csAppName + " SETUP - Installation Path";

  CString csExistingSetupPath = AdjustPathName2(m_csAppPath);

  do
  {
    while(1)
    {
      if(dlgPath.DoModal() != IDOK)
      {
        return FALSE;
      }

      csAppPath = AdjustPathName2(dlgPath.m_csPathName);

      int i1 = DriveSpaceCheck(csAppPath);

      if(i1 < 0)
        return FALSE;
      else if(!i1)
        continue;  // just re-prompt, and keep re-prompting

      if(!m_bUpgrading || !csExistingSetupPath.GetLength() ||
         !csExistingSetupPath.CompareNoCase(csAppPath))
      {
        break;  // no need to loop - path matches original, or not installed before
      }

      csMsg = theApp.LoadString(IDS_WARNING11);

      if(MyMessageBox(csMsg, MB_OKCANCEL | MB_ICONHAND | MB_SETFOREGROUND)
         == IDOK)
      {
        m_bUpgrading = FALSE;  // turn OFF the 'ugrading' flag (since I changed the path)
                               // and so that it doesn't prompt again

        break;  // accept the path "as-is"
      }

      dlgPath.m_csPathName = m_csAppPath;  // restore the path and re-prompt
    }

  } while(!CheckForAndCreateDirectory(csAppPath, TRUE));

  m_csAppPath = csAppPath;  // assign it now...

  return TRUE;
}

BOOL CSETUPApp::PromptForAndCreateAppCommon(void)
{
  CString csAppCommon = m_csAppCommon;

  // INITIALIZATION:  default app path

  if(!csAppCommon.GetLength())
  {
    csAppCommon = GetRegistryString(HKEY_LOCAL_MACHINE,
                                    szHKLM_WINDOWS_CURRENT_VERSION,
                                    "ProgramFilesDir");

    if(!csAppCommon.GetLength())
      csAppCommon = GetRegistryString(HKEY_LOCAL_MACHINE,
                                      szHKLM_WINDOWS_CURRENT_VERSION,
                                      "ProgramFilesPath");  // try both

    if(!csAppCommon.GetLength())
    {
      // use windows directory

      csAppCommon = m_csWinPath;
    }

    csAppCommon = AdjustPathName(csAppCommon)
                + "Common Files";
  }

  if(!ExpandStrings(csAppCommon))
  {
    CString csMsg;
    csMsg.Format(theApp.LoadString(IDS_ERROR52), (LPCSTR)csAppCommon);

    MyMessageBox(csMsg, MB_OK | MB_ICONHAND | MB_SETFOREGROUND);

    return FALSE;  // end SETUP
  }

  if(m_bQuietSetup ||  // in "quiet setup" mode I don't want to prompt unless I must...
     m_bUpgrading)     // or if I'm upgrading, I want to keep the EXISTING value
  {
    if(CheckForAndCreateDirectory(csAppCommon, FALSE))
    {
      m_csAppCommon = csAppCommon;  // make sure I assign it now...

      return TRUE;  // it works!
    }
  }

  // GET THE APPLICATION 'COMMON FILES' PATH!

  CPathDialog dlgPath;
  dlgPath.m_csPathName = csAppCommon;
  dlgPath.m_csTitle = m_csAppName + " SETUP - Common Files Installation Path";

  do
  {
    if(dlgPath.DoModal() != IDOK)
    {
      return FALSE;
    }

    csAppCommon = AdjustPathName(dlgPath.m_csPathName);

  } while(!CheckForAndCreateDirectory(csAppCommon, TRUE));

  m_csAppCommon = AdjustPathName2(csAppCommon);  // assign it now...

  return TRUE;
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

BOOL CSETUPApp::ExpandStrings(CString &csStringVal)
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
            // appcommon was referenced

            if(PromptForAndCreateAppCommon())
            {
              return FALSE;
            }

            csTemp = AdjustPathName2(m_csAppCommon);
          }
          else if(!csTemp.CompareNoCase("uninst"))
          {
            ASSERT(m_csUninstPath.GetLength());

            if(!m_csUninstPath.GetLength())
            {
              m_csUninstPath = AdjustPathName(theApp.m_csAppPath)
                             + "UNINST\\"
                             + AppNameToPathName(theApp.m_csAppName)
                             + "\\";
            }

            csTemp = AdjustPathName2(m_csUninstPath);
                // really, it's only valid after beginning the extract...
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

            // just to be sure, CREATE it (ignore errors - they'll show up later)

            CheckForAndCreateDirectory(csTemp, FALSE);
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
              return FALSE;
          }
          else if(!csTemp.CompareNoCase("windrive"))
          {
            csTemp = DriveFromPath(m_csWinPath);

            if(!csTemp.GetLength())
              return FALSE;
          }
          else if(!csTemp.CompareNoCase("sysdrive"))
          {
            csTemp = DriveFromPath(m_csSysPath);

            if(!csTemp.GetLength())
              return FALSE;
          }
          else if(!csTemp.CompareNoCase("bootdrive"))
          {
            csTemp = DriveFromPath(m_csWinPath);  // for now - later fix

            if(!csTemp.GetLength())
              return FALSE;
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
              return FALSE;
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
              return FALSE;
          }
          else if(!csTemp.CompareNoCase("srcdrive"))
          {
            csTemp = DriveFromPath(m_csEXEPath);

            if(!csTemp.GetLength())
              return FALSE;
          }
          else
          {
            // on ERROR, return FALSE!!!!!

            return FALSE;
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

  return TRUE;
}

CString CSETUPApp::GetFileEntryDestPath(LPCSTR szFileEntry)
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

int CSETUPApp::GetFileEntryCabNumber(LPCSTR szFileEntry)
{
  CString csFileEntry = szFileEntry;
  CStringArray acsTemp;

  DoParseString(csFileEntry, acsTemp);

  if(acsTemp.GetSize() < 2)
    return(0);  // an error

  int iRval = atoi(acsTemp[0]);

  if(iRval == 0)
    iRval = -atoi(acsTemp[1]);

  return(iRval);
}

BOOL CSETUPApp::IsFileEntryShared(LPCSTR szFileEntry)
{
  CString csFileEntry = szFileEntry;
  CStringArray acsTemp;

  DoParseString(csFileEntry, acsTemp);

  if(acsTemp.GetSize() < 3)
    return(0);  // an error

  int i1;
  for(i1=0; i1 < m_acsShared.GetSize(); i1++)
  {
    if(!m_acsShared[i1].CompareNoCase(acsTemp[2]))
    {
      return TRUE;  // shared file
    }
  }

  return FALSE;  // not in 'system/shared' section
}

BOOL CSETUPApp::IsFileEntryOverwriteAlways(LPCSTR szFileEntry)
{
  CString csFileEntry = szFileEntry;
  CStringArray acsTemp;

  DoParseString(csFileEntry, acsTemp);

  if(acsTemp.GetSize() < 3)
    return(0);  // an error

  int i1;
  for(i1=0; i1 < m_acsOverwrite.GetSize(); i1++)
  {
    if(!m_acsOverwrite[i1].CompareNoCase(acsTemp[2]))
    {
      return TRUE;  // shared file
    }
  }

  return FALSE;  // not in 'system/shared' section
}

CString CSETUPApp::GetCabFilePath(int iCab)
{
  // get cabinet file path and optionally prompt for
  // a diskette containing the cabinet file.

  CString csCabFile;
  CStringArray acsTemp;

  int i1, iDisk;
  for(i1=0; i1 < m_acsCabs.GetSize(); i1++)
  {
    DoParseString(m_acsCabs[i1], acsTemp);

    if(acsTemp.GetSize() < 3)
      continue;

    if(atoi(acsTemp[0]) == iCab)
    {
      csCabFile = acsTemp[2];
      iDisk = atoi(acsTemp[1]);
      break;
    }
  }

  if(!csCabFile.GetLength())
  {
    MyMessageBox((IDS_ERROR53), MB_OK | MB_ICONHAND | MB_SETFOREGROUND);
    return("");
  }

  CString csPath = GetPathAndPromptForDiskette(iDisk);

  if(!csPath.GetLength())
    return("");

  return(AdjustPathName(csPath) + csCabFile);
}

CString CSETUPApp::GetPathAndPromptForDiskette(int iDisk)
{
  // check for disk inserted.
  // If the path "..\disk#" exists, do not prompt.
  // If this path does not exist, check for the existence of one
  // of the target files on this disk.  In all cases it will either
  // be the first cabinet file on the diskette, *OR* (if no cab files
  // are on it) the first file written to the diskette.
  //
  // NOTE: DISK #1 checks for 'SETUP.INF'.

  // 1st, validate disk #

  CString csTemp, csTemp2, csDiskTitle, csTargetFile, csMsg;
  CStringArray acsTemp;
  int i1, i2;
  LPSTR lp1;

  for(i1=0; i1 < m_acsDisks.GetSize(); i1++)
  {
    DoParseString(m_acsDisks[i1], acsTemp);

    if(acsTemp.GetSize() < 2)
      continue;

    i2 = atol(acsTemp[0]);
    if(i2 == iDisk)
    {
      csDiskTitle = acsTemp[1];

      if(!csDiskTitle.GetLength())
        csDiskTitle.Format("SETUP Disk #%d", iDisk);

      break;
    }
  }

  if(!csDiskTitle.GetLength())
  {
    MyMessageBox((IDS_ERROR54), MB_OK | MB_ICONHAND | MB_SETFOREGROUND);
    return("");
  }

  // find out what the target file(s) are

  if(iDisk == 1)  // check for 'SETUP.INF'
  {
    csTargetFile = "SETUP.INF";
  }
  else
  {
    for(i1=0; i1 < m_acsCabs.GetSize(); i1++)
    {
      DoParseString(m_acsCabs[i1], acsTemp);

      if(acsTemp.GetSize() < 3)
        continue;

      if(atoi(acsTemp[1]) == iDisk)
      {
        csTargetFile = acsTemp[2];
        break;
      }
    }

    if(!csTargetFile.GetLength())  // no CAB found
    {
      for(i1=0; i1 < m_acsFiles.GetSize(); i1++)
      {
        DoParseString(m_acsFiles[i1], acsTemp);

        if(acsTemp.GetSize() < 3 ||
           atoi(acsTemp[0]))        // cab # must be ZERO
        {
          continue;
        }

        if(atoi(acsTemp[1]) == iDisk)
        {
          csTargetFile = acsTemp[2];
          break;
        }
      }
    }
  }

  if(!csTargetFile.GetLength())
  {
    MyMessageBox((IDS_ERROR55), MB_OK | MB_ICONHAND | MB_SETFOREGROUND);
    return("");
  }


  do
  {
    // see if the 'DISKn' path exists... and use that as my path if it does

    csTemp.Format("..\\DISK%d", iDisk);
    csTemp = AdjustPathName(m_csEXEPath) + csTemp;

    if(!GetFullPathName(csTemp, MAX_PATH, 
                        csTemp2.GetBufferSetLength(MAX_PATH), &lp1))
    {
      csTemp2.ReleaseBuffer(0);
      csTemp2 = csTemp;
    }
    else
    {
      csTemp2.ReleaseBuffer(-1);
    }

    WIN32_FIND_DATA fd;
    HANDLE hFF = FindFirstFile(csTemp2, &fd);

    if(hFF != INVALID_HANDLE_VALUE)
    {
      // this MUST be a directory, also
      BOOL bIsDir = fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
      FindClose(hFF);

      if(bIsDir)
      {
        csTemp2 += '\\';

        // path exists - look for 'signature' file and ensure it's there

        hFF = FindFirstFile(csTemp2 + csTargetFile, &fd);

        if(hFF != INVALID_HANDLE_VALUE)
        {
          BOOL bIsDir = fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
          FindClose(hFF);

          if(!bIsDir)
            return(csTemp2);  // return the path to the files
        }
      }

      MyMessageBox((IDS_ERROR56), MB_OK | MB_ICONHAND | MB_SETFOREGROUND);
      return("");  // all other conditions here are an error
    }

    // check for appropriate files on this disk(ette), and if not found,
    // prompt user to insert the correct one.


    hFF = FindFirstFile(AdjustPathName(m_csEXEPath) + csTargetFile, &fd);

    if(hFF != INVALID_HANDLE_VALUE)
    {
      BOOL bIsDir = fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
      FindClose(hFF);

      if(!bIsDir)
      {
        return(AdjustPathName(m_csEXEPath));
      }
    }

    csMsg.Format(theApp.LoadString(IDS_ERROR57), (LPCSTR)csDiskTitle, (LPCSTR)m_csEXEPath.Left(2));

    if(MyMessageBox(csMsg, MB_OKCANCEL | MB_ICONASTERISK | MB_SETFOREGROUND)
       != IDOK)
    {
      break;
    }

  } while(1);

  return("");
}

int CSETUPApp::GetCabNumberFromCabName(LPCSTR szCabName)
{
  int i1;
  CStringArray acsTemp;

  for(i1=0; i1 < m_acsCabs.GetSize(); i1++)
  {
    DoParseString(m_acsCabs[i1], acsTemp);

    if(acsTemp.GetSize() < 3)
      continue;

    if(!acsTemp[2].CompareNoCase(szCabName))
    {
      return(atoi(acsTemp[0]));
    }
  }

  return(0);  // none
}

CString CSETUPApp::GetFileEntryFromName(LPCSTR szInternalName)
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

int CSETUPApp::GetCertFileCount(void)
{
CString csTemp;
int iRval;

  csTemp = theApp.GetInfEntry("CertFiles", "CertFileCount");
  if(csTemp.GetLength())
  {
    iRval = atoi(csTemp);
  }
  else
  {
    iRval = 0;
  }

  return iRval;
}

CString CSETUPApp::GetCertFileEntry(int iIndex)
{
CString csTemp;

  csTemp.Format("certs\\cert%03d.cer", iIndex);

  return theApp.GetInfEntry("CertFiles", csTemp);
}


// OTHER (global) UTILITIES

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
      return FALSE;
    }

    bIsDir = TRUE;  // because a drive and a directory are kinda the same
    return TRUE;  // assume valid
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

    return TRUE; // it *does* exist!!
  }

  return FALSE;  // it does *NOT* exist
}

BOOL IsValidDirectory(LPCSTR szDirName)
{
  BOOL bIsDrive, bIsDir;

  return(CheckForExistence(szDirName, bIsDir, bIsDrive) &&
         (bIsDir || bIsDrive));  // either a drive or a directory, and it exists
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

  BOOL bIsDir = (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;

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

BOOL CheckForAndCreateDirectory(LPCSTR szPath, BOOL bPromptCreate /* = FALSE */)
{
  CString csMsg, csPath = szPath;

  BOOL bIsDir = FALSE, bIsDrive = FALSE;

  if(!CheckForExistence(csPath, bIsDir, bIsDrive))
  {
    if(bIsDrive)  // this means the drive was invalid
    {
      csMsg.Format(theApp.LoadString(IDS_ERROR58), (LPCSTR)csPath);

      MyMessageBox(csMsg, MB_OK | MB_ICONHAND | MB_SETFOREGROUND);
      return FALSE;
    }

    // here's where I create it
  }
  else if(!bIsDrive && !bIsDir)  // it's NOT a directory or a drive
  {
    MyMessageBox((IDS_ERROR59), MB_OK | MB_ICONHAND | MB_SETFOREGROUND);
    return FALSE;
  }
  else
  {
    return TRUE;  // everything's ok at this point, and it exists, and it's a directory/drive
  }

  csMsg.Format(theApp.LoadString(IDS_ERROR60), (LPCSTR)csPath);

  if(bPromptCreate &&
     MyMessageBox(csMsg, MB_OKCANCEL | MB_ICONQUESTION | MB_SETFOREGROUND)
     != IDOK)
  {
    return FALSE;  // user presses 'cancel'
  }


  // OK, time to recursively create a directory.

  CString csTemp;
  int iPos, i1;

  if(csPath.GetLength() > 2 &&
     csPath[0] == '\\' && csPath[1] == '\\')
  {
    // network paths are treated differently

    for(iPos=2; iPos < csPath.GetLength(); iPos++)
    {
      if(csPath[iPos] == '\\')
      {
        iPos++;  // point past the '\'
        break;
      }
    }

    // 'iPos' points to start of share name or end of string

    for(; iPos < csPath.GetLength(); iPos++)
    {
      if(csPath[iPos] == '\\')
      {
        iPos++;  // point past the '\' again
        break;
      }
    }

    // 'iPos' points to the start of the first directory or end of string
  }
  else if(csPath.GetLength() > 3 &&
          toupper(csPath[0]) >= 'A' &&
          toupper(csPath[0]) <= 'Z' &&
          csPath[1] == ':' &&
          csPath[2] == '\\')
  {
    // file begins with root directory (which I don't want to create)

    iPos = 3;  // point just past the '\' in the root dir
  }
  else
  {
    iPos = 0;  // start from the beginning
  }

  while(iPos < csPath.GetLength())
  {
    for(i1=iPos; i1 < csPath.GetLength(); i1++)
    {
      if(csPath[i1] == '\\')
        break;
    }

    csTemp = csPath.Left(i1);  // not including backslash

    if(i1 == iPos)
    {
      // if at end of string it's not an error

      if(i1 == (csPath.GetLength() - 1))
      {
        csPath = csTemp;  // trims the extraneous backslash
        break;
      }
      else
      {
        csMsg.Format(theApp.LoadString(IDS_ERROR61), (LPCSTR)csTemp);

        MyMessageBox(csMsg, MB_OK | MB_ICONHAND | MB_SETFOREGROUND);
        return FALSE;
      }
    }
    else if(!IsValidDirectory(csTemp))  // verify first
    {
      if(!CreateDirectory(csTemp, NULL))  // default security descriptor
      {
        csMsg.Format(theApp.LoadString(IDS_ERROR62), (LPCSTR)csTemp);

        MyMessageBox(csMsg, MB_OK | MB_ICONHAND | MB_SETFOREGROUND);
        return FALSE;
      }
    }

    if(i1 < csPath.GetLength())
      i1++;

    iPos = i1;  // points to end of string or next section
  }

  return(IsValidDirectory(csPath));
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


HRESULT CreateLink(LPCSTR lpszPathObj, LPCSTR lpszPathLink, 
                   LPCSTR lpszDesc, LPCSTR lpszPath /*= NULL*/,
                   LPCSTR lpszArgs /*= NULL*/, UINT uiIcon /* = (UINT)-1*/,
                   LPCSTR lpszIconFile /* = NULL */, int iCmdShow /* = SW_SHOWNORMAL */)
{
  // adapted to C++ from Microsoft 'C' SAMPLE CODE 
  // included in MSVC 4.0 help file (see 'IShellLink' docs)

  HRESULT hres; 
  IShellLink* psl; 

  // Get a pointer to the IShellLink interface. 
  hres = CoCreateInstance(CLSID_ShellLink, NULL, 
                          CLSCTX_INPROC_SERVER, 
                          IID_IShellLink, (LPVOID *)&psl); 

  TRACE("CoCreateInstance returns %08xH\r\n", (DWORD)hres);

  if(SUCCEEDED(hres))
  { 
    IPersistFile* ppf; 

    // Set the path to the shortcut target, and add the 
    // description. 
    psl->SetPath(lpszPathObj); 
    psl->SetDescription(lpszDesc); 

    psl->SetShowCmd(iCmdShow);  // ignore error (if any)


    // next the "start directory" (or 'working directory')

    CString csStartDir = lpszPath ? lpszPath : "";
    
    if(!csStartDir.GetLength())
    {
      // is this file a directory name?  if it is, don't assign a working directory,
      // since the shell will automatically open up correctly with just the dir name.

      DWORD dwAttr = GetFileAttributes(lpszPathObj);
      if(dwAttr == (DWORD)-1 ||
         (dwAttr & FILE_ATTRIBUTE_DIRECTORY))
      {
        csStartDir = "";
      }
      else
      {
        csStartDir = lpszPathObj;
        int iBS = csStartDir.ReverseFind('\\');

        if(iBS > 2)  // i.e. not "D:\"
          csStartDir = csStartDir.Left(iBS);  // no trailing backslash
        else
          csStartDir = csStartDir.Left(iBS + 1);  // include backslash for this one
      }
    }

    if(csStartDir.GetLength())  // oh, boy, I've got a directory!
    {
      psl->SetWorkingDirectory(csStartDir);
    }

    if(lpszArgs && *lpszArgs)
    {
      psl->SetArguments(lpszArgs);
    }
    else
    {
      psl->SetArguments("");
    }

    if(uiIcon != (UINT)-1 || lpszIconFile)
    {
      if(!lpszIconFile)
        lpszIconFile = lpszPathObj;

      if(uiIcon == (UINT)-1)
        uiIcon = 0;

      psl->SetIconLocation(lpszIconFile, (int)uiIcon);
    }


    // Query IShellLink for the IPersistFile interface for saving the 
    // shortcut in persistent storage. 
    hres = psl->QueryInterface(IID_IPersistFile, (LPVOID *)&ppf);

    if(SUCCEEDED(hres))
    { 
      WORD wsz[MAX_PATH]; 

      CString csPathLink = lpszPathLink;
      
      int i1 = csPathLink.ReverseFind('.');
      int i2 = csPathLink.ReverseFind('\\');

      if(i2 >= i1 || i1 < 0)  // no extension
      {
        csPathLink += ".LNK";  // the *DEFAULT* extension (important!)
      }
      else if(csPathLink.Mid(i1).CompareNoCase(".LNK") &&
              csPathLink.Mid(i1).CompareNoCase(".PIF"))
      {
        // neither a LNK or a PIF be

        csPathLink += ".LNK";  // force it in THIS case (a bug fix for XP)
      }

      if(i2 > 0)  // a directory is present and it's not the root directory
      {
        CheckForAndCreateDirectory(csPathLink.Left(i2), FALSE);
      }

      // Ensure that the string is ANSI. 
      MultiByteToWideChar(CP_ACP, 0, (LPSTR)(LPCSTR)csPathLink, -1, 
                          (LPWSTR)wsz, MAX_PATH); 

      // Save the link by calling IPersistFile::Save. 
      hres = ppf->Save((LPCOLESTR)wsz, TRUE); 
      ppf->Release(); 
    }
    else
    {
      MyMessageBox((IDS_WARNING12), MB_OK | MB_ICONHAND | MB_SYSTEMMODAL | MB_SETFOREGROUND);
    }

    psl->Release(); 
  }
  else
  {
    MyMessageBox((IDS_WARNING13), MB_OK | MB_ICONHAND | MB_SYSTEMMODAL | MB_SETFOREGROUND);
  }

  return(hres);
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


      } // end of giant 'switch' block
    }

    csRval += c1;
  }

  return(csRval);
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

BOOL MyCopyRegistryKey(HKEY hkeySource, HKEY hkeyDest)
{
  // copies all keys and subkeys fropm one key to another key
  // this eliminates certain setup/upgrade issues

  // enumerate values, copying them into 'hkeyDest'

  CString csTemp;

  char szNameBuf[MAX_PATH * 4 + 4];

  LPSTR lp1 = NULL;
  DWORD dw1, cb1, cb2, dwType;
  
  dw1 = 0;
  cb1 = 0;
  cb2 = sizeof(szNameBuf);

  // copy all of the values, first

  while(RegEnumValue(hkeySource, dw1++,
                     (char *)memset(szNameBuf, 0, cb2 = sizeof(szNameBuf)),  // returns 'szNameBuf'
                     &cb2, NULL, NULL, NULL, NULL)
        == ERROR_SUCCESS)
  {
    szNameBuf[cb2] = 0;

    cb1 = 0;
    if(RegQueryValueEx(hkeySource, szNameBuf, NULL, &dwType, NULL, &cb1)
       != ERROR_SUCCESS)
    {
      return FALSE;
    }

    lp1 = csTemp.GetBufferSetLength(++cb1);

    if(!lp1)
    {
      return FALSE;
    }

    if(RegQueryValueEx(hkeySource, szNameBuf, NULL, &dwType, (BYTE *)lp1, &cb1)
       != ERROR_SUCCESS)
    {
      return FALSE;
    }

    // write the value to the destination

    if(RegSetValueEx(hkeyDest, szNameBuf, 0, dwType, (BYTE *)lp1, cb1)
       != ERROR_SUCCESS)
    {
      return FALSE;
    }
  }

  // NOW look at sub-keys, and recurse them

  CStringArray acsKeys;

  memset(szNameBuf, 0, sizeof(szNameBuf));

  for(dw1=0; RegEnumKey(hkeySource, dw1, szNameBuf, sizeof(szNameBuf))
             == ERROR_SUCCESS; dw1++)
  {
    acsKeys.Add(szNameBuf);
    memset(szNameBuf, 0, sizeof(szNameBuf));
  }

  csTemp.ReleaseBuffer(0);

  int i1;
  for(i1=0; i1 < acsKeys.GetSize(); i1++)
  {
    HKEY hkeySrc2 = NULL, hkeyDest2 = NULL;

    if(RegOpenKeyEx(hkeySource, acsKeys[i1],
                    0, KEY_POWERUSER_ACCESS,
                    &hkeySrc2)
       != ERROR_SUCCESS)
    {
      return FALSE;
    }

    if(RegCreateKeyEx(hkeyDest, acsKeys[i1],
                      0, NULL, 0, KEY_POWERUSER_ACCESS, NULL,
                      &hkeyDest2, NULL)
       != ERROR_SUCCESS)
    {
      RegCloseKey(hkeySrc2);

      return FALSE;
    }

    if(!MyCopyRegistryKey(hkeySrc2, hkeyDest2))
    {
      RegCloseKey(hkeySrc2);
      RegCloseKey(hkeyDest2);

      return FALSE;
    }

    RegCloseKey(hkeySrc2);
    RegCloseKey(hkeyDest2);
  }

  return TRUE;
}

BOOL EnumRegistryKeys(HKEY hkeyRoot, LPCSTR szKeyName, CStringArray &acsKeys)
{
  acsKeys.RemoveAll();

  HKEY hKey;

  if(RegOpenKeyEx(hkeyRoot, szKeyName, 0, KEY_READ, &hKey)
     != ERROR_SUCCESS)
  {
    return FALSE;
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

  return TRUE;
}

// delete all keys within the specified key ('95 default)

BOOL MyDeleteRegistryKey(HKEY hkeyRoot, LPCSTR szKeyName)
{
  CStringArray acsKeys;

  if(hkeyRoot == HKEY_CLASSES_ROOT ||
     hkeyRoot == HKEY_LOCAL_MACHINE ||
     hkeyRoot == HKEY_CURRENT_USER ||
     hkeyRoot == HKEY_CURRENT_CONFIG)
  {
    return FALSE;
  }

  if(!EnumRegistryKeys(hkeyRoot, szKeyName, acsKeys))
  {
    return FALSE;
  }

  int i1;
  for(i1=0; i1 < acsKeys.GetSize(); i1++)
  {
    if(!MyDeleteRegistryKey(hkeyRoot, (CString)szKeyName + "\\" + acsKeys[i1]))
      return FALSE;
  }

  return(RegDeleteKey(hkeyRoot, szKeyName) == ERROR_SUCCESS);
}

// delete key ONLY if no sub-keys (NT default)

BOOL MyDeleteRegistryKey0(HKEY hkeyRoot, LPCSTR szKeyName)
{
  CStringArray acsKeys;

  if(hkeyRoot == HKEY_CLASSES_ROOT ||
     hkeyRoot == HKEY_LOCAL_MACHINE ||
     hkeyRoot == HKEY_CURRENT_USER ||
     hkeyRoot == HKEY_CURRENT_CONFIG)
  {
    return FALSE;
  }

  if(!EnumRegistryKeys(hkeyRoot, szKeyName, acsKeys) ||
     acsKeys.GetSize())
  {
    return FALSE;
  }

  return(RegDeleteKey(hkeyRoot, szKeyName) == ERROR_SUCCESS);
}

BOOL MoveApplication(LPCSTR szOldApp, LPCSTR szNewApp)
{
  if(!_stricmp(szOldApp, szNewApp))
    return TRUE;  // ignore it

  if(!szOldApp || !*szOldApp || !szNewApp || !*szNewApp)
    return FALSE;

  HKEY hKeySETUP = NULL;
  CString csMsg, csTemp;  // I cheated.  Yeah

  if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                  AdjustPathName2(szSETUP_KEY), // "SOFTWARE\\Stewart~Frazier Tools\\SETUP"
                  0, KEY_POWERUSER_ACCESS,
                  &hKeySETUP) != ERROR_SUCCESS)
  {
    return FALSE;
  }
  else
  {
    HKEY hkeySource = NULL, hkeyDest = NULL;

    if(RegOpenKeyEx(hKeySETUP,
                    szOldApp,
                    0, KEY_POWERUSER_ACCESS,
                    &hkeySource)
       != ERROR_SUCCESS)
    {
      RegCloseKey(hKeySETUP);
      return FALSE;
    }

    if(RegCreateKeyEx(hKeySETUP, szNewApp,
                      0, NULL, 0, KEY_POWERUSER_ACCESS, NULL,
                      &hkeyDest, NULL)
       != ERROR_SUCCESS ||
       !MyCopyRegistryKey(hkeySource, hkeyDest))
    {
      if(hkeyDest)
        RegCloseKey(hkeyDest);

      RegCloseKey(hkeySource);
      RegCloseKey(hKeySETUP);

      if(MyMessageBox((IDS_ERROR45), MB_OKCANCEL | MB_ICONHAND | MB_SETFOREGROUND)
         != IDOK)
      {
        // delete the 'szNewApp' key - don't even want it around

        MyDeleteRegistryKey(hKeySETUP, szNewApp);

        return FALSE;
      }

      return TRUE;
    }

    RegCloseKey(hkeyDest);
    hkeyDest = NULL;  // not needed now

    RegCloseKey(hkeySource);
    hkeySource = NULL;
  }

  MyDeleteRegistryKey(hKeySETUP, szOldApp);  // byby to OLD key - NEW key is now "it"
                                             // (I don't bail out anywhere else below)

  // copy the UNINST key info also

  HKEY hkeyUSrc = NULL, hkeyUDest = NULL, hkeyUninst = NULL;

  if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                  AdjustPathName2(szUNINST_KEY), // "SOFTWARE\\Stewart~Frazier Tools\\UNINST"
                  0, KEY_POWERUSER_ACCESS,
                  &hkeyUninst) == ERROR_SUCCESS)
  {
    BOOL bUninstallMoveOK = FALSE;

    if(RegOpenKeyEx(hkeyUninst, szOldApp,
                    0, KEY_POWERUSER_ACCESS,
                    &hkeyUSrc)
       == ERROR_SUCCESS)
    {
      if(RegCreateKeyEx(hkeyUninst, szNewApp,
                        0, NULL, 0, KEY_POWERUSER_ACCESS, NULL,
                        &hkeyUDest, NULL)
         != ERROR_SUCCESS ||
         !MyCopyRegistryKey(hkeyUSrc, hkeyUDest))
      {
        csMsg.Format(theApp.LoadString(IDS_ERROR80), (LPCSTR)szOldApp, (LPCSTR)szOldApp,
                     (LPCSTR)szNewApp, (LPCSTR)szNewApp);

        MyMessageBox(csMsg, MB_OK | MB_ICONEXCLAMATION | MB_SETFOREGROUND);

        RegCloseKey(hkeyUSrc);
        hkeyUSrc = NULL;

        if(hkeyUDest)
        {
          RegCloseKey(hkeyUDest);
          hkeyUDest = NULL;

          MyDeleteRegistryKey(hkeyUninst, szNewApp);  // make sure it's gone
        }
      }
      else
      {
        RegCloseKey(hkeyUSrc);
        hkeyUSrc = NULL;

        MyDeleteRegistryKey(hkeyUninst, szOldApp);  // byby to OLD key

        // and I flow through to the NEXT section
      }

      if(hkeyUDest)
      {

        // TODO:  anything else to 'hkeyUDest' ?


        RegCloseKey(hkeyUDest);
        hkeyUDest = NULL;

        bUninstallMoveOK = TRUE;  // so I know about it later
      }
    }

    RegCloseKey(hkeyUninst);
    hkeyUninst = NULL;  // I'll re-use this in a bit


    if(bUninstallMoveOK)
    {
      // NOW I must delete the *OLD* uninstall information and
      // rename the uninstall directory

      csTemp = theApp.m_csAppPath;
      if(csTemp.GetLength())
      {
        if(csTemp[csTemp.GetLength() - 1] != '\\')
          csTemp += '\\';

        csTemp += "UNINST\\";
        MoveFile(csTemp + szOldApp, csTemp + szNewApp);
          // TODO:  check result??

        // modify the 'SETUP.INF' file to reflect the *NEW* name...

        WritePrivateProfileString("SetupOptions","AppName", szNewApp,
                                  csTemp + (CString)szNewApp + "\\SETUP.INF");
      }

      // finally, remove and re-add the 'uninstall information' key

      if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                      (CString)szHKLM_WINDOWS_CURRENT_VERSION // "SOFTWARE\\Microsoft\\Windows\\CurrentVersion"
                      + "\\Uninstall",
                      0, KEY_POWERUSER_ACCESS,
                      &hkeyUninst)
         == ERROR_SUCCESS)
      {
        MyDeleteRegistryKey(hkeyUninst, szOldApp);  // go away!

        if(csTemp.GetLength())
        {
          csTemp += (CString)szNewApp + "\\UNINST.EXE";

          if(RegCreateKeyEx(hkeyUninst, szNewApp,
                            0, NULL, 0, KEY_POWERUSER_ACCESS, NULL,
                            &hkeyUDest, NULL)
             == ERROR_SUCCESS)
          {
            RegSetValueEx(hkeyUDest, "", 0, REG_SZ, (const BYTE *)"", 1);
            RegSetValueEx(hkeyUDest, "DisplayName", 0, REG_SZ,
                          (const BYTE *)szNewApp, strlen(szNewApp) + 1);

            RegSetValueEx(hkeyUDest, "UninstallString", 0, REG_SZ,
                          (const BYTE *)(LPCSTR)csTemp,
                          csTemp.GetLength() + 1);

            RegCloseKey(hkeyUDest);
          }
        }

        RegCloseKey(hkeyUninst);
      }
    }

    // get the start menu path so I can re-name the uninstaller

    csTemp = GetRegistryString(HKEY_LOCAL_MACHINE,
                               szSETUP_KEY
                               + AppNameToRegKey(szNewApp),
                               "StartMenuFolderName");

    if(csTemp.GetLength())
    {
      csTemp = AdjustPathName(csTemp);

      DeleteFile(csTemp + "Un-Install.LNK");  // just in case (make it go away, always)

      MoveFile(csTemp + "Un-Install " + AppNameToPathName(szOldApp) + ".LNK",
               csTemp + "Un-Install " + AppNameToPathName(szNewApp) + ".LNK");
    }
  }

  RegCloseKey(hKeySETUP);

  return TRUE;
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

  if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                  "SOFTWARE\\Microsoft\\.NetFramework",
                  0, KEY_READ,
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

BOOL ShellExecApp(LPCSTR szApp, LPCSTR szParms, UINT nCmdShow /* = SW_SHOWNORMAL */)
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
    csMsg.Format(theApp.LoadString(IDS_ERROR07), szApp);

    MyMessageBox(csMsg, MB_OK | MB_ICONHAND | MB_SETFOREGROUND);

    return FALSE;  // this is really a FATAL error...
  }
  else
  {
    WaitForSingleObject(sei.hProcess, INFINITE);

    DWORD dwExitCode;
    if(!GetExitCodeProcess(sei.hProcess, &dwExitCode))
    {
      csMsg.Format(theApp.LoadString(IDS_WARNING07), szApp);

      MyMessageBox(csMsg, MB_OK | MB_ICONASTERISK | MB_SETFOREGROUND);

      bRval = FALSE;
    }
    else if(dwExitCode == STILL_ACTIVE)
    {
      csMsg.Format(theApp.LoadString(IDS_WARNING08), szApp);

      MyMessageBox(csMsg, MB_OK | MB_ICONASTERISK | MB_SETFOREGROUND);

      bRval = FALSE;
    }
    else if(dwExitCode)
    {
      csMsg.Format(theApp.LoadString(IDS_WARNING09), szApp, dwExitCode);

      MyMessageBox(csMsg, MB_OK | MB_ICONASTERISK | MB_SETFOREGROUND);

      bRval = FALSE;
    }
  }

  return(bRval);
}

int CSETUPApp::DoMessageBox(LPCTSTR lpszPrompt, UINT nType, UINT nIDPrompt)
{
  HWND hwndParent = m_pMyFrameWnd ? m_pMyFrameWnd->m_hWnd : NULL;

	// determine icon based on type specified
	if ((nType & MB_ICONMASK) == 0)
	{
		switch (nType & MB_TYPEMASK)
		{
		case MB_OK:
		case MB_OKCANCEL:
			nType |= MB_ICONEXCLAMATION;
			break;

		case MB_YESNO:
		case MB_YESNOCANCEL:
			nType |= MB_ICONEXCLAMATION;
			break;

		case MB_ABORTRETRYIGNORE:
		case MB_RETRYCANCEL:
			// No default icon for these types, since they are rarely used.
			// The caller should specify the icon.
			break;
		}
	}

  if(m_csAppName.GetLength())
    return(::MessageBox(hwndParent, lpszPrompt, m_csAppName + " SETUP", nType));
  else
    return(::MessageBox(hwndParent, lpszPrompt, MyGetAppName()/*theApp.m_pszAppName*/, nType));
//return CWinApp::DoMessageBox(lpszPrompt, nType, nIDPrompt);
}

