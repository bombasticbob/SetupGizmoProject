//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//    ____         _                   ____  _                              //
//   / ___|   ___ | |_  _   _  _ __   / ___|(_) ____    ___  _ __   _ __    //
//   \___ \  / _ \| __|| | | || '_ \ | |  _ | ||_  /   / __|| '_ \ | '_ \   //
//    ___) ||  __/| |_ | |_| || |_) || |_| || | / /  _| (__ | |_) || |_) |  //
//   |____/  \___| \__| \__,_|| .__/  \____||_|/___|(_)\___|| .__/ | .__/   //
//                            |_|                           |_|    |_|      //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//       Copyright (c) 1998-2016 by S.F.T. Inc. - All rights reserved       //
//  Use, copying, and distribution of this software are licensed according  //
//    to the GPLv2, LGPLv2, or BSD license, as appropriate (see COPYING)    //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////

// NOTE:  this wonderful header was generated using 'figlet' and a shell script
//        which I'm able to do by typing in the name of the file as a parameter

#include "stdafx.h"
#include "SetupGiz.h"
#include "SetupGizDlg.h"
#include "NagDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define REGISTRY_KEY_BASE HKEY_CURRENT_USER /* HKEY_LOCAL_MACHINE */

#if 0 /* removed because W95 version won't compile any more in DevStudio 2010 */
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
// CSetupGizApp

BEGIN_MESSAGE_MAP(CSetupGizApp, CWinApp)
	//{{AFX_MSG_MAP(CSetupGizApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetupGizApp construction

CSetupGizApp::CSetupGizApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CSetupGizApp object

CSetupGizApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CSetupGizApp initialization

