///////////////////////////////////////////////////////////////////////////////////
//                                                                               //
//   ____           ____        _      _____  _  _                               //
//  |  _ \   ___   / ___| __ _ | |__  |  ___|(_)| |  ___     ___  _ __   _ __    //
//  | | | | / _ \ | |    / _` || '_ \ | |_   | || | / _ \   / __|| '_ \ | '_ \   //
//  | |_| || (_) || |___| (_| || |_) ||  _|  | || ||  __/ _| (__ | |_) || |_) |  //
//  |____/  \___/  \____|\__,_||_.__/ |_|    |_||_| \___|(_)\___|| .__/ | .__/   //
//                                                               |_|    |_|      //
//                                                                               //
///////////////////////////////////////////////////////////////////////////////////
//                                                                               //
//         Copyright (c) 1998-2016 by S.F.T. Inc. - All rights reserved          //
//     Use, copying, and distribution of this software are licensed according    //
//       to the GPLv2, LGPLv2, or BSD license, as appropriate (see COPYING)      //
//                                                                               //
///////////////////////////////////////////////////////////////////////////////////


#include "StdAfx.h"
#include "DoCabFile.h"

// NOTE: this ginormous 'do it all' function sort of has to be
//       a really long ginormous function or else all of the necessary
//       intermediate stuff will end up being passed to multiple utilities
//       so here it is, all in one big ginormous and glorious pile
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
                    int iInstallType, int iDisk1Reserve, int iRebootFlag)
{
  CString csTemp, csTemp2, csDDF;
  int i1, i2, i3;


  // OVERALL PROCESS
  // 1.  make a DDF file that instructs MAKECAB how to do things
  // 2.  run MAKECAB to create directories, cabinets, and an 'INF' file
  // 3.  rename, copy, and move things to appropriate places.
  //
  // THIS function handles the DDF file portion.  It will include many lines
  // that instruct how to create the INF file as well as how to pack everything
  // into the cabinet file(s).


  // Start making a 'DDF' file's output text

  // TODO:  option for "signing" cabinet must reserve 6144 bytes in '.cab' file
  // TODO:  option for "cab only" must mark 'SETUP.EXE' as "executable" and
  //        assume all files extracted to the temporary folder before running it!

  csDDF = ".OPTION EXPLICIT\r\n"
          ".Define Infrename=\r\n"  // the '/rename' parameter
          ".Set RptFileName=\""
        + csDestPath
        + "_SETUP.RPT\"\r\n"
          ".Set InfFileName=\""
        + csDestPath
        + "_SETUP.INF\"\r\n"
          ".Set CabinetNameTemplate=SFTCAB.*\r\n";

  if(bLZCompress)
  {
    csDDF += ".Set CompressionType=LZX\r\n";
  }
  else
  {
    csDDF += ".Set CompressionType=MSZIP\r\n";  // the default
  }

  // cabinet layout method - directory, max size

  if(iInstallType == 0)                              // DVD image
  {
    csTemp.Format("%lld", 4500LL * 1024LL * 1024LL); // 4.5Gb, enough space for file system + data (?)
    // NOTE:  if this poses a problem, change the max to '0' for "no limit"

    csDDF += ".Set DiskDirectory1=\""
           + csDestPath
           + "SETUP\"\r\n"                            // all in one directory for DVD install
             ".Set ClusterSize=4096\r\n"              // cluster size 4096 (was 512 like a diskette)
             ".Set MaxDiskSize="
           + csTemp
           + "\r\n";                                  // max disk size for DVD
  }
  else if(iInstallType == 1 || iInstallType == 2)     // CD images as part of DVD
  {
    csDDF += ".Set DiskDirectoryTemplate=\""
           + csDestPath
           + "Disk*\"\r\n"   // one per cab
             ".Set ClusterSize=4096\r\n"               // cluster size 4096 (was 512 like a diskette)
             ".Set MaxDiskSize=CDROM\r\n";            // default max disk size (was 1.44mb)
//             ".Set MaxDiskFileCount=1.44M\r\n"        // file count based on 1.44Mb floppy

    if(iDisk1Reserve > 0)
    {
      if(iDisk1Reserve < 4096)
      {
        // reserve a minimum of 8Mb
        csTemp.Format("%d", 640 * 1024 * 1024 - 1024 * 4096);
      }
      else
      {
        csTemp.Format("%d", 640 * 1024 * 1024 /*1457664*/ - 1024 * iDisk1Reserve
                      - 1024 * 4096); // additional 4Mb for setup, uninst, etc.
      }

      csDDF += ".Set MaxDiskSize1="
             + csTemp
             + "\r\n";                              // max size so I can reserve space
    }
  }
  else if(iInstallType == 3)                        // self-extractor
  {
    csDDF += ".Set DiskDirectory1=\""
           + csDestPath.Left(csDestPath.GetLength() > 3 ? csDestPath.GetLength() - 1 : csDestPath.GetLength())
           + "\"\r\n"  // all in one
             ".Set MaxDiskSize=0\r\n";                // INFINITE max disk size
  }
  else                                                // everything else (CDROM)
  {
    // DEFAULT settings - same directory, maxed out size

    csDDF += ".Set DiskDirectory1=\""
           + csDestPath.Left(csDestPath.GetLength() > 3 ? csDestPath.GetLength() - 1 : csDestPath.GetLength())
           + "\"\r\n"  // all in one
             ".Set MaxDiskSize=CDROM\r\n";
  }

  // diskette labels

  if(iInstallType == 1 || iInstallType == 2)      // CD (was diskette) images
  {
    csDDF +="\r\n"
            ".Set DiskLabel1=" 
          + csAppName
          + " SETUP Disk #1\r\n"
            ".Set DiskLabelTemplate="
          + csAppName
          + " Disk #*\r\n";
  }
  else
  {
    csDDF +="\r\n"
            ".Set DiskLabel1=" 
          + csAppName
          + " SETUP Disk\r\n";
  }

  // INF header information

  csDDF +="\r\n\r\n"
          ".Set InfDiskHeader=\"\"\r\n"
          ".Set InfDiskHeader1=\"[disk list]\"\r\n"
          ".Set InfDiskHeader2=\";<disk number>,<disk label>\"\r\n"
          ".Set InfDiskLineFormat=\"*disk#*,*label*\"\r\n"
          "\r\n"
          ".Set InfCabinetHeader=\"\"\r\n"
          ".Set InfCabinetHeader1=\"[cabinet list]\"\r\n"
          ".Set InfCabinetHeader2=\";<cabinet number>,<disk number>,<cabinet file name>\"\r\n"
          ".Set InfCabinetLineFormat=\"*cab#*,*disk#*,*cabfile*\"\r\n"
          "\r\n"
          ".Set InfFileHeader=\"\"\r\n"
          ".Set InfFileHeader1=\";-----------------------------------------------------------------------\"\r\n"
          ".Set InfFileHeader2=\";\"\r\n"
          ".Set InfFileHeader3=\";            SETUP INF File generated by S.F.T. Setup Gizmo\"\r\n"
          ".Set InfFileHeader4=\";     S.F.T. Setup Gizmo is Copyright (c) 1998-2016 by S.F.T. Inc.\"\r\n"
          ".Set InfFileHeader5=\";                         (all rights reserved)\"\r\n"
          ".Set InfFileHeader6=\";\"\r\n";

  // put the license information in the INF file if it's licensed
  if(theApp.m_csAuthCompany.GetLength()) // TODO:  validate the license info?
  {
    csTemp = theApp.m_csAuthCompany;

    InPlaceEscapeQuotes(csTemp); // just in case, escape any quotes

    csDDF += ".Set InfFileHeader7=\";The S.F.T. Setup Gizmo software is licensed to:\"\r\n"
             ".Set InfFileHeader8=\";  " + csTemp + "\"\r\n";
             ".Set InfFileHeader9=\";\"\r\n";
             ".Set InfFileHeader10=\";-----------------------------------------------------------------------\"\r\n"
             ".Set InfFileHeader11=\"\"\r\n"
             ".Set InfFileHeader12=\"\"\r\n"; // blank lines
  }
  else
  {
    csDDF += ".Set InfFileHeader7=\";-----------------------------------------------------------------------\"\r\n"
             ".Set InfFileHeader8=\"\"\r\n"
             ".Set InfFileHeader9=\"\"\r\n"; // blank lines
  }

  // define the line format for the file section
  csDDF +=  "\r\n"
            ".Set InfFileLineFormat=\"*disk#*,*cab#*,*file*,*rename*,*date*,*size*\"\r\n"
            "\r\n";


  // ** FILE COPY SECTION **

  csDDF += ".Set GenerateInf=OFF\r\n"; // RELATIONAL MODE - Do disk layout first

  if(iInstallType != 3) // NOT a self-extractor
  {
    // ** Setup files.  These don't need to be in the INF file, so we put
    //    /inf=NO on these lines so that MakeCAB won't generate an error when
    //    it finds that these files are not mentioned in the INF portion of
    //    the DDF.  BUT... they need to go into the 'SETUPGIZ' sub-tree in the cabinet

    csDDF +=".set Compress=OFF\r\n"
            ".set Cabinet=OFF\r\n";
//          ".Set InfAttr=\r\n"                  // 'INF' attributes off (readonly, etc.)


// this section used to apply when doing diskette images.  not any more.
//    if(iInstallType != 0) // CD image on CDROM
//    {
//      csDDF += "\""
//             + csExePath
//             + "setup.exe\"  SETUPGIZ\\setup.exe /inf=NO\r\n";   // This file doesn't show up in INF
//    }

    csDDF += "\""
           + csDestPath
           + "setupgiz.ddf\" setup.inf /inf=NO\r\n"; // NOTE:  'dummy' SETUP.INF file

    if(!bNoUninstall)
    {
      csDDF +="\""
            + csExePath
            + "uninst.exe\"  SETUPGIZ\\uninst.exe /inf=NO\r\n";
    }


    if(csLicenseFile.GetLength())
    {
      // A license file, if it exists, must be called 'license.txt' or
      // 'license.rtf' and exist on disk #1 along with 'setup.inf'.

      if(csLicenseFile.GetLength() > 4 &&
         !_strnicmp(((LPCSTR)csLicenseFile) +
                    csLicenseFile.GetLength() - 4,
                    ".RTF", 4))
      {
        csDDF += "\""
              + csLicenseFile
              + "\" license.rtf /inf=NO\r\n";
      }
      else
      {
        csDDF += "\""
              + csLicenseFile
              + "\" license.txt /inf=NO\r\n";
      }
    }
  }


//
//    ;** Files in cabinets
//    ;
  csDDF +=".set Compress=ON\r\n"
          ".set Cabinet=ON\r\n"
          "\r\n";

  // CERT FILES are placed in the cabinet first.  just because...
  // for CERT FILES, I must ensure there is a unique file name by making each one
  // have a name like 'cert001.cer' 'cert002.cer' etc.

  if(acsCertFiles.GetCount() > 0)
  {
    for(i1=0; i1 < acsCertFiles.GetCount(); i1++)
    {
      csTemp.Format("certs\\cert%03d.cer", i1);

      csDDF += "\""
            + acsCertFiles[i1]
            + "\" "
            + csTemp // new cert file name in 'certs' sub-directory
            + " /inf=NO\r\n";
    }
  }


  // *************************
  // *** FILE COPY SECTION ***
  // *************************

  // add those files to be compressed in the cabinet to the DDF script

  csTemp = "";

  if(iInstallType == 3 &&  // self-extract needs compressed UNINST
     !bNoUninstall)
  {
     csTemp = "\"" + csExePath + "uninst.exe\" SETUPGIZ\\uninst.exe\r\n";
  }

  for(i1=0; i1 < aFileList.GetSize(); i1++)
  {
    // must use file name with directory to identify file uniquely

    csTemp += "\"" 
            + csSourcePath             // combine these to get the
            + aFileList[i1].csFileName // original path to file
            + "\"";

    // NOTE:  path not saved unless I tell it to... and how to.

    if(aFileList[i1].csFileName.Find('\\') >= 0) // name contains a path
    {
      csTemp += " \""
              + aFileList[i1].csFileName
              + "\"\r\n";
    }
    else // no path, no need to re-name it inside the CAB file
    {
      csTemp += "\r\n";
    }
  }



  // ********************************
  // ** INF FILE GENERATOR SECTION **
  // ********************************

  csDDF += csTemp           // add script code to generate INF file
         + "\r\n\r\n"
           ".set GenerateInf=ON\r\n";

  // if I have cert files, they must be enumerated in their own section
  // along with some additional info (as needed)
  if(acsCertFiles.GetCount() > 0)
  {
    csTemp.Format("CertFileCount=%d", acsCertFiles.GetCount());

    csDDF += ".InfWrite \";Cert Files that are automatically installed\"\r\n"
             ".InfBegin File\r\n"
             "[CertFiles]\r\n"
             + csTemp + "\r\n";

    for(i1=0; i1 < acsCertFiles.GetCount(); i1++)
    {
      csTemp2 = GetBaseName(acsCertFiles[i1]);
      InPlaceEscapeQuotes(csTemp2); // just in case, escape any quotes

      csTemp.Format("certs\\cert%03d.cer=", i1);

      csDDF += csTemp
             + "\""
             + csTemp2
             + "\",\""
             + GetCertFileFriendlyName(acsCertFiles[i1])
             + "\","
             + acsCertRepositories[i1] // TODO:  escape any quotes, quote it, etc.
//             + (CString)"0" // for now; later, it's the dest store (this is ROOT)
             + "\r\n";
    }

    csDDF += ".InfEnd\r\n" // ends the 'File' block
             ".InfWrite \"\"\r\n"
             ".InfWrite \"\"\r\n"; // a couple of blank lines follow
  }
  
  csDDF += ".InfWrite \";*** File List ***\"\r\n"
           ".InfWrite \";<disk number>,<cabinet number>,<filename>,<destname>,<date>,<size>\"\r\n"
           ".InfWrite \";Note: File is not in a cabinet if cab# is 0\"\r\n"
           ".InfWrite \"\"\r\n"
           "\r\n"
           ".InfWrite \";Standard Installation Files\"\r\n"
           ".InfBegin File\r\n"
           "[Files]\r\n"
           ".InfEnd\r\n";

  csTemp = "";

  // compressed uninstall for self-extracting setup

  if(iInstallType == 3 &&  // self-extract needs compressed UNINST
     !bNoUninstall)
  {
    csTemp = "SETUPGIZ\\uninst.exe /rename=\"%uninst%\\uninst.exe\"\r\n";
  }

  for(i1=0; i1 < aFileList.GetSize(); i1++)
  {
    if(aFileList[i1].dwOSFlag)
      continue;  // just skip

    if(!aFileList[i1].csGroupName.GetLength())
    {
      csTemp += GetFileEntryLine(aFileList[i1]);
    }
  }

  csDDF += csTemp + "\r\n\r\n";


  // OS-SPECIFIC version of 'Files' (only when something's there)

  static int dwOSMask[17] = {1,2,4,
                             8,16,32,
                             64,
                             128,256,
                             512,1024,
                             2048,4096,
                             8192,16384,
                             32768,
                             /* insert new items here - see below also */
                             0x40000000};
  static const char *pszOS[17] = {"Win95","Win98","WinNT4",
                                  "Win2000","WinME","WinXP",
                                  "WSrv2003",
                                  "Vista","WSrv2008",
                                  "Win7","WSrv2008R2",
                                  "Win8.0","WSrv2012",
                                  "Win8.1","WSrv2012R2",
                                  "Win10",
                                  /* insert new items here - see above also */
                                  "NEW_WINOS"};

  for(i3=0; i3 < sizeof(dwOSMask) / sizeof(*dwOSMask); i3++)
  {
    for(i1=0; i1 < aFileList.GetSize(); i1++)
    {
      if(aFileList[i1].dwOSFlag & dwOSMask[i3] &&
         !aFileList[i1].csGroupName.GetLength())
      {
        break;  // something to add, I see...
      }
    }

    if(i1 < aFileList.GetSize())
    {
      csDDF += ".set GenerateInf=ON\r\n"
               ".InfWrite \"\"\r\n"
               ".InfBegin File\r\n"
               "[Files."
             + (CString)pszOS[i3]
             + "]\r\n"
               ".InfEnd\r\n";

      csTemp = "";

      for(i1=0; i1 < aFileList.GetSize(); i1++)
      {
        if((aFileList[i1].dwOSFlag & dwOSMask[i3]) &&
           !aFileList[i1].csGroupName.GetLength())
        {
          csTemp += GetFileEntryLine(aFileList[i1]);
        }
      }

      csDDF += csTemp + "\r\n\r\n";
    }
  }

  // ************************************
  // ** OPTIONAL INSTALLATION SECTIONS **
  // ************************************

  csTemp = ".set GenerateInf=ON\r\n"
           ".InfWrite \"\"\r\n"
           ".InfWrite \"[Options]\"\r\n";

  for(i2=0; i2 < aGroupList.GetSize(); i2++)
  {
    csTemp += ".InfWrite \"\"\""
            + aGroupList[i2].csGroupName
            + "\"\",\"\""
            + aGroupList[i2].csDesc
            + "\"\",\"\""
            + aGroupList[i2].csLongDesc
            + "\"\"\"\r\n";
  }

  csTemp += ".InfWrite \"\"\r\n\r\n";

  for(i2=0; i2 < aGroupList.GetSize(); i2++)
  {
    CString csGroup = aGroupList[i2].csGroupName;

    csTemp += ".set GenerateInf=ON\r\n"
              ".InfBegin File\r\n"
              "[Option." + csGroup + "]\r\n"
              ".InfEnd\r\n";

    for(i1=0; i1 < aFileList.GetSize(); i1++)
    {
      if(aFileList[i1].dwOSFlag)
        continue;  // just skip

      if(!csGroup.CompareNoCase(aFileList[i1].csGroupName))
      {
        csTemp += GetFileEntryLine(aFileList[i1]);
      }
    }

    csTemp += ".InfWrite \"\"\r\n\r\n";
  }

  csDDF += csTemp;

  // *************************
  // ** OS-SPECIFIC OPTIONS **
  // *************************

  // now, the same thing for OS-specific optional stuff (wow!)

  csTemp = "";

  for(i3=0; i3 < sizeof(dwOSMask) / sizeof(*dwOSMask); i3++)
  {
    for(i2=0; i2 < aGroupList.GetSize(); i2++)
    {
      CString csGroup = aGroupList[i2].csGroupName;

      for(i1=0; i1 < aFileList.GetSize(); i1++)
      {
        if(aFileList[i1].dwOSFlag & dwOSMask[i3] &&
           !csGroup.CompareNoCase(aFileList[i1].csGroupName))
        {
          break;  // something to add, I see...
        }
      }

      if(i1 < aFileList.GetSize())
      {
        csTemp += ".set GenerateInf=ON\r\n"
                  ".InfWrite \"\"\r\n"
                  ".InfBegin File\r\n"
                  "[Option."
                + csGroup 
                + "."
                + (CString)pszOS[i3]
                + "]\r\n"
                  ".InfEnd\r\n";

        for(i1=0; i1 < aFileList.GetSize(); i1++)
        {
          if(aFileList[i1].dwOSFlag & dwOSMask[i3] &&
             !csGroup.CompareNoCase(aFileList[i1].csGroupName))
          {
            csTemp += GetFileEntryLine(aFileList[i1]);
          }
        }

        csTemp += ".InfWrite \"\"\r\n\r\n";
      }
    }
  }

  csDDF += csTemp;


  // *************
  // REGISTRY KEYS
  // *************

  csDDF += ".set GenerateInf=ON\r\n"
           ".InfWrite \"\"\r\n"
           ".InfWrite \";Registry Keys to be added during SETUP and removed during UNINSTALL\"\r\n"
           ".InfBegin File\r\n"
           "[RegistryKeys]\r\n";

  for(i1=0; i1 < acsRegistry.GetSize(); i1++)
  {
    csTemp = acsRegistry[i1];

    // convert to registrykey=@="value" or registrykey="string"="value"

    i2 = csTemp.Find(']');
    if(csTemp[0] != '[' || i2 <= 0)
    {
      AfxMessageBox("SYNTAX ERROR IN REGISTRY KEY '" + csTemp + "' - ignoring this one...",
                    MB_OK | MB_ICONHAND | MB_SETFOREGROUND);
      continue;
    }

    csTemp.SetAt(i2, '=');
    csTemp = csTemp.Mid(1);

    csDDF += csTemp + "\r\n";
  }

  csDDF += ".InfEnd\r\n\r\n\r\n";


  // **************************
  // TITLES PROMPTS AND OPTIONS
  // **************************

  csDDF += ".set GenerateInf=ON\r\n"
           ".InfWrite \"\"\r\n"
           ".InfWrite \";SETUP Options - used by SETUP.EXE and UNINST.EXE\"\r\n"
           ".InfBegin File\r\n"
           "[SetupOptions]\r\n";

  csDDF += "AppName=" + csAppName + "\r\n";
  csDDF += "CompanyName=" + csCompanyName + "\r\n";

  if(iRebootFlag == 0)       // automatic
    csDDF += "RebootFlag=0\r\n";
  else if(iRebootFlag == 1)  // always
    csDDF += "RebootFlag=1\r\n";
  else if(iRebootFlag == 2)  // never
    csDDF += "RebootFlag=-1\r\n";

  // any other 'reboot flag' value will NOT write to the output file.

  if(csAuthCode.GetLength())  // authorized copy
  {
    // this tells SETUP that it's an authorized version.  SETUP will
    // use the company name to decode the authorization string, and
    // display the registering company's name.  Anything else will
    // display "UNREGISTERED COPY" nice and large, in yellow.

    csDDF += "AuthCode=" + csAuthCode + "\r\n";
  }

  csDDF += "StartMessage=" + csStartMessage + "\r\n";
  csDDF += "AppPath=" + csAppPath + "\r\n";
  csDDF += "AppCommon=" + csAppCommon + "\r\n";
  csDDF += "StartMenuFolderName=" + csStartMenu + "\r\n";

  if(bNoUninstall)
    csDDF += "NoUninstall=1\r\n";
  else
    csDDF += "NoUninstall=0\r\n";

  if(bQuietSetup)
    csDDF += "QuietSetup=1\r\n";
  else
    csDDF += "QuietSetup=0\r\n";

  csTemp = "";
  for(i1=0; i1 < acsUpgrade.GetSize(); i1++)
  {
    if(!acsUpgrade[i1].GetLength())
      continue;

    if(csTemp.GetLength())
      csTemp += ',';

    csTemp += '"';

    for(i2=0; i2 < acsUpgrade[i1].GetLength(); i2++)
    {
      if(acsUpgrade[i1].GetAt(i2) == '"')
        csTemp += "\"\"";
      else
        csTemp += acsUpgrade[i1].GetAt(i2);
    }

    csTemp += '"';
  }

  if(csTemp.GetLength())
    csDDF += "Upgrade=" + csTemp + "\r\n";

  csDDF += ".InfEnd\r\n"
           ".InfWrite \"\"\r\n"
           ".InfWrite \"\"\r\n\r\n";


  // *************************************************
  // UNINSTALL section - this is actually important...
  // *************************************************

  if(!bNoUninstall)  // do not put it here if we're not uninstalling
  {
    // put non-SYSTEM/SHARED files in '[UnInstall]', and
    // SYSTEM/SHARED files in '[UnInstall.Shared]'

    csTemp = "";
    for(i1=0; i1 < aFileList.GetSize(); i1++)
    {
      if(aFileList[i1].bShared)
        continue;  // NON-SHARED section

      if(aFileList[i1].bNoUninstall ||  // don't uninstall
         aFileList[i1].bAutoDelete ||   // auto-delete on setup
         aFileList[i1].iDestPath == 2)  // 'tmpdir' implies 'autodelete'
      {
        continue;  // do not include this file
      }

      // THIS USES THE DESTINATION FILE NAME!
      // NOTE:  don't need to double '%'s here or quote file names

      if(aFileList[i1].iDestPath == 0)
      {
        csTemp += "%sysdir%";
      }
  //    else if(aFileList[i1].iDestPath == 2)  not likely, here for ref only
  //    {
  //      csTemp += "%tmpdir%";
  //    }
      else if(aFileList[i1].iDestPath == 3 &&
              aFileList[i1].csOtherPathName.GetLength()) 
      {
        csTemp += aFileList[i1].csOtherPathName;
      }

      if(aFileList[i1].iDestPath != 1 &&
         csTemp[csTemp.GetLength() - 1] != '\\')
      {
        csTemp += '\\';
      }

      i3 = aFileList[i1].csFileName.ReverseFind('\\');

      if(i3 >= 0 && aFileList[i1].iDestPath <= 3)
      {
        csTemp += aFileList[i1].csFileName.Mid(i3 + 1)
                + "\r\n";
      }
      else
      {
        csTemp += aFileList[i1].csFileName
                + "\r\n";
      }
    }

    csDDF += ".set GenerateInf=ON\r\n"
             ".InfWrite \"\"\r\n"
             ".InfWrite \";Files to remove on UNINSTALL\"\r\n"
             ".InfBegin File\r\n"
             "[UnInstall]\r\n"
           + csTemp
           + ".InfEnd\r\n"
             ".InfWrite \"\"\r\n"
             ".InfWrite \"\"\r\n\r\n";

    // NOW, do it again for 'shared' files

    csTemp = "";
    for(i1=0; i1 < aFileList.GetSize(); i1++)
    {
      if(!aFileList[i1].bShared)
      {
        continue;  // SHARED section
      }

      if(aFileList[i1].bNoUninstall ||  // don't uninstall
         aFileList[i1].bAutoDelete ||   // auto-delete on setup
         aFileList[i1].iDestPath == 2)  // 'tmpdir' implies 'autodelete'
      {
        continue;  // do not include this file
      }

      // THIS USES THE DESTINATION FILE NAME!
      // NOTE:  don't need to double '%'s here or quote file names

      if(aFileList[i1].iDestPath == 0)
      {
        csTemp += "%sysdir%";
      }
  //    else if(aFileList[i1].iDestPath == 2) not gonna happen
  //    {
  //      csTemp += "%tmpdir%";
  //    }
      else if(aFileList[i1].iDestPath == 3 &&
              aFileList[i1].csOtherPathName.GetLength()) 
      {
        csTemp += aFileList[i1].csOtherPathName;
      }

      if(aFileList[i1].iDestPath != 1 &&
         csTemp[csTemp.GetLength() - 1] != '\\')
      {
        csTemp += '\\';
      }

      i3 = aFileList[i1].csFileName.ReverseFind('\\');

      if(i3 >= 0 && aFileList[i1].iDestPath <= 3)
      {
        csTemp += aFileList[i1].csFileName.Mid(i3 + 1)
                + "\r\n";
      }
      else
      {
        csTemp += aFileList[i1].csFileName
                + "\r\n";
      }
    }

    if(csTemp.GetLength())  // ONLY if I have any entries here....
    {
      csDDF += ".set GenerateInf=ON\r\n"
               ".InfWrite \"\"\r\n"
               ".InfWrite \";SHARED/SYSTEM Files to unregister/remove on UNINSTALL\"\r\n"
               ".InfBegin File\r\n"
               "[UnInstall.Shared]\r\n"
             + csTemp
             + ".InfEnd\r\n"
               ".InfWrite \"\"\r\n"
               ".InfWrite \"\"\r\n\r\n";
    }
  }

  // *******************************
  // ** OTHER 'attribute' ENTRIES **
  // *******************************

  // ** THE FOLLOWING ENTRIES USE THE 'INTERNAL' NAME **

  // 'SYSTEM/SHARED' FILES

  csTemp = "";
  for(i1=0; i1 < aFileList.GetSize(); i1++)
  {
    if(!aFileList[i1].bShared)
    {
      continue;  // do not include this file
    }

    // THIS USES THE INTERNAL FILE NAME!

    csTemp += aFileList[i1].csFileName
            + "\r\n";
  }

  csDDF += ".set GenerateInf=ON\r\n"
           ".InfWrite \"\"\r\n"
           ".InfWrite \";Shared Components - will automatically be added to 'SharedDLLs' list\"\r\n"
           ".InfBegin File\r\n"
           "[SharedComponents]\r\n"
         + csTemp
         + ".InfEnd\r\n"
           ".InfWrite \"\"\r\n"
           ".InfWrite \"\"\r\n\r\n";


  // 'OVERWRITE ALWAYS' FILES

  csTemp = "";
  for(i1=0; i1 < aFileList.GetSize(); i1++)
  {
    if(!aFileList[i1].bOverwrite)
    {
      continue;  // do not include this file
    }

    // THIS USES THE INTERNAL FILE NAME!

    csTemp += aFileList[i1].csFileName
            + "\r\n";
  }

  csDDF += ".set GenerateInf=ON\r\n"
           ".InfWrite \"\"\r\n"
           ".InfWrite \";Files that always install, no matter what\"\r\n"
           ".InfBegin File\r\n"
           "[OverwriteAlways]\r\n"
         + csTemp
         + ".InfEnd\r\n"
           ".InfWrite \"\"\r\n"
           ".InfWrite \"\"\r\n\r\n";



  // ** THE FOLLOWING ENTRIES USE THE 'DESTINATION' NAME **

  // 'EXECUTE AFTER COPY' FILES

  csTemp = "";
  for(i1=0; i1 < aFileList.GetSize(); i1++)
  {
    if(!aFileList[i1].bExecute)
    {
      continue;  // do not include this file
    }

    // THIS USES THE DESTINATION FILE NAME!
    // NOTE:  don't need to double '%'s here
    // format:  filename,arglist  (any spaces around comma are removed within 'SETUP')

    csTemp += "\"";  // leading quote mark

    if(aFileList[i1].iDestPath == 0)
    {
      csTemp += "%sysdir%";
    }
    else if(aFileList[i1].iDestPath == 2)
    {
      csTemp += "%tmpdir%";
    }
    else if(aFileList[i1].iDestPath == 3 &&
            aFileList[i1].csOtherPathName.GetLength()) 
    {
      csTemp += aFileList[i1].csOtherPathName;
    }

    if(aFileList[i1].iDestPath != 1 &&
       csTemp[csTemp.GetLength() - 1] != '\\')
    {
      csTemp += '\\';
    }

    i3 = aFileList[i1].csFileName.ReverseFind('\\');

    if(i3 >= 0 && aFileList[i1].iDestPath <= 3)
    {
      csTemp += aFileList[i1].csFileName.Mid(i3 + 1) + "\"";
    }
    else
    {
      csTemp += aFileList[i1].csFileName + "\"";
    }

    if(aFileList[i1].csExecArgs.GetLength())
    {
      csTemp += "," + aFileList[i1].csExecArgs;
    }

    csTemp += "\r\n";
  }

  csDDF += ".set GenerateInf=ON\r\n"
           ".InfWrite \"\"\r\n"
           ".InfWrite \";Files to execute after copying all files to the target machine\"\r\n"
           ".InfBegin File\r\n"
           "[ExecuteAfterCopy]\r\n"
         + csTemp
         + ".InfEnd\r\n"
           ".InfWrite \"\"\r\n"
           ".InfWrite \"\"\r\n\r\n";


  // 'EXECUTE ON UNINSTALL' FILES

  csTemp = "";
  for(i1=0; i1 < aFileList.GetSize(); i1++)
  {
    if(!aFileList[i1].bExecute ||
       aFileList[i1].bNoUninstall ||
       !aFileList[i1].csUninstArgs.GetLength())
    {
      continue;  // do not include this file
    }

    // THIS USES THE DESTINATION FILE NAME!
    // NOTE:  don't need to double '%'s here
    // SAME FORMAT AS 'exec on install' FILES

    csTemp += "\"";  // leading quote mark

    if(aFileList[i1].iDestPath == 0)
    {
      csTemp += "%sysdir%";
    }
    else if(aFileList[i1].iDestPath == 2)
    {
      csTemp += "%tmpdir%";
    }
    else if(aFileList[i1].iDestPath == 3 &&
            aFileList[i1].csOtherPathName.GetLength()) 
    {
      csTemp += aFileList[i1].csOtherPathName;
    }

    if(aFileList[i1].iDestPath != 1 &&
       csTemp[csTemp.GetLength() - 1] != '\\')
    {
      csTemp += '\\';
    }

    i3 = aFileList[i1].csFileName.ReverseFind('\\');

    if(i3 >= 0 && aFileList[i1].iDestPath <= 3)
    {
      csTemp += aFileList[i1].csFileName.Mid(i3 + 1) + "\"";
    }
    else
    {
      csTemp += aFileList[i1].csFileName + "\"";
    }

    if(aFileList[i1].csUninstArgs.GetLength())
    {
      csTemp += "," + aFileList[i1].csUninstArgs;
    }

    csTemp += "\r\n";
  }

  csDDF += ".set GenerateInf=ON\r\n"
           ".InfWrite \"\"\r\n"
           ".InfWrite \";Files to execute before uninstalling any files from the target machine\"\r\n"
           ".InfBegin File\r\n"
           "[ExecuteOnUninstall]\r\n"
         + csTemp
         + ".InfEnd\r\n"
           ".InfWrite \"\"\r\n"
           ".InfWrite \"\"\r\n\r\n";


  // 'REGISTER CONTROL' FILES

  csTemp = "";
  csTemp2 = "";
  for(i1=0; i1 < aFileList.GetSize(); i1++)
  {
    if(!aFileList[i1].bRegister)
    {
      continue;  // do not include this file
    }

    // THIS USES THE DESTINATION FILE NAME!
    // NOTE:  don't need to double '%'s here or quote file names

    if(aFileList[i1].iDestPath == 0)
    {
      if(aFileList[i1].bRegDotNet)
        csTemp2 += "%sysdir%";

      csTemp += "%sysdir%";
    }
    else if(aFileList[i1].iDestPath == 2)
    {
      if(aFileList[i1].bRegDotNet)
        csTemp2 += "%tmpdir%";

      csTemp += "%tmpdir%";
    }
    else if(aFileList[i1].iDestPath == 3 &&
            aFileList[i1].csOtherPathName.GetLength()) 
    {
      if(aFileList[i1].bRegDotNet)
        csTemp2 += aFileList[i1].csOtherPathName;

      csTemp += aFileList[i1].csOtherPathName;
    }

    if(aFileList[i1].bRegDotNet)
    {
      if(aFileList[i1].iDestPath != 1 &&
        csTemp2[csTemp2.GetLength() - 1] != '\\')
      {
        csTemp2 += '\\';
      }
    }

    if(aFileList[i1].iDestPath != 1 &&
       csTemp[csTemp.GetLength() - 1] != '\\')
    {
      csTemp += '\\';
    }

    i3 = aFileList[i1].csFileName.ReverseFind('\\');

    if(i3 >= 0 && aFileList[i1].iDestPath <= 3)
    {
      if(aFileList[i1].bRegDotNet)
        csTemp2 += aFileList[i1].csFileName.Mid(i3 + 1)
                 + "\r\n";

      csTemp += aFileList[i1].csFileName.Mid(i3 + 1)
              + "\r\n";
    }
    else
    {
      if(aFileList[i1].bRegDotNet)
        csTemp2 += aFileList[i1].csFileName
                 + "\r\n";

      csTemp += aFileList[i1].csFileName
              + "\r\n";
    }
  }

  // NOTE:  DLL's that are registered for ".NET" appear in
  //        BOTH the [RegisterControl] and [RegAsmControl]
  //        sections (to minimize code changes)

  csDDF += ".set GenerateInf=ON\r\n"
           ".InfWrite \"\"\r\n"
           ".InfWrite \";ActiveX controls and OLE/COM servers (OCX,DLL) to Register before SETUP completes\"\r\n"
           ".InfWrite \";Standard OCX and DLL call DllRegisterServer, Dot Net calls 'regasm'\"\r\n"
           ".InfBegin File\r\n"
           "[RegisterControl]\r\n"  // all of them
         + csTemp
         + "\r\n"
         + "[RegAsmControl]\r\n"    // only '.Net' ones
         + csTemp2
         + "\r\n"
         + ".InfEnd\r\n"
           ".InfWrite \"\"\r\n"
           ".InfWrite \"\"\r\n\r\n";


  // 'ADD SHORTCUT' FILES
  // [AddShortcut]
  // destfilename=Description,ShortcutName[,StartPath,Args,IconFile,IconIndex,nCMDShow]

  csTemp = "";
  for(i1=0; i1 < aFileList.GetSize(); i1++)
  {
    if(!aFileList[i1].bAddShortcut)
    {
      continue;  // do not include this file
    }

    // THIS USES THE DESTINATION FILE NAME!
    // NOTE:  don't need to double '%'s here or quote file names

    if(aFileList[i1].iDestPath == 0)
    {
      csTemp += "%sysdir%";
    }
    else if(aFileList[i1].iDestPath == 2)
    {
      csTemp += "%tmpdir%";
    }
    else if(aFileList[i1].iDestPath == 3 &&
            aFileList[i1].csOtherPathName.GetLength()) 
    {
      csTemp += aFileList[i1].csOtherPathName;
    }

    if(aFileList[i1].iDestPath != 1 &&
       csTemp[csTemp.GetLength() - 1] != '\\')
    {
      csTemp += '\\';
    }

    csTemp2.Format("%ld,%d",
                   aFileList[i1].uiIconIndex,
                   aFileList[i1].iShow);

    i3 = aFileList[i1].csFileName.ReverseFind('\\');

    // there's a path in it and I'm not keeping the directory
    // I must extract the file name itself
    if(i3 >= 0 && aFileList[i1].iDestPath <= 3)
    {
      csTemp += aFileList[i1].csFileName.Mid(i3 + 1);  //  go past the backslash
    }
    else
    {
      csTemp += aFileList[i1].csFileName;
    }

    csTemp += ",\""
            + aFileList[i1].csShortcutDescription
            + "\",\""
            + aFileList[i1].csShortcutFileName
            + "\",\""
            + aFileList[i1].csStartPath
            + "\",\""
            + aFileList[i1].csArgs
            + "\",\""
            + aFileList[i1].csIconFile  // this is a 'source file' name - SETUP must translate!
            + "\","
            + csTemp2
            + "\r\n";

  }

  csDDF += ".set GenerateInf=ON\r\n"
           ".InfWrite \"\"\r\n"
           ".InfWrite \";Files that will be added to the Start Menu as shortcuts\"\r\n"
           ".InfWrite \";<dest file name>,<display name>,<shortcut file name>\"\r\n"
           ".InfBegin File\r\n"
           "[AddShortcut]\r\n"
         + csTemp
         + ".InfEnd\r\n"
           ".InfWrite \"\"\r\n"
           ".InfWrite \"\"\r\n\r\n";


  // 'AUTODELETE' FILES

  csTemp = "";
  for(i1=0; i1 < aFileList.GetSize(); i1++)
  {
    if(!aFileList[i1].bAutoDelete &&   // auto-delete on setup
       aFileList[i1].iDestPath != 2)  // 'tmpdir' implies 'autodelete'
    {
      continue;  // do not include this file
    }

    // THIS USES THE DESTINATION FILE NAME!
    // NOTE:  don't need to double '%'s here or quote file names

    if(aFileList[i1].iDestPath == 0)
    {
      csTemp += "%sysdir%";
    }
    else if(aFileList[i1].iDestPath == 2)
    {
      csTemp += "%tmpdir%";
    }
    else if(aFileList[i1].iDestPath == 3 &&
            aFileList[i1].csOtherPathName.GetLength()) 
    {
      csTemp += aFileList[i1].csOtherPathName;
    }

    if(aFileList[i1].iDestPath != 1 &&
       csTemp[csTemp.GetLength() - 1] != '\\')
    {
      csTemp += '\\';
    }

    i3 = aFileList[i1].csFileName.ReverseFind('\\');

    if(i3 >= 0 && aFileList[i1].iDestPath <= 3)
    {
      csTemp += aFileList[i1].csFileName.Mid(i3 + 1)
              + "\r\n";
    }
    else
    {
      csTemp += aFileList[i1].csFileName
              + "\r\n";
    }
  }

  csDDF += ".set GenerateInf=ON\r\n"
           ".InfWrite \"\"\r\n"
           ".InfWrite \";Files to remove when SETUP completes\"\r\n"
           ".InfBegin File\r\n"
           "[AutoDelete]\r\n"
         + csTemp
         + ".InfEnd\r\n"
           ".InfWrite \"\"\r\n"
           ".InfWrite \"\"\r\n\r\n";


  return csDDF;
}

