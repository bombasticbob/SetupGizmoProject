//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//                  ____         _                    _                     //
//                 / ___|   ___ | |_  _   _  _ __    | |__                  //
//                 \___ \  / _ \| __|| | | || '_ \   | '_ \                 //
//                  ___) ||  __/| |_ | |_| || |_) |_ | | | |                //
//                 |____/  \___| \__| \__,_|| .__/(_)|_| |_|                //
//                                          |_|                             //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//       Copyright (c) 1998-2016 by S.F.T. Inc. - All rights reserved       //
//  Use, copying, and distribution of this software are licensed according  //
//    to the GPLv2, LGPLv2, or BSD license, as appropriate (see COPYING)    //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////


#if !defined(AFX_SETUP_H__B3134EE9_0D1F_11D2_A115_004033901FF3__INCLUDED_)
#define AFX_SETUP_H__B3134EE9_0D1F_11D2_A115_004033901FF3__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

//#ifndef __AFXWIN_H__
//	#error include 'stdafx.h' before including this file for PCH
//#endif

#include "resource.h"		// main symbols

// KEY_ALL_ACCESS works only if administrator is accessing HKLM
// KEY_POWERUSER_ACCESS works if not admin

#define KEY_POWERUSER_ACCESS (KEY_READ | KEY_WRITE)

/////////////////////////////////////////////////////////////////////////////
// CSETUPApp:
// See SETUP.cpp for the implementation of this class
//

class CProgressDialog;
class CMainFrame;

class CSETUPApp : public CMyApp//CWinApp
{
public:
	CSETUPApp();

  CMainFrame *m_pMyFrameWnd;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSETUPApp)
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
  CProgressDialog *m_pProgressDlg;   // modeless dialog box for progress
  CMyThread *m_pCopyThread;         // thread executing copy


  CString m_csEXEPath;  // full path to this EXE (on CD or whatever)
                        // WIN32 will cache the program, so I don't
                        // need to copy it anywhere...
  CString m_csINFFile;  // entire contents of INF file
                        // (I need to cache this)

  CString m_csAppName, m_csCompanyName, m_csStartMessage, m_csAuthCode,
          m_csAuthCodeCompany;
    // SETUP initialization information gleaned from the INF file

  CString m_csAppPath, m_csAppCommon;  // destination paths (thread assigns them)
  CString m_csUninstPath;              // this is where UNINSTALL was written

  CString m_csStartMenuFolderName;     // when I create shortcuts, this gets assigned
                                       // to the start menu folder the user picked

  BOOL m_bRebootFlag;         // when TRUE, I must re-boot the system (renaming shared files)

  BOOL m_bSelfExtract;        // when TRUE, resources contain the CAB and 'SETUP.INF' files
                              // and 'UNINST.EXE' is compressed within the CAB file.

  BOOL m_bUpgrading;          // TRUE if I'm upgrading an existing install and the
  CString m_csUpgradeApp;     // destination paths match.  'm_csUpgradeApp' is the
                              // name of the application that I'm upgrading.

  BOOL m_bNoUninstall;        // TRUE if I'm not to copy/setup UNINST.EXE
  BOOL m_bQuietSetup;         // no prompts (except license, when present, and options)
                              // and reboot (when applicable).  Default 'app path' derived
                              // from the windows directory if not specified on command line.
                              // (uninst path is app path + app name)
  BOOL m_bExtraQuietMode;     // the '/Q' option - does not allow REBOOT prompt
                              // caller must check to see if a re-boot is required
                              // implies 'Quiet Setup' mode (above)

  BOOL m_bForceReboot, m_bNoReboot;  // additional flags
    // use /FORCEREBOOT and /NOREBOOT for these 2 flags

  BOOL m_bNoLogo;  // use '/NOLOGO' for this one

  CString m_csTempBatFile;    // if I have to do a reboot, the "temp bat file" serves as a
                              // way of doing registrations, etc.
                              // If this string is NOT blank, a temporary '.bat' file must
                              // be created, and this string written to it.  The '.bat' file
                              // must then be added to the 'RunOnce\SETUP' key with an
                              // appropriate command line.

  // cached information from INF file
  CStringArray m_acsFiles, m_acsCabs, m_acsDisks,  // cached entries for
               m_acsShared, m_acsOverwrite;        // frequent reference

  CStringArray m_acsOptions,                       // list of options that I have selected
               m_acsUpgradeInstalledOptions;       // list of options installed "last time"
                                                   // files are added to 'm_acsFiles'

  unsigned long long m_dwTotalFileSize;            // total size all files
  DWORD m_dwTotalFileSizeX;                        // 'extended' file size (64k clusters)
  CArray<long,long> m_aFileSize;                   // array of file size info

  BOOL DoSelfExtract(const BYTE *lpEnd, const BYTE *lpSelf, DWORD cbSelf);  // does what it says
  void GetAndValidateAuthCode(); // and so does this

  CString GetInfEntry(LPCSTR szKey, LPCSTR szString);
  void GetInfEntries(LPCSTR szKey, CStringArray &acsEntries);

  BOOL ExpandStrings(CString &csStringVal);           // expand '%apppath%' (etc.)

  CString GetFileEntryDestPath(LPCSTR szFileEntry);  // returns name of dest file from file entry
  int GetFileEntryCabNumber(LPCSTR szFileEntry);     // gets cab # from file entry
                                                     // if cab # is 0, returns negative disk #

  BOOL IsFileEntryShared(LPCSTR szFileEntry);        // if file is system/shared it returns TRUE
  BOOL IsFileEntryOverwriteAlways(LPCSTR szFile);    // if file should always overwrite, returns TRUE

  CString GetCabFilePath(int iCab);                  // gets cabinet path from cab #
  CString GetPathAndPromptForDiskette(int iDisk);    // check for diskette inserted, and prompt if not
                                                     // returns correct path for disk

  int GetCabNumberFromCabName(LPCSTR szCabName);     // CAB SDK callback needs this
  CString GetFileEntryFromName(LPCSTR szInternalName); // registration (and other things) need this

  BOOL UpgradeCheck(void);
  int DriveSpaceCheck(LPCSTR szPath, BOOL bNoRetry = FALSE);
  BOOL PromptForAndCreateAppCommon(void);
  BOOL PromptForAndCreateAppPath(void);

  BOOL IsRebootPending(void);  // this checks the state of the 'rename after reboot' schedule

  UINT GetEntryFileSize(int iEntry)
  {
    if(iEntry >= 0 && iEntry < m_aFileSize.GetSize())
      return(m_aFileSize[iEntry]);
    else 
      return(0);
  }

  void SetProgress(UINT uiPercent);
  void SetProgressText(LPCSTR szText);

  int GetCertFileCount(void);
  CString GetCertFileEntry(int iIndex);

