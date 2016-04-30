///////////////////////////////////////////////////////////////////////////////////////////
//                                                                                       //
//    ____        _      _               _    _   _        _                     _       //
//   / ___| __ _ | |__  (_) _ __    ___ | |_ | | | |  ___ | | _ __    ___  _ __ | |__    //
//  | |    / _` || '_ \ | || '_ \  / _ \| __|| |_| | / _ \| || '_ \  / _ \| '__|| '_ \   //
//  | |___| (_| || |_) || || | | ||  __/| |_ |  _  ||  __/| || |_) ||  __/| | _ | | | |  //
//   \____|\__,_||_.__/ |_||_| |_| \___| \__||_| |_| \___||_|| .__/  \___||_|(_)|_| |_|  //
//                                                           |_|                         //
//                                                                                       //
///////////////////////////////////////////////////////////////////////////////////////////
//                                                                                       //
//                Copyright (c) 2016 by S.F.T. Inc. - All rights reserved                //
//                                                                                       //
///////////////////////////////////////////////////////////////////////////////////////////

#ifndef _CABINET_HELPER_H_INCLUDED_
#define _CABINET_HELPER_H_INCLUDED_


// main cabinet helper API functions
BOOL CopyFilesFromCabinet(CStringArray &acsEntries);
BOOL ExtractFilesFromTempCabinet(LPCSTR szCab, LPCSTR szDestPath);
CString ExtractFileFromCabAsString(int iCab, LPCSTR szFileName);

// other useful utility functions (some used by above, generically available)
BOOL CompareFiles(LPCSTR szFile1, LPCSTR szFile2);
int CompareFileTime(FILETIME &time1, FILETIME &time2);
CString GetFinalFileName(LPCSTR szFile);
CString ReadFileIntoString(LPCSTR szFile);




#endif // _CABINET_HELPER_H_INCLUDED_
