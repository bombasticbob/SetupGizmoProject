//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//             ____  _                   _      _  _        _               //
//            / ___|| |  __ _  ___  ___ | |    (_)| |__    | |__            //
//           | |    | | / _` |/ __|/ __|| |    | || '_ \   | '_ \           //
//           | |___ | || (_| |\__ \\__ \| |___ | || |_) |_ | | | |          //
//            \____||_| \__,_||___/|___/|_____||_||_.__/(_)|_| |_|          //
//                                                                          //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//          Copyright (c) 2016 by S.F.T. Inc. - All rights reserved         //
//  Use, copying, and distribution of this software are licensed according  //
//    to the GPLv2, LGPLv2, or BSD license, as appropriate (see COPYING)    //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////

// This is a 'clone' of some basic MFC implementations, particularly those that
// I need to make SETUP and Uninstall and Autorun work.  It's not a rip-off,
// it's a FRESH implementation that I wrote from scratch, MOSTLY following the
// specs of MFC [so I didn't have to re-write applications to use it], and with
// some minor differences that require a bit of code editing or care in how I
// make use of the NEW class definitions.  But the edits are SMALL.
//
// this is "as-is" if anyone wants to use it.  You're welcome.  NO warranty, etc.
//
// OK, *WHY* did I do something that's already been done?  Why reinvent the wheel?
//
// ANSWER:  because the WHEEL was TOO HEAVY and TOO MONOLITHIC and, quite frankly,
//          contains WAY TOO MUCH CRAP, and made EXE images 1.5Mb TOO LARGE
//
// I needed to make my EXE images *TINY*.  I mean *REALLY* *TINY*.  And using
// a "shared MFC" and "shared runtime" DLL is *IMPOSSIBLE* for an INSTALLER.
//
// In the PAST, MFC could be made 'leaner' by turning certain things OFF.
// That is now *BROKEN* (in DevStudio 2010 anyway) and I do NOT expect it to
// NOT be broken in LATER versions of DevStudio (because of the 'the METRO'
// and 'Universal' focus Microsoft currently has).  In fact, I expect it to be
// WORSE than before in later DevStudio releases.
//
// In the mean time, REGULAR developers (like us) who don't want MEGABYTES OF
// GARBAGE filling up the downloadable files for installers need to find a way
// around it.  In THIS case, I simply re-wrote the appropriate classes.
//
// The CString and CArray clones might be usable in POSIX code.  I haven't tried
// it but I used standard C library functions.  I use these things a LOT, and so
// you're welcome to use them.
//
// Keep in mind I did NOT write this for EFFICIENCY.  I wrote it for EXPEDIENCY
// and FUNCTIONALITY and TINY FOOTPRINT.  Not everything meets all 3 goals, but
// it's pretty good and I like it as it is.


#ifndef _CLASSLIB_H_INCLUDED_
#define _CLASSLIB_H_INCLUDED_

#pragma once

#include <Windows.h>

#include <stdio.h>
#include <stdarg.h>
#include <varargs.h>
#include <new> // for placement new operators

// this is where I define my new updated application class
typedef UINT (__cdecl *MYTHREADPROC)(LPVOID);

// debug things like 'ASSERT' and 'TRACE'

#ifdef _DEBUG
void DoTrace(UINT nLine, LPCSTR szFile, LPCSTR szFormat, ...);
#define TRACE(X,...) DoTrace(__LINE__, __FILE__, X, __VA_ARGS__)
#define ASSERT(X) { if(!(X)) { CString csTemp; csTemp.Format("WARNING:  ASSERT fail at %d in %s", __LINE__, __FILE__); \
                            int iRval = MyMessageBox(csTemp, MB_ABORTRETRYIGNORE | MB_SYSTEMMODAL); \
                            if(iRval == IDABORT) { exit(-1); } else if(iRval == IDRETRY) { DebugBreak(); } } }
#define TRACE0(X) ::OutputDebugString(X)
#else // !_DEBUG
#define TRACE(X,...)
#define ASSERT(X)
#define TRACE0(X)
#endif // _DEBUG

// ---------------------------------------------------------------
// string and string array classes - simplified, TINY, *NOT* MFC
// these were WRITTEN FROM SCRATCH.  The names were NOT changed so
// that existing MFC code would NOT break when I use THIS instead.

#pragma warning(disable: 4996)
class CString
{
public:

  CString() { Initialize(); }
  virtual ~CString();

  CString(char);
  CString(const char *);
  CString(const CString &);

  int Format(const char *, ...);
  int Format(UINT, ...);

  CString & operator +=(char);
  CString & operator +=(const char *);
  CString & operator +=(const CString &);

  CString & operator =(char);
  CString & operator =(const char *);
  CString & operator =(const CString &);

  char GetAt(int nIndex) { if(m_pBuffer && nIndex < (int)m_cbStringLength) return m_pBuffer[nIndex]; return 0; }
  char operator [] (int nIndex) { return GetAt(nIndex); }

