//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                              //
//   ____         _   __  _____        _                      _                __  __         _                 //
//  / ___|   ___ | | / _|| ____|__  __| |_  _ __  __ _   ___ | |_  ___   _ __ |  \/  |  __ _ | | __ ___  _ __   //
//  \___ \  / _ \| || |_ |  _|  \ \/ /| __|| '__|/ _` | / __|| __|/ _ \ | '__|| |\/| | / _` || |/ // _ \| '__|  //
//   ___) ||  __/| ||  _|| |___  >  < | |_ | |  | (_| || (__ | |_| (_) || |   | |  | || (_| ||   <|  __/| |     //
//  |____/  \___||_||_|  |_____|/_/\_\ \__||_|   \__,_| \___| \__|\___/ |_|   |_|  |_| \__,_||_|\_\\___||_|     //
//                                                                                                              //
//                                                                                                              //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                              //
//                            Copyright (c) 2014 by S.F.T. Inc. - All rights reserved                           //
//                    Use, copying, and distribution of this software are licensed according                    //
//                      to the GPLv2, LGPLv2, or BSD license, as appropriate (see COPYING)                      //
//                                                                                                              //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "SelfExtractorMaker.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


//#ifndef FALSE
//#define FALSE 0
//#define TRUE !0
//#endif // FALSE

// The one and only application object

CWinApp theApp;

using namespace std;

BOOL DoTheThing(CString &csDestPath, CString &csLicenseFile);

// USAGE:
// SelfExtractorMaker [destdir] [licensefile]
//
// where 'destdir' is the destination directory
//  and  'licensefile' is the optional license file name

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	HMODULE hModule = ::GetModuleHandle(NULL);

	if (hModule != NULL)
	{
		// initialize MFC and print and error on failure
		if (!AfxWinInit(hModule, NULL, ::GetCommandLine(), 0))
		{
			// TODO: change error code to suit your needs
			_tprintf(_T("Fatal Error: MFC initialization failed\n"));
			nRetCode = 1;
		}
		else
		{
			CString csDestPath, csLicenseFile;

      if(argc > 2)
      {
        csLicenseFile = argv[2];
      }
      if(argc > 1)
      {
        csDestPath = argv[1];
      }
      else
      {
//        csDestPath = ".\\";
        fputs("Usage:  SelfExtractorMaker [destpath] [licensefile]\r\n", stderr);
#ifdef _DEBUG
        Sleep(5000);
#endif // _DEBUG
        return 1;
      }

      if(!DoTheThing(csDestPath, csLicenseFile))
      {
        nRetCode = 1;
      }
      else
      {
        nRetCode = 0;
      }
    }
	}
	else
	{
		// TODO: change error code to suit your needs
		_tprintf(_T("Fatal Error: GetModuleHandle failed\n"));
		nRetCode = 1;
	}

	return nRetCode;
}

// use 'csDestPath' and optionally 'csLicenseFile' to create a self-extracting
// executable setup program from the following files:
//   _SETUP.EXE
//   _SETUP.INF
//   SFTCAB.1
// (optionally) the file name in csLicenseFile
//
// output file is SETUP.EXE
//
// _SETUP.EXE's image will be followed by:
// 4 bytes: length of INF file
// n bytes: INF file data
// 4 bytes: length of license file (can be zero)
// n bytes: license file data (or nothing if zero-length)
// 4 bytes: length of cabinet file
// n bytes: cabinet file data


