///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                               //
//  __        __ _             _                  _   _        _                                                 //
//  \ \      / /(_) _ __    __| |  ___ __      __| | | |  ___ | | _ __    ___  _ __  ___     ___  _ __   _ __    //
//   \ \ /\ / / | || '_ \  / _` | / _ \\ \ /\ / /| |_| | / _ \| || '_ \  / _ \| '__|/ __|   / __|| '_ \ | '_ \   //
//    \ V  V /  | || | | || (_| || (_) |\ V  V / |  _  ||  __/| || |_) ||  __/| |   \__ \ _| (__ | |_) || |_) |  //
//     \_/\_/   |_||_| |_| \__,_| \___/  \_/\_/  |_| |_| \___||_|| .__/  \___||_|   |___/(_)\___|| .__/ | .__/   //
//                                                               |_|                             |_|    |_|      //
//                                                                                                               //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                               //
//                            Copyright (c) 2016 by S.F.T. Inc. - All rights reserved                            //
//                     Use, copying, and distribution of this software are licensed according                    //
//                       to the GPLv2, LGPLv2, or BSD license, as appropriate (see COPYING)                      //
//                                                                                                               //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#include "WindowHelpers.h"

CMasterWindowClassList theMasterWindowClassList;

CString CMasterWindowClassList::GetClassName(UINT uiStyle, HICON hIcon, HCURSOR hCursor,
                                             HBRUSH hbrBackground, LPCSTR lpszMenuName,
                                             LPCSTR lpszClassName /* = NULL */)
{
  if(!hIcon)
    hIcon = (HICON)::LoadCursor(NULL, MAKEINTRESOURCE(IDC_ICON));

  if(!hCursor)
    hCursor = ::LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW));

  if(!hbrBackground)
    hbrBackground = (HBRUSH)::GetStockObject(WHITE_BRUSH);

  return(GetClassName(uiStyle, 0, 0, hIcon, hIcon, hCursor,
                      hbrBackground, lpszMenuName, lpszClassName));
}

CString CMasterWindowClassList::GetClassName(UINT uiStyle, int cbClsExtra, int cbWndExtra,
                                             HICON hIcon, HICON hIconSm, HCURSOR hCursor,
                                             HBRUSH hbrBackground, LPCSTR lpszMenuName,
                                             LPCSTR lpszClassName /* = NULL */)
{
  // 1st, generate a name

  CString csName, csTemp;

  if(lpszClassName && *lpszClassName)
    csName = lpszClassName;
  else
  {
    csName = "SFT:";

#define FUBAR(X) \
  if(X) csTemp.Format("%X:", X); else csTemp = "0:"; csName += csTemp;

    FUBAR(uiStyle);
    FUBAR(cbClsExtra);
    FUBAR(cbWndExtra);
    FUBAR(hIcon);
    FUBAR(hIconSm);
    FUBAR(hCursor);
    FUBAR(hbrBackground);

    if(lpszMenuName)
    {
      if((INT_PTR)lpszMenuName < 0x10000)
        csTemp.Format("#%d", (int)(INT_PTR)lpszMenuName);
      else
        csTemp = lpszMenuName;
    }
    else
      csTemp = "0";

    csName += csTemp;

#undef FUBAR
  }

  int i1;
  for(i1 = m_acsClasses.GetSize() - 1; i1 >= 0; i1--)
  {
    if(!csName.CompareNoCase(m_acsClasses[i1]))
      return(csName);  // assume found
  }

  i1 = m_acsClasses.Add(csName);

  if(i1 < 0)
  {
    return "";
  }

  WNDCLASSEX wex;
  memset(&wex, 0, sizeof(wex));

  wex.cbSize = sizeof(wex);
  wex.style = uiStyle;
  wex.cbClsExtra = cbClsExtra;
  wex.cbWndExtra = cbWndExtra;
  wex.hIcon = hIcon;
  wex.hIconSm = hIconSm;
  wex.hCursor = hCursor;
  wex.hbrBackground = hbrBackground;
  wex.lpszMenuName = lpszMenuName;
  wex.lpfnWndProc = CMyWindowBaseClass::WindowProc;

  wex.lpszClassName = (LPCSTR)csName;


  if(!RegisterClassEx(&wex))
  {
    m_acsClasses.RemoveAt(i1);
    return("");
  }

  return(csName);
}

