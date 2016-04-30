///////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                   //
//    ____        _      _               _    _   _        _                                         //
//   / ___| __ _ | |__  (_) _ __    ___ | |_ | | | |  ___ | | _ __    ___  _ __  ___  _ __   _ __    //
//  | |    / _` || '_ \ | || '_ \  / _ \| __|| |_| | / _ \| || '_ \  / _ \| '__|/ __|| '_ \ | '_ \   //
//  | |___| (_| || |_) || || | | ||  __/| |_ |  _  ||  __/| || |_) ||  __/| | _| (__ | |_) || |_) |  //
//   \____|\__,_||_.__/ |_||_| |_| \___| \__||_| |_| \___||_|| .__/  \___||_|(_)\___|| .__/ | .__/   //
//                                                           |_|                     |_|    |_|      //
//                                                                                                   //
///////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                   //
//                   Copyright (c) 1998-2016 by S.F.T. Inc. - All rights reserved                    //
//                                                                                                   //
///////////////////////////////////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include <io.h>      // for things like '_open()'
#include <fcntl.h>
#include <sys\types.h>
#include <sys\stat.h>
#include "fdi.h"

#include "SETUP.h"
#include "CabinetHelper.h"



// locally defined structure for file copies

struct FileCopyInfo
{
  CStringArray acsTempFileName;        // array of 'dest' file names (temporary)
  CStringArray acsInternalFileName;    // array of internal file names (within cab)
  CStringArray acsDestFileName;        // array of destination path names
  CArray<BOOL,BOOL> abOverwriteAlways; // file is always overwritten
  CArray<BOOL,BOOL> abSharedFile;      // file is 'shared'
  CArray<long,long> alFileSize;        // file size array
  CString csCurrentDestPath;           // makes things easier later
  int iCurrentCabNumber;
  int iCurrentIndex;
  long long lCurrentSize;
};


// prototypes not in the header
int PreFileCopyCompareVersions(FileCopyInfo *pFCI, FILETIME timeNew);
int FileCopyCompareVersions(FileCopyInfo *pFCI, FILETIME timeNew);


///////////////////////////////////////////////////////
// helper functions for extracting files from a cabinet
///////////////////////////////////////////////////////


// NOTE:  some of these were copied from sample code
/*
 * Memory allocation function
 */
FNALLOC(mem_alloc)
{
//  return malloc(cb);
  return(GlobalAllocPtr(GHND, cb));
}


/*
 * Memory free function
 */
FNFREE(mem_free)
{
//  free(pv);
  GlobalFreePtr(pv);
}


FNOPEN(file_open)
{
//  return _open(pszFile, oflag, pmode);

  UINT uiFlag = FILE_ATTRIBUTE_NORMAL;
  UINT uiAccess = GENERIC_READ;
  UINT uiDisp = OPEN_EXISTING;
  UINT uiShare = FILE_SHARE_READ;

  if(oflag & _O_CREAT)
  {
    if(oflag & _O_EXCL)
    {
      uiDisp = CREATE_ALWAYS;
    }
    else
    {
      uiDisp = CREATE_NEW;
    }
  }
  else
  {
    uiDisp = OPEN_EXISTING;
  }

  if(oflag & _O_RDONLY)
  {
    uiAccess = GENERIC_READ;
  }
  else if(oflag & _O_WRONLY)
  {
    uiAccess = GENERIC_WRITE;
    uiShare = 0;  // exclusive if I have write access
  }
  else if(oflag & _O_RDWR)
  {
    uiAccess = GENERIC_READ | GENERIC_WRITE;
    uiShare = 0;  // exclusive if I have write access
  }

  if(oflag & _O_EXCL)  // exclusive forces sharing mode "0"
  {
    uiShare = 0;
  }

  // ignore pmode

  uiFlag = FILE_ATTRIBUTE_NORMAL;


  return((int)CreateFile(pszFile, uiAccess, uiShare,
                         NULL, uiDisp, uiFlag, NULL));
}


FNREAD(file_read)
{
//  return _read(hf, pv, cb);
  DWORD cb1;

  if(!ReadFile((HANDLE)hf, pv, cb, &cb1, NULL))
  {
    DWORD dwErr = GetLastError();

    return(-1);
  }

  return(cb1);
}


FNWRITE(file_write)
{
//  return _write(hf, pv, cb);
  DWORD cb1;

  if(!WriteFile((HANDLE)hf, pv, cb, &cb1, NULL) ||
     cb1 != cb)
  {
    DWORD dwErr = GetLastError();

    return(-1);
  }

  return(cb1);
}


FNCLOSE(file_close)
{
//  return _close(hf);
  return(CloseHandle((HANDLE)hf) ? 0 : -1);
}


FNSEEK(file_seek)
{
//  return _lseek(hf, dist, seektype);

  UINT uiPos = FILE_BEGIN;
  if(seektype == SEEK_SET)
    uiPos = FILE_BEGIN;
  else if(seektype == SEEK_END)
    uiPos = FILE_END;
  else if(seektype == SEEK_CUR)
    uiPos = FILE_CURRENT;

  return(SetFilePointer((HANDLE)hf, dist, NULL, uiPos));
}

CString return_fdi_error_string(FDIERROR err)
{
  switch (err)
  {
    case FDIERROR_NONE:
      return "No error";

    case FDIERROR_CABINET_NOT_FOUND:
      return "Cabinet not found";
      
    case FDIERROR_NOT_A_CABINET:
      return "Not a cabinet";
      
    case FDIERROR_UNKNOWN_CABINET_VERSION:
      return "Unknown cabinet version";
      
    case FDIERROR_CORRUPT_CABINET:
      return "Corrupt cabinet";
      
    case FDIERROR_ALLOC_FAIL:
      return "Memory allocation failed";
      
    case FDIERROR_BAD_COMPR_TYPE:
      return "Unknown compression type";
      
    case FDIERROR_MDI_FAIL:
      return "Failure decompressing data";
      
    case FDIERROR_TARGET_FILE:
      return "Failure writing to target file";
      
    case FDIERROR_RESERVE_MISMATCH:
      return "Cabinets in set have different RESERVE sizes";
      
    case FDIERROR_WRONG_CABINET:
      return "Cabinet returned on fdintNEXT_CABINET is incorrect";
      
    case FDIERROR_USER_ABORT:
      return "User aborted";
      
    default:
      return "Unknown error";
  }
}


