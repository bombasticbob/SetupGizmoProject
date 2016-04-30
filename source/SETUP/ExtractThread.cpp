/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                         //
//   _____        _                      _   _____  _                            _                         //
//  | ____|__  __| |_  _ __  __ _   ___ | |_|_   _|| |__   _ __  ___   __ _   __| |    ___  _ __   _ __    //
//  |  _|  \ \/ /| __|| '__|/ _` | / __|| __| | |  | '_ \ | '__|/ _ \ / _` | / _` |   / __|| '_ \ | '_ \   //
//  | |___  >  < | |_ | |  | (_| || (__ | |_  | |  | | | || |  |  __/| (_| || (_| | _| (__ | |_) || |_) |  //
//  |_____|/_/\_\ \__||_|   \__,_| \___| \__| |_|  |_| |_||_|   \___| \__,_| \__,_|(_)\___|| .__/ | .__/   //
//                                                                                         |_|    |_|      //
//                                                                                                         //
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                         //
//                      Copyright (c) 1998-2016 by S.F.T. Inc. - All rights reserved                       //
//                  Use, copying, and distribution of this software are licensed according                 //
//                    to the GPLv2, LGPLv2, or BSD license, as appropriate (see COPYING)                   //
//                                                                                                         //
/////////////////////////////////////////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include <io.h>      // for things like '_open()'
#include <fcntl.h>
#include <sys\types.h>
#include <sys\stat.h>

// for cert support
#include <wincrypt.h>
//#pragma comment(lib,"crypt32.lib") // include crypto lib

#include "fdi.h"
#include "SETUP.h"
#include "ExtractThread.h"
#include "StartMenuFolderDlg.h"
#include "MainFrame.h"
#include "CabinetHelper.h"
#include "CertificateDlg.h"

//#ifdef _DEBUG
//#define new DEBUG_NEW
//#undef THIS_FILE
//static char THIS_FILE[] = __FILE__;
//#endif




BOOL SetupThreadProc();
BOOL SetupThreadAdjustUninstPath(void);
BOOL SetupThreadUpdateRegistryEntries(CStringArray &acsRegKeys);
BOOL SetupThreadDoSharedComponents(CStringArray &acsSharedDLLs);
BOOL SetupThreadDoRegisterDLL(CStringArray &acsRegisterControl,
                              CStringArray &acsRegAsmControl,
                              CStringArray &acsSharedDLLs,
                              CStringArray &acsRegkeys,
                              BOOL &bRebootPendingFlag);
BOOL SetupThreadDoExecAfterCopy(CStringArray &acsExecuteAfterCopy,
                                CStringArray &acsSharedDLLs,
                                BOOL &bRebootPendingFlag);

HCERTSTORE InternalOpenSystemCAStore(HMODULE hCrypt32, LPCWSTR lpwszStoreName);
BOOL InternalAddCert(HMODULE hCrypt32, HCERTSTORE hSysStore, CString &csCert,
                     CString & csCertName, CString &csRepository);
void InternalCloseSystemCAStore(HMODULE hCrypt32, HCERTSTORE hSysStore);

/////////////////////////////////////////////////////////////////////////////
// CExtractThread

//IMPLEMENT_DYNCREATE(CExtractThread, CWinThread)

CExtractThread::CExtractThread()
{
//  m_bAutoDelete = FALSE;  // don't auto-delete the thread
}

CExtractThread::~CExtractThread()
{
}

static BOOL SafeRunThreadProc()
{
  char tbuf[256];  // "An unknown system exception has taken place - this program will now close"
  char tbuf2[64];  // "** FATAL SETUP ERROR **"

  LoadString(theApp.m_hInstance, IDS_ERROR71, tbuf, sizeof(tbuf));
  LoadString(theApp.m_hInstance, IDS_ERROR72, tbuf2, sizeof(tbuf2));

  __try
  {
    return(SetupThreadProc());
  }
  __except(EXCEPTION_EXECUTE_HANDLER)
  {
    // TODO:  should I do this as a message string or not???

    ::MessageBox(NULL, tbuf, tbuf2,
                 MB_OK | MB_ICONHAND | MB_SETFOREGROUND);
  }

  return FALSE;
}

int CExtractThread::ExitInstance() 
{
  int iRval; //= CWinThread::ExitInstance();
  if(m_msgCur.message == WM_QUIT)
  {
    iRval = (int)m_msgCur.wParam; 
  }
  else
  {
    iRval = 0;
  }

  CoUninitialize();

  return(iRval);
}