  operator const char *(void) const;
  void * operator new(size_t, void *pAct) { return pAct; } // typed new/delete for use in CArray
  void operator delete(void *, void *) { }  // placement delete does nothing

  int Compare(const CString &rX) const { return strcmp((LPCSTR)*this, (LPCSTR)rX); }
  int Compare(const char *szX) const { return strcmp((LPCSTR)*this, szX); }
  int Compare(char c) const { char tbuf[2]; tbuf[0] = c; tbuf[1] = 0; return Compare(tbuf); }

  int CompareNoCase(const CString &rX) const  { return stricmp((LPCSTR)*this, (LPCSTR)rX); }
  int CompareNoCase(const char *szX) const { return stricmp((LPCSTR)*this, szX); }
  int CompareNoCase(char c) const { char tbuf[2]; tbuf[0] = c; tbuf[1] = 0; return CompareNoCase(tbuf); }

  int operator < (const char *pX) const { return Compare(pX) < 0; }
  int operator < (char x) const { return Compare(x) < 0; }
  int operator < (const CString &rX) const { return Compare(rX) < 0; }
  int operator > (const char *pX) const { return Compare(pX) > 0; }
  int operator > (char x) const { return Compare(x) > 0; }
  int operator > (const CString &rX) const { return Compare(rX) > 0; }
  int operator <= (const char *pX) const { return Compare(pX) <= 0; }
  int operator <= (char x) const { return Compare(x) <= 0; }
  int operator <= (const CString &rX) const { return Compare(rX) <= 0; }
  int operator >= (const char *pX) const { return Compare(pX) >= 0; }
  int operator >= (char x) const { return Compare(x) >= 0; }
  int operator >= (const CString &rX) const { return Compare(rX) >= 0; }

  int operator == (const char *pX) const { return !Compare(pX); }
  int operator == (char x) const { return !Compare(x); }
  int operator == (const CString &rX) const { return !Compare(rX); }
  int operator != (const char *pX) const { return Compare(pX); }
  int operator != (char x) const { return Compare(x); }
  int operator != (const CString &rX) const { return Compare(rX); }

  char * GetBufferSetLength(int nLength);
  void ReleaseBuffer(void);
  void ReleaseBuffer(int nLength);
  char * GetBuffer() { return GetBufferSetLength(0); } // 0 leaves the length alone

  int GetLength() const { return (int)m_cbStringLength; }

  void SetAt(int iIndex, char cValue)
  {
    LPSTR lp1;
    if(GetLength() < iIndex)
    {
      lp1 = GetBufferSetLength(iIndex + 2);
      lp1[iIndex + 1] = 0;
    }
    else
    {
      lp1 = GetBuffer();
    }
    if(lp1)
    {
      lp1[iIndex] = cValue;
    }
    ReleaseBuffer();
  }

  CString Left(int nLen) const;
  CString Mid(int nPos, int nLen=-1) const;
  CString Right(int nLen) const;

  CString &MakeUpper(void)
  {
    int i1, iLen = GetLength();
    LPSTR lp1 = GetBuffer();
    if(lp1)
    {
      for(i1=0; i1 < iLen; i1++, lp1++)
      {
        *lp1 = toupper(*lp1);
      }
    }
    return *this;
  }

  CString &MakeLower(void)
  {
    int i1, iLen = GetLength();
    LPSTR lp1 = GetBuffer();
    if(lp1)
    {
      for(i1=0; i1 < iLen; i1++, lp1++)
      {
        *lp1 = tolower(*lp1);
      }
    }
    return *this;
  }

  CString & LoadString(int nID);

  BSTR AllocSysString(void) // borrowed from CString
  {
    LPCSTR lpcTemp = (LPCSTR)*this;
    int iLen = GetLength();

    if(!lpcTemp || iLen <= 0)
    {
      return NULL;
    }

    int nLen = MultiByteToWideChar(CP_UTF8, 0, lpcTemp, iLen,
                                   NULL, NULL); // calc conversion buffer length
    BSTR bstr = ::SysAllocStringLen(NULL, nLen);
    if (bstr != NULL)
    {
      MultiByteToWideChar(CP_UTF8, 0, lpcTemp, iLen, bstr, nLen);
    }

    return bstr;
  }

  static LPCSTR strrstr(LPCSTR sz1, LPCSTR sz2);