FNFDINOTIFY(notification_function)
{
CString csTemp, csMsg;
int i1;

  FileCopyInfo *pFCI = (FileCopyInfo *)pfdin->pv;
//struct FileCopyInfo
//{
//  CStringArray acsTempFileName;        // array of 'dest' file names (temporary)
//  CStringArray acsInternalFileName;    // array of internal file names (within cab)
//  CStringArray acsDestFileName;        // array of destination path names
//  CArray<BOOL,BOOL> abSharedFile;      // file is 'shared'
//  CArray<int,int> aiPercentComplete;   // '% complete' array...
//  int iCurrentCabNumber;
//  int iCurrentIndex;
//};

  switch (fdint)
  {
    case fdintCABINET_INFO: // general information about the cabinet
      TRACE("fdintCABINET_INFO\n"
            "  next cabinet     = %s\n"
            "  next disk        = %s\n"
            "  cabinet path     = %s\n"
            "  cabinet set ID   = %d\n"
            "  cabinet # in set = %d (zero based)\n"
            "\n",
            pfdin->psz1,
            pfdin->psz2,
            pfdin->psz3,
            pfdin->setID,
            pfdin->iCabinet );

      return 0;

    case fdintPARTIAL_FILE: // first file in cabinet is continuation
      TRACE("fdintPARTIAL_FILE\n"
            "   name of continued file            = %s\n"
            "   name of cabinet where file starts = %s\n"
            "   name of disk where file starts    = %s\n",
            pfdin->psz1,
            pfdin->psz2,
            pfdin->psz3 );

      return 0;  // this is acceptable, always

    case fdintCOPY_FILE:  // file to be copied

      TRACE("fdintCOPY_FILE\n"
            "  file name in cabinet = %s\n"
            "  uncompressed file size = %d\n",
            pfdin->psz1,
            pfdin->cb );

      for(i1=0; i1 < pFCI->acsInternalFileName.GetSize(); i1++)
      {
        if(!pFCI->acsInternalFileName[i1].CompareNoCase(pfdin->psz1))
        {
          theApp.SetProgressText("EXTRACTING FILE\n" + pFCI->acsDestFileName[i1]);

          pFCI->csCurrentDestPath = pFCI->acsTempFileName[i1];

          if(!pFCI->csCurrentDestPath.GetLength())
            pFCI->csCurrentDestPath = pFCI->acsDestFileName[i1];

          int handle = file_open((LPSTR)(LPCSTR)pFCI->csCurrentDestPath,
                                 _O_BINARY | _O_CREAT | _O_EXCL 
                                 | _O_WRONLY | _O_SEQUENTIAL,
                                 _S_IREAD | _S_IWRITE );

          if(handle != -1)
            pFCI->iCurrentIndex = i1;
          else
            pFCI->iCurrentIndex = -1;

          return(handle);
        }
      }

      return(0); /* skip file */


    case fdintCLOSE_FILE_INFO:  // close the file, set relevant info
    {
      TRACE("fdintCLOSE_FILE_INFO\n"
            "   file name in cabinet = %s\n"
            "\n",
            pfdin->psz1 );

      // ** DO THE VERSION CHECKING NOW! **

      FILETIME datetime, local_filetime;

      if(!DosDateTimeToFileTime(pfdin->date,
                                pfdin->time,
                                &datetime))
      {
        // use today's date/time

        SYSTEMTIME tm;
        GetLocalTime(&tm);
        
        SystemTimeToFileTime(&tm, &datetime);
      }
      
      if(LocalFileTimeToFileTime(&datetime, &local_filetime))
      {
        /*
         * Set date/time on result (temporary) file
         */

        SetFileTime((HANDLE)pfdin->hf, &local_filetime,
                    NULL, &local_filetime );

        file_close(pfdin->hf);
      }

      /*
       * Mask out attribute bits other than readonly,
       * hidden, system, and archive, since the other
       * attribute bits are reserved for use by
       * the cabinet format.
       */

      DWORD attrs = pfdin->attribs;
      attrs &= (_A_RDONLY | _A_HIDDEN | _A_SYSTEM | _A_ARCH);

      SetFileAttributes(pFCI->csCurrentDestPath, attrs);


      ///////////////////////////////////////////////////////////////
      // NOTE:  pfdin->cb is 1 if file is to be executed; however,
      //        as I have my own set of flags for this.... ignore it!
      ///////////////////////////////////////////////////////////////


      // if the destination file already exists, I need to version check
      // the file and copy the newer one on top of it, accounting for the
      // fact that it may currently be in use.

      if(!stricmp((LPCSTR)pFCI->acsTempFileName[pFCI->iCurrentIndex],
                  (LPCSTR)pFCI->acsDestFileName[pFCI->iCurrentIndex]))
      {
        // if I told the cabinet extract to extract a file that is
        // a temporary file or 'directly to output' file, then I do
        // not want to copy it on top of itself

        TRACE("NOT COPYING %s onto itself\r\n",
              (LPCSTR)pFCI->acsTempFileName[pFCI->iCurrentIndex]);
      }
      else
      {
        TRACE("COPYING %s to %s\r\n",
              (LPCSTR)pFCI->acsTempFileName[pFCI->iCurrentIndex],
              (LPCSTR)pFCI->acsDestFileName[pFCI->iCurrentIndex]);

        if(pFCI->acsTempFileName[pFCI->iCurrentIndex].GetLength())
        {
          // ONLY if we've extracted to a temporary file.... which is ALWAYS, now

          i1 = FileAlreadyExists(pFCI->acsDestFileName[pFCI->iCurrentIndex]);
          if(i1 < 0)
          {
            CString csMsg;
            csMsg.Format(theApp.LoadString(IDS_ERROR08), (LPCSTR)pFCI->acsDestFileName[pFCI->iCurrentIndex]);

            MyMessageBox(csMsg, MB_OK | MB_ICONHAND | MB_SETFOREGROUND);
            return FALSE;
          }
          else if(i1 == 0)  // file not found - RENAME!
          {
            if(!MoveFile(pFCI->acsTempFileName[pFCI->iCurrentIndex],
                         pFCI->acsDestFileName[pFCI->iCurrentIndex]))
            {
              CString csMsg;
              csMsg.Format(theApp.LoadString(IDS_ERROR09), (LPCSTR)pFCI->acsDestFileName[pFCI->iCurrentIndex]);

              MyMessageBox(csMsg, MB_OK | MB_ICONHAND | MB_SETFOREGROUND);
              return FALSE;
            }

            pFCI->acsTempFileName.SetAt(pFCI->iCurrentIndex, "");  // empty string (no post delete)
          }
          else // if(i1 > 0)  file exists and is not a directory name
          {
            if(pFCI->abOverwriteAlways[pFCI->iCurrentIndex])
            {
              i1 = 1;  // forces it to overwrite this file with whatever's in the SETUP image
            }
            else
            {
              // this returns < 0 on error, 0 if existing file is to be kept, >0 if
              // the file in the SETUP image is to overwrite the existing file

              i1 = FileCopyCompareVersions(pFCI, datetime);
            }

            if(i1 < 0)
            {
              return FALSE;  // user aborted
            }
            else if(i1 > 0)   // file is newer or exactly the same date/version with binary mismatch
            {
              // FIRST:  ensure that the "read-only" flag is CLEARED on the destination file!
              //         (and prompt the user to 'cancel' if it was set)

              DWORD dwAttr = GetFileAttributes(pFCI->acsDestFileName[pFCI->iCurrentIndex]);
              if(dwAttr & FILE_ATTRIBUTE_READONLY)
              {
                CString csMsg;
                csMsg.Format(theApp.LoadString(IDS_ERROR10), (LPCSTR)pFCI->acsDestFileName[pFCI->iCurrentIndex]);

                if(MyMessageBox(csMsg, MB_OKCANCEL | MB_ICONHAND | MB_SETFOREGROUND)
                   != IDOK)
                {
                  return FALSE;
                }

                SetFileAttributes(pFCI->acsDestFileName[pFCI->iCurrentIndex],
                                  dwAttr & ~FILE_ATTRIBUTE_READONLY);
              }

              // *IF* the file is system/shared, *AND* the file is
              // currently in use, do a "deferred re-name" and force
              // the user to reboot (or at least CANCEL a reboot).

              if(pFCI->abSharedFile[pFCI->iCurrentIndex])
              {
                i1 = file_open((LPSTR)(LPCSTR)pFCI->acsDestFileName[pFCI->iCurrentIndex],
                               _O_BINARY | _O_WRONLY | _O_SEQUENTIAL,
                               _S_IREAD | _S_IWRITE);

                if(i1 >= 0)
                  file_close(i1);  // not needed - information obtained
              }

              if(i1 < 0)  // an error - assume it's "in use"
              {
                if(!theApp.m_bIsNT || theApp.m_dwMajorVer < 4 ||
                   (theApp.m_dwMajorVer == 4 && theApp.m_dwMinorVer < 10))
                {
                  // '95 - use 'WININIT.INI'

                  WritePrivateProfileString("Rename", pFCI->acsDestFileName[pFCI->iCurrentIndex],
                                            pFCI->acsTempFileName[pFCI->iCurrentIndex],
                                            theApp.m_csWinPath + "WININIT.INI");
                }
                else
                {
                  // NT - use 'MoveFileEx()'

                  if(!MoveFileEx(pFCI->acsTempFileName[pFCI->iCurrentIndex],
                                 pFCI->acsDestFileName[pFCI->iCurrentIndex],
                                 MOVEFILE_REPLACE_EXISTING | 
                                 MOVEFILE_DELAY_UNTIL_REBOOT))
                  {
                    CString csMsg;
                    csMsg.Format(theApp.LoadString(IDS_ERROR11), (LPCSTR)pFCI->acsDestFileName[pFCI->iCurrentIndex]);

                    MyMessageBox(csMsg, MB_OK | MB_ICONHAND | MB_SETFOREGROUND);
                    return FALSE;
                  }
                }

                // I must clear out the temp file name now... 

                pFCI->acsTempFileName.SetAt(pFCI->iCurrentIndex, "");  // empty string (no post delete)
                theApp.m_bRebootFlag = TRUE;  // must re-boot
              }
              else
              {
                // this will fail if a file is 'in use' and not 'shared', and will keep
                // prompting the user until the file is no longer 'in use'.

                theApp.SetProgressText("COPYING FILE\n" + pFCI->acsDestFileName[pFCI->iCurrentIndex]);

                do
                {
                  i1 = file_open((LPSTR)(LPCSTR)pFCI->acsDestFileName[pFCI->iCurrentIndex],
                                 _O_BINARY | _O_WRONLY | _O_SEQUENTIAL,
                                 _S_IREAD | _S_IWRITE);

                  if(i1 >= 0)
                  {
                    file_close(i1);  // not needed - information obtained

                    if(!CopyFile(pFCI->acsTempFileName[pFCI->iCurrentIndex],
                                pFCI->acsDestFileName[pFCI->iCurrentIndex],
                                FALSE))
                    {
                      i1 = -1;  // a flag
                    }
                  }

                  if(i1 < 0)
                  {
                    CString csMsg;
                    csMsg.Format(theApp.LoadString(IDS_ERROR12), (LPCSTR)pFCI->acsDestFileName[pFCI->iCurrentIndex]);

                    i1 = MyMessageBox(csMsg, MB_ABORTRETRYIGNORE | MB_ICONHAND | MB_SETFOREGROUND);

                    if(i1 == IDABORT)
                      return FALSE;
                    else if(i1 == IDIGNORE)
                      break;
                    else
                      i1 = -1;  // for the loop test, below

                    // default:  retry!
                  }
                  else
                  {
                    // SETTING FILE DATE/TIME!!! (in case it didn't happen automatically)

                    HANDLE hFile = CreateFile((LPCSTR)pFCI->acsDestFileName[pFCI->iCurrentIndex],
                                              GENERIC_READ | GENERIC_WRITE, 0, NULL,
                                              OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

                    if(hFile == INVALID_HANDLE_VALUE ||
                       !SetFileTime(hFile, &local_filetime, NULL, &local_filetime))
                    {
                      csMsg.Format(theApp.LoadString(IDS_WARNING10), (LPCSTR)pFCI->acsDestFileName[pFCI->iCurrentIndex]);

                      MyMessageBox(csMsg, MB_OK | MB_ICONEXCLAMATION | MB_SETFOREGROUND);
                    }

                    if(hFile != INVALID_HANDLE_VALUE)
                      CloseHandle(hFile);
                  }

                } while(i1 < 0);
              }
            }
            else // if(i1 == 0)   [file is older or exactly the same]
            {
              if(pFCI->acsTempFileName[pFCI->iCurrentIndex].GetLength())
                DeleteFile(pFCI->acsTempFileName[pFCI->iCurrentIndex]);

              pFCI->acsTempFileName.SetAt(pFCI->iCurrentIndex, "");  // empty string (no post delete)
            }
          }
        }
      }

      pFCI->lCurrentSize += pFCI->alFileSize[pFCI->iCurrentIndex];

      if(theApp.m_dwTotalFileSize > 0)
        theApp.SetProgress((UINT)(100 * pFCI->lCurrentSize / theApp.m_dwTotalFileSize));

      return TRUE;
    }

    case fdintNEXT_CABINET: // file continued to next cabinet
    {
      TRACE("fdintNEXT_CABINET\n"
            "   name of next cabinet where file continued = %s\n"
            "   name of next disk where file continued    = %s\n"
            "   cabinet path name                         = %s\n"
            "\n",
            pfdin->psz1,
            pfdin->psz2,
            pfdin->psz3 );

      // modify 'psz3' (256-byte buffer) to point to correct CAB path

      int iCab = theApp.GetCabNumberFromCabName(pfdin->psz1);
      if(iCab <= 0)
        return(-1);

      csTemp = theApp.GetCabFilePath(iCab);
      int i1 = csTemp.ReverseFind('\\');
      csTemp = csTemp.Left(i1 + 1);
      lstrcpy(pfdin->psz3, csTemp);

      return 0;
    }
  }

  return 0;
}


// AND NOW......


// THIS FUNCTION DOES THE MOST WORK
// IT COPIES FILES FROM THE CABINET TO THEIR DESTINATION

BOOL CopyFilesFromCabinet(CStringArray &acsEntries)
{
  // This function scans 'acsEntries' for cabinet information,
  // sorts them efficiently, then copies the files.
  // For now, 'execute on copy' will be handled differently...

  HFDI      hfdi;
  ERF       erf;
  FDICABINETINFO  fdici;
  int       hf = -1;

  CString csTemp;
  CStringArray acsTemp;

  hfdi = FDICreate(mem_alloc,
                   mem_free,
                   file_open,
                   file_read,
                   file_write,
                   file_close,
                   file_seek,
                   cpu80386,
                   &erf );

  if(!hfdi)
  {
    csTemp.Format(theApp.LoadString(IDS_ERROR13), erf.erfOper, 
                  return_fdi_error_string((FDIERROR)erf.erfOper) );

    MyMessageBox(csTemp,
                  MB_OK | MB_ICONHAND | MB_SETFOREGROUND);

    return FALSE;
  }

  CString csCabPath, csCabName;

  int i1, i2, iLastCab=0;
  long long lCurrentSize = 0;

  theApp.SetProgress(0);

  for(i1=0; i1 < acsEntries.GetSize(); i1++)
  {
    int iCab = theApp.GetFileEntryCabNumber(acsEntries[i1]);

    if(!iCab)
    {
      // TODO:  copy file

      MyMessageBox((IDS_ERROR14),
                    MB_OK | MB_ICONHAND | MB_SETFOREGROUND);

      if(hf >= 0)
      {
        file_close(hf);
        hf = -1;
      }

      FDIDestroy(hfdi);

      return FALSE;
    }
    else if(iCab != iLastCab)
    {
      // get path for this cab file and open it

      if(hf >= 0)
      {
        file_close(hf);
        hf = -1;
      }

      iLastCab = iCab;

      csCabPath = theApp.GetCabFilePath(iCab);

      hf = file_open((char *)(LPCSTR)csCabPath,
                     _O_BINARY | _O_RDONLY | _O_SEQUENTIAL,
                     0 );
      if(hf < 0 ||
         !FDIIsCabinet(hfdi, hf, &fdici))
      {
        if(hf >= 0)
          file_close(hf);

        CString csMsg;
        csMsg.Format(theApp.LoadString(IDS_ERROR15), (LPCSTR)csCabPath);

        MyMessageBox(csMsg, MB_OK | MB_ICONHAND | MB_SETFOREGROUND);

        FDIDestroy(hfdi);

        return FALSE;
      }

      file_close(hf);  // I'm done with it now
      hf = -1;

      i2 = csCabPath.ReverseFind('\\');
      csCabName = csCabPath.Mid(i2 + 1);
      csCabPath = csCabPath.Left(i2 + 1);  // include backslash
    }

    // for each file that has the same cabinet #, add it to the list
    // of files for this cabinet.  Stop when the cabinet # changes.

    FileCopyInfo fci;

    do
    {
      // find out where the file is supposed to go
      TRACE("***PROCESSING FILE ENTRY:  %s\r\n", (LPCSTR)acsEntries[i1]);
      CString csDestPath = theApp.GetFileEntryDestPath(acsEntries[i1]);

      // ensure destination path exists...

      i2 = csDestPath.ReverseFind('\\');
      if(i2 < 0)
        csTemp = "";
      else
        csTemp = csDestPath.Left(i2 + 1);

      if(!csTemp.GetLength())
      {
        CString csMsg;
        csMsg.LoadString(IDS_ERROR16);
        csMsg += acsEntries[i1];

        MyMessageBox(csMsg, MB_OK | MB_ICONHAND | MB_SETFOREGROUND);

        FDIDestroy(hfdi);
        return FALSE;
      }

      if(!CheckForAndCreateDirectory(csTemp))
      {
        CString csMsg;
        csMsg.Format(theApp.LoadString(IDS_ERROR17), (LPCSTR)csTemp);

        MyMessageBox(csMsg, MB_OK | MB_ICONHAND | MB_SETFOREGROUND);

        FDIDestroy(hfdi);
        return FALSE;
      }

      // additional information
      DoParseString(acsEntries[i1], acsTemp);

      if(!csDestPath.GetLength() || acsTemp.GetSize() < 6)
      {
        csDestPath.Format(theApp.LoadString(IDS_ERROR18), i1);
        MyMessageBox(csDestPath, MB_OK | MB_ICONHAND | MB_SETFOREGROUND);

        FDIDestroy(hfdi);

        return FALSE;  // bail out now
      }

      // PREPARE TO COPY!

      // get a temp file name
      CString csTempPath = "";

      i2 = csDestPath.ReverseFind('\\');
      csTemp = csDestPath.Left(i2 > 3 ? i2 : i2 + 1);

      if(!GetTempFileName(csTemp, "~SETUP", 0, 
                          csTempPath.GetBufferSetLength(MAX_PATH)))
      {
        csTempPath.ReleaseBuffer(0);

        csTempPath.Format(theApp.LoadString(IDS_ERROR19), (LPCSTR)csDestPath);

        MyMessageBox(csTempPath, MB_OK | MB_ICONHAND | MB_SETFOREGROUND);

        FDIDestroy(hfdi);

        return FALSE;
      }

      // add entry to structure

      i2 = fci.acsInternalFileName.GetSize();

      fci.acsInternalFileName.SetAtGrow(i2,acsTemp[2]);
      fci.acsTempFileName.SetAtGrow(i2,csTempPath);
      fci.acsDestFileName.SetAtGrow(i2,csDestPath);
      fci.abOverwriteAlways.SetAtGrow(i2, theApp.IsFileEntryOverwriteAlways(acsEntries[i1]));
      fci.abSharedFile.SetAtGrow(i2,theApp.IsFileEntryShared(acsEntries[i1]));
      fci.alFileSize.SetAtGrow(i2, theApp.GetEntryFileSize(i1));

      if((i1 + 1) >= acsEntries.GetSize() ||
         iCab != theApp.GetFileEntryCabNumber(acsEntries[i1 + 1]))
      {
        break;
      }

      i1++;  // next file

    } while(TRUE);

    fci.iCurrentIndex = 0;
    fci.iCurrentCabNumber = iCab;  // last known cabinet index

    fci.lCurrentSize = lCurrentSize;


#ifdef _DEBUG
    for(i2=0; i2 < fci.acsInternalFileName.GetSize(); i2++)
    {
      TRACE("%s %s %s %d %d %lu\r\n",
            (LPCSTR)fci.acsInternalFileName[i2],
            (LPCSTR)fci.acsTempFileName[i2],
            (LPCSTR)fci.acsDestFileName[i2],
            fci.abOverwriteAlways[i2],
            fci.abSharedFile[i2],
            fci.alFileSize[i2]);
    }
#endif // _DEBUG

    // extract all of the files for this cabinet

    if(fci.acsInternalFileName.GetSize() &&
       !FDICopy(hfdi,(char *)(LPCSTR)csCabName,
                (char *)(LPCSTR)csCabPath, 0,
                notification_function, NULL,
                (LPVOID)&fci ))
    {
      CString csMsg;
      csMsg.Format(theApp.LoadString(IDS_ERROR20), (LPCSTR)fci.acsInternalFileName[fci.iCurrentIndex],
                   (LPCSTR)return_fdi_error_string((FDIERROR)erf.erfOper));

      MyMessageBox(csMsg, MB_OK | MB_ICONHAND | MB_SETFOREGROUND);

      FDIDestroy(hfdi);

      return FALSE;
    }

    lCurrentSize = fci.lCurrentSize;  // the running total (save it for next loop)

    for(i2=0; i2 < fci.acsTempFileName.GetSize(); i2++)
    {
      if(fci.acsTempFileName[i2].GetLength())
        DeleteFile(fci.acsTempFileName[i2]);  // I'm done with it now.
    }

    if(theApp.m_dwTotalFileSize > 0)
      theApp.SetProgress((UINT)(100L * lCurrentSize / theApp.m_dwTotalFileSize));
  }

  (void) FDIDestroy(hfdi);

  return TRUE;
}

CString ExtractFileFromCabAsString(int iCab, LPCSTR szFileName)
{
HFDI      hfdi;
ERF       erf;
FileCopyInfo fci;
int i1;
CString csTemp, csTempPath, csCabName, csCabPath, csRval;


  csCabPath = theApp.GetCabFilePath(iCab); // always cabinet 1

  hfdi = FDICreate(mem_alloc,
                   mem_free,
                   file_open,
                   file_read,
                   file_write,
                   file_close,
                   file_seek,
                   cpu80386,
                   &erf );

  if(!hfdi)
  {
    csTemp.Format(theApp.LoadString(IDS_ERROR13), erf.erfOper, 
                  return_fdi_error_string((FDIERROR)erf.erfOper) );

    MyMessageBox(csTemp,
                  MB_OK | MB_ICONHAND | MB_SETFOREGROUND);

    return ""; // error return
  }



  // get a temp file name into 'csTempPath'
  csTemp = "%tmpdir%\\SFTSETUP\\";
  theApp.ExpandStrings(csTemp);

  if(!GetTempFileName(csTemp, "~SETUP", 0, 
                      csTempPath.GetBufferSetLength(MAX_PATH)))
  {
    csTempPath.ReleaseBuffer(0);

    csTempPath.Format("Unable to create temporary file name in \"%s\"",
                      (LPCSTR)csTemp);

    MyMessageBox(csTempPath, MB_OK | MB_ICONHAND | MB_SETFOREGROUND);

    FDIDestroy(hfdi);

    return "";
  }

  // add extract name and dest path to 'fci' structure

  fci.acsInternalFileName.SetAtGrow(0,szFileName);
  fci.acsTempFileName.SetAtGrow(0,csTempPath);
  fci.acsDestFileName.SetAtGrow(0,csTempPath);
  fci.abOverwriteAlways.SetAtGrow(0, 1); // overwrite always
  fci.abSharedFile.SetAtGrow(0, 0); // not shared
  fci.alFileSize.SetAtGrow(0, 0);   // unknown size (use 0)
  fci.iCurrentIndex = 0;
  fci.iCurrentCabNumber = 1;  // always in cabinet #1
  fci.lCurrentSize = 0;

  i1 = csCabPath.ReverseFind('\\');
  if(i1 >= 0)
  {
    csCabName = csCabPath.Mid(i1 + 1);
    csCabPath = csCabPath.Left(i1 + 1);  // include backslash
  }
  else
  {
    csCabName = csCabPath;
    csCabPath = "";
  }

  DeleteFile(csTempPath); // make sure that temporary file isn't there (no overwrite needed now)

  if(!FDICopy(hfdi,(char *)(LPCSTR)csCabName,
              (char *)(LPCSTR)csCabPath, 0,
              notification_function, NULL,
              (LPVOID)&fci ))
  {
//    csTemp.Format(theApp.LoadString(IDS_ERROR19), (LPCSTR)csTempPath); // TODO:  figure out what to do with this
//
//    MyMessageBox(csTempPath, MB_OK | MB_ICONHAND | MB_SETFOREGROUND);

    DeleteFile(csTempPath);

    FDIDestroy(hfdi);

    return "";
  }

  FDIDestroy(hfdi);

  // I've now copied to "that file", so need to read it into a
  // CString, then delete it and return the CString

  csRval = ReadFileIntoString(csTempPath);
#ifdef _DEBUG
  if(!csRval.GetLength())
  {
    MyMessageBox("Read error on " + csTempPath);
  }
#endif // _DEBUG

  DeleteFile(csTempPath);

  return csRval;
}

// special notification proc to extract all files from a 'temp cabinet',
// like for the bitmaps (later I may also do this with a "message file"
// for NLS support)

FNFDINOTIFY(notification_function2)
{
  LPCSTR szDestPath = (LPCSTR)pfdin->pv;

  switch (fdint)
  {
    case fdintCABINET_INFO: // general information about the cabinet
      TRACE("fdintCABINET_INFO\n"
            "  next cabinet     = %s\n"
            "  next disk        = %s\n"
            "  cabinet path     = %s\n"
            "  cabinet set ID   = %d\n"
            "  cabinet # in set = %d (zero based)\n"
            "\n",
            pfdin->psz1,
            pfdin->psz2,
            pfdin->psz3,
            pfdin->setID,
            pfdin->iCabinet );

      return 0;

    case fdintPARTIAL_FILE: // first file in cabinet is continuation
      TRACE("fdintPARTIAL_FILE\n"
            "   name of continued file            = %s\n"
            "   name of cabinet where file starts = %s\n"
            "   name of disk where file starts    = %s\n",
            pfdin->psz1,
            pfdin->psz2,
            pfdin->psz3 );

      return 0;  // this is acceptable, always

    case fdintCOPY_FILE:  // file to be copied

      TRACE("fdintCOPY_FILE\n"
            "  file name in cabinet = %s\n"
            "  uncompressed file size = %d\n",
            pfdin->psz1,
            pfdin->cb );

      {
        CString csDestPath = AdjustPathName(szDestPath);

        csDestPath += pfdin->psz1;

        int handle = file_open((LPSTR)(LPCSTR)csDestPath,
                               _O_BINARY | _O_CREAT | _O_EXCL 
                               | _O_WRONLY | _O_SEQUENTIAL,
                                 _S_IREAD | _S_IWRITE );

        return(handle);
      }


    case fdintCLOSE_FILE_INFO:  // close the file, set relevant info
    {
      TRACE("fdintCLOSE_FILE_INFO\n"
            "   file name in cabinet = %s\n"
            "\n",
            pfdin->psz1 );

      file_close(pfdin->hf);

      return TRUE;
    }

    case fdintNEXT_CABINET: // file continued to next cabinet
    {
      TRACE("fdintNEXT_CABINET\n"
            "   name of next cabinet where file continued = %s\n"
            "   name of next disk where file continued    = %s\n"
            "   cabinet path name                         = %s\n"
            "\n",
            pfdin->psz1,
            pfdin->psz2,
            pfdin->psz3 );

      return(-1);  // for now
    }
  }

  return 0;
}



BOOL ExtractFilesFromTempCabinet(LPCSTR szCab, LPCSTR szDestPath)
{
  HFDI      hfdi;
  ERF       erf;
  FDICABINETINFO  fdici;
  int       hf = -1;

  hfdi = FDICreate(mem_alloc,
                   mem_free,
                   file_open,
                   file_read,
                   file_write,
                   file_close,
                   file_seek,
                   cpu80386,
                   &erf );

  if(!hfdi)
    return FALSE;

  hf = file_open((char *)szCab,
                 _O_BINARY | _O_RDONLY | _O_SEQUENTIAL,
                 0 );
  if(hf < 0 || !FDIIsCabinet(hfdi, hf, &fdici))
  {
    if(hf >= 0)
      file_close(hf);

    FDIDestroy(hfdi);

    return FALSE;
  }

  file_close(hf);  // not needed now

  CString csCab = szCab;
  int i1 = csCab.ReverseFind('\\');

  CString csPath = csCab.Left(i1+1);
  csCab = csCab.Mid(i1 + 1);

  if(!FDICopy(hfdi,(char *)(LPCSTR)csCab,
              (char *)(LPCSTR)csPath, 0,
              notification_function2, NULL,
              (LPVOID)szDestPath ))
  {
    CString csMsg;
    csMsg.Format(theApp.LoadString(IDS_ERROR23), (LPCSTR)szCab,
                 (LPCSTR)return_fdi_error_string((FDIERROR)erf.erfOper));

    MyMessageBox(csMsg, MB_OK | MB_ICONHAND | MB_SETFOREGROUND);

    FDIDestroy(hfdi);

    return FALSE;
  }


  FDIDestroy(hfdi);
  return TRUE;
}



// ADDITIONAL, MORE GENERIC FILE UTILITIES

BOOL CompareFiles(LPCSTR szFile1, LPCSTR szFile2)
{
  // map view of file on 2 files.  Do a 'memcmp' on them.
  // return TRUE if they match.

  HANDLE h1 = CreateFile(szFile1, GENERIC_READ, FILE_SHARE_READ,
                         NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,
                         NULL);

  if(h1 == INVALID_HANDLE_VALUE)
    return FALSE;

  HANDLE h2 = CreateFile(szFile2, GENERIC_READ, FILE_SHARE_READ,
                         NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,
                         NULL);

  if(h2 == INVALID_HANDLE_VALUE)
  {
    CloseHandle(h1);
    return FALSE;
  }

  DWORD dwFileSize = GetFileSize(h1, NULL);  // file size must match
  if(dwFileSize != GetFileSize(h2, NULL))
  {
    CloseHandle(h1);
    CloseHandle(h2);

    return FALSE;
  }

  BOOL bRval = FALSE;
  HANDLE hMap1 = NULL, hMap2 = NULL;

  hMap1 = CreateFileMapping(h1, NULL, PAGE_READONLY, 0, 0, NULL);
  hMap2 = CreateFileMapping(h2, NULL, PAGE_READONLY, 0, 0, NULL);

  if(hMap1 && hMap2)
  {
    LPVOID lp1 = MapViewOfFile(hMap1, FILE_MAP_READ, 0, 0, 0);
    LPVOID lp2 = MapViewOfFile(hMap2, FILE_MAP_READ, 0, 0, 0);

    if(lp1 && lp2)
    {
      bRval = !memcmp(lp1, lp2, dwFileSize);
    }
    
    if(lp1)
      UnmapViewOfFile(lp1);

    if(lp2)
      UnmapViewOfFile(lp2);
  }

  if(hMap1)
    CloseHandle(hMap1);
  if(hMap2)
    CloseHandle(hMap2);

  CloseHandle(h1);
  CloseHandle(h2);
  return(bRval);
}

int CompareFileTime(FILETIME &time1, FILETIME &time2)
{
  if(time1.dwHighDateTime < time2.dwHighDateTime)
    return(-1);
  else if(time1.dwHighDateTime > time2.dwHighDateTime)
    return(1);
  else if(time1.dwLowDateTime < time2.dwLowDateTime)
    return(-1);
  else if(time1.dwLowDateTime > time2.dwLowDateTime)
    return(1);
  else
    return(0);
}

CString GetFinalFileName(LPCSTR szFile)
{
  // check WININIT.INI or registry HKLM\SYSTEM\CurrentControlSet\Control\Session Manager\PendingFileRenameOperations
  // to see what the LAST entry says about this file.

  // NOTE:  I may have to read 'WININIT.INI' directly because GetPrivateProfileString won't
  //        grab the LAST matching entry - it grabs the FIRST matching entry!
  //        SOOO.... for now I'll assume only one entry per name (that's how MY program does it!)

  CString csTemp, csShortName;

  GetShortPathName(szFile, csShortName.GetBufferSetLength(MAX_PATH * 2), MAX_PATH * 2);
  csShortName.ReleaseBuffer(-1);

  if(GetVersion() & 0x80000000)  // Windows '9x or ME
  {
    csTemp = "";
    int iRet = GetPrivateProfileString("Rename", szFile, "",
                                       csTemp.GetBufferSetLength(32767),
                                       32767,
                                       theApp.m_csWinPath + "WININIT.INI");

    if(iRet > 0)
    {
      csTemp.ReleaseBuffer(iRet);
      csTemp.ReleaseBuffer(-1);

      return(csTemp);
    }
    
    // now check for SHORT file names

    if(csShortName.GetLength() && csShortName.CompareNoCase(szFile))
    {
      csTemp = "";
      iRet = GetPrivateProfileString("Rename", csShortName, "",
                                     csTemp.GetBufferSetLength(32767),
                                     32767,
                                     theApp.m_csWinPath + "WININIT.INI");

      if(iRet > 0)
      {
        csTemp.ReleaseBuffer(iRet);
        csTemp.ReleaseBuffer(-1);

        return(csTemp);
      }
    }

    // LAST, check for quoted file names

    csTemp = "";
    iRet = GetPrivateProfileString("Rename", "\"" + (CString)szFile + "\"", "",
                                   csTemp.GetBufferSetLength(32767),
                                   32767,
                                   theApp.m_csWinPath + "WININIT.INI");
    if(iRet > 0)
    {
      csTemp.ReleaseBuffer(iRet);
      csTemp.ReleaseBuffer(-1);

      return(csTemp);
    }

    if(csShortName.GetLength() && csShortName.CompareNoCase(szFile))
    {
      csTemp = "";
      iRet = GetPrivateProfileString("Rename", "\"" + csShortName + "\"", "",
                                     csTemp.GetBufferSetLength(32767),
                                     32767,
                                     theApp.m_csWinPath + "WININIT.INI");

      if(iRet > 0)
      {
        csTemp.ReleaseBuffer(iRet);
        csTemp.ReleaseBuffer(-1);

        return(csTemp);
      }
    }

    // TODO:  should I enumerate the entries instead and 'get anal' with all of them?
    //        (a partially shortened path might cause trouble here)
  }
  else
  {
    // Windows 'NT or 2000 - use the registry!

    HKEY hKey = NULL;

    if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                    "SYSTEM\\CurrentControlSet\\Control\\Session Manager",
                    0, KEY_QUERY_VALUE,
                    &hKey)
       == ERROR_SUCCESS)
    {
      DWORD dwType = 0, cbBuf=65535;  // shouldn't exceed this (if it does, system needs booting BAD!)

      CString csTemp2 = "";

      if(RegQueryValueEx(hKey, "PendingFileRenameOperations", NULL, &dwType,
                         (LPBYTE)csTemp2.GetBufferSetLength(cbBuf), &cbBuf)
         == ERROR_SUCCESS)
      {
        csTemp2.ReleaseBuffer(cbBuf);
      }
      else
      {
        csTemp2.ReleaseBuffer(0);
      }

      RegCloseKey(hKey);

      if(dwType == REG_MULTI_SZ && cbBuf > 2)
      {
        CString csRval = "";

        // go through the array and find the one I want, and check it against wha
        // I have above.

        LPCSTR lpc1 = csTemp2;
        char tbuf[MAX_PATH * 4];

        while(*lpc1)
        {
          LPCSTR lpc2 = lpc1;

          lpc1 += lstrlen(lpc1) + 1;

          csTemp = lpc1;

          lpc1 += lstrlen(lpc1) + 1;

          // compare 'short' names if I can, else long names.
          // Keep the 'last' one I find that matches.

          if((csShortName.GetLength()
              && GetShortPathName(lpc2, tbuf, sizeof(tbuf))
              && !csShortName.CompareNoCase(tbuf)) ||
             !_stricmp(lpc2, szFile))
          {
            csRval = csTemp;
          }
        }

        csTemp2.ReleaseBuffer(0);

        if(csRval.GetLength())
          return(csRval);
      }
    }
  }

  return(szFile);  // if I don't find it, I keep the existing file name as-is
}