// Implementation

	//{{AFX_MSG(CSETUPApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
//	DECLARE_MESSAGE_MAP()
  virtual int Run();
  virtual int DoMessageBox(LPCTSTR lpszPrompt, UINT nType, UINT nIDPrompt);
};


extern CSETUPApp theApp;

//#define SETUP_KEY "SOFTWARE\\Stewart~Frazier Tools\\SETUP\\"
//#define UNINST_KEY "SOFTWARE\\Stewart~Frazier Tools\\UNINST\\"
extern const char szSETUP_KEY[];
extern const char szUNINST_KEY[];
extern const char szHKLM_WINDOWS_CURRENT_VERSION[];


// OTHER (global) UTILITIES

void DoParseString(CString &csLine, CStringArray &acsColumns);

BOOL IsValidDirectory(LPCSTR szDirName);   // TRUE if exists AND is drive/directory
int FileAlreadyExists(LPCSTR szFileName);  // -1 on error/dir, 1 if found, 0 if not

BOOL CheckForExistence(LPCSTR szPath, BOOL &bIsDir, BOOL &bIsDrive);
  // this one comprehensively checks for existence, drive only, directory, and file.

BOOL CheckForAndCreateDirectory(LPCSTR szPath, BOOL bPromptCreate=FALSE);

CString GetRegistryString(HKEY hkeyRoot, LPCSTR szKeyName, LPCSTR szString);

HRESULT CreateLink(LPCSTR lpszPathObj, LPCSTR lpszPathLink, 
                   LPCSTR lpszDesc, LPCSTR lpszPath = NULL,
                   LPCSTR lpszArgs = NULL, UINT uiIcon = (UINT)-1,
                   LPCSTR lpszIconFile = NULL, int iCmdShow = SW_SHOWNORMAL);

CString TranslateEscapeCodes(CString &csSource);

BOOL ExtractFilesFromTempCabinet(LPCSTR szCab, LPCSTR szDestPath);
  // use this to get files from a temp cabinet (like bitmaps)

CString AppNameToPathName(LPCSTR szAppName, BOOL bStripBackSlash = TRUE);
  // converts 'AppName' to something that can be used as a path name
  // this should apply to registry keys as well

CString AppNameToRegKey(LPCSTR szAppName);
  // similar to the one above, but only strips out things registry keys don't like

CString AdjustPathName(LPCSTR szPathName);
  // adds an ending backslash to a path name if it doesn't already have one

CString AdjustPathName2(LPCSTR szPathName);
  // removes an ending backslash from a path name if it exists

BOOL MyCopyRegistryKey(HKEY hkeySource, HKEY hkeyDest);
  // copy a registry key from one to another

BOOL EnumRegistryKeys(HKEY hkeyRoot, LPCSTR szKeyName, CStringArray &acsKeys);
  // enumerates ALL child keys for a given registry key into a CStringArray

BOOL MyDeleteRegistryKey(HKEY hkeyRoot, LPCSTR szKeyName);
BOOL MyDeleteRegistryKey0(HKEY hkeyRoot, LPCSTR szKeyName);
  // delete registry keys in a predictable manner.  '0' behaves like NT default
  // and does not delete keys that have 'child' keys.

BOOL MoveApplication(LPCSTR szOldApp, LPCSTR szNewApp);
  // moves an application's registry settings from 'szOldApp' to 'szNewApp'
  // including uninstall information. Start Menu icons and folders remain as-is.

BOOL IsDotNetAvailable();  // TRUE if the function below succeeds
CString LocateBoguSoftNotYetRegistrationApp(); // locates 'regasm'

BOOL ShellExecApp(LPCSTR szApp, LPCSTR szArgs, UINT nCmdShow = SW_SHOWNORMAL);
  // 'ShellExec' and wait


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETUP_H__B3134EE9_0D1F_11D2_A115_004033901FF3__INCLUDED_)