  int Find(char c, int nStart = 0) const { LPCSTR lpcTemp = (LPCSTR)*this; if(nStart >= GetLength()) { return -1; } LPCSTR lpc1 = strchr(lpcTemp + nStart, c); if(!lpc1) return -1; else return lpc1 - lpcTemp; }
  int ReverseFind(char c) const { LPCSTR lpcTemp = (LPCSTR)*this; LPCSTR lpc1 = strrchr(lpcTemp, c); if(!lpc1) return -1; else return lpc1 - lpcTemp; }
  int Find(const char *szX, int nStart = 0) const { LPCSTR lpcTemp = (LPCSTR)*this;  if(nStart >= GetLength()) { return -1; } LPCSTR lpc1 = strstr(lpcTemp + nStart, szX); if(!lpc1) return -1; else return lpc1 - lpcTemp; }
  int ReverseFind(const char *szX) const { LPCSTR lpcTemp = (LPCSTR)*this; LPCSTR lpc1 = strrstr(lpcTemp, szX); if(!lpc1) return -1; else return lpc1 - lpcTemp; }
  int Find(const CString &rX, int nStart = 0) const { LPCSTR lpcTemp = (LPCSTR)*this;  if(nStart >= GetLength()) { return -1; } LPCSTR szX = (LPCSTR)rX; LPCSTR lpc1 = strstr(lpcTemp + nStart, szX); if(!lpc1) return -1; else return lpc1 - lpcTemp; }
  int ReverseFind(const CString &rX) const { LPCSTR lpcTemp = (LPCSTR)*this; LPCSTR szX = (LPCSTR)rX; LPCSTR lpc1 = strrstr(lpcTemp, szX); if(!lpc1) return -1; else return lpc1 - lpcTemp; }

  int FindOneOf(const char *szX) const { if(!szX || !*szX) return -1; int i1, iL = strlen(szX); for(i1=0; i1 < iL; i1++) { int i2 = Find(szX[i1]); if(i2 >= 0) return i2; } return -1; }

  void TrimRight(void);
  void TrimLeft(void);


protected:
  void Initialize() { m_pBuffer = NULL; m_cbStringLength = m_cbBufLength = 0; }

  char *m_pBuffer;
  unsigned int m_cbStringLength;
  unsigned int m_cbBufLength;

  friend class CStringArray;

};

// handling the '+' operator
CString operator + (const CString &, const char *);
CString operator + (const char *, const CString &);
CString operator + (const CString &, char);
CString operator + (char, const CString &);
CString operator + (const CString &, const CString &);

// ------------------------------------------------------------------------
// GENERIC 'CArray' "clone" class implementation
//
// this array type USED to use std::vector, however it's less desirable to
// use 'vector', so I implemented it with a malloc'd array instead.
// It's not that efficient.  I don't care.  It works and it's small.
// for an example, see what I did with 'CStringArray' which is derived
// from 'CArray' and correctly makes use of the template parameters

// NOTE:  'Y' is the storage type, 'X' is the return value
//        for a struct or class, you will need compatible 'placement'
//        new and delete operators.
//        typically you will have X and Y the same, but if you want to
//        return a reference instead, use 'datatype &' for 'X'.


BOOL CArrayUniversalCheckGrowSize(int iIndex, int cbObjectSize, void **ppData,
                                  int *pSize, int *pMaxSize, int nGranularity);