#pragma warning(push)
#pragma warning(disable:4307)
CRect CMyWindowBaseClass::rectDefault(CW_USEDEFAULT, CW_USEDEFAULT,
                                      2L * CW_USEDEFAULT, 2L * CW_USEDEFAULT);
#pragma warning(pop)

CMyWindowBaseClass::CMyWindowBaseClass()
{
}

CMyWindowBaseClass::~CMyWindowBaseClass()
{
  if(m_hWnd)
    ::DestroyWindow(m_hWnd);

  m_hWnd = NULL;
}


BOOL CMyWindowBaseClass::CreateEx(DWORD dwExStyle, LPCSTR lpszWindowClass,
                                  LPCSTR lpszWindowText, DWORD dwStyle,
                                  CRect &rctWnd /*= rectDefault*/,
                                  HWND hwndParent /*= NULL*/,
                                  HMENU hMenu /*= NULL*/,
                                  int nIDIcon /*= IDR_MAINFRAME */)
{
  CString csClass;

  if(!lpszWindowClass || !*lpszWindowClass)
    csClass = theMasterWindowClassList.GetClassName(CS_VREDRAW | CS_HREDRAW | CS_OWNDC,
                                                    MyGetApp()->LoadIcon(nIDIcon));
  else
    csClass = lpszWindowClass;

  if(!csClass.GetLength())
    return FALSE;

  m_hWnd = CreateWindowEx(dwExStyle, csClass, lpszWindowText, dwStyle,
                          rctWnd.left, rctWnd.top,
                          rctWnd.right - rctWnd.left,
                          rctWnd.bottom - rctWnd.top,
                          hwndParent, hMenu, MyGetApp()->m_hInstance,
                          (LPVOID)(INT_PTR)this);

  if(!m_hWnd)
  {
#ifdef _DEBUG
    DWORD dwErr = GetLastError();
   
    CString csTemp;
    csTemp.Format("ERROR %ld (%08xH)", dwErr, dwErr);

    MyMessageBox(csTemp);
#endif // _DEBUG
    return FALSE;
  }

//  ::ShowWindow(m_hWnd, SW_SHOW);  // be visible

  return TRUE;
}

LRESULT WINAPI CMyWindowBaseClass::WindowProc(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
  CMyWindowBaseClass *pOwner = (CMyWindowBaseClass *)FromHandle(hWnd);

  if(uiMsg == WM_CREATE)
  {
    LPCREATESTRUCT lpCCS = (LPCREATESTRUCT)lParam;

    if(lpCCS && lpCCS->lpCreateParams &&
       !((CMyWindowBaseClass *)lpCCS->lpCreateParams)->m_hWnd)
    {
      ((CMyWindowBaseClass *)lpCCS->lpCreateParams)->m_hWnd = hWnd;
    }
  }

  // TODO:  this is where I do something cool, if needed.

  LRESULT lRval = CMySubClassWindow::WindowProc(hWnd, uiMsg, wParam, lParam);

  if(uiMsg == WM_DESTROY)
  {
    CMyThread *pThread = MyGetCurrentThread();

    if(pThread && pThread->m_hWnd == hWnd)
    {
      pThread->m_hWnd = NULL; // when window destroyed, thread 'Run' exits
    }

//    CMyApp *pApp = MyGetApp();
//    if(pApp && pApp->m_hWnd == hWnd)
//    {
//      pApp->m_hWnd = NULL;
//    }
  }

  return lRval;
}


/////////////////////////////////////////////////////////////////////////////
// CMyPaintDC

CMyPaintDC::CMyPaintDC(HWND hWnd)
{
  m_hWnd = hWnd;
  Attach(::BeginPaint(hWnd, &m_ps));
};

CMyPaintDC::~CMyPaintDC()
{
  HDC hdcOldPaint = Detach();
  ::EndPaint(m_hWnd, &m_ps);
}




/////////////////////////////////////////////////////////////////////////////
// CMySubClassWindow

CArray<CMySubClassWindow *,CMySubClassWindow *> CMySubClassWindow::m_aObjects;
volatile LONG CMySubClassWindow::m_lSpinLock = 0;