BOOL CExtractThread::InitInstance()
{
CStringArray acsAutoDelete;

// NOTE:  do *NOT* use 'AfxOleInit()'
//
  if(CoInitialize(NULL) != S_OK) // CoInitializeEx(NULL, COINIT_APARTMENTTHREADED) != S_OK)
  {
    MyMessageBox((IDS_ERROR01), MB_OK | MB_ICONHAND | MB_SETFOREGROUND);

    return FALSE;
  }

//  m_bAutoDelete = FALSE;  // don't auto-delete the thread


  // call the main thread proc first
  // everything that's 'cleanup' (like post-delete, etc.)
  // will happen after SafeRunThreadProc returns

  BOOL bRval = SafeRunThreadProc();
  CString csMsg1, csMsg2, csMsg3;

  csMsg1 = theApp.LoadString(IDS_ERROR46);
  csMsg3.Format(theApp.LoadString(IDS_ERROR48), (LPCSTR)theApp.m_csAppName);

  if(!bRval)
  {
    csMsg2.Format(theApp.LoadString(IDS_ERROR47), (LPCSTR)theApp.m_csAppName);

    ::MessageBox(theApp.m_pMyFrameWnd ? theApp.m_pMyFrameWnd->m_hWnd : NULL,
                 csMsg2, csMsg1, MB_OK | MB_ICONHAND | MB_SETFOREGROUND | MB_TOPMOST);
  }
  else if(!theApp.m_bExtraQuietMode)
  {
    // this next part is the 'setup complete' message handler
    // and 'reboot notification'

    // for WinME, or NT 5 and later, call 'LockSetForegroundWindow' to make this work
    BOOL (WINAPI *pLockSetForegroundWindow)(UINT uLockCode) = NULL;
#ifndef LSFW_LOCK
#define LSFW_LOCK       1
#endif // LSFW_LOCK
#ifndef LSFW_UNLOCK
#define LSFW_UNLOCK     2
#endif // LSFW_UNLOCK

    if((theApp.m_bIsNT && theApp.m_dwMajorVer >= 5) ||               // Win 2k or later
       (!theApp.m_bIsNT && 
        (theApp.m_dwMajorVer > 4 ||
         theApp.m_dwMajorVer == 4 && theApp.m_dwMinorVer > 0x0a)))  // winders ME
    {
      HMODULE hLib = GetModuleHandle("USER32");

      if(hLib)
      {
        (FARPROC &)pLockSetForegroundWindow = GetProcAddress(hLib, "LockSetForegroundWindow");
      }
    }

    if(pLockSetForegroundWindow)   // prevent other applications from setting foreground window
      pLockSetForegroundWindow(LSFW_LOCK);

    if(!theApp.m_bRebootFlag)
    {
      csMsg2 = theApp.LoadString(IDS_ERROR49);

      ::MessageBox(theApp.m_pMyFrameWnd ? theApp.m_pMyFrameWnd->m_hWnd : NULL,
                   csMsg2, csMsg3, MB_OK | MB_ICONINFORMATION | MB_SETFOREGROUND | MB_TOPMOST);
    }
    else // if(theApp.m_bRebootFlag)
    {
      csMsg2.Format(theApp.LoadString(IDS_ERROR50), (LPCSTR)theApp.m_csAppName);

      ::MessageBox(theApp.m_pMyFrameWnd ? theApp.m_pMyFrameWnd->m_hWnd : NULL,
                   csMsg2, csMsg3, MB_OK | MB_ICONASTERISK | MB_SETFOREGROUND | MB_TOPMOST);
    }

    if(pLockSetForegroundWindow) // allow other applications to set foreground window
      pLockSetForegroundWindow(LSFW_UNLOCK);
  }


  // For the options that I have installed, set the correct registry key info
  // (this won't happen on error)

  if(bRval)
  {
    if(theApp.m_acsOptions.GetSize())
    {
      CString csTemp, csVal;

      int i1, i2;
      for(i1=0; i1 < theApp.m_acsOptions.GetSize(); i1++)
      {
        csTemp = "";

        for(i2=0; i2 < theApp.m_acsOptions[i1].GetLength(); i2++)
        {
          if(theApp.m_acsOptions[i1].GetAt(i2) == '"')
            csTemp += "\"\"";  // doubled quote
          else
            csTemp += theApp.m_acsOptions[i1].GetAt(i2);
        }

        if(csVal.GetLength())
          csVal += ',' + csTemp;
        else
          csVal += csTemp;
      }

      HKEY hKey = NULL;
      if(RegCreateKeyEx(HKEY_LOCAL_MACHINE,
                        szSETUP_KEY
                        + AppNameToRegKey(theApp.m_csAppName)
                        + "\\InstalledOptions",
                        0, NULL, 0, KEY_POWERUSER_ACCESS,NULL,
                        &hKey, NULL)
         != ERROR_SUCCESS)
      {
        RegSetValueEx(hKey, "", 0, REG_SZ, 
                      (const LPBYTE)(LPCSTR)csVal,
                      csVal.GetLength() + 1);

        RegCloseKey(hKey);
      }
    }
    else
    {
      HKEY hKey = NULL;
      if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                      szSETUP_KEY
                      + AppNameToRegKey(theApp.m_csAppName),
                      0, KEY_POWERUSER_ACCESS,
                      &hKey)
         == ERROR_SUCCESS)
      {
        RegDeleteKey(hKey, "InstalledOptions");

        RegCloseKey(hKey);
      }
    }
  }


  // LAST but not least - delete files that I'm supposed to delete
  // THIS WILL HAPPEN WHETHER OR NOT THE PREVIOUS FUNCTION SUCCEEDS!

  theApp.GetInfEntries("AutoDelete", acsAutoDelete);

  for(int i1=0; i1 < acsAutoDelete.GetSize(); i1++)
  {
    CString csTemp = acsAutoDelete[i1];

    int i2 = csTemp.ReverseFind('\\');
    if(i2 < 0)
    {
      csTemp = AdjustPathName(theApp.m_csAppPath) + csTemp;
    }
    else
    {
      if(!theApp.ExpandStrings(csTemp))
      {
        return FALSE; // I can bail NOW without consequence, really
      }
    }

    if(csTemp.GetLength()) // just in case
      DeleteFile(csTemp);  // don't bother checking for success...
  }

  int iRetCode = bRval ? 0 : 10;  // return 10 on error, 0 on success

  if(!theApp.m_pMyFrameWnd)
  {
    /*AfxGetApp()->*/theApp.PostThreadMessage(WM_QUIT,iRetCode,0);  // I like insurance on this
  }
  else
  {
    CMyWaitCursor wait;

    if(theApp.m_bRebootFlag)
      Sleep(1000);  // a WIN2000 hack - let window respond "do not shut me down"
    
    if(!theApp.m_pMyFrameWnd)  // it closed
    {
      /*AfxGetApp()->*/theApp.PostThreadMessage(WM_QUIT,iRetCode,0);  // I like insurance on this
    }
    else
    {
      if(iRetCode)
        /*AfxGetApp()->*/theApp.PostThreadMessage(WM_QUIT,iRetCode,0);  // I must make it quit NOW

      HWND hwndApp = theApp.m_pMyFrameWnd ? theApp.m_pMyFrameWnd->m_hWnd : NULL;
      if(hwndApp)
      {
        ::PostMessage(hwndApp, WM_CLOSE, 0, 0);

        Sleep(500);  // give main window the time it needs to go away gracefully

        if(theApp.m_pMyFrameWnd)
        {
          /*AfxGetApp()->*/theApp.PostThreadMessage(WM_QUIT,iRetCode,0);  // if window doesn't close, do this anyway
        }
      }
      else
      {
        /*AfxGetApp()->*/theApp.PostThreadMessage(WM_QUIT,iRetCode,0);
          // if there's no window I must *ALWAYS* do this!
      }
    }
  }

  return FALSE;  // always
}

CExtractThread * CExtractThread::BeginThread(void)
{
  CExtractThread* pThread = new CExtractThread();

  // this must use the base class's 'CreateThread' to make sure
  // that the TLS is correctly assigned for the thread
  if(!pThread->CreateThread(CREATE_SUSPENDED))
  {
    delete pThread;
    return NULL;
  }

  pThread->ResumeThread();
  return pThread;
}

//BEGIN_MESSAGE_MAP(CExtractThread, CWinThread)
//  //{{AFX_MSG_MAP(CExtractThread)
//    // NOTE - the ClassWizard will add and remove mapping macros here.
//  //}}AFX_MSG_MAP
//END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExtractThread message handlers

// THE MAIN PROC