template <typename Y, typename X> class CArray
{
public:

  CArray<Y,X>()
  {
    m_pData = NULL;
    m_nSize = m_nMaxSize = 0;

    // calculate default granularity

    if(sizeof(Y) >= 4096)
      m_nGranularity = 1;
    else if(sizeof(Y) >= 2048)
      m_nGranularity = 2;
    else if(sizeof(Y) >= 1024)
      m_nGranularity = 4;
    else if(sizeof(Y) >= 512)
      m_nGranularity = 8;
    else if(sizeof(Y) >= 256)
      m_nGranularity = 16;
    else if(sizeof(Y) >= 128)
      m_nGranularity = 32;
    else if(sizeof(Y) >= 64)
      m_nGranularity = 64;
    else if(sizeof(Y) >= 32)
      m_nGranularity = 128;
    else if(sizeof(Y) >= 16)
      m_nGranularity = 256;
    else if(sizeof(Y) >= 8)
      m_nGranularity = 512;
    else if(sizeof(Y) >= 4)
      m_nGranularity = 1024;
    else if(sizeof(Y) >= 2)
      m_nGranularity = 2048;
    else // if(sizeof(Y) >= 2)
      m_nGranularity = 4096;
  }

  virtual ~CArray<Y,X>() { if(m_pData) { RemoveAll(); free(m_pData); m_pData = NULL; } }

  int GetSize() const { if(m_pData) return m_nSize; else return 0; }

//#pragma warning(push)
//#pragma warning(disable:4715)
  // WARNING - to avoid, well, WARNINGS, I return 'm_NULLObj' which COULD be re-assigned by rogue code.
  //           if you don't like this, change it.
  Y & GetAt(int iIndex) { if(m_pData && iIndex < m_nSize) return m_pData[iIndex]; ASSERT(0); return m_NULLObj; }
  const Y & GetAt(int iIndex) const { if(m_pData && iIndex < m_nSize) return (const Y &)(m_pData[iIndex]); ASSERT(0); return m_NULLObj; }

  Y & operator [] (int iIndex) { if(m_pData && iIndex < m_nSize) return m_pData[iIndex]; ASSERT(0); return m_NULLObj; }
  const Y & operator [] (int iIndex) const { if(m_pData && iIndex < m_nSize) return (const Y &)(m_pData[iIndex]); ASSERT(0); return m_NULLObj; }
//#pragma warning(pop)

  void InsertAt(int iIndex, Y val)
  {
    int nNewSize = iIndex + 1;

    if(nNewSize <= m_nSize)
    {
      nNewSize = m_nSize + 1;
    }

    if(!CheckGrowSize(nNewSize))
    {
      return;
    }

    if(m_nSize > iIndex)
    {
      memmove(&(m_pData[iIndex + 1]), &(m_pData[iIndex]), 
              sizeof(Y) * (m_nSize - iIndex));
    }

    m_nSize++;
    new((void *)&(m_pData[iIndex])) Y; // re-initialize

    m_pData[iIndex] = val; // the actual assignment happens here
  }

  int Add(X val)
  {
    int nIndex = m_nSize;
    if(!m_pData)
    {
      nIndex = 0; // make sure
    }

    int iRval = SetAtGrow(nIndex, val);
    ASSERT(iRval == nIndex && nIndex == (m_nSize - 1));

    return iRval;
  }

  void RemoveAt(int iIndex)
  {
    if(iIndex < 0 || iIndex >= GetSize())
      return;

    Y *pObj = m_pData + iIndex;
    pObj->~Y(); // destructor first, then call 'placement delete' by convention
//    operator delete(pObj, pObj);

    if(iIndex < m_nSize)
    {
      memcpy(&(m_pData[iIndex]), &(m_pData[iIndex + 1]),
             (m_nSize - iIndex) * sizeof(Y));

      m_nSize--;

      memset(&(m_pData[m_nSize]), 0, sizeof(Y));
    }
  }

  void RemoveAll()
  {
    int i1;

    for(i1=m_nSize - 1; i1 >= 0; i1--)
    {
      Y *pObj = m_pData + i1;
      pObj->~Y();
//      operator delete (pObj, pObj);
    }
  }

  // MFC-compatible function names
  void SetAt(int iIndex, const X val)
  {
    int nNewSize = iIndex + 1;

    if(nNewSize >= m_nMaxSize)
    {
      ASSERT(0);
      return;
    }

    int i1;
    for(i1=m_nSize; i1 <= iIndex; i1++)
    {
      void *p1 = new((void *)(m_pData + i1)) Y; // initialize via constructor
    }

    if(iIndex >= m_nSize) // did I extend it?
    {
      m_nSize = iIndex + 1;
    }

    m_pData[iIndex] = val;
  }

  int SetAtGrow(int iIndex, const X val)
  {
    int nNewSize = iIndex + 1;

    if(nNewSize >= m_nMaxSize)
    {
      if(!CheckGrowSize(nNewSize))
      {
        return -1;
      }
    }

    int i1;
    for(i1=m_nSize; i1 <= iIndex; i1++)
    {
      void *p1 = new((void *)(m_pData + i1)) Y; // initialize via constructor

      ASSERT(p1 == (void *)(m_pData + i1)); // using correct 'new' operator
    }

    if(iIndex >= m_nSize) // did I extend it?
    {
      m_nSize = iIndex + 1;
    }

    m_pData[iIndex] = val;
    return iIndex;
  }

  Y *GetData(void) { return &(m_pData[0]); }


protected:
  int m_nGranularity;
  int m_nSize, m_nMaxSize;
  Y *m_pData;
  Y m_NULLObj; // what I return when I'm returning 'NULL'

  BOOL CheckGrowSize(int iIndex)
  {
    // utility prevents me creating a big function for EVERY! STINKING! IMPLEMENTATION!
    return CArrayUniversalCheckGrowSize(iIndex, sizeof(Y), (void **)&m_pData,
                                        &m_nSize, &m_nMaxSize, m_nGranularity);
  }


};

// CStringArray - an array of 'CString's !!
// I find this to be one of the most CONVENIENT classes and I use it
// a LOT!  I based it on 'CArray' not only as an example of how to
// use the CArray template, but ALSO because it's EASIER that way.

class CStringArray : public CArray<CString, CString &>
{
public:

  // empty constructor/destructor for default behavior
  CStringArray() { }
  virtual ~CStringArray() { }

  // overloads for 'Add' 'InsertAt' 'SetAt' and 'SetAtGrow'
  // CString arrays need these extra 'things' to work properly.
  // note explicit calls of base (template) class 'Add' to disambiguate
  // as Microsoft's compiler gets confused easily and can't handle it otherwise

  int Add(char cVal) { return CArray<CString,CString &>::Add(CString(cVal)); }
  int Add(const char *szVal) { return CArray<CString,CString &>::Add(CString(szVal)); }

  void InsertAt(int iIndex, char cVal) { CArray<CString,CString &>::InsertAt(iIndex, CString(cVal)); }
  void InsertAt(int iIndex, const char *szVal) { CArray<CString,CString &>::InsertAt(iIndex, CString(szVal)); }