CString GetFileEntryLine(SetupFileInfo &sfi)
{
  CString csRval;

  int i1 = sfi.csFileName.ReverseFind('\\');

//  if(i1 >= 0)
  if(i1 >= 0 && sfi.iDestPath <= 3)
  {
    csRval += "\""
            + sfi.csFileName
            + "\" /rename=\"";

    if(sfi.iDestPath == 0)
    {
      csRval += "%sysdir%";
    }
    else if(sfi.iDestPath == 2)
    {
      csRval += "%tmpdir%";
    }
    else if(sfi.iDestPath == 3 &&
            sfi.csOtherPathName.GetLength()) 
    {
      csRval += sfi.csOtherPathName;

//      for(i4=0; i4 < sfi.csOtherPathName.GetLength(); i4++)
//      {
//        csRval += sfi.csOtherPathName[i4];
//        if(sfi.csOtherPathName[i4] == '%')
//          csRval += '%';  // double percent signs
//      }
    }

    if(sfi.iDestPath != 1 &&
       csRval[csRval.GetLength() - 1] != '\\')
    {
      csRval += '\\';
    }

    csRval +=sfi.csFileName.Mid(i1 + 1)
           + "\"\r\n";
  }
  else if(sfi.iDestPath != 1 && sfi.iDestPath <= 3)
  {
    csRval += "\""
            + sfi.csFileName 
            + "\" /rename=\"";

    if(sfi.iDestPath == 0)
    {
      csRval += "%sysdir%";
    }
    else if(sfi.iDestPath == 2)
    {
      csRval += "%tmpdir%";
    }
    else if(sfi.iDestPath == 3 &&
            sfi.csOtherPathName.GetLength()) 
    {
      csRval += sfi.csOtherPathName;

//      for(i4=0; i4 < sfi.csOtherPathName.GetLength(); i4++)
//      {
//        csRval += sfi.csOtherPathName[i4];
//        if(sfi.csOtherPathName[i4] == '%')
//          csRval += '%';  // double percent signs
//      }
    }

    if(sfi.iDestPath != 1 &&
       csRval[csRval.GetLength() - 1] != '\\')
    {
      csRval += '\\';
    }

    csRval +=sfi.csFileName
           + "\"\r\n";
  }
  else
  {
    csRval += "\""
            + sfi.csFileName
            + "\"\r\n";
  }

  return(csRval);
}