// PreFileCopyCompareVersions
// return value:  0  do not overwrite (existing file is newer)
//                1  overwrite (existing file is older and/or binary mismatch)
//                2  files match, regardless of date/time, so don't overwrite or prompt
//               -1  error

int PreFileCopyCompareVersions(FileCopyInfo *pFCI, FILETIME timeNew)
{

  // validate file name and check for existence

  CString csFile = GetFinalFileName(pFCI->acsDestFileName[pFCI->iCurrentIndex]);
  CString csTempFile = pFCI->acsTempFileName[pFCI->iCurrentIndex];

  DWORD dwSizeOld, dwSizeNew;

  FILETIME timeOld, timeOld0;
  WIN32_FIND_DATA fd;

  HANDLE hFF = FindFirstFile(csFile, &fd);

  if(hFF == INVALID_HANDLE_VALUE)
  {
    CString csMsg;
    csMsg.Format(theApp.LoadString(IDS_ERROR21), (LPCSTR)csFile);

    MyMessageBox(csMsg, MB_OK | MB_ICONHAND | MB_SETFOREGROUND);

    return(-1);  // file does not exist - error (at this point, anyway)
  }

  // timeOld = max(fd.ftCreationTime, fd.ftLastWriteTime);
  // can't do this - see below for equivalent

  timeOld0 = fd.ftCreationTime;

  if(CompareFileTime(timeOld0, fd.ftLastWriteTime) < 0)
  {
    timeOld0 = fd.ftLastWriteTime;
  }

  // must convert file time to local time... 'timeNew' is in
  // 'local time' also.

  FileTimeToLocalFileTime(&timeOld0, &timeOld);

  
  dwSizeOld = fd.nFileSizeLow;
  ASSERT(!fd.nFileSizeHigh);

  FindClose(hFF);

  // now get info on temp file (just size, though)

  hFF = FindFirstFile(csTempFile, &fd);

  if(hFF == INVALID_HANDLE_VALUE)
  {
    CString csMsg;
    csMsg.Format(theApp.LoadString(IDS_ERROR21), (LPCSTR)csTempFile);
    MyMessageBox(csMsg, MB_OK | MB_ICONHAND | MB_SETFOREGROUND);

    return(-1);  // file does not exist - error (at this point, anyway)
  }

  dwSizeNew = fd.nFileSizeLow;
  ASSERT(!fd.nFileSizeHigh);

  FindClose(hFF);

#ifdef _DEBUG
  int iTemp = CompareFileTime(timeOld , timeNew);

  if(iTemp < 0)
    TRACE("* EXISTING FILE IS OLDER THAN THIS ONE - " + csFile + "\r\n");
  else if(iTemp > 0)
    TRACE("* EXISTING FILE IS NEWER THAN THIS ONE - " + csFile + "\r\n");
  else
    TRACE("* EXISTING FILE HAS THE SAME DATE/TIME - " + csFile + "\r\n");

#endif // _DEBUG


  // first thing's first - see if current file is newer

  DWORD dw1;

  if(!GetBinaryType(csTempFile, &dw1))  // use temp file to avoid sharing problems
  {
    // check to see if there is version information, nonetheless.  There might
    // be, after all, and if there is, it's probably a DLL, so I must treat it as
    // a 32-bit image file in the next section...

    DWORD dwHandle = 0;

    if(GetFileVersionInfoSize((LPSTR)(LPCSTR)csTempFile, &dwHandle) > 0)
      dw1 = SCS_32BIT_BINARY;  // just because, do this
    else
      dw1 = 0xffffffff;
  }

  if(dw1 != 0xffffffff &&
     dw1 != SCS_DOS_BINARY &&
     dw1 != SCS_OS216_BINARY &&
     dw1 != SCS_PIF_BINARY)      // these never have version info that I can read
  {

    DWORD dwHandle, cbSize, dwVerH, dwVerL;
    UINT cbLen;
    LPSTR lpBuf;
    VS_FIXEDFILEINFO *pInfo = NULL;
    CString csTemp, csApp;

    int iVerOld[4]={0,0,0,0}, iVerNew[4]={0,0,0,0};

    // VERSION INFO FOR ORIGINAL FILE

    cbSize = GetFileVersionInfoSize((LPSTR)(LPCSTR)csFile, &dwHandle);

    lpBuf = csTemp.GetBufferSetLength(cbSize + 32);

    if(cbSize &&
       GetFileVersionInfo((LPSTR)(LPCSTR)csApp, dwHandle, cbSize + 32, lpBuf))
    {
      if(VerQueryValue(lpBuf, "\\", (LPVOID *)&pInfo, &cbLen))
      {
        dwVerH = pInfo->dwFileVersionMS;
        dwVerL = pInfo->dwFileVersionLS;
    
        iVerOld[0] = HIWORD(dwVerH);
        iVerOld[1] = LOWORD(dwVerH);
        iVerOld[2] = HIWORD(dwVerL);
        iVerOld[3] = LOWORD(dwVerL);
      }
    }

    csTemp.ReleaseBuffer(0);

    // VERSION INFO FOR TEMPORARY FILE

    cbSize = GetFileVersionInfoSize((LPSTR)(LPCSTR)csTempFile, &dwHandle);

    lpBuf = csTemp.GetBufferSetLength(cbSize + 32);

    if(cbSize &&
       GetFileVersionInfo((LPSTR)(LPCSTR)csApp, dwHandle, cbSize + 32, lpBuf))
    {
      if(VerQueryValue(lpBuf, "\\", (LPVOID *)&pInfo, &cbLen))
      {
        dwVerH = pInfo->dwFileVersionMS;
        dwVerL = pInfo->dwFileVersionLS;
    
        iVerNew[0] = HIWORD(dwVerH);
        iVerNew[1] = LOWORD(dwVerH);
        iVerNew[2] = HIWORD(dwVerL);
        iVerNew[3] = LOWORD(dwVerL);
      }
    }

    csTemp.ReleaseBuffer(0);

    if(iVerOld[0] > iVerNew[0])
      return(0);
    if(iVerOld[0] < iVerNew[0])
      return(1);

    if(iVerOld[1] > iVerNew[1])
      return(0);
    if(iVerOld[1] < iVerNew[1])
      return(1);

    if(iVerOld[2] > iVerNew[2])
      return(0);
    if(iVerOld[2] < iVerNew[2])
      return(1);

    if(iVerOld[3] > iVerNew[3])
      return(0);
    if(iVerOld[3] < iVerNew[3])
      return(1);

    // version #'s are equal.  See if the files are the same...

    if(dwSizeOld != dwSizeNew)
    {
      // size mismatch - check dates

      if(CompareFileTime(timeOld , timeNew) <= 0)
      {
        return(1);  // overwrite always
      }
      else
      {
        return(0);
      }
    }
    else
    {
      // sizes match - check if files match...
      // NOTE:  binary files are handled differently.... may be shared

      if(CompareFiles(csFile, csTempFile))
      {
        return(2);  // overwrite (sorta) but neither prompt nor
                    // actually write the file... it's the same,
                    // so just leave it alone.  It may be 'in use'
      }
      else if(CompareFileTime(timeOld , timeNew) <= 0)
      {
        return(1);  // overwrite always
      }
      else
      {
        return(0);
      }
    }
  }

  // ** AT THIS POINT, COMPARE DATES AND TIMES **

  if(dwSizeOld != dwSizeNew)  // sizes don't match
  {
    if(CompareFileTime(timeOld , timeNew) <= 0)
    {
      return(1);  // overwrite always
    }
    else
    {
      return(0);
    }
  }
  else // same size, may indicate 'same file'
  {
    if(CompareFiles(csFile, csTempFile))
    {
      if(CompareFileTime(timeOld , timeNew) >= 0)
        return(2);  // don't actually copy - just "leave it" and don't prompt
      else
        return(1);  // overwrite always (just because)
    }
    else if(CompareFileTime(timeOld , timeNew) <= 0)
    {
      return(1);  // overwrite always
    }
    else
    {
      return(0);
    }
  }

  return(-1);  // if it gets here it's an error
}