  void SetAt(int iIndex, char cVal) { CArray<CString,CString &>::SetAt(iIndex, CString(cVal)); }
  void SetAt(int iIndex, const char *szVal) { CArray<CString,CString &>::SetAt(iIndex, CString(szVal)); }

  int SetAtGrow(int iIndex, char cVal) { return CArray<CString,CString &>::SetAtGrow(iIndex, CString(cVal)); }
  int SetAtGrow(int iIndex, const char *szVal) { return CArray<CString,CString &>::SetAtGrow(iIndex, CString(szVal)); }

};

// other wrapper/helper classes I use

class CDC; // must forward reference this thing

class CGdiObject
{
public:
  CGdiObject() { }
  virtual ~CGdiObject() { }

  int GetObject(int cbSize, LPVOID pObj) { return ::GetObject(GetGdiObjectHandle(), cbSize, pObj); }

  virtual HGDIOBJ GetGdiObjectHandle() = 0; // so I can select it
};

class CFont : public CGdiObject
{
public:

  CFont() { m_hFont = NULL; m_bTemp = FALSE; }
  CFont(HFONT hFont) { m_hFont = hFont; m_bTemp = FALSE; }
  virtual ~CFont() { if(m_hFont && !m_bTemp) ::DeleteObject(m_hFont); }

  operator HFONT() { return m_hFont; }

  void Attach(HFONT hFont) { if(m_hFont && !m_bTemp) ::DeleteObject(m_hFont); m_hFont = hFont; }
  void Detach(void) { m_hFont = NULL; }

  void DeleteObject(void) { if(m_hFont) {::DeleteObject(m_hFont); m_hFont = NULL;} }

  void CreateFont(int cH, int cW, int cE, int cO, DWORD cWgt, DWORD bI,
                  DWORD bU, DWORD bSO, DWORD iCS, DWORD iOP, DWORD iCP,
                  DWORD iQ, DWORD iPAF, const char * pszFN)
  {
    DeleteObject();
    m_hFont = ::CreateFont(cH, cW, cE, cO, cWgt, bI, bU, bSO,
                           iCS, iOP, iCP, iQ, iPAF, pszFN);
  }

  void CreateFontIndirect(LOGFONT *pLF) { DeleteObject(); m_hFont = ::CreateFontIndirect(pLF); }

//  static CFont FromHandle(HFONT hFont) { CFont rVal; rVal.m_bTemp = TRUE; rVal.Attach(hFont); return rVal; }

  virtual HGDIOBJ GetGdiObjectHandle() { return m_hFont; }

protected:
  HFONT m_hFont;
  BOOL m_bTemp; // if TRUE, does not delete the object
};

class CBitmap : public CGdiObject
{
public:

  CBitmap() { m_hBitmap = NULL; m_bTemp = FALSE; }
  CBitmap(HBITMAP hBMP) { m_hBitmap = hBMP; m_bTemp = FALSE; }
  virtual ~CBitmap() { if(m_hBitmap && !m_bTemp) ::DeleteObject(m_hBitmap); }

  operator HBITMAP() { return m_hBitmap; }

  void Attach(HBITMAP hBitmap) { if(m_hBitmap && !m_bTemp) ::DeleteObject(m_hBitmap); m_hBitmap = hBitmap; }
  void Detach(void) { m_hBitmap = NULL; }

  void DeleteObject(void) { if(m_hBitmap) {::DeleteObject(m_hBitmap); m_hBitmap = NULL;} }

  int GetObject(int cbSize, BITMAP *pBMP) { return ::GetObject(m_hBitmap, cbSize, (LPVOID)pBMP); }

  inline void CreateCompatibleBitmap(CDC *pDC, int iWidth, int iHeight);
//  static CBitmap FromHandle(HBITMAP hBitmap) { CBitmap rVal; rVal.m_bTemp = TRUE; rVal.Attach(hBitmap); return rVal; }

  inline void LoadBitmap(int nID);
  inline void LoadBitmap(const char *szID);


  virtual HGDIOBJ GetGdiObjectHandle() { return m_hBitmap; }

protected:
  HBITMAP m_hBitmap;
  BOOL m_bTemp; // if TRUE, does not delete the object
};

class CBrush : public CGdiObject
{
public:

  CBrush() { m_hBrush = NULL; m_bTemp = FALSE; }
  virtual ~CBrush() { if(m_hBrush && !m_bTemp) ::DeleteObject(m_hBrush); }

  operator HBRUSH() { return m_hBrush; }

  void Attach(HBRUSH hBrush) { if(m_hBrush && !m_bTemp) ::DeleteObject(m_hBrush); m_hBrush = hBrush; }
  void Detach(void) { m_hBrush = NULL; }

  void DeleteObject(void) { if(m_hBrush) { ::DeleteObject(m_hBrush); m_hBrush = NULL; } }