CMySubClassWindow::CMySubClassWindow()
{
  m_hWnd = NULL;       // pre-assign these first
  m_pSuperWndProc = NULL;

  AddToObjectsArray();
}

CMySubClassWindow::~CMySubClassWindow()
{
  RemoveFromObjectsArray();

  // do any additional destruction here
}

void CMySubClassWindow::InnerAddToObjectsArray()
{
  int i1;

//  TRY
//  {
    for(i1 = (int)m_aObjects.GetSize() - 1; i1 >= 0; i1--)
      if(CMySubClassWindow::m_aObjects[i1] == this)
        break;

    if(i1 < 0)
      m_aObjects.Add(this);
//  }
//  END_TRY;
}

void CMySubClassWindow::AddToObjectsArray()
{
  SpinLock();
  __try
  {
    InnerAddToObjectsArray();
  }
  __except(EXCEPTION_EXECUTE_HANDLER)
  {
  }
  SpinUnlock();
}

void CMySubClassWindow::InnerRemoveFromObjectsArray()
{
  int i1;
//  TRY
//  {
    for(i1 = (int)m_aObjects.GetSize() - 1; i1 >= 0; i1--)
    {
      if(m_aObjects[i1] == this)
      {
        m_aObjects.RemoveAt(i1);
      }
    }
//  }
//  END_TRY;
}

void CMySubClassWindow::RemoveFromObjectsArray()
{
  SpinLock();

  __try
  {
    InnerRemoveFromObjectsArray();
  }
  __except(EXCEPTION_EXECUTE_HANDLER)
  {
  }

  SpinUnlock();
}

void CMySubClassWindow::SpinLock()
{
  while(InterlockedExchange(&m_lSpinLock, 1))
    Sleep(0);
}

void CMySubClassWindow::SpinUnlock()
{
  m_lSpinLock = 0;
}

CMySubClassWindow *CMySubClassWindow::FromHandle(HWND hWnd)
{
  int i1;
  CMySubClassWindow *pRval = NULL;

  SpinLock();

  __try
  {
    for(i1=(int)m_aObjects.GetSize() - 1; i1 >= 0; i1--)
    {
      CMySubClassWindow *pTemp = m_aObjects[i1];
      if(pTemp)
      {
        __try
        {
          if(pTemp->m_hWnd == hWnd)
          {
            pRval = pTemp;
            break;
          }
        }
        __except(EXCEPTION_EXECUTE_HANDLER)
        {
        }
      }
    }
  }
  __except(EXCEPTION_EXECUTE_HANDLER)
  {
  }

  SpinUnlock();

  return(pRval);
}

void CMySubClassWindow::Attach(HWND hWnd)
{
  // will only attach to one object

  if(!hWnd || FromHandle(hWnd))
    return;

  WNDPROC pProc = (WNDPROC)(INT_PTR)::GetWindowLongPtr(hWnd, GWLP_WNDPROC);

  if(!pProc)  // probably an error
    return;

  SpinLock();  // do this to make sure everything's consistent, first

  m_hWnd = hWnd;
  m_pSuperWndProc = pProc;

  SpinUnlock();

  ::SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG)(INT_PTR)WindowProc);
    // I am now "subclassed"
}

HWND CMySubClassWindow::Detach()
{
  if(!m_hWnd || !m_pSuperWndProc)
    return(NULL);

  HWND hwndRval = m_hWnd;

  ::SetWindowLongPtr(m_hWnd, GWLP_WNDPROC, (LONG)(INT_PTR)m_pSuperWndProc);
    // no longer subclassed

  SpinLock();  // do this to make sure everything's consistent, first

  m_hWnd = NULL;
  m_pSuperWndProc = NULL;

  SpinUnlock();

  return(hwndRval);
}

LRESULT CMySubClassWindow::DefWindowProc(UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
  if(!m_pSuperWndProc)
    return(::DefWindowProc(m_hWnd, uiMsg, wParam, lParam));

  return(m_pSuperWndProc(m_hWnd, uiMsg, wParam, lParam));
}

LRESULT CMySubClassWindow::WindowProc(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
  // a pointer to the class, please (stored in DWLP_USER)

  CMySubClassWindow *pOwner = FromHandle(hWnd);

  if(!pOwner)
    return(::DefWindowProc(hWnd, uiMsg, wParam, lParam));  // just in case

  return(pOwner->MessageHandler(uiMsg, wParam, lParam));
}

