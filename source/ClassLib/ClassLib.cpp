//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//       ____  _                   _      _  _                              //
//      / ___|| |  __ _  ___  ___ | |    (_)| |__     ___  _ __   _ __      //
//     | |    | | / _` |/ __|/ __|| |    | || '_ \   / __|| '_ \ | '_ \     //
//     | |___ | || (_| |\__ \\__ \| |___ | || |_) |_| (__ | |_) || |_) |    //
//      \____||_| \__,_||___/|___/|_____||_||_.__/(_)\___|| .__/ | .__/     //
//                                                        |_|    |_|        //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//          Copyright (c) 2016 by S.F.T. Inc. - All rights reserved         //
//  Use, copying, and distribution of this software are licensed according  //
//    to the GPLv2, LGPLv2, or BSD license, as appropriate (see COPYING)    //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////



#include "ClassLib.h"

// implementation of strings, arrays, application and thread classes
// and a whole bunch of other supporting things

#pragma warning(suppress: 4985)

CMyApp *CMyApp::m_pTheApp = NULL;
DWORD CMyThread::m_nTLS = TLS_OUT_OF_INDEXES;

extern "C" int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                              _In_ LPTSTR lpCmdLine, int nCmdShow)
{
  // copied from AfxWinMain and then trimmed down...

  int nReturnCode = -1;
  CMyApp *pApp = MyGetApp();

  // AFX internal initialization
  pApp->m_hInstance = hInstance;
  pApp->m_hPrevInstance = hPrevInstance;
  pApp->m_nCmdShow = nCmdShow;
  pApp->m_lpCmdLine = lpCmdLine;

  if(pApp->m_nTLS != TLS_OUT_OF_INDEXES)
  {
    TlsSetValue(pApp->m_nTLS, (LPVOID)pApp); // assign thread local storage
  }

  // App global initializations (rare)
  if (pApp != NULL && !pApp->InitApplication())
    goto InitFailure;

  // Perform specific initializations
  if (!pApp->InitInstance())
  {
    nReturnCode = pApp->ExitInstance();
    goto InitFailure;
  }

  nReturnCode = pApp->Run();

InitFailure:

//  AfxWinTerm(); do not call this
//  AfxUnregisterWndClasses(); // the only thing AfxWinTerm does that's important
  return nReturnCode;
}


BOOL CMyApp::InitApplication()
{
  // TODO:  parse command line, etc.

  return TRUE; // success
}

void CMyApp::PostThreadMessage(UINT nMessage, WPARAM wParam, LPARAM lParam)
{
  ::PostMessage(NULL, nMessage, wParam, lParam);
}


CString CMyApp::LoadString(UINT nID)
{
CString csRval;
int cbLen;
LPSTR p1;

  cbLen = 4096;
  p1 = csRval.GetBufferSetLength(cbLen + 1);

  if(!p1)
  {
    return "";
  }

  ::LoadString(m_hInstance, nID, p1, cbLen + 1);

  csRval.ReleaseBuffer(-1);

  return csRval;
}

int CMyThread::Run()
{
  while(m_hWnd)
  {
    if(::PeekMessage(&m_msgCur, NULL, NULL, NULL, PM_REMOVE))
    {
      ::TranslateMessage(&m_msgCur);

      // TODO:  application messages?
      ::DispatchMessageA(&m_msgCur);

      if(m_msgCur.message == WM_QUIT)
        break;
    }
    else
    {
      // TODO:  idle processing?
      Sleep(1);
    }
  }

  return (int)(m_msgCur.wParam);
}

BOOL CMyThread::InitInstance()
{
  return TRUE; // for now
}

int CMyThread::ExitInstance()
{
  if(m_msgCur.message == WM_QUIT)
  {
    return (int)m_msgCur.wParam;
  }

// TODO:  don't do this for an application, but if I'm a 'dynamically created' thread...
//  delete this; // TODO:  'auto delete' flag?

  return 0;
}