  void CreateSolidBrush(COLORREF clr) { if(m_hBrush) { ::DeleteObject(m_hBrush); } m_hBrush = ::CreateSolidBrush(clr); }

//  static CBrush FromHandle(HBRUSH hBrush) { CBrush rVal; rVal.m_bTemp = TRUE; rVal.Attach(hBrush); return rVal; }

  virtual HGDIOBJ GetGdiObjectHandle() { return m_hBrush; }

protected:
  HBRUSH m_hBrush;
  BOOL m_bTemp; // if TRUE, does not delete the object
};

class CRgn : public CGdiObject
{
public:

  CRgn() { m_hRgn = NULL; m_bTemp = FALSE; }
  virtual ~CRgn() { if(m_hRgn && !m_bTemp) ::DeleteObject(m_hRgn); }

  operator HRGN() { return m_hRgn; }

  void Attach(HRGN hRgn) { if(m_hRgn && !m_bTemp) ::DeleteObject(m_hRgn); m_hRgn = hRgn; }
  void Detach(void) { m_hRgn = NULL; }

  void DeleteObject(void) { if(m_hRgn) {::DeleteObject(m_hRgn); m_hRgn = NULL;} }

  void CreateRectRgn(int iX1, int iY1, int iX2, int iY2)
  {
    DeleteObject();
    m_hRgn = ::CreateRectRgn(iX1, iY1, iX2, iY2);
  }

  void CombineRgn(CRgn *pR1, CRgn *pR2, int nMode)
  {
    DeleteObject();
    m_hRgn = ::CreateRectRgn(0,0,0,0);
    ::CombineRgn(m_hRgn, pR1->m_hRgn, pR2->m_hRgn, nMode);
  }


//  static CRgn FromHandle(HRGN hRgn) { CRgn rVal; rVal.m_bTemp = TRUE; rVal.Attach(hRgn); return rVal; }
  virtual HGDIOBJ GetGdiObjectHandle() { return m_hRgn; }

protected:
  HRGN m_hRgn;
  BOOL m_bTemp; // if TRUE, does not delete the object
};

class CPen : public CGdiObject
{
public:

  CPen() { m_hPen = NULL; m_bTemp = FALSE; }
  CPen(HPEN hPen) { m_hPen = hPen; m_bTemp = FALSE; }
  virtual ~CPen() { if(m_hPen && !m_bTemp) ::DeleteObject(m_hPen); }

  operator HPEN() { return m_hPen; }

  void Attach(HPEN hPen) { if(m_hPen && !m_bTemp) ::DeleteObject(m_hPen); m_hPen = hPen; }
  void Detach(void) { m_hPen = NULL; }

  void DeleteObject(void) { if(m_hPen) {::DeleteObject(m_hPen); m_hPen = NULL;} }

  void CreatePen(int iStyle, int cWidth, COLORREF clr)
  {
    DeleteObject();
    m_hPen = ::CreatePen(iStyle, cWidth, clr);
  }

//  static CPen FromHandle(HPEN hPen) { CPen rVal; rVal.m_bTemp = TRUE; rVal.Attach(hPen); return rVal; }
  virtual HGDIOBJ GetGdiObjectHandle() { return m_hPen; }

protected:
  HPEN m_hPen;
  BOOL m_bTemp; // if TRUE, does not delete the object
};

class CRect : public RECT
{
public:

  CRect() { left = right = top = bottom = 0; }
  CRect(const RECT &rRCT) { memcpy(this, &rRCT, sizeof(RECT)); }
  CRect(const CRect &rRCT) { memcpy(this, &rRCT, sizeof(CRect)); }
  CRect(int iL, int iT, int iR, int iB)
  {
    left = iL; 
    top = iT;
    right = iR; 
    bottom = iB;
  }

  ~CRect() { }


  CRect & operator = (const RECT &rRCT) { memcpy(this, &rRCT, sizeof(RECT)); return *this; }
  CRect & operator = (const CRect &rRCT) { memcpy(this, &rRCT, sizeof(CRect)); return *this; }

  int Width() { return right - left; }
  int Height() { return bottom - top; }


};

class CPoint : public POINT
{
public:

  CPoint() { x = y = 0; }
  CPoint(int iX, int iY) { x = iX; y = iY; }
  ~CPoint() { }

  CPoint(const POINT &rP) { memcpy(this, &rP, sizeof(POINT)); }
  CPoint(const CPoint &rP) { memcpy(this, &rP, sizeof(POINT)); }

  CPoint & operator = (const POINT &rP) { memcpy(this, &rP, sizeof(POINT)); return *this; }
  CPoint & operator = (const CPoint &rP) { memcpy(this, &rP, sizeof(POINT)); return *this; }
};


class CDC
{
public:

  CDC() { m_hDC = NULL; m_bTemp = FALSE; }
  CDC(HDC hDC) { m_hDC = hDC; m_bTemp = FALSE; }
//  CDC(CDC &rDC) { m_hDC = rDC.m_hDC; m_bTemp = FALSE; }
  virtual ~CDC() { if(m_hDC && !m_bTemp) ::DeleteDC(m_hDC); }