BOOL SetupThreadProc()
{
CString csTemp, csTemp2, csTemp3, csMsg, csParm, csUninstKey;
CStringArray acsRegKeys, acsSharedDLLs, acsRegisterControl,
             acsRegAsmControl, acsExecuteAfterCopy;
BOOL bRebootPendingFlag = FALSE, bRval = TRUE;
HKEY hKey = NULL, hkeyRoot = NULL, hkeyUninst = NULL, hkeySetup = NULL;
HCERTSTORE hSysStore = NULL;
int i1, i2, i3;


  // ** FILE COPIES **

  // copy SETUP.INF and UNINST.EXE to the '%apppath%\\UNINST' directory


  // NOTE:  if I'm "self-extracting", I don't need to do UNINST.EXE

  if(!theApp.m_bNoUninstall)  // do NOT do this if the creator said "no uninstall"
  {
    if(!SetupThreadAdjustUninstPath()) // adjust path as required
    {
#ifdef _DEBUG
      MyMessageBox("Unable to adjust uninst path (continuing)");
#else // _DEBUG
      return FALSE;
#endif // _DEBUG
    }
  }

  // COPYING FILES FROM THE CABINET [a one-liner]

  if(!CopyFilesFromCabinet(theApp.m_acsFiles))
  {
#ifdef _DEBUG
    MyMessageBox("Unable to copy files from cabinet (continuing)");
#else // _DEBUG
    return FALSE;
#endif // _DEBUG
  }

  theApp.SetProgress(100);
  theApp.SetProgressText("COPIES COMPLETE!!");

  Sleep(500);  // let people SEE it


  // NEXT, do the registry entries

  theApp.GetInfEntries("RegistryKeys", acsRegKeys);

  theApp.SetProgressText("UPDATING REGISTRY ENTRIES");

  // ** ADD REGISTRY ENTRIES **

  if(!SetupThreadUpdateRegistryEntries(acsRegKeys))
  {
#ifdef _DEBUG
    MyMessageBox("Unable to update registry entries (continuing)");
#else // _DEBUG
    return FALSE;  // what I do when there's an error
#endif // _DEBUG
  }

  if(acsRegKeys.GetSize())
  {
    theApp.SetProgress(50);
  }
  else
  {
    theApp.SetProgress(0);
  }

  Sleep(250);  // let 'em see it (again) like we did something


  // AUTOMATIC ENTRIES
  //
  // for each DLL and/or file marked 'shared/system'
  //   REG KEY: HKLM\SOFTWARE\Microsoft\Windows\Current Version\SharedDLLs
  //     value: {path}\FileName.DLL  (DWORD){incremented value}
  //
  // NOTE:  the following keys are only added ONCE after the first successful install
  //
  //   REG KEY: HKLM\SOFTWARE\{CompanyName}\{AppName}\UNINST
  //     value: {default}       "%apppath%\UNINST\UNINST.EXE"
  //          : {path}\FileName.DLL   (DWORD){previous incremented value from above}
  //   (original file will become 'FileName.DL#' if replaced by SETUP)
  //   (uninstall must prompt user "do you want to keep" and delete the old if NO)
  //
  // 'Uninstall'
  //
  //   REG KEY: HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\{app}
  //     value: {default}       ""
  //          : DisplayName     "{application name}"
  //          : UninstallString "%apppath%\UNINST\{AppName}\UNINST.EXE"
  //


  // get a list of shared DLL's

  theApp.GetInfEntries("SharedComponents", acsSharedDLLs);

  // installation for shared components

  if(!SetupThreadDoSharedComponents(acsSharedDLLs))
  {
#ifdef _DEBUG
    MyMessageBox("Unable to setup shared components (continuing)");
#else // _DEBUG
    return FALSE;  // what I do when there's an error
#endif // _DEBUG
  }

  if(acsSharedDLLs.GetSize())
  {
    if(acsRegKeys.GetSize())
      theApp.SetProgress(70);  // 70% at this point (estimate)
    else
      theApp.SetProgress(50);  // 70% at this point (estimate)

    Sleep(250);  // let 'em see it (again) like we did something
  }


  // *************************
  // INSTALLING CERTIFICATES *
  // *************************

  // install any certificates NOW, before I run or register
  // anything that might require these for signing, etc.

  i1 = theApp.GetCertFileCount();

  if(i1 > 0) // at least one cert file
  {
    HMODULE hCrypt32 = LoadLibrary("crypt32");

    if(!hCrypt32)
    {
      MyMessageBox("WARNING:  unable to install root certificates - no 'crypt32.dll'");
    }
    else
    {
      CCertificateDlg dlg;
      if(dlg.DoModal() != IDOK)
      {
        MyMessageBox("Certificates will not be installed.  The application may not function correctly as a result.",
                      MB_OK | MB_ICONEXCLAMATION | MB_SETFOREGROUND);
      }
      else
      {
        for(i2=0; i2 < i1; i2++)
        {
          // TODO:  I also need to be able to put it into 'Trusted Publishers'
          //        stored in: HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\SystemCertificates\TrustedPublisher
          //        and possibly 'Trusted Devices'
          //        stored in: HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\SystemCertificates\TrustedDevices

          csTemp2.Format("certs\\cert%03d.cer", i2);
          csTemp3 = theApp.GetInfEntry("CertFiles", csTemp2); // get name,repository

          // csTemp3 contains:  "filename.cer","friendly name",repository

          // TODO: handle a quoted repository name.  for now assume ',' separates
          //       and embedded white space is valid, no embedded ','s

          i3 = csTemp3.ReverseFind(',');

          if(i3 < 0)
          {
            csTemp = "ROOT"; // default repository
          }
          else
          {
            csTemp = csTemp3.Mid(i3 + 1);
            csTemp.TrimRight();
            csTemp.TrimLeft();

            csTemp3 = csTemp3.Left(i3);
            csTemp3.TrimRight(); // mostly for debugging

            i3 = csTemp3.ReverseFind(','); // again
            if(i3 >= 0)
            {
              csTemp3 = csTemp3.Mid(i3 + 1);
              csTemp3.TrimLeft();
            }
          }

          csParm = ExtractFileFromCabAsString(1, csTemp2); // always cab 1

          if(!csParm.GetLength())
          {
            DWORD dwErr = GetLastError();
            csTemp.Format("ERROR:  unable to read \"%s\", error %d (%08xH)",
                          (LPCSTR)csTemp2, dwErr, dwErr);

            MyMessageBox(csTemp);
            // error exit?

            bRval = FALSE; // flag the error
          }
          else
          {
            // cheat and use the BSTR conversion
            BSTR bstrTemp = csTemp.AllocSysString();

            if(!bstrTemp)
            {
              DWORD dwErr = GetLastError();
              csTemp2.Format("ERROR INSTALLING CERTIFICATE %s:  unable to create BSTR for \"%s\", error %d (%08xH)",
                             (LPCSTR)csTemp3, (LPCSTR)csTemp, dwErr, dwErr);

              MyMessageBox(csTemp2);
              bRval = FALSE; // flag the error
            }
            else
            {
              hSysStore = InternalOpenSystemCAStore(hCrypt32, bstrTemp);

              SysFreeString(bstrTemp);

              if(hSysStore)
              {
                // csTemp3 is the cert name; csTemp is the repository name
                // these are for info purposes, as the next function may prompt
                if(!InternalAddCert(hCrypt32, hSysStore, csParm, csTemp3, csTemp))
                {
                  // error exit? - note, this does it's own message box
                  bRval = FALSE; // at least do THIS
                }

                InternalCloseSystemCAStore(hCrypt32, hSysStore);
              }
              else
              {
                DWORD dwErr = GetLastError();
                csTemp2.Format("ERROR INSTALLING CERTIFICATE %s:  unable to open \"%s\" cert system store, error %d (%08xH)",
                               (LPCSTR)csTemp3, (LPCSTR)csTemp, dwErr, dwErr);

                MyMessageBox(csTemp2);

                // error exit?

                bRval = FALSE; // flag the error
              }
            }
          }
        }

        FreeLibrary(hCrypt32);
      }
    }
  }


  // NEXT, register all of the 'OCX's and OLE DLL's
  // TODO:  do I want to DEFER registering these if they're shared & I'm rebooting???

  theApp.SetProgressText("REGISTERING DLL's...");

  theApp.GetInfEntries("RegisterControl", acsRegisterControl);
  theApp.GetInfEntries("RegAsmControl", acsRegAsmControl);

  bRebootPendingFlag = theApp.IsRebootPending();

  if(!SetupThreadDoRegisterDLL(acsRegisterControl, acsRegAsmControl,
                               acsSharedDLLs, acsRegKeys,
                               bRebootPendingFlag))
  {
#ifdef _DEBUG
    MyMessageBox("Unable to register DLLs (continuing)");
#else // _DEBUG
    return FALSE;
#endif // _DEBUG
  }

  if(acsRegisterControl.GetSize())
  {
    theApp.SetProgress(100);
    theApp.SetProgressText("REGISTERING DLL's... COMPLETE!");
    Sleep(250);  // let people SEE it
  }


  // NOW, run EACH application that I must run...

  theApp.SetProgressText("EXECUTING PROGRAMS...");

  theApp.GetInfEntries("ExecuteAfterCopy", acsExecuteAfterCopy);

  if(!SetupThreadDoExecAfterCopy(acsExecuteAfterCopy, acsSharedDLLs,
                                 bRebootPendingFlag))
  {
#ifdef _DEBUG
    MyMessageBox("Unable to do exec after copy (continuing)");
#else // _DEBUG
    return FALSE;
#endif // _DEBUG
  }

  if(acsExecuteAfterCopy.GetSize())
  {
    theApp.SetProgress(100);
    theApp.SetProgressText("EXECUTING PROGRAMS... COMPLETE!");
    Sleep(250);  // let people SEE it
  }





  // ************************************
  // REGISTERING THE UNINSTALL PROGRAM!!!
  // ************************************

  // NOTE: this assigns non-NULL to hkeyUninst, which is
  //       checked for later...

  if(!theApp.m_bNoUninstall)
  {
    csUninstKey = (CString)szHKLM_WINDOWS_CURRENT_VERSION // "SOFTWARE\\Microsoft\\Windows\\CurrentVersion"
                + "\\Uninstall\\"
                + AppNameToRegKey(theApp.m_csAppName);

    if(RegCreateKeyEx(HKEY_LOCAL_MACHINE,
                      csUninstKey,
                      0, NULL, 0, KEY_POWERUSER_ACCESS,NULL,
                      &hKey, NULL)
       == ERROR_SUCCESS)
    {
      RegSetValueEx(hKey, "", 0, REG_SZ, (const LPBYTE)"", 1);

      RegSetValueEx(hKey, "DisplayName", 0, REG_SZ,
                    (const LPBYTE)(LPCSTR)theApp.m_csAppName,
                    theApp.m_csAppName.GetLength() + 1);

      CString csPath = '"' + theApp.m_csUninstPath + "UNINST.EXE\"";

      RegSetValueEx(hKey, "UninstallString", 0, REG_SZ, 
                    (const LPBYTE)(LPCSTR)csPath,
                    csPath.GetLength() + 1);

      RegCloseKey(hKey);
    }

    // now tell 'UNINST' where I put everything, in case the user
    // decided to run it from the SETUP disk

    if(RegCreateKeyEx(HKEY_LOCAL_MACHINE, 
                      szUNINST_KEY // "SOFTWARE\\Stewart~Frazier Tools\\UNINST\\"
                      + AppNameToRegKey(theApp.m_csAppName),
                      0, NULL, 0, KEY_POWERUSER_ACCESS, NULL,
                      &hkeyUninst, NULL)
       == ERROR_SUCCESS)
    {
      CString csPath = AdjustPathName(theApp.m_csAppPath);

      RegSetValueEx(hkeyUninst, "", 0, REG_SZ, 
                    (const LPBYTE)(LPCSTR)csPath,
                    csPath.GetLength() + 1);

      // NOTE:  In case that 'AppCommon' files were NOT included in 
      //        the 'shareddll's' section...

      if(theApp.m_csAppCommon.GetLength() &&
         RegCreateKeyEx(hkeyUninst, "AppCommon",
                        0, NULL, 0, KEY_POWERUSER_ACCESS, NULL,
                        &hKey, NULL)
         == ERROR_SUCCESS)
      {
        csPath = AdjustPathName(theApp.m_csAppCommon);

        RegSetValueEx(hKey, "", 0, REG_SZ,
                      (const LPBYTE)(LPCSTR)csPath,
                      csPath.GetLength() + 1);

        RegCloseKey(hKey);
      }

      hKey = NULL;
    }
    else
    {
      hkeyUninst = NULL;  // make sure...
    }
  }

  // FINALLY, register the path where I set this thing up for any subsequent attempts
  // at running SETUP.  This avoids problems where users might try to upgrade, but end
  // up re-installing to a different path, causing problems.


  if(RegCreateKeyEx(HKEY_LOCAL_MACHINE, 
                    szSETUP_KEY + AppNameToRegKey(theApp.m_csAppName),
                    0, NULL, 0, KEY_POWERUSER_ACCESS,NULL,
                    &hkeySetup, NULL)
     == ERROR_SUCCESS)
  {
    CString csPath = AdjustPathName(theApp.m_csAppPath);

    RegSetValueEx(hkeySetup, "", 0, REG_SZ, 
                  (const LPBYTE)(LPCSTR)csPath,
                  csPath.GetLength() + 1);

    // NOTE:  In case that 'AppCommon' files were NOT included in 
    //        the 'shareddll's' section...

    if(theApp.m_csAppCommon.GetLength() &&
       RegCreateKeyEx(hkeySetup, "AppCommon",
                      0, NULL, 0, KEY_POWERUSER_ACCESS, NULL,
                      &hKey, NULL)
       == ERROR_SUCCESS)
    {
      csPath = AdjustPathName(theApp.m_csAppCommon);

      RegSetValueEx(hKey, "", 0, REG_SZ,
                    (const LPBYTE)(LPCSTR)csPath,
                    csPath.GetLength() + 1);

      RegCloseKey(hKey);
    }

    hKey = NULL;
  }
  else
  {
    hkeySetup = NULL;  // make sure...
  }


  theApp.SetProgress(100);
  theApp.SetProgressText("SETUP COMPLETE!!!");

  Sleep(1000);  // let 'em see it all!



  // ********************
  // ** PROGRAM FOLDER **
  // ********************


  CStartMenuFolderDlg dlg;
  dlg.DoIt();    // this takes care of EVERYTHING

  // NOTE:  this also closes hkeyUninst and hkeySetup if they're not NULL
  //        so it's a part of the 'resource cleanup' process

  if(hkeyUninst != NULL) // created in the section above
  {
    // save the start menu folder name in the registry

    if(theApp.m_csStartMenuFolderName.GetLength())
    {
      RegSetValueEx(hkeyUninst, "StartMenuFolderName", 0, REG_SZ, 
                    (const LPBYTE)(LPCSTR)theApp.m_csStartMenuFolderName,
                    theApp.m_csStartMenuFolderName.GetLength() + 1);
    }
    else
    {
      RegDeleteValue(hkeyUninst, "StartMenuFolderName");
    }

    RegCloseKey(hkeyUninst);
    hkeyUninst = NULL; // by convention
  }

  if(hkeySetup != NULL)
  {
    // and, save the start menu folder name in the registry for 'SETUP'

    if(theApp.m_csStartMenuFolderName.GetLength())
    {
      RegSetValueEx(hkeySetup, "StartMenuFolderName", 0, REG_SZ, 
                    (const LPBYTE)(LPCSTR)theApp.m_csStartMenuFolderName,
                    theApp.m_csStartMenuFolderName.GetLength() + 1);
    }
    else
    {
      RegDeleteValue(hkeySetup, "StartMenuFolderName");
    }

    RegCloseKey(hkeySetup);
    hkeySetup = NULL; // by convention
  }

  return(bRval);  // SETUP completed ok if it returns TRUE
}