DWORD DefaultThreadProc(LPVOID pParam)
{
CMyThread *pThread = (CMyThread *)pParam;
DWORD dwRval = 0;

  if(!pParam)
  {
    return 0;
  }

  // IMPORTANT:  set the thread local storage for this thread
  if(pThread->m_nTLS != TLS_OUT_OF_INDEXES)
  {
    TlsSetValue(pThread->m_nTLS, (LPVOID)pThread);
  }
    
  if(pThread->InitInstance() &&
     pThread->m_pfnThreadProc)
  {
    dwRval = pThread->m_pfnThreadProc(pThread->m_pThreadParams);

    pThread->m_msgCur.message = WM_QUIT;
    pThread->m_msgCur.wParam = (WPARAM)dwRval;
  }

  if(pThread->m_hWnd)
  {
    pThread->Run(); // does the message loop
  }

  return pThread->ExitInstance(); // this deletes the thread object
}

BOOL CMyThread::CreateThread(DWORD dwCreateFlags, UINT nStackSize,
                             LPSECURITY_ATTRIBUTES lpSecurityAttrs)
{
  m_hThread = ::CreateThread(lpSecurityAttrs, nStackSize,
                             (LPTHREAD_START_ROUTINE)DefaultThreadProc,
                             (LPVOID)this, dwCreateFlags | CREATE_SUSPENDED,
                             &m_dwThreadID);

  if(m_hThread != INVALID_HANDLE_VALUE &&
     !(dwCreateFlags & CREATE_SUSPENDED))
  {
    ResumeThread();
  }

  return m_hThread != INVALID_HANDLE_VALUE;
}

void CMyThread::ResumeThread(void)
{
  if(m_hThread != INVALID_HANDLE_VALUE)
  {
    ::ResumeThread(m_hThread);
  }
}


// CString stuff

CString::CString(char c1)
{
  Initialize();

  *this += c1;
}

CString::CString(const char *pX)
{
  Initialize();

  if(pX)
  {
    *this += pX;
  }
}

CString::CString(const CString &rX)
{
  Initialize();

  *this += rX;
}

CString::~CString()
{
  if(m_pBuffer)
  {
    free(m_pBuffer);
    m_pBuffer = NULL;
  }
}

int CString::Format(const char *szFormat, ...)
{
  // for now, allocate, copy, and delete - later incorporate 'asprintf'

  va_list va;
  va_start(va, szFormat);

  int iRval = -1;

  if(m_pBuffer)
  {
    ReleaseBuffer(0); // effectively sets length to zero
  }

  // if '_vscprintf' doesn't exist, I'm kinda hosed

  int i1 = _vscprintf(szFormat, va);  // determine how much memory I need

  if(i1 > 0)  // also '_vsnprintf' must exist
  {
    iRval = _vsnprintf(GetBufferSetLength(i1 + 1), i1 + 1, szFormat, va);
  }

  ReleaseBuffer(iRval);

  va_end(va);

  return(iRval);

}

int CString::Format(UINT nIDFormat, ...)
{
  // for now, allocate, copy, and delete - later incorporate 'asprintf'

  CString csFormat = MyGetApp()->LoadString(nIDFormat);

  const char *szFormat = (const char *)csFormat;

  va_list va;
  va_start(va, szFormat);

  int iRval = -1;

  if(m_pBuffer)
  {
    ReleaseBuffer(0); // effectively sets length to zero
  }

  // if '_vscprintf' doesn't exist, I'm kinda hosed

  int i1 = _vscprintf(szFormat, va);  // determine how much memory I need

  if(i1 > 0)  // also '_vsnprintf' must exist
  {
    iRval = _vsnprintf(GetBufferSetLength(i1 + 1), i1 + 1, szFormat, va);
  }

  ReleaseBuffer(iRval);

  va_end(va);

  return(iRval);

}

CString & CString::operator +=(char c1)
{
LPSTR lp1;

  int iOldLen = m_cbStringLength;

  if(!m_pBuffer)
  {
    iOldLen = 0; // make sure
    lp1 = GetBufferSetLength(2);
  }
  else
  {
    lp1 = GetBufferSetLength(iOldLen + 2);
  }

  if(lp1)
  {
    lp1[iOldLen] = c1;
    lp1[iOldLen + 1] = 0;
  }

  ReleaseBuffer(iOldLen + 1);

  return *this;  
}