  operator HDC() { return m_hDC; }
  HDC GetSafeHdc(void) { if(!this) return NULL; else return m_hDC; }

  void Attach(HDC hDC) { if(m_hDC && !m_bTemp) ::DeleteDC(m_hDC); m_hDC = hDC; }
  HDC Detach(void) { HDC hRval = m_hDC; m_hDC = NULL; return hRval; }

  int SaveDC(void) { if(m_hDC) return ::SaveDC(m_hDC); else return -1; }
  BOOL RestoreDC(int nSavedDC) { if(m_hDC) return ::RestoreDC(m_hDC, nSavedDC); else return FALSE; }

  void DeleteDC(void) { if(m_hDC) {::DeleteDC(m_hDC); m_hDC = NULL;} }

  void CreateCompatibleDC(CDC *pDC) { if(m_hDC) { ::DeleteDC(m_hDC); } m_hDC = ::CreateCompatibleDC(pDC->m_hDC); }

  void SelectClipRgn(CRgn *pRgn, int nMode = RGN_COPY)
  {
    if(!pRgn)
    {
      return;
    }

    if(nMode == RGN_COPY)
    {
      ::SelectClipRgn(m_hDC, (HRGN)pRgn->GetGdiObjectHandle());
    }
    else
    {
      CRgn rgnTemp, rgnDest;
      rgnTemp.CreateRectRgn(0,0,0,0);
      rgnDest.CreateRectRgn(0,0,0,0);

      ::GetClipRgn(m_hDC, (HRGN)rgnTemp.GetGdiObjectHandle());

      rgnDest.CombineRgn(pRgn, &rgnTemp, nMode);

      ::SelectClipRgn(m_hDC, (HRGN)rgnDest.GetGdiObjectHandle());
    }
  }

  void SetTextColor(COLORREF clr) { ::SetTextColor(m_hDC, clr); }
  void SetBkColor(COLORREF clr) { ::SetBkColor(m_hDC, clr); }
  void SetBkMode(int iMode) { ::SetBkMode(m_hDC, iMode); }

  void FillRect(const CRect *pR, CBrush *pB) { ::FillRect(m_hDC, (const RECT *)pR, (HBRUSH)*pB); }
  void DrawText(CString &csText, CRect *pR, UINT nF) { ::DrawText(m_hDC, (LPCSTR)csText, csText.GetLength(), (RECT *)pR, nF); }
  void DrawText(LPCSTR pszText, CRect *pR, UINT nF) { if(pszText) { ::DrawText(m_hDC, pszText, strlen(pszText), (RECT *)pR, nF); } }

  void MoveTo(int iX, int iY) { ::MoveToEx(m_hDC, iX, iY, NULL); }
  void MoveTo(POINT &rP) { ::MoveToEx(m_hDC, rP.x, rP.y, NULL); }
  void LineTo(int iX, int iY) { ::LineTo(m_hDC, iX, iY); }
  void LineTo(POINT &rP) { ::LineTo(m_hDC, rP.x, rP.y); }

  void DrawFocusRect(const RECT *pR) { ::DrawFocusRect(m_hDC, pR); }

  void SetStretchBltMode(int nMode) { ::SetStretchBltMode(m_hDC, nMode); }
  void StretchBlt(int iX, int iY, int iW, int iH,
                  CDC *pSrc, int iXSrc, int iYSrc, int iWSrc, int iHSrc, DWORD dwROP)
  {
    ::StretchBlt(m_hDC, iX, iY, iW, iH,
                 pSrc->m_hDC, iXSrc, iYSrc, iWSrc, iHSrc, dwROP);
  }

  BOOL GetTextMetrics(LPTEXTMETRIC pTM) { return ::GetTextMetrics(m_hDC, pTM); }
  int GetDeviceCaps(int iIndex) { return ::GetDeviceCaps(m_hDC, iIndex); }

  void DrawIcon(int iX, int iY, HICON hIcon) { ::DrawIcon(m_hDC, iX, iY, hIcon); }

  void SelectObject(CGdiObject *pObj) { ::SelectObject(m_hDC, pObj->GetGdiObjectHandle()); }
//  CPen* SelectObject(CPen* pPen);
//  CBrush* SelectObject(CBrush* pBrush);
//  CFont* SelectObject(CFont* pFont);
//  CBitmap* SelectObject(CBitmap* pBitmap);
//  int SelectObject(CRgn* pRgn);
//  CGdiObject* SelectObject(CGdiObject* pObject);

//  static CDC * FromHandle(HDC hDC) { CDC *pRval = new CDC; if(pRval) { pRval->m_bTemp = TRUE; pRval->Attach(hDC); } return pRval; }

  HDC m_hDC;
  BOOL m_bTemp; // if TRUE, does not delete the object
};


// THREAD and APPLICATION classes (MFC equivalents done MY way - simple and TINY)

class CMyThread
{
public:

