////////////////////////////////////////////////////////////////////////////////////////
//                                                                                    //
//   _____  _         _       _      ____                                             //
//  |  ___|(_) _ __  (_) ___ | |__  |  _ \  __ _   __ _   ___     ___  _ __   _ __    //
//  | |_   | || '_ \ | |/ __|| '_ \ | |_) |/ _` | / _` | / _ \   / __|| '_ \ | '_ \   //
//  |  _|  | || | | || |\__ \| | | ||  __/| (_| || (_| ||  __/ _| (__ | |_) || |_) |  //
//  |_|    |_||_| |_||_||___/|_| |_||_|    \__,_| \__, | \___|(_)\___|| .__/ | .__/   //
//                                                |___/               |_|    |_|      //
//                                                                                    //
////////////////////////////////////////////////////////////////////////////////////////
//                                                                                    //
//            Copyright (c) 1998-2016 by S.F.T. Inc. - All rights reserved            //
//       Use, copying, and distribution of this software are licensed according       //
//         to the GPLv2, LGPLv2, or BSD license, as appropriate (see COPYING)         //
//                                                                                    //
////////////////////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "setupgiz.h"
#include "FinishPage.h"
#include "SetupGizDlg.h"
#include "DoCabFile.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#if 0 /* moved to DoCabFile.h */
CString GetFileEntryLine(SetupFileInfo &sfi);

DWORD RunApplication(LPCSTR szCmdLine, LPCSTR szExecDir,
                     LPCSTR szAppName, LPCSTR szWindowTitle = NULL);
#endif // 0

//BOOL UpdateSetupExeResource(CString &csDestPath);

BOOL CreateAutoRunFiles(LPCSTR szExePath, LPCSTR szDestPath, BOOL bHasImages);

/////////////////////////////////////////////////////////////////////////////
// CFinishPage property page

IMPLEMENT_DYNCREATE(CFinishPage, CPropertyPage)

CFinishPage::CFinishPage() : CPropertyPage(CFinishPage::IDD)
{
  //{{AFX_DATA_INIT(CFinishPage)
  m_bAutoInsert = FALSE;
  m_iInstallType = -1;
  m_bDoNotClose = FALSE;
  m_bLZCompress = FALSE;
  m_bNoUninstall = FALSE;
  m_bQuietSetup = FALSE;
  m_bDoNotDelWorkFiles = FALSE;
  m_bDoNotRebuildCab = FALSE;
  m_iRebootFlag = -1;
  m_iDisk1Reserve = 0;
  //}}AFX_DATA_INIT

  DoClear();
}

CFinishPage::~CFinishPage()
{
}

void CFinishPage::DoClear()
{
  m_bAutoInsert = FALSE;
  m_iInstallType = -1;
  m_bDoNotClose = FALSE;
  m_bLZCompress = FALSE;
  m_bNoUninstall = FALSE;
  m_bQuietSetup = FALSE;
  m_bDoNotDelWorkFiles = FALSE;
  m_bDoNotRebuildCab = FALSE;

  m_iInstallType = 0;  // default is CDROM image
  m_iDisk1Reserve = 0;  // default is NONE

  m_iRebootFlag = 0;   // default is AUTO
}

void CFinishPage::DoDataExchange(CDataExchange* pDX)
{
  if(!m_hWnd)
    return;  // don't do anything if there's no window (to avoid trouble)

  CPropertyPage::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CFinishPage)
  DDX_Control(pDX, IDC_DISK1_RESERVE, m_wndDisk1Reserve);
  DDX_Control(pDX, IDC_AUTOINSERT, m_btnAutoInsert);
  DDX_Check(pDX, IDC_AUTOINSERT, m_bAutoInsert);
  DDX_Radio(pDX, IDC_CDROM, m_iInstallType);
  DDX_Check(pDX, IDC_DO_NOT_CLOSE, m_bDoNotClose);
  DDX_Check(pDX, IDC_LZCOMPRESS, m_bLZCompress);
  DDX_Check(pDX, IDC_NO_UNINSTALL, m_bNoUninstall);
  DDX_Check(pDX, IDC_QUIET_SETUP, m_bQuietSetup);
  DDX_Check(pDX, IDC_DO_NOT_DEL_WORKFILES, m_bDoNotDelWorkFiles);
  DDX_Check(pDX, IDC_DO_NOT_REBUILD_CAB, m_bDoNotRebuildCab);
  DDX_Radio(pDX, IDC_BOOT_AUTO, m_iRebootFlag);
  DDX_Text(pDX, IDC_DISK1_RESERVE, m_iDisk1Reserve);
  DDV_MinMaxInt(pDX, m_iDisk1Reserve, 0, 1024);
  //}}AFX_DATA_MAP