CString & CString::operator +=(const char *pszX)
{
LPSTR lp1;
int iLen;

  if(!pszX)
  {
    return *this;
  }

  iLen = strlen(pszX);

  if(!iLen)
  {
    return *this;
  }

  int iOldLen = m_cbStringLength;

  if(!m_pBuffer)
  {
    iOldLen = 0; // make sure
    lp1 = GetBufferSetLength(iLen + 1); // this re-assigns length
  }
  else
  {
    lp1 = GetBufferSetLength(iOldLen + iLen + 1);
  }

  if(lp1)
  {
    memcpy(lp1 + iOldLen, pszX, iLen);
    lp1[iOldLen + iLen] = 0;
  }

  ReleaseBuffer(iOldLen + iLen); // new length

  return *this;  
}

CString & CString::operator +=(const CString &rX)
{
LPSTR lp1;
int iLen = (int)rX.GetLength();
LPCSTR lpc1 = (LPCSTR)rX;

  if(iLen <= 0 || !lpc1)
  {
    return *this;
  }

  int iOldLen = m_cbStringLength;

  if(!m_pBuffer)
  {
    iOldLen = 0; // make sure
    lp1 = GetBufferSetLength(iLen + 1); // this re-assigns length
  }
  else
  {
    lp1 = GetBufferSetLength(iOldLen + iLen + 1);
  }

  if(lp1)
  {
    memcpy(lp1 + iOldLen, lpc1, iLen);
    lp1[iOldLen + iLen] = 0;
  }

  ReleaseBuffer(iOldLen + iLen); // new length

  return *this;  
}

CString & CString::operator =(char c1)
{
  m_cbStringLength = 0;
  *this += c1;
  return *this;
}

CString & CString::operator =(const char *pszX)
{
  m_cbStringLength = 0;
  *this += pszX;
  return *this;
}

CString & CString::operator =(const CString &rX)
{
  m_cbStringLength = 0;
  *this += rX;
  return *this;
}

CString::operator const char *(void) const
{
  if(m_pBuffer)
  {
    return m_pBuffer;
  }
  else
  {
    return ""; // empty string
  }
}


char * CString::GetBufferSetLength(int nLength)
{
  if(nLength < 0)
  {
    return NULL;  // do not allow
  }

  if(!m_pBuffer)
  {
    int nNewLength = (nLength + 1023) & ~511;

    m_pBuffer = (char *)malloc(nNewLength);

    if(!m_pBuffer)
    {
      return NULL;
    }

    m_cbBufLength = nNewLength;
  }
  else if((int)m_cbBufLength < nLength)
  {
    int nNewLength = (nLength + 1023) & ~511;

    char *p1 = (char *)realloc(m_pBuffer, nNewLength);
    if(!p1)
    {
      return NULL;
    }

    m_cbBufLength = nNewLength;
    m_pBuffer = p1;
  }

  if(nLength > 0) // 0 means 'leave length alone'
  {
    m_cbStringLength = nLength;
  }

  return m_pBuffer;
}

void CString::ReleaseBuffer()
{
  // my version does nothing
}

void CString::ReleaseBuffer(int nLength)
{
  if(!m_pBuffer || m_cbBufLength <= 0)
  {
    return; // do nothing
  }

  if(nLength < 0)
  {
    LPCSTR lpc1 = m_pBuffer;
    LPCSTR lpcEnd = lpc1 + m_cbBufLength - 1;

    while(lpc1 < lpcEnd && *lpc1)
    {
      lpc1++;
    }

    m_pBuffer[lpc1 - m_pBuffer] = 0; // make sure
    m_cbStringLength = lpc1 - m_pBuffer; // new string length
  }
  else
  {
    if(nLength > ((int)m_cbBufLength - 1))
    {
      nLength = m_cbBufLength - 1;
    }

    m_pBuffer[nLength] = 0;
    m_cbStringLength = nLength;
  }
}


LPCSTR CString::strrstr(LPCSTR sz1, LPCSTR sz2)
{
LPCSTR lpc1 = sz1 + strlen(sz1);

  // not caring about efficiency
  while(lpc1 > sz1)
  {
    lpc1--;
    if(!strcmp(lpc1, sz2))
    {
      return lpc1;
    }
  }

  return NULL;
}