// utilities called by 'SetupThread()'

BOOL SetupThreadAdjustUninstPath(void)
{
CString csTemp, csTemp2;

  // create an application-based file name for the UNINST.EXE and 'SETUP.INF'
  // files to be written to.  This allows multiple applications to be installed
  // in this directory without any trouble.

  if(!theApp.m_csUninstPath.GetLength())
  {
    theApp.m_csUninstPath = AdjustPathName(theApp.m_csAppPath)
                          + "UNINST\\"
                          + AppNameToPathName(theApp.m_csAppName)
                          + "\\";
  }

  // TODO:  if user copies this someplace else, fix it

  // make sure the path exists
  CheckForAndCreateDirectory(theApp.m_csUninstPath);

  csTemp = theApp.m_csUninstPath + "SETUP.INF";
  csTemp2 = theApp.GetPathAndPromptForDiskette(1) + "SETUP.INF";  // always on disk #1

  SetFileAttributes(csTemp, 0);  // make sure the "read only" flag is cleared
  if(!CopyFile(csTemp2, csTemp, FALSE))
  {
    MyMessageBox((IDS_ERROR02),
                  MB_OK | MB_ICONHAND | MB_SETFOREGROUND);

    return FALSE;
  }

  if(!theApp.m_bSelfExtract)
  {
    csTemp = theApp.m_csUninstPath + "UNINST.EXE";
    csTemp2 = theApp.GetPathAndPromptForDiskette(1) + "UNINST.EXE";  // always on disk #1

    SetFileAttributes(csTemp, 0);  // make sure the "read only" flag is cleared
    if(!CopyFile(csTemp2, csTemp, FALSE))
    {
#ifndef _DEBUG
      MyMessageBox(IDS_ERROR03,
                    MB_OK | MB_ICONHAND | MB_SETFOREGROUND);

      return FALSE;
#else // _DEBUG
      MyMessageBox("WARNING:  copy error on 'UNINST.EXE' - did you intend it to be self-extracting?",
                    MB_OK | MB_ICONASTERISK | MB_SETFOREGROUND);
#endif // _DEBUG
    }
  }

  return TRUE;
}