  CMyThread()
  {
    m_pThreadParams = NULL;
    m_pfnThreadProc = NULL;

    CommonInit();
  }

  // creating a generic thread with thread proc
  CMyThread(MYTHREADPROC pfnThreadProc, LPVOID pParam)
  {
    m_pThreadParams = pParam;
    m_pfnThreadProc = pfnThreadProc;

    CommonInit();
  }

  void CommonInit(void)
  {
    if(m_nTLS == TLS_OUT_OF_INDEXES) // initial value
    {
      m_nTLS = TlsAlloc(); // first time allocate it (will contain a zero)
    }

    memset(&m_msgCur, 0, sizeof(m_msgCur));

    m_hWnd = NULL; // handle to main window - assign NULL when window destroyed
    m_hThread = INVALID_HANDLE_VALUE;
    m_dwThreadID = -1; // not valid
  }

  // valid after construction

  virtual ~CMyThread() { if(m_hThread != INVALID_HANDLE_VALUE) CloseHandle(m_hThread); }

  virtual int Run();

  virtual BOOL InitInstance();
	virtual int ExitInstance();

  BOOL CreateThread(DWORD dwCreateFlags = 0, UINT nStackSize = 0,
                    LPSECURITY_ATTRIBUTES lpSecurityAttrs = NULL);

  void ResumeThread(void);

  void SetThreadPriority(int nPriority) { ::SetThreadPriority(m_hThread, nPriority); }


  HANDLE m_hThread;
  DWORD m_dwThreadID;
  MSG m_msgCur;

  // main window handle for this thread (other windows become children of it)
  HWND m_hWnd; // assign to a window to keep message loop running
               // when this is NULL, message loop will quit

  static DWORD m_nTLS; // TLS slot for current thread

  // these next 2 are for creating a thread with a specific thread proc
  LPVOID m_pThreadParams;
  MYTHREADPROC m_pfnThreadProc;
};

class CMyApp : public CMyThread
{
public:
  CMyApp(LPCSTR szAppName = NULL)
  {
    m_pTheApp = this;
    m_hInstance = m_hPrevInstance = NULL;
    m_nCmdShow = 0;
    m_lpCmdLine = NULL;
    if(szAppName)
    {
      m_csApplicationName = szAppName;
    }
  }

  virtual ~CMyApp() { }

  virtual BOOL InitApplication();

  // a few things I have to implement, minimally

  void SetRegistryKey(LPCSTR szRegKey) { m_csRegKey = szRegKey; }
  void PostThreadMessage(UINT nMessage, WPARAM wParam, LPARAM lParam);
  CString LoadString(UINT nID);
  HICON LoadIcon(int nResID) { return ::LoadIcon(m_hInstance, (LPCSTR)nResID); }

  HINSTANCE m_hInstance;
  HINSTANCE m_hPrevInstance;
  int m_nCmdShow;
  LPCSTR m_lpCmdLine;

  CString m_csRegKey;
  CString m_csApplicationName;

  static CMyApp *m_pTheApp; // only one, assigned in constructor

};

__inline CMyThread *MyGetCurrentThread(void) { return (CMyThread *)TlsGetValue(CMyThread::m_nTLS); }

__inline CMyApp *MyGetApp(void) { return CMyApp::m_pTheApp; }

LPCSTR MyGetAppName(void); // a litle too complex to be 

__inline MSG *MyGetCurrentMessage(void) { CMyApp *pApp = MyGetApp(); if(pApp) return &(pApp->m_msgCur); else return NULL; }

CMyThread* MyBeginThread(MYTHREADPROC pfnThreadProc, LPVOID pParam,
                         int nPriority, UINT nStackSize, DWORD dwCreateFlags,
                         LPSECURITY_ATTRIBUTES lpSecurityAttrs);

extern "C" int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                                _In_ LPTSTR lpCmdLine, int nCmdShow);

int MyMessageBox(LPCSTR szCaption, int nType = MB_OK | MB_ICONASTERISK | MB_SETFOREGROUND | MB_TOPMOST);
int MyMessageBox(UINT nStringID, int nType = MB_OK | MB_ICONASTERISK | MB_SETFOREGROUND | MB_TOPMOST);


// INLINE FUNCTIONS that must appear here

inline void CBitmap::CreateCompatibleBitmap(CDC *pDC, int iWidth, int iHeight)
{
  DeleteObject();
  m_hBitmap = ::CreateCompatibleBitmap((HDC)*pDC, iWidth, iHeight);
}

inline void CBitmap::LoadBitmap(int nID)
{
  DeleteObject();
  m_hBitmap = ::LoadBitmap(MyGetApp()->m_hInstance, MAKEINTRESOURCE(nID));
}

inline void CBitmap::LoadBitmap(const char *szID)
{
  DeleteObject();
  m_hBitmap = ::LoadBitmap(MyGetApp()->m_hInstance, szID);
}




#endif // _CLASSLIB_H_INCLUDED_
