//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//           ____   _____  _____  _  _                                      //
//          |  _ \ | ____||  ___|(_)| |  ___     ___  _ __   _ __           //
//          | |_) ||  _|  | |_   | || | / _ \   / __|| '_ \ | '_ \          //
//          |  __/ | |___ |  _|  | || ||  __/ _| (__ | |_) || |_) |         //
//          |_|    |_____||_|    |_||_| \___|(_)\___|| .__/ | .__/          //
//                                                   |_|    |_|             //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//       Copyright (c) 1998-2016 by S.F.T. Inc. - All rights reserved       //
//  Use, copying, and distribution of this software are licensed according  //
//    to the GPLv2, LGPLv2, or BSD license, as appropriate (see COPYING)    //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////

// utilities designed to read PE files

#include "stdafx.h"
#include "setup.h"
#include "PEFile.h"


LPCVOID WINAPI IsSelfExtracting(LPCVOID lpSelf, DWORD cbSelf)
{
  if(cbSelf < 0x1000)
    return(NULL);  // a simple test that covers it all...

  if(*(USHORT *)lpSelf != IMAGE_DOS_SIGNATURE)
    return(NULL);

  if(*(DWORD *)NTSIGNATURE(lpSelf) != IMAGE_NT_SIGNATURE)
    return(NULL);

  PIMAGE_NT_HEADERS pNTH = ((PIMAGE_NT_HEADERS)((BYTE *)lpSelf + ((PIMAGE_DOS_HEADER)lpSelf)->e_lfanew));
  PIMAGE_OPTIONAL_HEADER poh = (PIMAGE_OPTIONAL_HEADER)OPTHDROFFSET(lpSelf);
  PIMAGE_SECTION_HEADER pISH = IMAGE_FIRST_SECTION(pNTH);

  ASSERT(&(pNTH->OptionalHeader) == poh);

  int iNumSections = pNTH->FileHeader.NumberOfSections;

//  // OK, now I want to get the size of code + size of initialized data
//  // + size of headers and see if that's the end of the code...
//
//  DWORD cb1 = poh->SizeOfCode
//            + poh->SizeOfInitializedData
//            + poh->SizeOfHeaders;

  DWORD cb1 = 0, cb2;
  int i1, i2;
  for(i1=0; i1 < iNumSections; i1++)
  {
    cb2 = pISH[i1].SizeOfRawData + pISH[i1].PointerToRawData;

    cb1 = max(cb1, cb2);  // get end of file
  }

  // if DEBUG info is present, I must go past it...

  TRACE("FYI - 'FileHeader' characteristics = %08xH\r\n", pNTH->FileHeader.Characteristics);


//#ifdef _DEBUG
//
//  if(poh->NumberOfRvaAndSizes > IMAGE_DIRECTORY_ENTRY_RESOURCE)
//  {
//    // get virtual address for this section...
//
//    DWORD dwv1 = poh->DataDirectory[IMAGE_DIRECTORY_ENTRY_RESOURCE].VirtualAddress;
//    int cbItem = poh->DataDirectory[IMAGE_DIRECTORY_ENTRY_RESOURCE].Size;
//
//    // find out which image section header "contains" it
//
//    if(cbItem && dv1)
//    {
//      int iNumItems = cbItem / sizeof(IMAGE_RESOURCE_DIRECTORY);
//
//      for(i1=0; i1 < iNumSections; i1++)
//      {
//        if(dwv1 >= pISH[i1].VirtualAddress &&
//           dwv1 < (pISH[i1].VirtualAddress + pISH[i1].SizeOfRawData))
//        {
//          // It's this one - get the real address within the file...
//
//          PIMAGE_RESOURCE_DIRECTORY pdrRoot = (PIMAGE_RESOURCE_DIRECTORY)
//            ((DWORD)lpSelf + dwv1 - pISH[i1].VirtualAddress
//                           + pISH[i1].PointerToRawData);
//
//
//          PIMAGE_RESOURCE_DIRECTORY_ENTRY prde = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)
//            (pdrRoot + 1);
//
//          for(i2=0; i2 < pdrRoot->NumberOfIdEntries; i2++)
//          {
//            PIMAGE_RESOURCE_DIRECTORY pdrType = (PIMAGE_RESOURCE_DIRECTORY)
//              (((int)prdRoot + (int)(prde->OffsetToData & 0x7fffffff)));
//
////	          /* mask off most significant bit of the data offset */
////	          prdType = (PIMAGE_RESOURCE_DIRECTORY)
////              ((DWORD)prdType ^ 0x80000000);  // why???
//
//            PIMAGE_RESOURCE_DIRECTORY_ENTRY prde2 = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)
//              (pdrType + 1);
//          }
//
//          break;
//        }
//      }
//    }
//  }
//
//
//#endif _DEBUG


  if(!(pNTH->FileHeader.Characteristics & IMAGE_FILE_DEBUG_STRIPPED))
  {
    // find debug information 'IMAGE_DIRECTORY_ENTRY_DEBUG'

    if(poh->NumberOfRvaAndSizes > IMAGE_DIRECTORY_ENTRY_DEBUG)
    {
      // get virtual address for this section...

      DWORD dwv1 = poh->DataDirectory[IMAGE_DIRECTORY_ENTRY_DEBUG].VirtualAddress;
      int iNumItems = poh->DataDirectory[IMAGE_DIRECTORY_ENTRY_DEBUG].Size
                    / sizeof(IMAGE_DEBUG_DIRECTORY);  // # of items in directory

      // find out which image section header "contains" it

      for(i1=0; i1 < iNumSections; i1++)
      {
        if(dwv1 >= pISH[i1].VirtualAddress &&
           dwv1 < (pISH[i1].VirtualAddress + pISH[i1].SizeOfRawData))
        {
          // It's this one - get the real address within the file...

          PIMAGE_DEBUG_DIRECTORY pdd = (PIMAGE_DEBUG_DIRECTORY)
            ((DWORD)lpSelf + dwv1 - pISH[i1].VirtualAddress
                           + pISH[i1].PointerToRawData);

          // using the debug directory, calculate its size and
          // update the total file size.

          for(i2=0; i2 < iNumItems; i2++)
          {
            cb2 = (pdd[i2].PointerToRawData + pdd[i2].SizeOfData)
                % poh->FileAlignment;

            if(cb2)  // need to go UP to the next 'SectionAlignment'
              cb2 = poh->FileAlignment - cb2;

            cb2 += pdd[i2].PointerToRawData + pdd[i2].SizeOfData;

            cb1 = max(cb1,cb2);
          }

          break;
        }
      }
    }
  }
  else
  {
#ifdef _DEBUG
    ASSERT(0);
#endif // _DEBUG
  }

  // find the last header, and add its size to the offset within
  // the file to get the total file size.


  ASSERT(cb1 <= cbSelf);

  if(cb1 >= cbSelf)
    return(NULL);  // not a self-extracting file

  // TODO:  is this DEBUG info????



  return((BYTE *)lpSelf + cb1);
}