void CString::TrimRight(void)
{
  LPSTR lp1 = GetBuffer();

  if(lp1)
  {
    LPSTR lp2 = lp1 + GetLength();
    while(lp2 > lp1)
    {
      if(*(lp2 - 1) <= ' ')
      {
        lp2--;
        *lp2 = 0;
      }
      else
      {
        break;
      }
    }

    ReleaseBuffer(lp2 - lp1);  // the new length
  }
}

void CString::TrimLeft(void)
{
LPSTR lp1 = GetBuffer();
LPSTR lp2 = lp1;
int nLen = GetLength();

  if(!lp1 || !nLen)
  {
    return;
  }

  while(*lp2 <= ' ' && nLen > 0)
  {
    lp2++;
    nLen--;
  }

  if(nLen <= 0)
  {
    ReleaseBuffer(0);
  }
  else
  {
    memcpy(lp1, lp2, nLen);
    lp1[nLen] = 0; // make sure

    ReleaseBuffer(nLen);
  }
}


CString CString::Left(int nLen) const
{
CString csRval;
int iLen = nLen;
LPSTR lpTemp;
LPCSTR lpc1 = (LPCSTR)*this;

  if(iLen > (int)GetLength())
  {
    iLen = GetLength();
  }

  if(iLen <= 0)
  {
    return csRval;
  }

  lpTemp = csRval.GetBufferSetLength(iLen + 1);
  if(!lpTemp)
  {
    csRval.ReleaseBuffer(0); // make sure

    return csRval;
  }

  if(iLen > 0)
  {
    memcpy(lpTemp, lpc1, iLen);
  }

  lpTemp[iLen] = 0;
  csRval.ReleaseBuffer(-1);

  return csRval;
}

CString CString::Mid(int nPos, int nLen) const
{
CString csRval;
int iLen = nLen, cbLen = (int)GetLength();
LPSTR lpTemp;
LPCSTR lpc1 = (LPCSTR)*this;

  if(nPos < 0)
  {
    nPos = 0;  // just fix it
  }

  if(iLen < 0)
  {
    if(nPos >= cbLen) // beyond length of string
    {
      return csRval;
    }

    iLen = cbLen - nPos;
  }

  if(nPos + iLen > cbLen)
  {
    iLen = cbLen - nPos;
  }

  if(iLen <= 0 || nPos >= cbLen)
  {
    return csRval;
  }

  lpTemp = csRval.GetBufferSetLength(iLen + 1);
  if(!lpTemp)
  {
    csRval.ReleaseBuffer(0); // make sure

    return csRval;
  }

  if(iLen > 0)
  {
    memcpy(lpTemp, lpc1 + nPos, iLen);
  }

  lpTemp[iLen] = 0;
  csRval.ReleaseBuffer(-1);

  return csRval;
}

CString CString::Right(int nLen) const
{
int iLen = nLen, cbLen = (int)GetLength();

  if(iLen > cbLen)
  {
    iLen = cbLen;
  }

  return Mid(cbLen - iLen, iLen);
}

CString & CString::LoadString(int nID)
{
  CString csTemp = MyGetApp()->LoadString(nID);

  m_cbStringLength = 0;

  *this += csTemp;
  return *this;
}

CString operator + (const CString &rX, const char *pY)
{
  CString csTemp(rX);

  csTemp += pY;
  return csTemp;
}

CString operator + (const char *pX, const CString &rY)
{
  CString csTemp(pX);

  csTemp += rY;
  return csTemp;
}

CString operator + (const CString &rX, char cY)
{
  CString csTemp(rX);

  csTemp += cY;
  return csTemp;
}

CString operator + (char cX, const CString &rY)
{
  CString csTemp(cX);

  csTemp += rY;
  return csTemp;
}

CString operator + (const CString &rX, const CString &rY)
{
  CString csTemp(rX);

  csTemp += rY;
  return csTemp;
}