BOOL SetupThreadUpdateRegistryEntries(CStringArray &acsRegKeys)
{
BOOL bRval = TRUE; // success
CString csTemp, csTemp2, csMsg, csKey, csString, csValue;
int i1, i2, i3;
HKEY hKey, hkeyRoot;


  for(i1=0; i1 < acsRegKeys.GetSize(); i1++)
  {
    theApp.SetProgress(50 * i1 / acsRegKeys.GetSize());

    // parse the entry (very strict adherence to 'the rules'

    csTemp = acsRegKeys[i1], csKey, csString, csValue;

    if(!csTemp.GetLength())
      continue;

    // find the first '=' (no quotes, either)
    i2 = csTemp.Find('=');
    if(i2 <= 0)
    {
      csMsg.Format(theApp.LoadString(IDS_ERROR04), (LPCSTR)csTemp);

      MyMessageBox(csMsg, MB_OK | MB_ICONASTERISK | MB_SETFOREGROUND);

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
      csMsg.Format(theApp.LoadString(IDS_WARNING01), (LPCSTR)acsRegKeys[i1]);

      MyMessageBox(csMsg, MB_OK | MB_ICONASTERISK | MB_SETFOREGROUND);

      continue;
    }


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
      hkeyRoot = HKEY_LOCAL_MACHINE;
      i2 = csKey.Find('\\');
      csKey = csKey.Mid(i2 + 1);
    }
    else if(KEYCHECK(szHKCU) || KEYCHECK(szHKEY_CURRENT_USER))
    {
      hkeyRoot = HKEY_LOCAL_MACHINE;
      i2 = csKey.Find('\\');
      csKey = csKey.Mid(i2 + 1);
    }
    else if(KEYCHECK(szHKU) || KEYCHECK(szHKEY_USERS))
    {
      hkeyRoot = HKEY_USERS;
      i2 = csKey.Find('\\');
      csKey = csKey.Mid(i2 + 1);
    }
    else if(KEYCHECK(szHKCR) || KEYCHECK(szHKEY_CLASSES_ROOT))
    {
      hkeyRoot = HKEY_CLASSES_ROOT;
      i2 = csKey.Find('\\');
      csKey = csKey.Mid(i2 + 1);
    }
    else
    {
      csMsg.Format(theApp.LoadString(IDS_WARNING02), (LPCSTR)csKey);

      MyMessageBox(csMsg, MB_OK | MB_ICONASTERISK | MB_SETFOREGROUND);
      hkeyRoot = HKEY_CLASSES_ROOT;
    }

#undef KEYCHECK

    // open/create the key

    if(csValue.GetLength())
    {
      if(RegCreateKeyEx(hkeyRoot, csKey,
                        0, NULL, 0, KEY_POWERUSER_ACCESS,NULL,
                        &hKey, NULL)
         == ERROR_SUCCESS)
      {
        RegSetValueEx(hKey, csString, 0, REG_SZ, 
                      (const LPBYTE)(LPCSTR)csValue,
                      csValue.GetLength() + 1);

        RegCloseKey(hKey);
      }
      else
      {
        csMsg.Format(theApp.LoadString(IDS_ERROR05), (LPCSTR)csKey);

        MyMessageBox(csMsg, MB_OK | MB_ICONHAND | MB_SETFOREGROUND);

        return FALSE;  // fail it NOW
      }
    }
    else if(csString.GetLength())
    {
      if(RegOpenKeyEx(hkeyRoot, csKey,
                      0, KEY_POWERUSER_ACCESS,
                      &hKey)
         == ERROR_SUCCESS)
      {
        // NOTE:  ignore errors here...

        RegDeleteValue(hKey, csString);

        RegCloseKey(hKey);
      }
    }
    else
    {
      RegDeleteKey(hkeyRoot, csKey);

      // for now, ignore errors...
    }
  }
  
  return bRval; // reserved, return FALSE if error
}