//  if(m_iInstallType == 0 || m_iInstallType == 1)
//  {
//    m_btnAutoInsert.EnableWindow(1);
//  }
//  else
//  {
//    m_btnAutoInsert.EnableWindow(0);
//  }

  if(m_iInstallType == 1 || m_iInstallType == 2)
  {
    m_wndDisk1Reserve.EnableWindow(1);
  }
  else
  {
    m_wndDisk1Reserve.EnableWindow(0);
  }
}


BEGIN_MESSAGE_MAP(CFinishPage, CPropertyPage)
  //{{AFX_MSG_MAP(CFinishPage)
  ON_BN_CLICKED(IDC_SAVE, OnSave)
  ON_BN_CLICKED(IDC_CDROM, OnCdrom)
  ON_BN_CLICKED(IDC_CDROM_MULTI, OnCdromMulti)
  ON_BN_CLICKED(IDC_DISKETTE, OnDiskette)
  ON_BN_CLICKED(IDC_SELF_EXTRACT, OnSelfExtract)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFinishPage message handlers

BOOL CFinishPage::OnSetActive() 
{
  CSetupGizDlg *pSheet = CSetupGizDlg::GetPropertySheet(this);

  if(pSheet)
  {
    pSheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_FINISH);
  }

  return CPropertyPage::OnSetActive();
}

LRESULT CFinishPage::OnWizardBack() 
{
  UpdateData(1);
  
  return CPropertyPage::OnWizardBack();
}

BOOL CFinishPage::OnKillActive() 
{
  // TODO: Add your specialized code here and/or call the base class
  
  return CPropertyPage::OnKillActive();
}

void CFinishPage::OnSave() 
{
  CWaitCursor wait;

  CSetupGizDlg *pSheet = CSetupGizDlg::GetPropertySheet(this);

  if(!pSheet)
  {
    AfxMessageBox("Internal initialization error",
                  MB_OK | MB_ICONHAND | MB_SETFOREGROUND);
    return;
  }

  pSheet->DoSave();
}

BOOL CFinishPage::OnWizardFinish() 
{
  UpdateData(1);  // get my options squared away

  CSetupGizDlg *pSheet = CSetupGizDlg::GetPropertySheet(this);

  if(!pSheet)
  {
    AfxMessageBox("Internal initialization error",
                  MB_OK | MB_ICONHAND | MB_SETFOREGROUND);
    return(0);  // so I stay visible
  }

  if(!DoWizardFinish(pSheet))
    return(0);    // i.e. "do not 'finish' this wizard"

  // at this point 'DoWizardFinish' worked properly.
  // now I check to see if I close the dialog or not, etc. etc.

  if(!m_bDoNotClose)
  {
    if(!pSheet->m_csFileName.GetLength())
    {
      int i1 = AfxMessageBox("Save configuration before closing?",
                             MB_YESNOCANCEL | MB_ICONQUESTION | MB_SETFOREGROUND);

      if(i1 == IDCANCEL)
      {
        return(0);
      }
      else if(i1 == IDYES)
      {
        if(!pSheet->DoSave())
          return(0);  // user canceled
      }
    }

    return CPropertyPage::OnWizardFinish();
  }
  else
    return(0);  // so I stay visible
}


void CFinishPage::OnCdrom() 
{
  // this is now DVD
  UpdateData(1);  
}

void CFinishPage::OnCdromMulti() 
{
  // this is now DVD multi
  UpdateData(1);  
}

void CFinishPage::OnDiskette() 
{
  // NOTE:  this is now 'CDROM'; CD is now DVD
  UpdateData(1);  
}

void CFinishPage::OnSelfExtract() 
{
  UpdateData(1);  
}



// *****************************
// UTILITY NON-MEMBER FUNCTIONS
// *****************************

// UNUSED UTILITIES (left here for reference)