BOOL CSetupGizApp::InitInstance()
{
  CWinApp::InitInstance();

  SetRegistryKey(_T("Stewart-Frazier_Tools_Inc"));

  AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

//	Enable3dControlsStatic();	// Call this when linking to MFC statically

  BOOL bValidate = ValidateAuthCode();

	CSetupGizDlg dlg;
  m_pMainWnd = NULL;  // make sure it's NULL, initially

  CString csTemp, csTemp2;

  int iType = 0;
  BOOL bBatch = FALSE, bQuiet = FALSE, bNoUninstall = FALSE,
       bLZ = FALSE, bAutoCD = FALSE,
       bSetup=FALSE, bDemo=FALSE, bUninst=FALSE, bRegister = FALSE;
  int iRebootFlag = -1;
  int iDisk1Reserve = 0;

  if(m_lpCmdLine && *m_lpCmdLine)
  {
    // parse for switches, first
    //
    // COMMAND LINE:
    //
    // setupgiz [/?|/H[ELP]][/BATCH][/TYPE[:1|2|3|4]][/QUIET][/NOUN[INSTALL]]
    //          [/LZ][/AUTOCD][/BOOT[:0|+|-]] filename.giz
    // also     /SETUP and /REGISTER (for internal use only)
    //
    // TYPE [default is 4] - sets type of image to create
    // 1 = CDROM image, 2 = CDROM image with diskette image,
    // 3 = diskette image only, 4 = self-extracting
    //
    // QUIET - turns on 'quiet install' mode
    //
    // NOUNINSTALL - does not include UNINSTALL
    //
    // LZ - enables LZ compression
    //
    // AUTOCD - valid only for types 1 and 2, creates an 'AUTORUN.INF' file.
    //
    // if 'filename.giz' is NOT specified, the dialog box appears as normal.

    LPCSTR lpc1 = m_lpCmdLine;

    while(*lpc1)
    {
      while(*lpc1 && *lpc1 <= ' ')
        lpc1++;

      if(!*lpc1)
        break;

      if(*lpc1 != '/' && *lpc1 != '-')  // not a switch
        break;  // assume the rest of the line is a file name

      lpc1++;

      LPCSTR lpc2 = lpc1;

      while(*lpc1 > ' ')
        lpc1++;  // find "next term"

      if(lpc1 > lpc2)
      {
        csTemp = ((CString)lpc2).Left(lpc1 - lpc2);
      }
      else
      {
        if(!bValidate)
          csTemp = "SFT Setup Gizmo - Warning {Unlicensed Copy}";
        else
          csTemp = "SFT Setup Gizmo - Command Line Warning";

        ::MessageBox(NULL, "Invalid [blank] switch specified - ignored",
                     csTemp, MB_OK | MB_ICONEXCLAMATION | MB_SETFOREGROUND);
        continue;
      }

      if(!csTemp.CompareNoCase("REG") ||
         !csTemp.CompareNoCase("REGISTER"))
      {
        bRegister = TRUE;
        break;
      }
      else if(!csTemp.CompareNoCase("BATCH"))
      {
        bBatch = TRUE;
      }
      else if(!csTemp.CompareNoCase("QUIET"))
      {
        bQuiet = TRUE;
      }
      else if(!csTemp.CompareNoCase("LZ"))
      {
        bLZ = TRUE;
      }
      else if(!csTemp.CompareNoCase("AUTOCD"))
      {
        bAutoCD = TRUE;
      }
      else if(!csTemp.CompareNoCase("SETUP"))
      {
        bSetup = TRUE;
        break;
      }
      else if(!csTemp.CompareNoCase("DEMO"))
      {
        bDemo = TRUE;
        break;
      }
      else if(!csTemp.CompareNoCase("UNINST"))
      {
        bUninst = TRUE;
        break;
      }
      else if(csTemp.GetLength() >= 4 && csTemp.GetLength() <= 11 &&
              !csTemp.CompareNoCase(((CString)"NOUNINSTALL").Left(csTemp.GetLength())))
      {
        bNoUninstall = TRUE;
      }
      else if((csTemp.GetLength() > 4 && !csTemp.Left(4).CompareNoCase("RES:")) ||
              (csTemp.GetLength() > 5 && !csTemp.Left(5).CompareNoCase("RESE:")) ||
              (csTemp.GetLength() > 6 && !csTemp.Left(6).CompareNoCase("RESER:")) ||
              (csTemp.GetLength() > 7 && !csTemp.Left(7).CompareNoCase("RESERV:")) ||
              (csTemp.GetLength() > 8 && !csTemp.Left(8).CompareNoCase("RESERVE:")))
      {
        ASSERT(!iDisk1Reserve);  // should NOT have been assigned yet

        // find the ':'

        iDisk1Reserve = csTemp.Find(':');

        csTemp = csTemp.Mid(iDisk1Reserve + 1);

        if(csTemp.GetLength())
          iDisk1Reserve = atoi(csTemp);
        else
          iDisk1Reserve = 0;

        if(iDisk1Reserve < 0)
          iDisk1Reserve = 0;
        else if(iDisk1Reserve > 1024)
          iDisk1Reserve = 1024;
      }
      else if(!csTemp.CompareNoCase("BOOT") ||
              !csTemp.CompareNoCase("BOOT:0"))
      {
        if(iRebootFlag < 0)  // only if NOT assigned already
          iRebootFlag = 0;
        else
          AfxMessageBox("WARNING:  '/BOOT' was specified more than once in the command line!");
      }
      else if(!csTemp.CompareNoCase("BOOT:+"))
      {
        if(iRebootFlag < 0)  // only if NOT assigned already
          iRebootFlag = 1;
        else
          AfxMessageBox("WARNING:  '/BOOT' was specified more than once in the command line!");
      }
      else if(!csTemp.CompareNoCase("BOOT:-"))
      {
        if(iRebootFlag < 0)  // only if NOT assigned already
          iRebootFlag = 2;
        else
          AfxMessageBox("WARNING:  '/BOOT' was specified more than once in the command line!");
      }
      else if(!csTemp.CompareNoCase("TYPE"))
      {
        iType = 4;  // the default when not specified
      }
      else if(csTemp.GetLength() > 4 && !csTemp.Left(5).CompareNoCase("TYPE:"))
      {
        iType = atoi(csTemp.Mid(5));

        if(iType == 0)
          iType = 4;  // default is 4 (again) if not specified
      }
      else if((csTemp.GetLength() == 1 && (toupper(csTemp[0]) == 'H' || csTemp[0] == '?'))
              || !csTemp.CompareNoCase("HELP"))
      {
        if(!bValidate)
          csTemp2 = "SFT Setup Gizmo - Command Line HELP {Unlicensed Copy}";
        else
          csTemp2 = "SFT Setup Gizmo - Command Line HELP";

        ::MessageBox(NULL,
                     "SetupGiz.EXE - Copyright © 2000 by Stewart~Frazier Tools, Inc.\r\n"
                     "COMMAND LINE ARGUMENTS:\r\n"
                     "SETUPGIZ [/?|/H[ELP]][/BATCH][/TYPE[:1|2|3|4]][/QUIET]\r\n"
                     "                 [/NOUN[INSTALL]][/LZ][/AUTOCD] filename.giz\r\n"
                     "  where     /? or /H or /HELP displays this dialog box\r\n"
                     "    and       /BATCH creates the setup without using the wizard\r\n"
                     "    and       'filename.giz' is an existing '.giz' file\r\n"
                     "\r\n"
                     "For information on the meaning and use of the other switches, refer\r\n"
                     "to the Setup Gizmo HELP file (SETUPGIZ.HLP) included with the software.",
                     csTemp2, MB_OK | MB_ICONINFORMATION | MB_SETFOREGROUND);

        m_pMainWnd = NULL;  // to avoid errors

        return(FALSE);
      }
      else
      {
        if(!bValidate)
          csTemp2 = "SFT Setup Gizmo - Command Line Warning {Unlicensed Copy}";
        else
          csTemp2 = "SFT Setup Gizmo - Command Line Warning";

        if(::MessageBox(NULL, "WARNING: illegal switch '/" + csTemp + "' specified on command line - switch ignored",
                        csTemp2, MB_OKCANCEL | MB_ICONASTERISK | MB_SETFOREGROUND)
           != IDOK)
        {
          m_pMainWnd = NULL;  // to avoid errors

          return(FALSE);  // bail now (was an error worth canceling over)
        }
      }
    }

    dlg.m_csFileName = lpc1;
    dlg.m_csFileName.TrimRight();      
    dlg.m_csFileName.TrimLeft();

    if(dlg.m_csFileName.GetLength())
    {
      if(dlg.m_csFileName[0] == '"')  // quoted
      {
        if(dlg.m_csFileName.GetLength() >= 2 &&
           dlg.m_csFileName[dlg.m_csFileName.GetLength() - 1] == '"')
        {
          dlg.m_csFileName = dlg.m_csFileName.Mid(1,dlg.m_csFileName.GetLength() - 2);
        }
        else
        {
          dlg.m_csFileName = dlg.m_csFileName.Mid(1);  // no trailing quote - ignore
        }
      }
    }
  }

  if(bUninst)
  {
    // TODO:  additional "uninstall" thingies

    m_pMainWnd = NULL;  // make *SURE* I do this!  If not, it may crash on exit
    return(FALSE);
  }
  else if(bDemo)
  {
    // TODO:  additional 'demo' thingies (like timeouts ??)

    m_pMainWnd = NULL;  // make *SURE* I do this!  If not, it may crash on exit
    return(FALSE);
  }
  else if(bSetup || bRegister)
  {
    if(!bValidate || bRegister)  // if I didn't already upgrade it...
    {
      CNagDialog dlgNag;
      m_pMainWnd = &dlgNag;

      if(bValidate)
      {
        dlgNag.m_csAuthCompany = m_csAuthCompany;
//        dlgNag.m_csAuthCode = m_csAuthCode;
      }
      else
      {
        theApp.WriteProfileInt("BypassRegistration", "Enable", 0); // resets registration check
      }

show_nag_dialog:
      if(dlgNag.DoModal() == IDCANCEL)
      {
        if(bValidate)
        {
          if(AfxMessageBox("You pressed 'Cancel' on the license verification - is this what you really wanted to do?  (Your license information has not been altered)\n"
                           "If you continue, your license information will remain as-is.  If you want to go back, press 'YES'; otherwise, 'No' to continue with the "
                           "license information as-is", MB_YESNO | MB_ICONEXCLAMATION)
             == IDYES)
          {
            goto show_nag_dialog; // just show it again.
          }
        }

        theApp.WriteProfileInt("BypassRegistration", "Enable", 0); // resets registration check
        // NOTE:  if I'm already registered, this won't make much of a difference, but it
        //        DOES flag [for later?] that the user hit 'cancel' on the verification.
      }
    }

    m_pMainWnd = NULL;  // make *SURE* I do this!  If not, it may crash on exit
    return(FALSE);
  }


  // *THIS* is where I determine if I'm licensed and display the 'Nag' dialog box

  if(!bValidate)
  {
    m_pMainWnd = NULL;  // make sure

    CNagDialog dlgNag;

    if(!bRegister && theApp.GetProfileInt("BypassRegistration", "Enable", 0) == 1)
    {
      // bypass the registration dialog box if I did not
      // specify the '-reg' or '-register' flag on the command line
    }
    else if(dlgNag.DoModal() == IDOK)
    {
      if(theApp.m_csAuthCompany.GetLength())
      {
        dlg.m_Page1.m_csCompanyName = theApp.m_csAuthCompany;
          // this re-sets company information now that I've registered
          // But if I have specified a file name, it gets taken care of below...
      }
    }
    else
    {
      dlg.m_Page1.m_csCompanyName = "";
    }
  }

  bValidate = ValidateAuthCode();  // once more so that everything "lines up" properly

	m_pMainWnd = &dlg;  // to avoid problems, do THIS *NOW*

  if(dlg.m_csFileName.GetLength())
  {
    dlg.DoOpen();
  }

  dlg.m_Finish.m_bAutoInsert = bAutoCD;
  dlg.m_Finish.m_bLZCompress = bLZ;
  dlg.m_Finish.m_bQuietSetup = bQuiet;
  dlg.m_Finish.m_bNoUninstall = bNoUninstall;
  dlg.m_Finish.m_iDisk1Reserve = iDisk1Reserve;

  if(iRebootFlag >= 0)  // this means that I specified a value on the command line
  {
    // the value is 0, 1, or 2 for reboot flag corresponding to auto, always, never

    dlg.m_Finish.m_iRebootFlag = iRebootFlag;
  }

  if(iType > 0 && iType <= 4)
  {
    dlg.m_Finish.m_iInstallType = iType - 1;  // always 0 through 3
  }
  else if(bBatch)
  {
    iType = 4;  // the default is "SELF-EXTRACTING"
    dlg.m_Finish.m_iInstallType = iType - 1;
  }

  if(bBatch)
  {
    MSG *pMsg = AfxGetCurrentMessage();
    if(pMsg)
      pMsg->wParam = 0; // so the return code is zero
                        // see CWinApp::ExitInstance(), appcore.cpp

    if(!dlg.m_csFileName.GetLength())
    {
      if(!bValidate)
        csTemp2 = "SFT Setup Gizmo - Command Line Error {Unlicensed Copy}";
      else
        csTemp2 = "SFT Setup Gizmo - Command Line Error";

      ::MessageBox(NULL,
                   "ERROR:  '/BATCH' not valid with no '.giz' file name",
                   csTemp2, MB_OK | MB_ICONHAND | MB_SETFOREGROUND);

      MSG *pMsg = AfxGetCurrentMessage();
      if(pMsg)
        pMsg->wParam = 1;  // so the return code is NON-zero
    }
    else
    {
      if(!dlg.m_Finish.DoWizardFinish(&dlg))
      {
        if(!bValidate)
          csTemp2 = "SFT Setup Gizmo - Command Line Error {Unlicensed Copy}";
        else
          csTemp2 = "SFT Setup Gizmo - Command Line Error";

        ::MessageBox(NULL,
                     "ERROR CREATING SETUP",
                     csTemp2, MB_OK | MB_ICONHAND | MB_SETFOREGROUND);

        MSG *pMsg = AfxGetCurrentMessage();
        if(pMsg)
          pMsg->wParam = 1;  // so the return code is NON-zero
      }
    }
  }
  else
  {
	  int nResponse = dlg.DoModal();
	  if (nResponse == IDOK)
	  {
		  // TODO: Place code here to handle when the dialog is
		  //  dismissed with OK
	  }
	  else if (nResponse == IDCANCEL)
	  {
		  // TODO: Place code here to handle when the dialog is
		  //  dismissed with Cancel
	  }
  }

  if(dlg.m_hWnd)
    dlg.DestroyWindow();

  m_pMainWnd = NULL;  // make *SURE* I do this!  If not, it may crash on exit

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

HKEY CSetupGizApp::GetRegistryKey(BOOL bReadOnly)
{
  DWORD dwAccess = KEY_READ;

  if(!bReadOnly)
    dwAccess = KEY_POWERUSER_ACCESS; // KEY_ALL_ACCESS

  HKEY hKey;
  if(RegCreateKeyEx(REGISTRY_KEY_BASE,
                    "SOFTWARE\\Stewart~Frazier Tools\\SFT Setup Gizmo",
                    0, "", REG_OPTION_NON_VOLATILE,
                    dwAccess, NULL, &hKey, NULL)
     == ERROR_SUCCESS)
  {
    return(hKey);
  }

  return(NULL);
}

BOOL CSetupGizApp::ValidateAuthCode(LPCSTR szAuthCode /* = NULL */,
                                    LPCSTR szAuthCompany /* = NULL */)
{
  if(!szAuthCode)
  {
    HKEY hKey = GetRegistryKey(TRUE);

    if(!hKey)
      return(FALSE);

    LPSTR lp1 = m_csAuthCode.GetBufferSetLength(MAX_PATH);

    DWORD dwType, cb1=MAX_PATH;

    if(RegQueryValueEx(hKey, "Registration", NULL, &dwType, (LPBYTE)lp1, &cb1)
       != ERROR_SUCCESS)
    {
      m_csAuthCode.ReleaseBuffer(0);
    }
    else
    {
      m_csAuthCode.ReleaseBuffer(cb1);
      m_csAuthCode.ReleaseBuffer(-1);
    }

    szAuthCode = m_csAuthCode;

    RegCloseKey(hKey);
  }

  if(!szAuthCompany)
  {
    HKEY hKey = GetRegistryKey(TRUE);

    if(!hKey)
      return(FALSE);

    LPSTR lp1 = m_csAuthCompany.GetBufferSetLength(MAX_PATH);

    DWORD dwType, cb1=MAX_PATH;

    if(RegQueryValueEx(hKey, "Company", NULL, &dwType, (LPBYTE)lp1, &cb1)
       != ERROR_SUCCESS)
    {
      m_csAuthCompany.ReleaseBuffer(0);
    }
    else
    {
      m_csAuthCompany.ReleaseBuffer(cb1);
      m_csAuthCompany.ReleaseBuffer(-1);
    }

    szAuthCompany = m_csAuthCompany;

    RegCloseKey(hKey);
  }

  if(!szAuthCode || !*szAuthCode ||
     !szAuthCompany || !*szAuthCompany)
  {
    m_csAuthCode = "";
    m_csAuthCompany = "";

    return(FALSE);
  }

//  // {234711A4-7010-4c81-B1A2-AA01C429C741}
//  static const GUID beta = 
//  { 0x234711a4, 0x7010, 0x4c81, { 0xb1, 0xa2, 0xaa, 0x1, 0xc4, 0x29, 0xc7, 0x41 } };
//
//  CString csTemp;
//  csTemp.Format("%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x",
//                beta.Data1, beta.Data2, beta.Data3,
//                (BYTE)beta.Data4[0],
//                (BYTE)beta.Data4[1],
//                (BYTE)beta.Data4[2],
//                (BYTE)beta.Data4[3],
//                (BYTE)beta.Data4[4],
//                (BYTE)beta.Data4[5],
//                (BYTE)beta.Data4[6],
//                (BYTE)beta.Data4[7]);
//
//  if(!csTemp.CompareNoCase(szAuthCode))
//  {
//    m_csAuthCode = szAuthCode;
//    m_csAuthCompany = "{Beta Release}";
//  }
//  else
//  {
  char szRadix[32];
  CreateRadix(szRadix);  // the seed for the hash (right, it's NOT random)

  // this uses a simple hashing algorithm, not unlike password
  // checks on a POSIX system.  Please do not work around this
  // code or things that make use of it.  Its intent is to allow
  // for licensed copies of the software.  YES you COULD work
  // around it.  YES, you COULD derive your own work that is
  // always "free", all the time, and even in BINARY form on
  // top of that (and if you do, please change the name).

  if(!_stricmp(szAuthCode, AuthCodeEncrypt(szAuthCompany, szRadix)))
  {
    // 'auth company' matches the 'auth code' - it is valid!

    m_csAuthCode = szAuthCode;
    m_csAuthCompany = szAuthCompany;
  }
  else
  {
    m_csAuthCode = "";
    m_csAuthCompany = "";
  }
//  }

  return(m_csAuthCode.GetLength() != 0);
}


// utilities

CString MyGetLongPathName(LPCSTR szShortName)
{
  // assume full path already...

  CString csPath = szShortName;

  int i1 = csPath.ReverseFind('\\');

  CString csDir = i1 > 0 ? csPath.Left(i1 + 1) : "";
  csPath = csPath.Mid(i1 + 1);  // trims off backslash if there is one
  
  if(csDir.GetLength() > 3) // min length for full path's directory
  {
    CString csTemp = MyGetLongPathName(csDir.Left(csDir.GetLength() - 1));

    if(csTemp.GetLength())
      csDir = csTemp + "\\";
  }

  if(csPath.GetLength())
  {
    WIN32_FIND_DATA ff;
    HANDLE hFF = FindFirstFile(csDir + csPath, &ff);

    if(hFF != INVALID_HANDLE_VALUE)  // I found it
    {
      if(ff.cFileName[0])       // it's not blank
        csPath = ff.cFileName;  // the "long name" (always)

      FindClose(hFF);
    }
  }

  return(csDir + csPath);
}

LPCSTR GetBaseName(LPCSTR szFullName)
{
  LPCSTR lpc1 = szFullName + strlen(szFullName);

  while(lpc1 > szFullName && *(lpc1 - 1) != '\\')
  {
    lpc1 --;  // reverse find the first char preceded by '\'
  }

  return lpc1;  // return this pointer, assuming that which was passed in is still good
}

void InPlaceEscapeQuotes(CString &csString)
{
  int i1;

  for(i1=0; i1 < csString.GetLength(); i1++)
  {
    if(csString[i1] == '"')
    {
      csString = csString.Left(i1)
               + '\\'
               + csString.Mid(i1);
      i1++; // skip the '"' so I don't do it again
    }
  }
}

CString ReadFileIntoString(LPCSTR szFile)
{
CString csRval;
char *p1;
DWORD cb1, cb2=0;


  HANDLE hFile = CreateFile(szFile, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);

  if(hFile == INVALID_HANDLE_VALUE)
  {
    return "";
  }

  cb1 = SetFilePointer(hFile, 0, NULL, FILE_END);
  SetFilePointer(hFile, 0, NULL, FILE_BEGIN);

  if(cb1 == INVALID_SET_FILE_POINTER)
  {
error_return:
    CloseHandle(hFile);
    return "";
  }

  p1 = csRval.GetBufferSetLength(cb1);
  if(!p1)
  {
    goto error_return;
  }

  if(!ReadFile(hFile, p1, cb1, &cb2, NULL) ||
     cb1 != cb2)
  {
    goto error_return;
  }

  CloseHandle(hFile);

  csRval.ReleaseBufferSetLength(cb1);

  return csRval;
}

CString GetCurrentDirectory(void)
{
CString csRval;

  if(!GetCurrentDirectory(MAX_PATH, csRval.GetBufferSetLength(MAX_PATH)))
  {
    return ""; // error return
  }

  csRval.ReleaseBuffer(-1);

  return csRval;
}





#ifdef _DEBUG
static const char radix_str[] = "012345678ABCDEFGHJKMNPRSTUVWXYZ";        // base 31 character set
static const char charset_str[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"; // entire character set
#endif // _DEBUG


void CreateRadix(char szRadix[32])
{
  register int i1, i2;

  // to make reverse engineering a pain in the ass, build 'radix_str' in a loop.
  // and yet, here I am OPEN SOURCING it.  Oh, well, unzipped my fly...

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

CString AuthCodeEncrypt(LPCSTR szCompany, LPCSTR szRadix)
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

  // this is the 'private key' - I hope you like it
  // it doesn't really do a whole lot considering that the hash
  // is only the equivalent of about 39 bits... which is rather
  // pathetic but there you go.  Sure, now that it's open source,
  // ANYBODY could 'crack' this and issue authentication codes, but
  // I ask really really NICE that NOBODY do this, ok?  Like that would
  // do a lot of good, but STILL, it's open source now, and like it is
  // with SHAREWARE, you can use the program without registering it under
  // the license terms.  Registering is a form of 'supporting the arts'
  // and is purely voluntary, just like "please leave this alone" is
  // voluntary.  Same idea.

  static const GUID iid = {0x234711a4,0x7010,0x4c81,{0xb1,0xa2,0xaa,0x01,0xc4,0x29,0xc7,0x41}};
  csTemp.Format("%08X%04X%04X%02X%02X%02X%02X%02X%02X%02X%02X",
                iid.Data1, iid.Data2, iid.Data3,
                (BYTE)iid.Data4[0],
                (BYTE)iid.Data4[1],
                (BYTE)iid.Data4[2],
                (BYTE)iid.Data4[3],
                (BYTE)iid.Data4[4],
                (BYTE)iid.Data4[5],
                (BYTE)iid.Data4[6],
                (BYTE)iid.Data4[7]);

  CString csB = csTemp + csCompany;
  CString csC = csB.Left(7); // the 'seed'

  int i2 = 0;

  // build the 'szCharset' array, 0-9 and A-Z
  for(i1='0', i2=0; i1 <= '9'; i1++)
    szCharset[i2++] = (char)i1;

  for(i1='A'; i1 <= 'Z'; i1++)
    szCharset[i2++] = (char)i1;

  szCharset[i2] = 0;
  ASSERT(i2 == 36 && !_stricmp(charset_str, szCharset));

  int nRadix = strlen(szRadix); // this should be 31

  // this is a pretty simple hashing algorithm.  It's easy
  // to implement and not patented (as far as I know).
  for(i1=7; i1 < csB.GetLength(); i1++)
  {
    // previous 7 'digits' and their position within 'szCharset'
    // note that 'szCharset' is 0-9 and A-Z, which differs a bit
    // from 'szRadix', which is 'missing' a few.  The result is
    // that there are 'gaps' in 'szRadix', and the index values
    // from 'szCharset' will reflect that.  It makes the algorithm
    // harder to reverse engineer, but it's open source now and
    // I suspect this is rather pointless...
    UINT c8 = ((CString)szCharset).Find(csC[i1 - 7]) + 1;
    UINT c7 = ((CString)szCharset).Find(csC[i1 - 6]) + 1;
    UINT c6 = ((CString)szCharset).Find(csC[i1 - 5]) + 1;
    UINT c5 = ((CString)szCharset).Find(csC[i1 - 4]) + 1;
    UINT c4 = ((CString)szCharset).Find(csC[i1 - 3]) + 1;
    UINT c3 = ((CString)szCharset).Find(csC[i1 - 2]) + 1;
    UINT c2 = ((CString)szCharset).Find(csC[i1 - 1]) + 1;
    // 'current digit'
    UINT c1 = ((CString)szCharset).Find(csB[i1]) + 1;

    // the formula (a type of moving window hash)
    // using SHA256 would be an improvement, yeah
    c1 = c1 * c2 + c3 * c4; // won't overflow
    c2 = c5 * c7 + c6 * c8; // won't overflow

    c1 = c1 ^ c2; // yeah the obligatory XOR

    c1 = c1 % nRadix; // 'nRadix' is a prime number (31)

    ASSERT(c1 >= 0 && c1 < (UINT)nRadix);

    csC += szRadix[c1]; // tack onto the end, move the window
  }

  // 'bits' of hash is log2(31 ** 8) or about 39.  yeah, not too great.  so what.
  // the fact that it's *WEAK* means no gummint should care if I post the source

  return(csC.Right(8)); // seed plus hashed output, final 8 characters
}

DWORD Base31(LPCSTR szSource, LPCSTR szRadix)
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

CString Base31(DWORD dwValue, LPCSTR szRadix)
{
  CString csRval = "";

  ASSERT(strlen(szRadix) == 31);

  do
  {
    csRval = szRadix[dwValue % 31] + csRval;

    dwValue /= 31;

  } while(dwValue > 0);

  return(csRval);
}

void InPlaceConvertMultStringToStringList(char *pData, unsigned int cbData)
{
  if(!pData || !cbData)
  {
    return;
  }

  char *lp1 = pData;
  int i1 = 0;

  // inline 'safe' strlen
  while(pData[i1] != 0 && i1 < (int)cbData)
  {
    i1++;
  }

  if(i1 < (int)cbData)
  {
    pData[i1] = 0; // add 2nd terminating 0 byte
    i1++; // strlen(pData) + 1
  }
  else
  {
    pData[cbData - 1] = 0; // make sure it's 0-terminated (even if I truncate)
  }

  // eliminate leading white space

  while((int)(lp1 - pData) < i1 &&
        *lp1 && *lp1 <= ' ')
  {
    lp1++;
  }

  if((int)(lp1 - pData) < i1 &&
      lp1 > pData && *lp1)
  {
    memcpy(pData, lp1,
           i1 - (unsigned int)(lp1 - pData) + 1);
  }
  else if(*pData <= ' ')
  {
    *pData = 0; // end of name list, white space at end

    if(cbData > 1)
    {
      pData[1] = 0;
    }

    return; // nothing but white space
  }

  lp1 = pData; // reset it

  // NOW, separate strings from '"string1" "string2" "string3"' into
  //   string1\0string2\0string3\0\0

  while((int)(lp1 - pData) < i1 && *lp1)
  {
    if(*lp1 == '"') // for now, require double-quote. later, fix this?
    {
      register char quote_mark = *lp1; // reserved, allow different marks

      memcpy(lp1, lp1 + 1,
              i1 - (int)(lp1 - pData));
      // TODO:  make it more more efficient?

      // TODO:  check for escaped quote?
      while((lp1 - pData) < i1 &&
            *lp1 && *lp1 != quote_mark)
      {
        lp1++;
      }

      if((lp1 - pData) >= i1)
      {
        break;
      }

      if(*lp1 == quote_mark)
      {
        *(lp1++) = 0;
      }

      if((lp1 - pData) >= i1)
      {
        break;
      }

      const char *lp2 = lp1;

      while((int)(lp2 - pData) < i1 &&
            *lp2 && *lp2 <= ' ')
      {
        lp2++;
      }

      if((int)(lp2 - pData) < i1 &&
         lp2 > lp1)
      {
        memcpy(lp1, lp2,
               i1 - (int)(lp2 - pData) + 1);
      }
      else if(*lp1 <= ' ')
      {
        *lp1 = 0; // end of name list, white space at end
        break;
      }
    }
    else
    {
      while((lp1 - pData) < i1 &&
            *lp1 && *lp1 > ' ')
      {
        lp1++;
      }

      if((lp1 - pData) >= i1)
      {
        break;
      }

      if(*lp1)
      {
        *(lp1++) = 0;
      }

      if((lp1 - pData) >= i1)
      {
        break;
      }

      const char *lp2 = lp1;

      while((int)(lp2 - pData) < i1 &&
            *lp2 && *lp2 <= ' ')
      {
        lp2++;
      }

      if((lp2 - pData) < i1 &&
         lp2 > lp1 && *lp2)
      {
        memcpy(lp1, lp2,
               i1 - (int)(lp2 - pData) + 1);
      }
      else if(*lp1 <= ' ')
      {
        *lp1 = 0; // end of name list, white space at end
        break;
      }
    }
  }

  // add an extra terminating 0 byte
  if((unsigned int)(lp1 - pData) < cbData)
  {
    *(lp1++) = 0;
  }
}


CString GetCertContextNameThingy(PCCERT_CONTEXT ctx, DWORD dwType, void *pOID = NULL)
{
  CString csRval;
  DWORD cb1, dw1;

  // typically use  CERT_NAME_FRIENDLY_DISPLAY_TYP
  cb1 = CertGetNameString(ctx, dwType, 0, pOID, NULL, 0);

  if(cb1)
  {
    LPSTR lp1 = csRval.GetBufferSetLength(cb1);
    dw1 = CertGetNameString(ctx, dwType, 0, pOID, lp1, cb1);

    csRval.ReleaseBuffer(-1); // don't include the zero byte!

    if(dw1 != cb1)
    {
      csRval = ""; // an error
    }
  }

  return csRval;
}

CString GetCertFileFriendlyName(LPCSTR szCertFile)
{
  CString csRval, csTemp;

  // attempt to open the cert file.  if I can't, simply use the
  // file name as the 'friendly' name.

  csTemp = ReadFileIntoString(szCertFile);

  if(!csTemp.GetLength())
  {
default_return: // error returns can have goto, because it's sensible

    csRval = GetBaseName(szCertFile);
    InPlaceEscapeQuotes(csRval);

    return csRval;
  }

  PCCERT_CONTEXT ctx = CertCreateCertificateContext(X509_ASN_ENCODING,
                                                    (const BYTE *)(LPCSTR)csTemp,
                                                    csTemp.GetLength());

  if(!ctx)
  {
    goto default_return;
  }

  // get the friendly name.  if blank, get the CN
  csRval = GetCertContextNameThingy(ctx, 5/*CERT_NAME_FRIENDLY_DISPLAY_TYPE*/);

  CertFreeCertificateContext(ctx);

  return csRval;
}