BOOL SetupThreadDoSharedComponents(CStringArray &acsSharedDLLs)
{
BOOL bRval = TRUE;
HKEY hkeyRoot, hKey, hkeyUninst;
int i1, i2;
DWORD dwType, dwVal, cbVal;
CString csKey, csEntry, csTemp, csTemp2, csMsg;


  if(acsSharedDLLs.GetSize())
  {
    hkeyRoot = HKEY_LOCAL_MACHINE;
    hKey = NULL;
    hkeyUninst = NULL;

    csKey = (CString)szHKLM_WINDOWS_CURRENT_VERSION // "SOFTWARE\\Microsoft\\Windows\\CurrentVersion"
          + "\\SharedDLLs";

    if(RegCreateKeyEx(hkeyRoot, csKey,
                      0, NULL, 0, KEY_POWERUSER_ACCESS, NULL,
                      &hKey, NULL)
       != ERROR_SUCCESS)
    {
      csMsg.Format(theApp.LoadString(IDS_ERROR05), (LPCSTR)csKey);

      MyMessageBox(csMsg, MB_OK | MB_ICONHAND | MB_SETFOREGROUND);

      return FALSE;  // fail it NOW
    }

    if(theApp.m_bNoUninstall ||
       RegCreateKeyEx(HKEY_LOCAL_MACHINE, 
                      szUNINST_KEY // "SOFTWARE\\Stewart~Frazier Tools\\UNINST\\"
                      + AppNameToRegKey(theApp.m_csAppName)
                      + "\\SharedDLLs",
                      0, NULL, 0, KEY_POWERUSER_ACCESS, NULL,
                      &hkeyUninst, NULL)
       != ERROR_SUCCESS)
    {
      // for now, just ignore it - UNINSTAL won't decrement usage counts
      // if it doesn't find this entry in the registry.

      hkeyUninst = NULL;
    }


    for(i1=0; i1 < acsSharedDLLs.GetSize(); i1++)
    {
      if(acsSharedDLLs.GetSize())
      {
        theApp.SetProgress(50 + 20 * i1 / acsSharedDLLs.GetSize());
      }
      else
      {
        theApp.SetProgress(50 * i1 / acsSharedDLLs.GetSize());
      }

      // TODO:  check for presence of existing DLL in another location
      //
      // FOR NOW, assume that this thing is the ONLY occurrence of this
      // DLL, and my instance needs to be reference counted....

      // NOTE:  the strings in this array use the *INTERNAL* file name,
      //        the one that appears in column #3 of 'files'

      csEntry = theApp.GetFileEntryFromName(acsSharedDLLs[i1]);

      if(csEntry.GetLength())
      {
        csTemp = theApp.GetFileEntryDestPath(csEntry); // theApp.m_acsFiles[i2]);

        // verify that 'csTemp' is one of the output paths in 'theApp.m_acsFiles'

        for(i2=0; i2 < theApp.m_acsFiles.GetSize(); i2++)
        {
          csTemp2 = theApp.GetFileEntryDestPath(theApp.m_acsFiles[i2]);

          if(!csTemp2.CompareNoCase(csTemp))  // found?
            break;
        }

        if(i2 >= theApp.m_acsFiles.GetSize())  // don't execute it... not installed!
          continue;

        // value name will be 'csTemp'
        dwVal=0;
        cbVal = sizeof(DWORD);

        if(hkeyUninst && theApp.m_bUpgrading &&
           RegQueryValueEx(hKey, csTemp, NULL, &dwType,
                           (BYTE *)&dwVal, &cbVal)
           == ERROR_SUCCESS && dwVal)
        {
          ASSERT(dwType == REG_DWORD);

          // I've already 'incremented' at least once.  See if it's listed
          // in the list o' things to UNINSTALL...

          cbVal = sizeof(DWORD);
          dwVal = 0;

          if(RegQueryValueEx(hkeyUninst, csTemp, NULL, &dwType,
                             (BYTE *)&dwVal, &cbVal)
             == ERROR_SUCCESS && dwVal)
          {
            ASSERT(dwType == REG_DWORD);

            continue;  // in *THIS* case, I don't need to increment it AGAIN.  I am
                       // *MERELY* upgrading the existing installation!  In this way,
                       // the uninstaller shouldn't have trouble with reference counts
          }  
        }

        if(RegQueryValueEx(hKey, csTemp, NULL, &dwType,
                           (BYTE *)&dwVal, &cbVal)
           != ERROR_SUCCESS)
        {
          // attempt to create the value with '1' as the ref count

          dwVal = 1;

          if(RegSetValueEx(hKey, csTemp, NULL, REG_DWORD, 
                           (BYTE *)&dwVal, sizeof(dwVal))
             != ERROR_SUCCESS)
          {
            csMsg.Format(theApp.LoadString(IDS_WARNING03), (LPCSTR)csTemp);

            MyMessageBox(csMsg, MB_OK | MB_ICONASTERISK | MB_SETFOREGROUND);
            dwVal = 0;
          }
        }
        else if(dwType != REG_DWORD &&
                dwType != REG_DWORD_LITTLE_ENDIAN)
        {
          csMsg.Format(theApp.LoadString(IDS_WARNING04), (LPCSTR)csTemp);

          MyMessageBox(csMsg, MB_OK | MB_ICONASTERISK | MB_SETFOREGROUND);
          dwVal = 0;
        }
        else
        {
          dwVal++;

          if(RegSetValueEx(hKey, csTemp, NULL, REG_DWORD,
                           (BYTE *)&dwVal, sizeof(dwVal))
             != ERROR_SUCCESS)
          {
            csMsg.Format(theApp.LoadString(IDS_WARNING05), (LPCSTR)csTemp);

            MyMessageBox(csMsg, MB_OK | MB_ICONASTERISK | MB_SETFOREGROUND);
            dwVal = 0;
          }
        }

        // NEXT, if I succeeded in updating this, add a key for the
        // UNINSTALLER and provide the current ref count...

        if(dwVal && hkeyUninst)
        {
          RegSetValueEx(hkeyUninst, csTemp, NULL, REG_DWORD,
                        (BYTE *)&dwVal, sizeof(dwVal));
            // this duplicates what I just put into the 'SharedDLLs' section
        }
      }
    }

    if(hkeyUninst)
      RegCloseKey(hkeyUninst);

    RegCloseKey(hKey);
  }

  return bRval;
}

