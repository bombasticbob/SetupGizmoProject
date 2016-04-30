    //////////////////////////////////////////////////////////////////////////////
//                                                                          //
//       ____           ____        _      _____  _  _           _          //
//      |  _ \   ___   / ___| __ _ | |__  |  ___|(_)| |  ___    | |__       //
//      | | | | / _ \ | |    / _` || '_ \ | |_   | || | / _ \   | '_ \      //
//      | |_| || (_) || |___| (_| || |_) ||  _|  | || ||  __/ _ | | | |     //
//      |____/  \___/  \____|\__,_||_.__/ |_|    |_||_| \___|(_)|_| |_|     //
//                                                                          //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//       Copyright (c) 1998-2016 by S.F.T. Inc. - All rights reserved       //
//  Use, copying, and distribution of this software are licensed according  //
//    to the GPLv2, LGPLv2, or BSD license, as appropriate (see COPYING)    //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////

#ifndef _DOCABFILE_H_INCLUDED_
#define _DOCABFILE_H_INCLUDED_

// this file assumes C++

#include "setupgiz.h"
#include "FinishPage.h"
#include "SetupGizDlg.h" // for 'SetupFileInfo' struct


CString GetFileEntryLine(SetupFileInfo &sfi);

CString PathFromCommandLine(LPCSTR szCommandLine);

DWORD RunApplication(LPCSTR szCmdLine, LPCSTR szExecDir,
                     LPCSTR szAppName, LPCSTR szWindowTitle = NULL);


// NOTE:  csAuthCode should be BLANK for the 'public' version
//        TODO:  PGP-like encryption to compare value for licensed version

CString MakeDDFFile(CString &csAppName, CString &csCompanyName,
                    CString &csAuthCode, CString &csSourcePath,
                    CString &csDestPath, CString &csExePath,
                    CString &csLicenseFile,
                    CStringArray &acsCertFiles,
                    CStringArray &acsCertRepositories,
                    CArray<SetupFileInfo,SetupFileInfo> &aFileList,
                    CArray<SetupGroupInfo,SetupGroupInfo> &aGroupList,
                    CStringArray &acsRegistry, CString &csStartMessage,
                    CString &csAppPath, CString &csAppCommon,
                    CString &csStartMenu, CStringArray &acsUpgrade,
                    BOOL bNoUninstall, BOOL bQuietSetup, BOOL bLZCompress,
                    int iInstallType, int iDisk1Reserve, int iRebootFlag);


#endif // _DOCABFILE_H_INCLUDED_