CString PathFromCommandLine(LPCSTR szCommandLine)
{
  CString csRval="";

  LPCSTR lpc1 = szCommandLine;

  while(*lpc1 && *lpc1 <= ' ')
    lpc1++;  // left-trim it (just in case)

  if(*lpc1 == '"')  // quoted?
  {
    LPCSTR lpc2 = ++lpc1;

    while(*lpc1 && *lpc1 != '"')
      lpc1++;

    if(lpc1 > lpc2)
      csRval = ((CString)lpc2).Left(lpc1 - lpc2);
  }
  else
  {
    LPCSTR lpc2 = lpc1;

    while(*lpc1 > ' ')
      lpc1++;

    if(lpc1 > lpc2)
      csRval = ((CString)lpc2).Left(lpc1 - lpc2);
  }

  return(csRval);
}


DWORD RunApplication(LPCSTR szCmdLine, LPCSTR szExecDir,
                     LPCSTR szAppName, LPCSTR szWindowTitle /* = NULL */)
{
  STARTUPINFO si;
  PROCESS_INFORMATION pi;
  CString csTemp;

  CString csExecDir = szExecDir;

  if(!csExecDir.GetLength())
    csExecDir = PathFromCommandLine(szCmdLine);

  memset(&si, 0, sizeof(si));
  si.cb = sizeof(si);

  si.lpTitle = (LPTSTR)szWindowTitle;
  si.wShowWindow = SW_SHOWNA;  // show but do not activate it!
  si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USEPOSITION;

  memset(&pi, 0, sizeof(pi));

  DWORD dwRval = 0;  // non-zero return is an error


  UINT uiFlags = CREATE_NEW_PROCESS_GROUP | CREATE_DEFAULT_ERROR_MODE;

  if(GetVersion() & 0x80000000)  // WIN '9x
  {
    uiFlags |= CREATE_NEW_CONSOLE;
  }
  else
  {
    uiFlags |= CREATE_SEPARATE_WOW_VDM;
  }

  if(!CreateProcess(NULL, (LPSTR)szCmdLine, NULL, NULL, FALSE,
                    uiFlags | NORMAL_PRIORITY_CLASS,
                    NULL, szExecDir, &si, &pi))
  {
    if(pi.hThread)
    {
      CloseHandle(pi.hThread);
    }

    if(pi.hProcess)
    {
      CloseHandle(pi.hProcess);
    }

    DWORD dwErr = GetLastError();
    csTemp.Format(" - error %08xH (%d)", dwErr, dwErr);

    AfxMessageBox("Unable to start "
                  + (CString)szAppName
                  + csTemp);

    dwRval = 0xffffffff;
  }
  else
  {
    // TODO:  do I check an 'abandon me' flag and loop?
    // TODO:  do I check for a 'WM_QUIT' message?

    while(1)
    {
      DWORD dwWait = ::WaitForSingleObject(pi.hProcess, 500);  // 1/2 sec wait

      if(dwWait == WAIT_OBJECT_0)
      {
        break;
      }

      if(dwWait != WAIT_TIMEOUT)
      {
        AfxMessageBox("Process wait failed on " + (CString)szAppName);

        dwRval = 0xffffffff;
        break;
      }

      Sleep(50);

      MSG msg;
      PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE);  // help 16 bit apps maybe?
    }

    DWORD dwExit = 0;
    if(!dwRval && !GetExitCodeProcess(pi.hProcess, &dwExit))
    {
      dwRval = 0xffffffff;

      dwExit= GetLastError();
      csTemp.Format("%ld (%08xH)", dwExit, dwExit);

      AfxMessageBox("Unable to get return code from "
                    + (CString)szAppName
                    + ", error code " + csTemp);
    }
    else if(dwExit != 0)
    {
      CString cs1;
      cs1.Format("%d", dwExit);

      AfxMessageBox("'" + (CString)szAppName + "' exits with error code " + cs1,
                    MB_OK | MB_ICONHAND | MB_SETFOREGROUND);

      dwRval = dwExit;
    }
  }

  if(pi.hThread)
  {
    CloseHandle(pi.hThread);
  }

  if(pi.hProcess)
  {
    CloseHandle(pi.hProcess);
  }

  Sleep(500);  // this helps Win98 systems "not hang"

  return(dwRval);
}

