//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//                 ____   _____  _____  _  _           _                    //
//                |  _ \ | ____||  ___|(_)| |  ___    | |__                 //
//                | |_) ||  _|  | |_   | || | / _ \   | '_ \                //
//                |  __/ | |___ |  _|  | || ||  __/ _ | | | |               //
//                |_|    |_____||_|    |_||_| \___|(_)|_| |_|               //
//                                                                          //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//       Copyright (c) 1998-2016 by S.F.T. Inc. - All rights reserved       //
//  Use, copying, and distribution of this software are licensed according  //
//    to the GPLv2, LGPLv2, or BSD license, as appropriate (see COPYING)    //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////

// some of this is adapted from existing microsoft source/documentation
// most of the PE File structures are defined in 'WinNT.h' in the SDK
// and/or 'ntimage.h' in the DDK.

#ifndef _PEFILE_H_
#define _PEFILE_H_

#define SIZE_OF_NT_SIGNATURE	sizeof (DWORD)


/* global macros to define header offsets into file */
/* offset to PE file signature				       */
#define NTSIGNATURE(a) ((LPVOID)((BYTE *)a		     +	\
			((PIMAGE_DOS_HEADER)a)->e_lfanew))

/* DOS header identifies the NT PEFile signature dword
   the PEFILE header exists just after that dword	       */
#define PEFHDROFFSET(a) ((LPVOID)((BYTE *)a		     +	\
			 ((PIMAGE_DOS_HEADER)a)->e_lfanew    +	\
			 SIZE_OF_NT_SIGNATURE))

/* PE optional header is immediately after PEFile header       */
#define OPTHDROFFSET(a) ((LPVOID)((BYTE *)a		     +	\
			 ((PIMAGE_DOS_HEADER)a)->e_lfanew    +	\
			 SIZE_OF_NT_SIGNATURE		     +	\
			 sizeof (IMAGE_FILE_HEADER)))

/* section headers are immediately after PE optional header    */
#define SECHDROFFSET(a) ((LPVOID)((BYTE *)a		     +	\
			 ((PIMAGE_DOS_HEADER)a)->e_lfanew    +	\
			 SIZE_OF_NT_SIGNATURE		     +	\
			 sizeof (IMAGE_FILE_HEADER)	     +	\
			 sizeof (IMAGE_OPTIONAL_HEADER)))

typedef struct tagImportDirectory
{
  DWORD    dwRVAFunctionNameList;
  DWORD    dwUseless1;
  DWORD    dwUseless2;
  DWORD    dwRVAModuleName;
  DWORD    dwRVAFunctionAddressList;
} IMAGE_IMPORT_MODULE_DIRECTORY, * PIMAGE_IMPORT_MODULE_DIRECTORY;


LPCVOID WINAPI IsSelfExtracting(LPCVOID,DWORD);  // points to end of file if it is

#endif // _PEFILE_H_
