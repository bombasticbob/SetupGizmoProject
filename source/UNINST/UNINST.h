//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//               _   _  _   _  ___  _   _  ____  _____   _                  //
//              | | | || \ | ||_ _|| \ | |/ ___||_   _| | |__               //
//              | | | ||  \| | | | |  \| |\___ \  | |   | '_ \              //
//              | |_| || |\  | | | | |\  | ___) | | | _ | | | |             //
//               \___/ |_| \_||___||_| \_||____/  |_|(_)|_| |_|             //
//                                                                          //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//       Copyright (c) 1998-2016 by S.F.T. Inc. - All rights reserved       //
//  Use, copying, and distribution of this software are licensed according  //
//    to the GPLv2, LGPLv2, or BSD license, as appropriate (see COPYING)    //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////


#if !defined(AFX_UNINST_H__B3134EF7_0D1F_11D2_A115_004033901FF3__INCLUDED_)
#define AFX_UNINST_H__B3134EF7_0D1F_11D2_A115_004033901FF3__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

//#ifndef __AFXWIN_H__
//  #error include 'stdafx.h' before including this file for PCH
//#endif

#include "resource.h"   // main symbols

// KEY_ALL_ACCESS works only if administrator is accessing HKLM
// KEY_POWERUSER_ACCESS works if not admin

#define KEY_POWERUSER_ACCESS (KEY_READ | KEY_WRITE)

/////////////////////////////////////////////////////////////////////////////
// CUNINSTApp:
// See UNINST.cpp for the implementation of this class
//

class CUNINSTApp : public CMyApp
{
public:
  CUNINSTApp();

// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CUNINSTApp)
  public:
  virtual BOOL InitInstance();
  virtual int ExitInstance();
  //}}AFX_VIRTUAL

  BOOL m_bIsNT;                      // TRUE for NT, FALSE for 95/98
  DWORD m_dwMajorVer, m_dwMinorVer;  // major/minor version
  WORD m_wSuiteMask;                 // "suite mask" for version info
  WORD m_wSPMajor, m_wSPMinor;       // service pack major/minor version
  CString m_csOS, m_csOSName;        // 'OS' string(s) for options

  CString m_csWinPath, m_csSysPath;  // windows and windows\system directories

  CString m_csEXEPath;  // full path to this EXE (on CD or whatever)
  CString m_csINFFile;  // entire contents of INF file

  CString m_csAppPath, m_csAppCommon;  // destination paths (get from registry)

  CString m_csAppName, m_csCompanyName;

  CString m_csStartMenuFolderName;     // start menu folder (also from registry)


  // arrays containing 'cached' information

  CStringArray m_acsFiles;  // files from the 'files' section (assume ALL options for this CPU)
  CStringArray m_acsShared; // files from the 'sharedcomponents' section

  CStringArray m_acsUnInstall, m_acsUnInstallShared;

  CString GetFileEntryDestPath(LPCSTR szFileEntry);
  CString GetFileEntryFromName(LPCSTR szInternalName); // registration (and other things) need this

  CString GetInfEntry(LPCSTR szKey, LPCSTR szString);
  void GetInfEntries(LPCSTR szKey, CStringArray &acsEntries);

  BOOL ExpandStrings(CString &csStringVal);           // expand '%apppath%' (etc.)

//  BOOL DoEvents();  // returns TRUE on 'WM_QUIT'

// Implementation

  //{{AFX_MSG(CUNINSTApp)
    // NOTE - the ClassWizard will add and remove member functions here.
    //    DO NOT EDIT what you see in these blocks of generated code !
  //}}AFX_MSG
//  DECLARE_MESSAGE_MAP()
  virtual int Run();
};


extern CUNINSTApp theApp;


// utility functions

void DoParseString(CString &csLine, CStringArray &acsColumns);
BOOL IsValidDirectory(LPCSTR szDirName);
int FileAlreadyExists(LPCSTR szFileName);  // -1 on error, 1 if found, 0 if not

CString GetRegistryString(HKEY hkeyRoot, LPCSTR szKeyName,
                          LPCSTR szString);
BOOL WriteRegistryString(HKEY hkeyRoot, LPCSTR szKeyName,
                         LPCSTR szString, LPCSTR szValue);
BOOL EnumRegistryKeys(HKEY hkeyRoot, LPCSTR szKeyName,
                      CStringArray &acsKeys);

BOOL CheckKeyDeleteOK(HKEY hkeyRoot, LPCSTR szKeyName);
BOOL MyDeleteRegistryKey(HKEY hkeyRoot, LPCSTR szKeyName);
BOOL MyDeleteRegistryKey0(HKEY hkeyRoot, LPCSTR szKeyName);

CString TranslateEscapeCodes(CString &csSource);

CString AppNameToPathName(LPCSTR szAppName, BOOL bStripBackSlash = TRUE);
CString AppNameToRegKey(LPCSTR szAppName);

CString AdjustPathName(LPCSTR szPathName);
CString AdjustPathName2(LPCSTR szPathName);

BOOL SafeCallDllUnregisterServer(LPCSTR szLibrary, CString &csMsg);

BOOL PathHasFiles(LPCSTR szPath);
BOOL NukeDirectory(LPCSTR szPath);

BOOL IsDotNetAvailable();  // TRUE if the function below succeeds
CString LocateBoguSoftNotYetRegistrationApp(); // locates 'regasm'

BOOL ShellExecApp(LPCSTR szApp, LPCSTR szArgs, UINT nCmdShow = SW_SHOWNORMAL);
  // 'ShellExec' and wait

BOOL DeleteSharesOnDirectory(LPCSTR szDirectory);
  // call this function to delete all shares on a directory
  // prior to removing the directory.  Shares are NOT recursively
  // checked for.

// additional structures, etc. that are best defined in an '.h' file

struct MYKEYINFO
{
  MYKEYINFO() { hkeyRoot = NULL; szKey = NULL; }
  MYKEYINFO & operator =(const MYKEYINFO &x) { hkeyRoot = x.hkeyRoot; szKey = x.szKey; return(*this); }
  HKEY hkeyRoot;
  LPCSTR szKey;
};



/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UNINST_H__B3134EF7_0D1F_11D2_A115_004033901FF3__INCLUDED_)