BOOL UpdateSetupExeResource(CString &csDestPath)
{
  // '_SETUP.INF' must be added as a resource 'SETUP_INF' type 'SETUP'
  // and 'SFTCAB.1' must be added as 'SFTCAB_1' type 'SETUP'.

  HANDLE h1 = BeginUpdateResource(csDestPath + "SETUP.EXE", FALSE);

  if(!h1)
  {
    DWORD dw1 = GetLastError();

    AfxMessageBox("Unable to update resources in SETUP.EXE!!!",
                  MB_OK | MB_ICONHAND | MB_SETFOREGROUND);
    return(0);
  }
                                  
  CString csBuf;
  HANDLE hFile;
  DWORD cb1, cbFile;

  // open '_SETUP.INF', read it, and write it to the resource.

  hFile = CreateFile(csDestPath + "_SETUP.INF",
                     GENERIC_READ, 0, NULL, OPEN_EXISTING,
                     FILE_ATTRIBUTE_NORMAL, NULL);

  if(hFile == INVALID_HANDLE_VALUE)
  {
    AfxMessageBox("Unable to open '_SETUP.INF'!!!",
                  MB_OK | MB_ICONHAND | MB_SETFOREGROUND);
    EndUpdateResource(h1, TRUE);  // discards changes
    return(0);
  }

  // find out size of file, then read the whole thing into 'm_csINFFile'

  cbFile = GetFileSize(hFile, NULL);

  if(!ReadFile(hFile, csBuf.GetBufferSetLength(cbFile),
               cbFile, &cb1, NULL)
     || cb1 != cbFile)
  {
    csBuf.ReleaseBuffer(0);
    CloseHandle(hFile);

    AfxMessageBox("Read error on 'SETUP.INF' - SETUP will now close",
                  MB_OK | MB_ICONHAND | MB_SETFOREGROUND);

    EndUpdateResource(h1, TRUE);  // discards changes
    return(FALSE);
  }
            
  csBuf.ReleaseBuffer(cbFile);
  CloseHandle(hFile);

  if(!UpdateResource(h1, "SETUP", "SETUP_INF",
                     MAKELANGID(LANG_NEUTRAL,SUBLANG_NEUTRAL),
                     (LPVOID)(LPCSTR)csBuf, cbFile))
  {
    AfxMessageBox("Unable to update 'SETUP_INF' resource - SETUP will now close",
                  MB_OK | MB_ICONHAND | MB_SETFOREGROUND);

    EndUpdateResource(h1, TRUE);  // discards changes
    return(FALSE);
  }



  // open 'SFTCAB.1', read it, and write it to the resource.

  hFile = CreateFile(csDestPath + "SFTCAB.1",
                     GENERIC_READ, 0, NULL, OPEN_EXISTING,
                     FILE_ATTRIBUTE_NORMAL, NULL);

  if(hFile == INVALID_HANDLE_VALUE)
  {
    AfxMessageBox("Unable to open 'SFTCAB.1'!!!",
                  MB_OK | MB_ICONHAND | MB_SETFOREGROUND);
    EndUpdateResource(h1, TRUE);  // discards changes
    return(0);
  }

  // find out size of file, then read the whole thing into 'm_csINFFile'

  cbFile = GetFileSize(hFile, NULL);

  if(!ReadFile(hFile, csBuf.GetBufferSetLength(cbFile),
               cbFile, &cb1, NULL)
     || cb1 != cbFile)
  {
    csBuf.ReleaseBuffer(0);
    CloseHandle(hFile);

    AfxMessageBox("Read error on 'SFTCAB.1' - SETUP will now close",
                  MB_OK | MB_ICONHAND | MB_SETFOREGROUND);

    EndUpdateResource(h1, TRUE);  // discards changes
    return(FALSE);
  }
            
  csBuf.ReleaseBuffer(cbFile);
  CloseHandle(hFile);

  if(!UpdateResource(h1, "SETUP", "SFTCAB_1",
                     MAKELANGID(LANG_NEUTRAL,SUBLANG_NEUTRAL),
                     (LPVOID)(LPCSTR)csBuf, cbFile))
  {
    AfxMessageBox("Unable to update 'SETUP_INF' resource - SETUP will now close",
                  MB_OK | MB_ICONHAND | MB_SETFOREGROUND);

    EndUpdateResource(h1, TRUE);  // discards changes
    return(FALSE);
  }


  // ** FINALLY ** I am done with it!

  if(!EndUpdateResource(h1, FALSE))
  {
    AfxMessageBox("Unable to commit updated resources in SETUP.EXE!!!",
                  MB_OK | MB_ICONHAND | MB_SETFOREGROUND);
    return(0);
  }

  return(TRUE);
}



// *************************************
// ****** WRITING THE OUTPUT FILE ******
// *************************************