// FileCopyCompareVersions (calls 'PreFileCopyCompareVersions')
// return value:  0  do not overwrite (existing file is newer)
//                1  overwrite (existing file is older and/or binary mismatch)
//               -1  error

int FileCopyCompareVersions(FileCopyInfo *pFCI, FILETIME timeNew)
{
  // use the file dates with 2 seconds of "slop", since I
  // may be working with a FAT-16 system, which will truncate
  // down to the nearest 2-second interval.

  timeNew.dwLowDateTime += 20000000L;  // 4 seconds (1 = 1e-7 sec)
  if(timeNew.dwLowDateTime < 20000000L)
    timeNew.dwHighDateTime ++;  // a pseudo-carry

  int iRval = PreFileCopyCompareVersions(pFCI, timeNew);

  if(iRval)
  {
    if(iRval == 1)
      return(iRval);  // overwrite this file
    else if(iRval == 2)
      return(0);      // don't overwrite, and don't prompt either
  }

  // *IF* the file is marked *SYSTEM* do *NOT* prompt if the existing file's version
  // is NEWER than the current one, and the destination directory is 'sysdir'.  Files
  // in this directory are expected to be updated frequently.

  if(pFCI->abSharedFile[pFCI->iCurrentIndex])
  {
    // shared file - check destination path to see if it matches '%sysdir%'

    CString csSysDir = "%sysdir%";
    if(theApp.ExpandStrings(csSysDir))
    {
      if(csSysDir.GetLength() && csSysDir[csSysDir.GetLength() - 1] != '\\')
        csSysDir += '\\';

      if(pFCI->acsDestFileName[pFCI->iCurrentIndex].GetLength() > csSysDir.GetLength())
      {
        if(!_strnicmp(csSysDir, pFCI->acsDestFileName[pFCI->iCurrentIndex],
                      csSysDir.GetLength()))
        {
          return(0);  // *DO NOT OVERWRITE!*
        }
      }
    }
  }

  CString csMsg;
  csMsg.Format(theApp.LoadString(IDS_ERROR22), (LPCSTR)pFCI->acsDestFileName[pFCI->iCurrentIndex]);

  int i1 = MyMessageBox(csMsg, MB_YESNOCANCEL | MB_DEFBUTTON1
                                | MB_ICONASTERISK | MB_SETFOREGROUND);

  if(i1 == IDYES)
    return(0);
  else if(i1 == IDNO)
    return(1);          // "no" is "overwrite"
  else // if(i1 == IDCANCEL)
    return(-1);

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

  csRval.ReleaseBuffer(cb1);

  return csRval;
}