// helper for CArray
BOOL CArrayUniversalCheckGrowSize(int iIndex, int cbObjectSize, void **ppData, int *pSize, int *pMaxSize, int nGranularity)
{
  // some debugging to help in the authoring process
  ASSERT(ppData && pSize && pMaxSize && nGranularity > 0 && cbObjectSize > 0);

  if(!ppData || !pSize || *pMaxSize)
  {
    return FALSE;
  }

  if(!*ppData || iIndex >= *pMaxSize)
  {
    if(!*ppData)
    {
      *pMaxSize = 0;
      *pSize = 0;  // make sure
    }

    int iNewSize = (*pMaxSize + nGranularity);
    // m_nGranularity * (1 + nSize / m_nGranularity);

    while(iIndex >= iNewSize)
    {
      iNewSize += nGranularity;  // TODO:  efficiency improvement
    }

    if(*ppData)
    {
      void *p1 = realloc(*ppData, iNewSize * cbObjectSize);

      ASSERT(p1);

      if(!p1)
      {
        return FALSE;
      }

      *pMaxSize += nGranularity;
      *ppData = p1;
    }
    else
    {
      void *p1 = malloc(iNewSize * cbObjectSize);

      ASSERT(p1);

      if(!p1)
      {
        return FALSE;
      }

      *ppData = p1;
    }

    if(*pMaxSize < iNewSize)
    {
      // zero out new memory area
      memset((char *)(*ppData) + (*pMaxSize) * cbObjectSize,
             0, (iNewSize - *pMaxSize) * cbObjectSize);
    }

    *pMaxSize = iNewSize;
  }

  return TRUE;
}


/////////////////////////////////////////////
// class-related utilities

LPCSTR MyGetAppName(void)
{
static char *pszModuleName = NULL;

  if(CMyApp::m_pTheApp && CMyApp::m_pTheApp->m_csApplicationName.GetLength())
  {
    return CMyApp::m_pTheApp->m_csApplicationName;
  }

  if(!pszModuleName)
  {
    pszModuleName = (char *)malloc(MAX_PATH + 2);

    if(!pszModuleName)
    {
      return "*ERROR*";
    }

    // base application name on module name
    ::GetModuleFileNameA(NULL, pszModuleName, MAX_PATH);
    char *p1 = strrchr(pszModuleName, '\\');
    if(p1)
    {
      strcpy(pszModuleName, p1 + 1); // just the name, not the path
    }
  }

  return pszModuleName;
}

int MyMessageBox(UINT nStringID, int nFlags)
{
  CMyApp *pApp = MyGetApp();
  if(pApp)
  {
    CString csTemp = pApp->LoadString(nStringID);
    return MyMessageBox(csTemp, nFlags);
  }
  else
  {
    return MyMessageBox("Unable to load message string", nFlags);
  }
}

int MyMessageBox(LPCSTR szCaption, int nFlags)
{
  CMyThread *pThread = MyGetCurrentThread();
  HWND hWnd = pThread ? pThread->m_hWnd : NULL;

  return ::MessageBox(hWnd, szCaption, MyGetAppName(), nFlags);
}

// creating a CMyThread, similar to AfxBeginThread

CMyThread* MyBeginThread(MYTHREADPROC pfnThreadProc, LPVOID pParam,
                         int nPriority, UINT nStackSize, DWORD dwCreateFlags,
                         LPSECURITY_ATTRIBUTES lpSecurityAttrs)
{
  CMyThread* pThread = new CMyThread(pfnThreadProc, pParam);

  if(!pThread->CreateThread(dwCreateFlags|CREATE_SUSPENDED, nStackSize,
                            lpSecurityAttrs))
  {
    delete pThread;
    return NULL;
  }

  if(!(dwCreateFlags & CREATE_SUSPENDED))
     pThread->ResumeThread();

  return pThread;
}

#ifdef _DEBUG

void DoTrace(UINT nLine, LPCSTR szFile, LPCSTR szFormat, ...)
{
//  CString csTemp, csMsg;
char tbuf[4096 + 256];
LPSTR lp1 = tbuf;

  memset(tbuf, 0, sizeof(tbuf));

  va_list va;
  va_start(va, szFormat);

  sprintf(lp1, "TRACE:  line %d in %s\r\n", nLine, szFile);
  lp1 += strlen(lp1);

  __try
  {
    vsnprintf(lp1, 4096, szFormat, va);
  }
  __except(EXCEPTION_EXECUTE_HANDLER)
  {
    // TODO:  should I do this as a message string or not???

    ::MessageBox(NULL, tbuf, "TRACE EXCEPTION",
                 MB_OK | MB_ICONHAND | MB_SETFOREGROUND);
  }

  ::OutputDebugString(tbuf);
}

#endif // _DEBUG