BOOL CFinishPage::DoWizardFinish(CSetupGizDlg *pSheet)
{
  if(!pSheet)
    return(FALSE);

  CWaitCursor wait;

  // FIRST, get the initial program elements in place.

  DWORD dwDisk1Size = 0;  // current size of disk #1


  // CREATE A SET OF 'MAKECAB' COMMANDS AND EXECUTE THEM
  // Yes, THAT would be the '.DDF' file.

  CString csTemp, csTemp2, csDDF, csExePath, csSourcePath, csDestPath;
  int i1 /*, i2, i3*/;

  // INITIALIZATION - paths

  csSourcePath = pSheet->m_Page1.m_csSourcePath;
  csDestPath = pSheet->m_Page1.m_csDestPath;

  if(csSourcePath[csSourcePath.GetLength() - 1] != '\\')
    csSourcePath += '\\';

  if(csDestPath[csDestPath.GetLength() - 1] != '\\')
    csDestPath += '\\';

  GetModuleFileName(NULL, csExePath.GetBufferSetLength(MAX_PATH), MAX_PATH);
  csExePath.ReleaseBuffer(-1);
  i1 = csExePath.ReverseFind('\\');

  csExePath = csExePath.Left(i1 + 1);  // include backslash, assume it was found

  CString csAuthCode;

  if(theApp.m_csAuthCompany.GetLength() &&
     theApp.m_csAuthCode.GetLength())  // authorized copy
  {
    // this tells SETUP that it's an authorized version.  SETUP will
    // use the company name to decode the authorization string, and
    // display the registering company's name.  Anything else will
    // display "UNREGISTERED COPY" nice and large, in yellow.


    // using the company name as specified in the wizard,
    // produce a hexadecimal version of the company name
    // and the authorization code, 'XOR'ed.

    char szRadix[32];
    CreateRadix(szRadix);

    // tagging the INF file with an 'Auth Code' to prove it's registered
    if(theApp.m_csAuthCompany.GetLength() && theApp.m_csAuthCode.GetLength() == 8)
    {
      csTemp2 = ("00" + Base31(theApp.m_csAuthCompany.GetLength()
                               + theApp.m_csAuthCode.GetLength()
                               + 2, szRadix)).Right(2)
              + theApp.m_csAuthCompany
              + theApp.m_csAuthCode;
    }
    else
    {
      csTemp2 = theApp.m_csAuthCode;  // assume 'beta' authorization
    }

    csTemp = pSheet->m_Page1.m_csCompanyName;

    while(csTemp.GetLength() < csTemp2.GetLength())
      csTemp += ' ';  // extend using white space

    csTemp = csTemp.Left(csTemp2.GetLength());  // make sure they match

    for(i1=0; i1 < csTemp2.GetLength(); i1++)
    {
      BYTE b1 = (BYTE)csTemp[i1] ^ (BYTE)csTemp2[i1];

      if(i1 & 7)  // rotation using the index
        b1 = (b1 << (i1 & 7)) | (b1 >> (8 - (i1 & 7)));

      // 'xor' with previous entry

      if(i1)
        b1 ^= (BYTE)csTemp[i1 - 1];

      // generate the hex equivalent as the 'Auth Code'

      static const char szHex[]="0123456789ABCDEF";

      csAuthCode += szHex[(b1 >> 4) & 0xf];
      csAuthCode += szHex[b1 & 0xf];
    }
  }

  // big chunk of code moved elsewhere so I don't clutter
  // up this thing, which should be a simple message handler
  // It builds the text of the DDF file based on everything
  // you see below.

  csDDF = MakeDDFFile(pSheet->m_Page1.m_csAppName,
                      pSheet->m_Page1.m_csCompanyName,
                      csAuthCode,
                      csSourcePath,
                      csDestPath,
                      csExePath,
                      pSheet->m_Page1.m_csLicenseFile,
                      pSheet->m_acsCertFileNameList,
                      pSheet->m_acsCertRepositoryList,
                      pSheet->m_aFileList,
                      pSheet->m_aGroupList,
                      pSheet->m_acsRegistry,
                      pSheet->m_Page1.m_csStartMessage,
                      pSheet->m_Page1.m_csAppPath,
                      pSheet->m_Page1.m_csAppCommon,
                      pSheet->m_Page1.m_csStartMenu,
                      pSheet->m_Page1.m_acsUpgrade,
                      m_bNoUninstall,
                      m_bQuietSetup,
                      m_bLZCompress,
                      m_iInstallType,
                      m_iDisk1Reserve,
                      m_iRebootFlag);


  // ******************************************************************
  //
  // FINALLY, write this bad boy to the destination path
  //
  // ******************************************************************

  CreateDirectory(csDestPath, NULL); // if output directory does NOT exist, create it NOW

  HANDLE hFile = CreateFile(csDestPath + "SETUPGIZ.DDF",
                            GENERIC_READ | GENERIC_WRITE,
                            0, NULL, CREATE_ALWAYS,
                            FILE_ATTRIBUTE_NORMAL, NULL);

  if(hFile == INVALID_HANDLE_VALUE)
  {
    AfxMessageBox("Unable to create output ('.DDF') file",
                  MB_OK | MB_ICONHAND | MB_SETFOREGROUND);
    return(FALSE);  // failed
  }

  DWORD cb1;

  if(!WriteFile(hFile, (LPCSTR)csDDF, csDDF.GetLength(), &cb1, NULL)
     || cb1 != (DWORD)csDDF.GetLength())
  {
    CloseHandle(hFile);

    AfxMessageBox("Write error on output ('.DDF') file",
                  MB_OK | MB_ICONHAND | MB_SETFOREGROUND);
    return(FALSE);  // failed
  }

  CloseHandle(hFile);


  // based on the install type, create output files

  if(m_bAutoInsert)
  {
    if(m_iInstallType == 0 || m_iInstallType == 1)      // DVD ROM
    {
      // for 'autorun' CD image, I need an 'AUTORUN.INF' file.
      //
      // [autorun]
      // OPEN=path\SETUP.EXE
      // ICON=path\SETUP.EXE,1
      //
      // (seems to work best when 'SETUP.EXE' has no "path\" preceeding it)

      if(!CreateAutoRunFiles(csExePath, csDestPath, m_iInstallType == 1))
      {
        return FALSE;
      }

      // for a 'DVD with CD Images', Disk1 needs these as well, but configured for Disk 1's file system

      if(m_iInstallType == 1)
      {
        if(!CreateAutoRunFiles(csExePath, csDestPath + "Disk1\\", FALSE))
        {
          return FALSE;
        }
      }
    }
    else if(m_iInstallType == 2)
    {
      if(!CreateAutoRunFiles(csExePath, csDestPath + "Disk1\\", FALSE))
      {
        return FALSE;
      }
    }
  }

  // NOW, invoke 'MAKECAB' with the correct command line

  // NOTE:  it used to be important in *this* case to make sure that 'csDestPath'
  //        represents a SHORT path name.  This may not be needed any more...

  CString csDDFFile;

  if(!GetShortPathName(csDestPath + "SETUPGIZ.DDF", csDDFFile.GetBufferSetLength(MAX_PATH), MAX_PATH))
  {
    csDDFFile.ReleaseBuffer(0);
    csDDFFile = csDestPath + "SETUPGIZ.DDF";
  }
  else
  {
    csDDFFile.ReleaseBuffer(-1);
  }

  CString csEXE = /*csExePath +*/ "MAKECAB.EXE"; // it's part of the OS now
  CString csParm = "/V1 /F " + csDDFFile;

  int iEndOfPath = csEXE.ReverseFind('\\');

  BOOL bWasExeError = FALSE;

  CString csMakeCabTitle = pSheet->m_csFileName;
  if(csMakeCabTitle.GetLength())
  {
    int i9 = csMakeCabTitle.ReverseFind('\\');

    if(i9 >= 0)
      csMakeCabTitle = csMakeCabTitle.Mid(i9 + 1);
  }

  if(csMakeCabTitle.GetLength())
    csMakeCabTitle = "MAKECAB - "
                   + pSheet->m_Page1.m_csAppName
                   + " - "
                   + csMakeCabTitle;
  else
    csMakeCabTitle = "MAKECAB - "
                   + pSheet->m_Page1.m_csAppName;


  // MAKE SURE that I delete any old files named 'setup.exe' because
  // for some damn reason, it gets 'pooched' during the copy...

  if(m_iInstallType == 0)                              // DVD image
  {
    DeleteFile(csDestPath + "SETUP\\setup.exe");
    DeleteFile(csDestPath + "SETUP\\uninst.exe");
  }
  else if(m_iInstallType == 1 || m_iInstallType == 2)  // CD or DVD with CD images
  {
    DeleteFile(csDestPath + "DISK1\\setup.exe");
    DeleteFile(csDestPath + "DISK1\\uninst.exe");
  }

  // MAKECAB COMMAND LINE:
  //   makecab.exe /V1 /F SETUPGIZ.DDF

  if(!m_bDoNotRebuildCab)
  {
    if(RunApplication("\"" + csEXE + "\" " + csParm, // command
                      (iEndOfPath > 0 ? (LPCSTR)csEXE.Left(iEndOfPath) : "."), // directory excluding backslash
                      "MAKECAB", csMakeCabTitle)) // window title etc.
    {
      AfxMessageBox((CString)_T("Run Application failed\r\n") + "\"" + csEXE + "\" " + csParm);
      bWasExeError = TRUE;
    }
  }

  // NOTE:  'RunApplication' waits until the above process has completed

  // FINALLY, copy the '.inf' file to the correct directory
  // using the correct name.

  BOOL bCopyFail = FALSE;

  if(m_iInstallType == 0)                             // DVD image
  {
    // re-copy SETUP.EXE to make sure it's not 'pooched'

    bCopyFail = !CopyFile(csExePath + "setup.exe",
                          csDestPath + "SETUP\\setup.exe",
                          FALSE)
             || !CopyFile(csDestPath + "_SETUP.INF",
                          csDestPath + "SETUP\\SETUP.INF",
                          FALSE);

    if(!bCopyFail && !m_bDoNotDelWorkFiles)
    {
      DeleteFile(csDestPath + "_SETUP.INF");  // get rid of it (don't want it on image!)
      DeleteFile(csDestPath + "_SETUP.RPT");  // and this one too! (since it worked)
    }
  }
  else if(m_iInstallType == 1 || m_iInstallType == 2)  // CD or DVD with CD images
  {
    bCopyFail = !CopyFile(csExePath + "setup.exe",
                          csDestPath + "DISK1\\setup.exe",
                          FALSE)
             || !CopyFile(csDestPath + "_SETUP.INF",
                          csDestPath + "DISK1\\SETUP.INF",
                          FALSE);

    if(!bCopyFail && m_iInstallType == 1 && !m_bDoNotDelWorkFiles)     // CDROM image
    {
      DeleteFile(csDestPath + "_SETUP.INF");  // get rid of it (don't want it on image!)
      DeleteFile(csDestPath + "_SETUP.RPT");  // and this one too! (since it worked)
    }
  }
  else if(m_iInstallType == 3)                        // self-extractor
  {
    // NOW, I get to do some 'funny' stuff... 

    // create an EXE that's self-extracting by appending the
    // 'SETUP.INF' file followed by 'LICENSE.TXT' followed by 
    // 'SFTCAB.1' to 'SETUP.EXE' in the following manner:

    bCopyFail = !CopyFile(csExePath + "setup.exe",
                          csDestPath + "setup.exe",
                          FALSE);

    if(bCopyFail) // typically when I'm debugging/developing
    {
      bCopyFail = !CopyFile(csExePath + "..\\setup\\Release\\setup.exe",
                            csDestPath + "setup.exe",
                            FALSE);
    }

    if(bCopyFail)
    {
      AfxMessageBox("Unable to write 'SETUP.EXE'!!!",
                    MB_OK | MB_ICONHAND | MB_SETFOREGROUND);

      return(FALSE);  // failed
    }

    CString csTemp, csInf, csLicense, csBuf;
    HANDLE hFile;
    DWORD cbFile, cb1, cb2;

    hFile = CreateFile(csDestPath + "_SETUP.INF",
                       GENERIC_READ, 0, NULL, OPEN_EXISTING,
                       FILE_ATTRIBUTE_NORMAL, NULL);

    if(hFile == INVALID_HANDLE_VALUE)
    {
      AfxMessageBox("Unable to open '_SETUP.INF'!!!",
                    MB_OK | MB_ICONHAND | MB_SETFOREGROUND);

      return(FALSE);  // failed
    }

    // find out size of file, then read it

    cbFile = GetFileSize(hFile, NULL);

    if(!ReadFile(hFile, csInf.GetBufferSetLength(cbFile),
                 cbFile, &cb1, NULL)
       || cb1 != cbFile)
    {
      csInf.ReleaseBuffer(0);
      CloseHandle(hFile);

      AfxMessageBox("Read error on 'SETUP.INF'",
                    MB_OK | MB_ICONHAND | MB_SETFOREGROUND);
      return(FALSE);
    }
            
    csInf.ReleaseBuffer(cbFile);
    csInf.ReleaseBuffer(-1);  // this trims null bytes

    CloseHandle(hFile);


    if(pSheet->m_Page1.m_csLicenseFile.GetLength())
    {
      // A license file, if it exists, must be called 'license.txt'
      // and exist on disk #1 along with 'setup.inf'.

      hFile = CreateFile(pSheet->m_Page1.m_csLicenseFile,
                         GENERIC_READ, 0, NULL, OPEN_EXISTING,
                         FILE_ATTRIBUTE_NORMAL, NULL);

      if(hFile == INVALID_HANDLE_VALUE)
      {
        AfxMessageBox("Unable to open 'LICENSE.TXT'!!!",
                      MB_OK | MB_ICONHAND | MB_SETFOREGROUND);

        return(FALSE);  // failed
      }

      // find out size of file, then read it

      cbFile = GetFileSize(hFile, NULL);

      if(!ReadFile(hFile, csLicense.GetBufferSetLength(cbFile),
                   cbFile, &cb1, NULL)
         || cb1 != cbFile)
      {
        csLicense.ReleaseBuffer(0);
        CloseHandle(hFile);

        AfxMessageBox("Read error on 'SETUP.INF'",
                      MB_OK | MB_ICONHAND | MB_SETFOREGROUND);
        return(FALSE);
      }
            
      csLicense.ReleaseBuffer(cbFile);
      csLicense.ReleaseBuffer(-1);  // this trims null bytes

      CloseHandle(hFile);
    }

    hFile = CreateFile(csDestPath + "SFTCAB.1",
                       GENERIC_READ, 0, NULL, OPEN_EXISTING,
                       FILE_ATTRIBUTE_NORMAL, NULL);

    if(hFile == INVALID_HANDLE_VALUE)
    {
      AfxMessageBox("Unable to open '_SETUP.INF'!!!",
                    MB_OK | MB_ICONHAND | MB_SETFOREGROUND);

      return(FALSE);  // failed
    }

    // find out size of file, then read it

    cbFile = GetFileSize(hFile, NULL);

    if(!ReadFile(hFile, csBuf.GetBufferSetLength(cbFile),
                 cbFile, &cb1, NULL)
       || cb1 != cbFile)
    {
      csBuf.ReleaseBuffer(0);
      CloseHandle(hFile);

      AfxMessageBox("Read error on 'SFTCAB.1'",
                    MB_OK | MB_ICONHAND | MB_SETFOREGROUND);
      return(FALSE);
    }
            
    csBuf.ReleaseBuffer(cbFile);
    CloseHandle(hFile);


    // OK, full plate - I got 'SFTCAB.1' and I got 'SETUP.INF'
    // all put together here, ready for packing.

    // NOW, open 'SETUP.EXE', append a "thingie" to it that marks
    // the size of the INF, the INF text itself, followed by the
    // 'CAB' file data.


    hFile = CreateFile(csDestPath + "SETUP.EXE",
                       GENERIC_READ | GENERIC_WRITE,
                       0, NULL, OPEN_EXISTING,
                       FILE_ATTRIBUTE_NORMAL, NULL);

    if(hFile == INVALID_HANDLE_VALUE)
    {
      AfxMessageBox("Unable to open 'SETUP.EXE'!!!",
                    MB_OK | MB_ICONHAND | MB_SETFOREGROUND);

      return(FALSE);  // failed
    }

    SetFilePointer(hFile, 0, NULL, FILE_END);  // point to the end

    // write the length of the INF file

    cb2 = csInf.GetLength();

    if(!WriteFile(hFile, &cb2, sizeof(cb2), &cb1, NULL)
       || cb1 != sizeof(cb2))
    {
      CloseHandle(hFile);
      AfxMessageBox("WRITE error on 'SETUP.EXE'",
                    MB_OK | MB_ICONHAND | MB_SETFOREGROUND);
      return(FALSE);  // failed
    }
            
    // write the INF text (no extra NULL byte either)

    if(!WriteFile(hFile, (LPCSTR)csInf, cb2, &cb1, NULL)
       || cb1 != cb2)
    {
      CloseHandle(hFile);
      AfxMessageBox("WRITE error on 'SETUP.EXE'",
                    MB_OK | MB_ICONHAND | MB_SETFOREGROUND);
      return(FALSE);  // failed
    }

    // write the length of the LICENSE file

    cb2 = csLicense.GetLength();

    if(!WriteFile(hFile, &cb2, sizeof(cb2), &cb1, NULL)
       || cb1 != sizeof(cb2))
    {
      CloseHandle(hFile);
      AfxMessageBox("WRITE error on 'SETUP.EXE'",
                    MB_OK | MB_ICONHAND | MB_SETFOREGROUND);
      return(FALSE);  // failed
    }
            
    // write the LICENSE text if it's not blank

    if(csLicense.GetLength() &&
       (!WriteFile(hFile, (LPCSTR)csLicense, cb2, &cb1, NULL)
        || cb1 != cb2))
    {
      CloseHandle(hFile);
      AfxMessageBox("WRITE error on 'SETUP.EXE'",
                    MB_OK | MB_ICONHAND | MB_SETFOREGROUND);
      return(FALSE);  // failed
    }

    // write the CAB file

    if(!WriteFile(hFile, (LPCSTR)csBuf, cbFile, &cb1, NULL)
       || cb1 != cbFile)
    {
      CloseHandle(hFile);
      AfxMessageBox("WRITE error on 'SETUP.EXE'",
                    MB_OK | MB_ICONHAND | MB_SETFOREGROUND);
      return(FALSE);  // failed
    }

    CloseHandle(hFile);  // I'm done with it now!  YAY!

  }
  else  // everything else
  {
    // TODO:  whatever
  }


  if(bCopyFail)
  {
    AfxMessageBox("COPY error during final processing - disk image not complete",
                  MB_OK | MB_ICONHAND | MB_SETFOREGROUND);
    return(FALSE);  // failed
  }


  // ** FINALLY, delete the '.ddf' file!!! ***

  if(!bWasExeError && !m_bDoNotDelWorkFiles)
  {
    DeleteFile(csDestPath + "SETUPGIZ.DDF");
  }

  return(TRUE);  // OK!
}