void CMySubClassWindow::GetWindowText(CString &csText)
{
  LPSTR p1 = csText.GetBufferSetLength(4096); // for now, later get actual length
  
  ::GetWindowText(m_hWnd, p1, 4096);

  csText.ReleaseBuffer(-1);
}


// CONTROL CLASSES (like CEdit)
LRESULT CEdit::MessageHandler(UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
  return DefWindowProc(uiMsg, wParam, lParam);
}

static void DoBitmapBackground(HWND, CBitmap *, CDC &);

LRESULT CStatic::MessageHandler(UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
  if(uiMsg == WM_ERASEBKGND)
  {
    if(m_Bitmap.GetGdiObjectHandle() != NULL)
    {
      CDC dcOrig((HDC)wParam);

      DoBitmapBackground(m_hWnd, &m_Bitmap, dcOrig);

      dcOrig.Detach();  // don't delete this DC

      return TRUE; // handled
    }
  }
  else if(uiMsg == WM_PAINT)
  {
    if(m_Bitmap.GetGdiObjectHandle() != NULL)
    {
      CMyPaintDC dc(m_hWnd); // device context for painting

      ::SendMessage(m_hWnd, WM_ERASEBKGND, (WPARAM)dc.GetSafeHdc(), 0);

//      DoBitmapBackground(m_hWnd, &m_Bitmap, dc);

      return TRUE;
    }
  }

  return DefWindowProc(uiMsg, wParam, lParam);
}

LRESULT CButton::MessageHandler(UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
  LRESULT lRval;

  lRval = DefWindowProc(uiMsg, wParam, lParam);

  if(uiMsg == WM_CREATE)
  {
    if(m_Bitmap.GetGdiObjectHandle() != NULL)
    {
      SetBitmap(); // make sure I assign it now
    }
  }

  return lRval;
}


void DoBitmapBackground(HWND hWnd, CBitmap *pBitmap, CDC &dcOrig)
{
  if(!GetUpdateRect(hWnd, NULL, FALSE))
    return;  // don't erase anything

  CDC dc;

  dc.CreateCompatibleDC(&dcOrig);
  dc.SaveDC();

  CRect rct, rctClip;

  if(!::GetUpdateRect(hWnd, &rctClip, FALSE))
    ::GetClientRect(hWnd, &rctClip);      // just use the client rect

  ::GetClientRect(hWnd, &rct);  // I also need this for the bitmap stretch...

  dc.SelectObject(pBitmap);

  CRgn rgnClip;

  rgnClip.CreateRectRgn(rctClip.left, rctClip.top, rctClip.right, rctClip.bottom);
  dcOrig.SelectClipRgn(&rgnClip, RGN_COPY);  // only erases the part I need to erase...

  rgnClip.DeleteObject();  // not needed now


  BITMAP bmp;
  memset(&bmp, 0, sizeof(bmp));

  pBitmap->GetObject(sizeof(bmp), &bmp);

  int iL1 = rct.left;
  int iT1 = rct.top;
  int iR1 = rct.right - rct.left;
  int iB1 = rct.bottom - rct.top;
  int iL2 = 0;
  int iT2 = 0;
  int iR2 = bmp.bmWidth;
  int iB2 = bmp.bmHeight;

  if(iR1 == iR2)
  {
    iL1 = rctClip.left;
    iL2 = rctClip.left - rct.left;

    iR1 = iR2 = rctClip.right - rctClip.left;
  }
  else if(iR2 == 1)
  {
    iL1 = rctClip.left;
    iR1 = rctClip.right - rctClip.left;
  }

  if(iB1 == iB2)
  {
    iT1 = rctClip.top;
    iT2 = rctClip.top - rct.top;
    iB1 = iB2 = rctClip.bottom - rctClip.top;
  }

  dcOrig.StretchBlt(iL1, iT1, iR1, iB1,
                    &dc, iL2, iT2, iR2, iB2, SRCCOPY);


  dc.RestoreDC(-1);
  dc.DeleteDC();

  dcOrig.RestoreDC(-1);
  dcOrig.Detach();
}