BOOL SetupThreadDoRegisterDLL(CStringArray &acsRegisterControl,
                              CStringArray &acsRegAsmControl,
                              CStringArray &acsSharedDLLs,
                              CStringArray &acsRegKeys,
                              BOOL &bRebootPendingFlag)
{
int i1, i2;
CString csTemp, csTemp2, csRegAsm, csMsg;
BOOL bDotNetFlag = FALSE;  // BOO - ".Not"
BOOL bRval = TRUE;


  for(i1=0; i1 < acsRegisterControl.GetSize(); i1++)
  {
    if(acsSharedDLLs.GetSize() && acsRegKeys.GetSize())
      theApp.SetProgress(70 + 30 * i1 / acsSharedDLLs.GetSize());
    else if(acsSharedDLLs.GetSize() || acsRegKeys.GetSize())
      theApp.SetProgress(50 + 50 * i1 / acsSharedDLLs.GetSize());
    else
      theApp.SetProgress(100 * i1 / acsSharedDLLs.GetSize());

    csTemp = acsSharedDLLs[i1];

    for(i2=0; i2 < acsRegAsmControl.GetSize(); i2++)
    {
      if(!csTemp.CompareNoCase(acsRegAsmControl[i2]))
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
      if(!theApp.ExpandStrings(csTemp))
      {
        return FALSE; // I can bail NOW without consequence, really
      }
    }

    // verify that 'csTemp' is one of the output paths in 'theApp.m_acsFiles'

    for(i2=0; i2 < theApp.m_acsFiles.GetSize(); i2++)
    {
      csTemp2 = theApp.GetFileEntryDestPath(theApp.m_acsFiles[i2]);

      if(!csTemp2.CompareNoCase(csTemp))  // found?
        break;
    }

    if(i2 >= theApp.m_acsFiles.GetSize())  // don't register it... not installed!
      continue;

    //---------------------------------------------------------------
    // see if this was a SHARED component... and if I need to reboot
    // (and if this is the case, I'll need to register it AFTERWARDS)
    //---------------------------------------------------------------

    if(bRebootPendingFlag)
    {
      for(i2=0; i2 < acsSharedDLLs.GetSize(); i2++)
      {
        csTemp2 = theApp.GetFileEntryFromName(acsSharedDLLs[i2]);

        if(csTemp2.GetLength())
        {
          csTemp2 = theApp.GetFileEntryDestPath(csTemp2);

          if(!csTemp2.CompareNoCase(csTemp))
            break;
        }
      }

      if(i2 < acsSharedDLLs.GetSize() && csTemp.GetLength())
      {
        // add an entry to the 'RunOnce\\SETUP' registry key to use 'regsvr32' to 
        // register this DLL.  This assumes that 'regsvr32.exe' exists.

        csTemp2 = acsSharedDLLs[i2];

        for(i2=0; i2 < csTemp2.GetLength(); i2++)
        {
          if(csTemp2[i2] == '\\')  // convert to an underscore
            csTemp2.SetAt(i2, '_');
        }

        csTemp2 = "REGISTERING " + csTemp2;  // the name that goes in the progress dialog box!

        if(csTemp[0] != '\"')  // assumed to be the case
          csTemp = "\"" + csTemp + "\"";

        if(bDotNetFlag && IsDotNetAvailable())
        {
          csTemp = LocateBoguSoftNotYetRegistrationApp()
                 + " /s "
                 + csTemp;
        }
        else
        {
          csTemp = "regsvr32.exe /s " + csTemp;
        }

//        HKEY hKey = NULL;
//
//        if(RegCreateKeyEx(HKEY_LOCAL_MACHINE,
//                          (LPCSTR)((CString)szHKLM_WINDOWS_CURRENT_VERSION + "\\RunOnce\\SETUP"),
//                          0, NULL, 0, KEY_POWERUSER_ACCESS, NULL,
//                          &hKey, NULL)
//           != ERROR_SUCCESS ||
//           RegSetValueEx(hKey, csTemp2, 0, REG_SZ,
//                         (const BYTE *)(LPCSTR)csTemp, csTemp.GetLength() + 1)
//           != ERROR_SUCCESS)
//        {
//          if(hKey)
//            RegCloseKey(hKey);
//
//          return FALSE; // I can bail NOW without consequence, really
//        }
//
//        RegCloseKey(hKey);  // done - this will execute it after reboot

        theApp.m_csTempBatFile += "call " + csTemp + "\r\n";

        continue;
      }
    }


    theApp.SetProgressText("REGISTERING DLL's...\n" + csTemp);

    if(bDotNetFlag && IsDotNetAvailable())
    {
      csRegAsm = LocateBoguSoftNotYetRegistrationApp();

      if(csRegAsm.GetLength())
      {
        if(!ShellExecApp(csRegAsm, "/s " + csTemp, SW_HIDE))
        {
          csMsg.Format(theApp.LoadString(IDS_ERROR06), (LPCSTR)(csRegAsm + " /s " + csTemp));

          MyMessageBox(csMsg, MB_OK | MB_ICONHAND | MB_SETFOREGROUND);

//          return FALSE;
        }
      }
    }
    else
    {
      HINSTANCE h1 = LoadLibrary(csTemp);

      if(!h1)
      {
        csMsg.Format(theApp.LoadString(IDS_WARNING04), (LPCSTR)csTemp);

        MyMessageBox(csMsg, MB_OK | MB_ICONASTERISK | MB_SETFOREGROUND);
      }
      else
      {
        HRESULT (STDAPICALLTYPE *pDllRegisterServer)(void);

        (FARPROC &)pDllRegisterServer = GetProcAddress(h1, "DllRegisterServer");

        if(pDllRegisterServer)  // it worked
        {
          if(pDllRegisterServer() != NO_ERROR)
          {
            csMsg.Format(theApp.LoadString(IDS_ERROR06), (LPCSTR)csTemp);

            MyMessageBox(csMsg, MB_OK | MB_ICONHAND | MB_SETFOREGROUND);

            return FALSE;
          }
        }

        FreeLibrary(h1);
      }
    }
  }

  return bRval;
}