BOOL DoTheThing(CString &csDestPath, CString &csLicenseFile)
{
  CString csTemp, csInf, csLicense, csBuf;
  HANDLE hFile;
  DWORD cbFile, cb1, cb2;

  hFile = CreateFile(csDestPath + "_SETUP.INF",
                      GENERIC_READ, 0, NULL, OPEN_EXISTING,
                      FILE_ATTRIBUTE_NORMAL, NULL);

  if(hFile == INVALID_HANDLE_VALUE)
  {
    fputs("Unable to open '_SETUP.INF'!!!\r\n", stderr);

    return FALSE;  // failed
  }

  // find out size of file, then read it

  cbFile = GetFileSize(hFile, NULL);

  if(!ReadFile(hFile, csInf.GetBufferSetLength(cbFile),
                cbFile, &cb1, NULL)
      || cb1 != cbFile)
  {
    csInf.ReleaseBuffer(0);
    CloseHandle(hFile);

    fputs("Read error on 'SETUP.INF'\r\n",stderr);
    return FALSE;
  }
            
  csInf.ReleaseBuffer(cbFile);
  csInf.ReleaseBuffer(-1);  // this trims null bytes

  CloseHandle(hFile);


  if(csLicenseFile.GetLength())
  {
    // A license file, if it exists, must be called 'license.txt'
    // and exist on disk #1 along with 'setup.inf'.

    hFile = CreateFile(csLicenseFile,
                        GENERIC_READ, 0, NULL, OPEN_EXISTING,
                        FILE_ATTRIBUTE_NORMAL, NULL);

    if(hFile == INVALID_HANDLE_VALUE)
    {
      fputs("Unable to open license file " + csLicenseFile + "\r\n", stderr);

      return FALSE;  // failed
    }

    // find out size of file, then read it

    cbFile = GetFileSize(hFile, NULL);

    if(!ReadFile(hFile, csLicense.GetBufferSetLength(cbFile),
                  cbFile, &cb1, NULL)
        || cb1 != cbFile)
    {
      csLicense.ReleaseBuffer(0);
      CloseHandle(hFile);

      fputs("Read error on 'SETUP.INF'\r\n", stderr);
      return FALSE;
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
    fputs("Unable to open '_SETUP.INF'!!!\r\n",stderr);

    return FALSE;  // failed
  }

  // find out size of file, then read it

  cbFile = GetFileSize(hFile, NULL);

  if(!ReadFile(hFile, csBuf.GetBufferSetLength(cbFile),
                cbFile, &cb1, NULL)
      || cb1 != cbFile)
  {
    csBuf.ReleaseBuffer(0);
    CloseHandle(hFile);

    fputs("Read error on 'SFTCAB.1'\r\n",stderr);
    return FALSE;
  }
            
  csBuf.ReleaseBuffer(cbFile);
  CloseHandle(hFile);


  // OK, full plate - I got 'SFTCAB.1' and I got 'SETUP.INF'
  // all put together here, ready for packing.

  DeleteFile("SETUP.EXE");

  if(!CopyFile("_SETUP.EXE", "SETUP.EXE", FALSE))
  {
    fputs("Unable to make a copy of 'SETUP.EXE' for output\r\n",stderr);
    return FALSE;
  }


  // NOW, open 'SETUP.EXE', append a "thingie" to it that marks
  // the size of the INF, the INF text itself, followed by the
  // 'CAB' file data.

  hFile = CreateFile(csDestPath + "SETUP.EXE",
                      GENERIC_READ | GENERIC_WRITE,
                      0, NULL, OPEN_EXISTING,
                      FILE_ATTRIBUTE_NORMAL, NULL);

  if(hFile == INVALID_HANDLE_VALUE)
  {
    fputs("Unable to open 'SETUP.EXE'!!!\r\n",stderr);

    return FALSE;  // failed
  }

  SetFilePointer(hFile, 0, NULL, FILE_END);  // point to the end

  // write the length of the INF file

  cb2 = csInf.GetLength();

  if(!WriteFile(hFile, &cb2, sizeof(cb2), &cb1, NULL)
      || cb1 != sizeof(cb2))
  {
    CloseHandle(hFile);
    fputs("WRITE error on 'SETUP.EXE'\r\n",stderr);
    return FALSE;  // failed
  }
            
  // write the INF text (no extra NULL byte either)

  if(!WriteFile(hFile, (LPCSTR)csInf, cb2, &cb1, NULL)
      || cb1 != cb2)
  {
    CloseHandle(hFile);
    fputs("WRITE error on 'SETUP.EXE'\r\n", stderr);
    return FALSE;  // failed
  }

  // write the length of the LICENSE file

  cb2 = csLicense.GetLength();

  if(!WriteFile(hFile, &cb2, sizeof(cb2), &cb1, NULL)
      || cb1 != sizeof(cb2))
  {
    CloseHandle(hFile);
    fputs("WRITE error on 'SETUP.EXE'\r\n",stderr);
    return FALSE;  // failed
  }
            
  // write the LICENSE text if it's not blank

  if(csLicense.GetLength() &&
      (!WriteFile(hFile, (LPCSTR)csLicense, cb2, &cb1, NULL)
      || cb1 != cb2))
  {
    CloseHandle(hFile);
    fputs("WRITE error on 'SETUP.EXE'\r\n",stderr);
    return FALSE;  // failed
  }

  // write the CAB file

  if(!WriteFile(hFile, (LPCSTR)csBuf, cbFile, &cb1, NULL)
      || cb1 != cbFile)
  {
    CloseHandle(hFile);
    fputs("WRITE error on 'SETUP.EXE'\r\n",stderr);
    return FALSE;  // failed
  }

  CloseHandle(hFile);  // I'm done with it now!  YAY!

  return TRUE; // success
}