BOOL CreateAutoRunFiles(LPCSTR szExePath, LPCSTR szDestPath, BOOL bHasImages)
{
HANDLE hFile;
DWORD cb1;
CString csDDF, csExePath, csDestPath;

  csExePath = szExePath;
  csDestPath = szDestPath;

  CreateDirectory(csDestPath, NULL); // if output directory does NOT exist, create it NOW

  hFile = CreateFile(csDestPath + "AUTORUN.INF",
                     GENERIC_READ | GENERIC_WRITE,
                     0, NULL, CREATE_ALWAYS,
                     FILE_ATTRIBUTE_NORMAL, NULL);

  if(hFile == INVALID_HANDLE_VALUE)
  {
    AfxMessageBox("Unable to create output ('AUTORUN.INF') file for CD 'autoinsert' notification",
                  MB_OK | MB_ICONHAND | MB_SETFOREGROUND);
    return(FALSE);  // failed
  }

  if(bHasImages)
  {
    csDDF = "[autorun]\r\n"
            "OPEN=AUTORUN\\AUTORUN.EXE \\DISK1\\SETUP.EXE\r\n"
            "ICON=AUTORUN\\AUTORUN.ICO\r\n\r\n";
  }
  else // if(m_iInstallType == 0)
  {
    csDDF = "[autorun]\r\n"
            "OPEN=AUTORUN\\AUTORUN.EXE \\SETUP\\SETUP.EXE\r\n"
            "ICON=AUTORUN\\AUTORUN.ICO\r\n\r\n";
  }

  if(!WriteFile(hFile, (LPCSTR)csDDF, csDDF.GetLength(), &cb1, NULL)
      || cb1 != (DWORD)csDDF.GetLength())
  {
    CloseHandle(hFile);

    AfxMessageBox("Write error on output ('AUTORUN.INF') file",
                  MB_OK | MB_ICONHAND | MB_SETFOREGROUND);
    return(FALSE);  // failed
  }

  CloseHandle(hFile);

  CreateDirectory(csDestPath + "AUTORUN", NULL);  // make sure it exists

  CString csBitmap = csExePath + "autorun.bmp";
  CString csIcon = csExePath + "autorun.ico";

  // TODO:  allow user to specify icon and bitmap file names

  if(!CopyFile(csExePath + "autorun.exe", csDestPath + "AUTORUN\\autorun.exe", FALSE) ||
      !CopyFile(csBitmap, csDestPath + "AUTORUN\\autorun.bmp", FALSE) ||
      !CopyFile(csIcon, csDestPath + "AUTORUN\\autorun.ico", FALSE))
  {
    AfxMessageBox("Copy error on 'Autorun CD' support files",
                  MB_OK | MB_ICONHAND | MB_SETFOREGROUND);
    return(FALSE);  // failed
  }

  return TRUE;
}