BOOL SetupThreadDoExecAfterCopy(CStringArray &acsExecuteAfterCopy, CStringArray &acsSharedDLLs,
                                BOOL &bRebootPendingFlag)
{
BOOL bRval = TRUE;
int i1, i2;
CString csTemp, csTemp2, csParm, csMsg;

  // each time I iterate through this, I must re-establish my window as
  // the foreground window.  I may also need to process messages... ?

  for(i1=0; i1 < acsExecuteAfterCopy.GetSize(); i1++)
  {
    theApp.SetProgress(100 * i1 / acsExecuteAfterCopy.GetSize());

    csTemp = acsExecuteAfterCopy[i1];  // filename.exe,parameter

    i2 = csTemp.Find(',');
    if(i2 >= 0)
    {
      csParm = csTemp.Mid(i2 + 1);
      csParm.TrimLeft();

      csTemp = csTemp.Left(i2);
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

    i2 = csTemp.ReverseFind('\\');
    if(i2 < 0)
    {
      csTemp = AdjustPathName(theApp.m_csAppPath) + csTemp;
    }
    else
    {
      if(!theApp.ExpandStrings(csTemp))
      {
        return FALSE; // I can bail NOW without consequence, really
      }
    }

    // verify that 'csTemp' is one of the output paths in 'theApp.m_acsFiles'

    for(i2=0; i2 < theApp.m_acsFiles.GetSize(); i2++)
    {
      csTemp2 = theApp.GetFileEntryDestPath(theApp.m_acsFiles[i2]);

      if(!csTemp2.CompareNoCase(csTemp))  // found?
        break;
    }

    if(i2 >= theApp.m_acsFiles.GetSize())  // don't execute it... not installed!
      continue;

    // ADDED 10/5/99 - expand parameters also

    if(csParm.GetLength() && !theApp.ExpandStrings(csParm))
    {
      // warn the user???
      csMsg.Format(theApp.LoadString(IDS_WARNING06), (LPCSTR)csParm);

      MyMessageBox(csMsg, MB_OK | MB_ICONASTERISK | MB_SETFOREGROUND);
    }

    //---------------------------------------------------------------
    // see if this was a SHARED component... and if I need to reboot
    //---------------------------------------------------------------

    if(bRebootPendingFlag)
    {
      for(i2=0; i2 < acsSharedDLLs.GetSize(); i2++)
      {
        csTemp2 = theApp.GetFileEntryFromName(acsSharedDLLs[i2]);

        if(csTemp2.GetLength())
        {
          csTemp2 = theApp.GetFileEntryDestPath(csTemp2);

          if(!csTemp2.CompareNoCase(csTemp))
            break;
        }
      }

      // installing shared DLLs after reboot

      if(i2 < acsSharedDLLs.GetSize() && csTemp.GetLength())
      {
        // OLD METHOD
        // create a 'RunOnce\SETUP' entry for this item
        // (better than merely a 'RunOnce' entry)

        // NEW METHOD, add a 'call' command for the 'temp bat file'
        // that gets placed into 'RunOnce'

        csTemp2 = acsSharedDLLs[i2];

        for(i2=0; i2 < csTemp2.GetLength(); i2++)
        {
          if(csTemp2[i2] == '\\')  // convert to an underscore
            csTemp2.SetAt(i2, '_');
        }

        csTemp2 = "INSTALLING " + csTemp2;  // for the progress dialog box!

        if(csParm.GetLength())
        {
          if(csTemp[0] != '\"')
          {
            csTemp = "\"" + csTemp + "\" " + csParm;
          }
          else
          {
            if(csTemp[csTemp.GetLength() - 1] != '\"')  // just in case
              csTemp += '\"';

            csTemp += " " + csParm;
          }
        }
        else if(csTemp[0] != '\"')
        {
          csTemp = "\"" + csTemp + "\"";
        }

//        HKEY hKey = NULL;
//
//        if(RegCreateKeyEx(HKEY_LOCAL_MACHINE,
//                          (LPCSTR)((CString)szHKLM_WINDOWS_CURRENT_VERSION + "\\RunOnce\\SETUP"),
//                          0, NULL, 0, KEY_POWERUSER_ACCESS, NULL,
//                          &hKey, NULL)
//           != ERROR_SUCCESS ||
//           RegSetValueEx(hKey, csTemp2, 0, REG_SZ,
//                         (const BYTE *)(LPCSTR)csTemp, csTemp.GetLength() + 1)
//           != ERROR_SUCCESS)
//        {
//          if(hKey)
//            RegCloseKey(hKey);
//
//          return FALSE; // I can bail NOW without consequence, really
//        }
//
//        RegCloseKey(hKey);  // done - this will execute after reboot

        theApp.m_csTempBatFile += "call " + csTemp + "\r\n";

        continue;
      }
    }

    theApp.SetProgressText("EXECUTING PROGRAMS...\n" + csTemp);

    if(!ShellExecApp(csTemp, csParm))
      bRval = FALSE;

    ::SetForegroundWindow(theApp.m_hWnd); // force this to become the foreground window again
    // re-setting foreground window *should* work around a few observed quirks
  }  

  return bRval;
}


// CERT support functions
// well-known store names:  "ROOT", "CA", "My"  [defaut should be ROOT]
HCERTSTORE InternalOpenSystemCAStore(HMODULE hCrypt32, LPCWSTR lpwszStoreName)
{
HCERTSTORE hSysStore;
HCERTSTORE (WINAPI *pCertOpenStore)(_In_  LPCSTR lpszStoreProvider,
                                    _In_  DWORD dwMsgAndCertEncodingType,
                                    _In_  HCRYPTPROV_LEGACY hCryptProv,
                                    _In_  DWORD dwFlags,
                                    _In_  const void *pvPara);


  (FARPROC &)pCertOpenStore = GetProcAddress(hCrypt32,
                                             "CertOpenStore");

  if(!pCertOpenStore)
  {
    MyMessageBox("GetProcAddress() failed to find \"CertOpenStore\"");
    return FALSE; // fail
  }


  if(!lpwszStoreName)
  {
    lpwszStoreName = L"ROOT"; // default store is ROOT
  }

  // NOTE:  for a user store, use CERT_SYSTEM_STORE_CURRENT_USER

  hSysStore = pCertOpenStore(CERT_STORE_PROV_SYSTEM,          // store provider type (SYSTEM)
                             0,                               // encoding type is not needed
                             NULL,                            // Use default HCRYPTPROV
                             CERT_SYSTEM_STORE_LOCAL_MACHINE, // Set store location in the registry
                             lpwszStoreName);

#ifdef _DEBUG

  if(!hSysStore && GetLastError() == 5) // access error I think
  {
    if(MyMessageBox("Unable to open system store, use 'User' store instead?",
                     MB_YESNO | MB_ICONQUESTION | MB_SETFOREGROUND)
       == IDYES)
    {
      hSysStore = pCertOpenStore(CERT_STORE_PROV_SYSTEM, 0, NULL,
                                 CERT_SYSTEM_STORE_CURRENT_USER,
                                 lpwszStoreName);
    }
  }

#endif // _DEBUG

  return hSysStore; // NULL if failed
}

// add a cert that was read-in using 
BOOL InternalAddCert(HMODULE hCrypt32, HCERTSTORE hSysStore, CString &csCert, CString & csCertName, CString &csRepository)
{
DWORD dwErr, cbBuf;
LPCSTR pBuf;
BOOL (WINAPI *pCertAddEncodedCertificateToStore)(_In_       HCERTSTORE hCertStore,
                                                 _In_       DWORD dwCertEncodingType,
                                                 _In_       const BYTE *pbCertEncoded,
                                                 _In_       DWORD cbCertEncoded,
                                                 _In_       DWORD dwAddDisposition,
                                                 _Out_opt_  PCCERT_CONTEXT *ppCertContext);


  (FARPROC &)pCertAddEncodedCertificateToStore = GetProcAddress(hCrypt32,
                                                                "CertAddEncodedCertificateToStore");

  if(!pCertAddEncodedCertificateToStore)
  {
    MyMessageBox("GetProcAddress() failed to find \"CertAddEncodedCertificateToStore\"");
    return FALSE; // fail
  }

  cbBuf = csCert.GetLength();
  pBuf = (LPCSTR)csCert;

  if(pCertAddEncodedCertificateToStore(hSysStore,
                                       X509_ASN_ENCODING,
                                       (const BYTE *)pBuf,
                                       cbBuf,
                                       CERT_STORE_ADD_NEW,
                                       NULL))
  {
    return TRUE; // success
  }
  else
  {
    dwErr = GetLastError();

    if(dwErr == CRYPT_E_EXISTS)
    {
      // TODO:  prompt to over-write?
      if(MyMessageBox("The " + csCertName
                      + " certificate already exists in the " + csRepository
                      + "\r\nDo you want to replace it?",
                      MB_YESNO | MB_ICONQUESTION | MB_SETFOREGROUND)
         == IDYES)
      {
        if(pCertAddEncodedCertificateToStore(hSysStore,
                                             X509_ASN_ENCODING,
                                             (const BYTE *)pBuf,
                                             cbBuf,
                                             CERT_STORE_ADD_REPLACE_EXISTING,
                                             NULL))
        {
          return TRUE; // very very cool
        }
      }
      else
      {
        // TODO:  prompt 'not replacing' or is that too irritating?
        return TRUE;  // this is also "not an error" since I did not choose to overwrite
      }
    }

    if(dwErr == E_INVALIDARG)
    {
      MyMessageBox("CERT is not valid\n");
    }
    else if(dwErr == CRYPT_E_ASN1_BADTAG)
    {
      MyMessageBox("CERT tag is bad\n");
    }
    else
    {
      CString csTemp;
      csTemp.Format("Unable to add cert, error %d (%08xH)\n", dwErr, dwErr);

      MyMessageBox(csTemp);
    }

    return FALSE; // error
  }

  // already returned 'ok' above
}

void InternalCloseSystemCAStore(HMODULE hCrypt32, HCERTSTORE hSysStore)
{
BOOL (WINAPI *pCertCloseStore)(_In_  HCERTSTORE hCertStore,
                               _In_  DWORD dwFlags);


  (FARPROC &)pCertCloseStore = GetProcAddress(hCrypt32,
                                              "CertCloseStore");

  if(!pCertCloseStore)
  {
    MyMessageBox("GetProcAddress() failed to find \"CertCloseStore\"");
    return; // fail
  }

  pCertCloseStore(hSysStore, 0);
}