#if 0


BOOL	WINAPI GetDosHeader (LPVOID, PIMAGE_DOS_HEADER);
BOOL	WINAPI GetPEFileHeader (LPVOID, PIMAGE_FILE_HEADER);
BOOL	WINAPI GetPEOptionalHeader (LPVOID, PIMAGE_OPTIONAL_HEADER);



/* copy dos header information to structure */
BOOL WINAPI GetDosHeader(LPVOID lpFile, PIMAGE_DOS_HEADER pHeader)
{
  /* dos header rpresents first structure of bytes in file */
  if(*(USHORT *)lpFile == IMAGE_DOS_SIGNATURE)
    CopyMemory((LPVOID)pHeader, lpFile, sizeof (IMAGE_DOS_HEADER));
  else
    return FALSE;
  
  return TRUE;
}

/* return file signature */
DWORD WINAPI ImageFileType(LPVOID lpFile)
{
  /* dos file signature comes first */

  if(*(USHORT *)lpFile == IMAGE_DOS_SIGNATURE)
  {
    /* determine location of PE File header from dos header */
    if(LOWORD(*(DWORD *)NTSIGNATURE(lpFile)) == IMAGE_OS2_SIGNATURE ||
      LOWORD(*(DWORD *)NTSIGNATURE(lpFile)) == IMAGE_OS2_SIGNATURE_LE)
    {
      return(DWORD)LOWORD(*(DWORD *)NTSIGNATURE(lpFile));
    }
    else if(*(DWORD *)NTSIGNATURE (lpFile) == IMAGE_NT_SIGNATURE)
    {
      return IMAGE_NT_SIGNATURE;
    }
    else
      return IMAGE_DOS_SIGNATURE;
  }
  else
  {
    /* unknown file type */
    return 0;
  }
}


/* copy file header information to structure */
BOOL WINAPI GetPEFileHeader(LPVOID lpFile,
                            PIMAGE_FILE_HEADER pHeader)
{
  /* file header follows dos header */
  if(ImageFileType(lpFile) == IMAGE_NT_SIGNATURE)
    CopyMemory((LPVOID)pHeader, PEFHDROFFSET (lpFile), sizeof (IMAGE_FILE_HEADER));
  else
    return FALSE;
  
  return TRUE;
}


/* copy optional header info to structure */
BOOL WINAPI GetPEOptionalHeader(LPVOID lpFile,
                                PIMAGE_OPTIONAL_HEADER pHeader)
{
  /* optional header follows file header and dos header */
  if(ImageFileType (lpFile) == IMAGE_NT_SIGNATURE)
    CopyMemory((LPVOID)pHeader, OPTHDROFFSET (lpFile), sizeof (IMAGE_OPTIONAL_HEADER));
  else
    return FALSE;
  
  return TRUE;
}


/* return the total number of sections in the module */
int WINAPI NumOfSections(LPVOID lpFile)
{
  /* number os sections is indicated in file header */

  return((int)((PIMAGE_FILE_HEADER)PEFHDROFFSET (lpFile))->NumberOfSections);
}

#endif // 0

