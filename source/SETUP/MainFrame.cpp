//////////////////////////////////////////////////////////////////////////////////////////
//                                                                                      //
//   __  __         _         _____                                                     //
//  |  \/  |  __ _ (_) _ __  |  ___|_ __  __ _  _ __ ___    ___     ___  _ __   _ __    //
//  | |\/| | / _` || || '_ \ | |_  | '__|/ _` || '_ ` _ \  / _ \   / __|| '_ \ | '_ \   //
//  | |  | || (_| || || | | ||  _| | |  | (_| || | | | | ||  __/ _| (__ | |_) || |_) |  //
//  |_|  |_| \__,_||_||_| |_||_|   |_|   \__,_||_| |_| |_| \___|(_)\___|| .__/ | .__/   //
//                                                                      |_|    |_|      //
//                                                                                      //
//////////////////////////////////////////////////////////////////////////////////////////
//                                                                                      //
//             Copyright (c) 1998-2016 by S.F.T. Inc. - All rights reserved             //
//        Use, copying, and distribution of this software are licensed according        //
//          to the GPLv2, LGPLv2, or BSD license, as appropriate (see COPYING)          //
//                                                                                      //
//////////////////////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "stdio.h"
#include "stdlib.h"
#include "math.h"
#include "setup.h"
#include "MainFrame.h"

//#ifdef _DEBUG
//#define new DEBUG_NEW
//#undef THIS_FILE
//static char THIS_FILE[] = __FILE__;
//#endif


void BrightnessAdjust(int &iR, int &iG, int &iB, int iLine, int iHeight)
{
  // adjust brightness to match reference brightness

  // brightness levels for white light:
  // GREEN 59.0%
  // BLUE 11.5%
  // RED 29.5%

  const int pctR=295, pctG=590, pctB=115;

  int fR1 = 80 * pctR;
  int fG1 = 80 * pctG;
  int fB1 = 255 * pctB;

  int fR2 = 255 * pctR;
  int fG2 = 88 * pctG;
  int fB2 = 88 * pctB;

  int iRef1 = fR1 + fG1 + fB1;  // level above 2/3
  int iRef2 = fR2 + fG2 + fB2;  // level below 1/3

  int iLevel = iLine - iHeight / 3;
  int iRange = 2 * iHeight / 3 - iHeight / 3;
  int iRange2 = iHeight - iHeight / 3 - iRange;

  if(iLevel < 0)
    iLevel = 0;

  int iRef;
  if(iLevel > iRange)
  {
    iLevel -= iRange;

    if(iLevel > iRange2)
      iLevel = iRange2;

//    double dRatio = (double)(iLevel * iLevel) / (iRange * iRange);
    double dRatio = (double)(iLevel * iLevel) / (iRange2 * iRange2);

    iRef = (int)(iRef1 - dRatio * iRef1);  // sweeps down to zero
  }
  else
  {
    double dRatio = (double)(iLevel * iLevel) / (iRange * iRange);

    iRef = (int)(iRef2 + dRatio * (iRef1 - iRef2));
  }

  int iCur = iR * pctR + iG * pctG + iB * pctB;

  if(!iCur)
    return;

  double dFactor = (double)iRef / iCur;  // what I must multiply by

  iR = (int)(iR * dFactor + .5);
  iG = (int)(iG * dFactor + .5);
  iB = (int)(iB * dFactor + .5);

  while(iR > 255 || iG > 255 || iB > 255)
  {
    if(iG > 255)
    {
      iG = 255;

      iCur = iR * pctR + iB * pctB;

      dFactor = (double)(iRef - 255 * pctG) / iCur;

      iR = (int)(iR * dFactor + .5);
      iB = (int)(iB * dFactor + .5);
    }
    if(iB > 255)
    {
      iB = 255;

      iCur = iR * pctR + iG * pctG;

      dFactor = (double)(iRef - 255 * pctB) / iCur;

      iR = (int)(iR * dFactor + .5);
      iG = (int)(iG * dFactor + .5);
    }
    if(iR > 255)
    {
      iR = 255;

      iCur = iG * pctG + iB * pctB;

      dFactor = (double)(iRef - 255 * pctR) / iCur;

      iG = (int)(iG * dFactor + .5);
      iB = (int)(iB * dFactor + .5);
    }
  }
}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame

//IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

CMainFrame::CMainFrame()
{
#ifdef NO_COMPRESS
  m_bmpPalmTree.LoadBitmap(IDB_PALM);
  m_bmpPromo.LoadBitmap(IDB_PROMO);
#else // NO_COMPRESS

  // bitmaps are in a resource.  I need temp file names

  CString csTemp = "%tmpdir%\\SFTSETUP";
  theApp.ExpandStrings(csTemp);

  CheckForAndCreateDirectory(csTemp, FALSE);

  csTemp = AdjustPathName(csTemp);

  BOOL bSuccess = FALSE;

  HANDLE hFile = CreateFile(csTemp + "BITMAPS.CAB",
                            GENERIC_READ | GENERIC_WRITE,
                            0, NULL, CREATE_ALWAYS,
                            FILE_ATTRIBUTE_NORMAL, NULL);

  if(hFile != INVALID_HANDLE_VALUE)
  {
    HRSRC hRes = FindResource(theApp.m_hInstance,
                              MAKEINTRESOURCE(IDR_BITMAPS_CAB),
                              "CAB");
    if(hRes)
    {
      DWORD cbRes = SizeofResource(theApp.m_hInstance, hRes);
      HANDLE h1 = LoadResource(theApp.m_hInstance, hRes);

      if(h1)
      {
        LPVOID lpV = LockResource(h1);

        if(lpV)
        {
          DWORD cb1;

          if(cbRes &&
             WriteFile(hFile, lpV, cbRes, &cb1, NULL) &&
             cb1 == cbRes)
          {
            bSuccess = TRUE;
          }

          UnlockResource(h1);
        }

        FreeResource(h1);
      }

    }

    CloseHandle(hFile);
  }

  ExtractFilesFromTempCabinet(csTemp + "BITMAPS.CAB", csTemp);

  HANDLE h1 = LoadImage(NULL, csTemp + "PALM.BMP", IMAGE_BITMAP, 0, 0,
                        LR_DEFAULTCOLOR | LR_LOADFROMFILE);

  if(h1)
  {
    m_bmpPalmTree.Attach((HBITMAP)h1);
  }

  h1 = LoadImage(NULL, csTemp + "SFTPROMO.BMP", IMAGE_BITMAP, 0, 0,
                 LR_DEFAULTCOLOR | LR_LOADFROMFILE);

  if(h1)
  {
    m_bmpPromo.Attach((HBITMAP)h1);
  }

  DeleteFile(csTemp + "PALM.BMP");
  DeleteFile(csTemp + "SFTPROMO.BMP");
  DeleteFile(csTemp + "BITMAPS.CAB");


#endif // NO_COMPRESS
}

CMainFrame::~CMainFrame()
{
}

LRESULT CMainFrame::MessageHandler(UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
  if(uiMsg == WM_CREATE)
  {
    return(OnCreate((LPCREATESTRUCT)lParam));
  }
  else if(uiMsg == WM_ERASEBKGND)
  {
    return OnEraseBkgnd((HDC)wParam);
  }
  else if(uiMsg == WM_ENDSESSION)
  {
    OnEndSession((BOOL)wParam);

    // and flow through to 'DefWindowProc' after this part
  }
  else if(uiMsg == WM_QUERYENDSESSION)
  {
    return(OnQueryEndSession());
  }
  else if(uiMsg == WM_PAINT)
  {
    OnPaint();

    return TRUE;
  }

  return(DefWindowProc(uiMsg, wParam, lParam));
}



//
//BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
//  //{{AFX_MSG_MAP(CMainFrame)
//  ON_WM_ERASEBKGND()
//  ON_WM_CREATE()
//  ON_WM_PAINT()
//  ON_WM_ENDSESSION()
//  ON_WM_QUERYENDSESSION()
//  //}}AFX_MSG_MAP
//END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers

//BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs) 
//{
//  BOOL rval = CFrameWnd::PreCreateWindow(cs);
//
//  cs.style = WS_POPUP | WS_MAXIMIZE | WS_VISIBLE;
//  cs.dwExStyle = WS_EX_LEFT;
//
//  return(rval);
//}

BOOL CMainFrame::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle,
                        const RECT& rect, CMyWindowBaseClass* pParentWnd, UINT nID)
{
static const char szWndClass[]="SFT179862A1179E11D2A115004033901FF3";

  // TODO: Add your specialized code here and/or call the base class

//  CString csWndClass = AfxRegisterWndClass(CS_CLASSDC, NULL, NULL, theApp.LoadIcon(IDR_MAINFRAME));

  // register my window class (this only executes once anyway)
  // (this part adapted from 'AfxRegisterWndClass' in 'wincore.cpp')

//  WNDCLASS wc;
//  wc.style = CS_CLASSDC;
//  wc.lpfnWndProc = ::DefWindowProc;
//  wc.cbClsExtra = wc.cbWndExtra = 0;
//  wc.hInstance = AfxGetInstanceHandle();
//  wc.hIcon = theApp.LoadIcon(IDR_MAINFRAME);
//  wc.hCursor = NULL;
//  wc.hbrBackground = (HBRUSH)::GetStockObject(NULL_BRUSH);
//  wc.lpszMenuName = NULL;
//  wc.lpszClassName = szWndClass;
//
//  if(!AfxRegisterClass(&wc))
//    AfxThrowResourceException();

  CString csClass = theMasterWindowClassList.GetClassName(CS_CLASSDC, 0, 0,
                                                          (HBRUSH)::GetStockObject(NULL_BRUSH),
                                                          NULL, szWndClass);
                                                          
  int iWidth = GetSystemMetrics(SM_CXSCREEN);
  int iHeight = GetSystemMetrics(SM_CYSCREEN);

//  return CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);

  BOOL bShowFlag = TRUE;

  // TODO:  do I want to 'not show' the background whenever another
  //        SETUP app is running, or only for 'quiet setup' stuff?

  if(theApp.m_bQuietSetup)
  {
    // see if there's another main window out there with THIS class
    // name, and if there is, don't show the window.

    if(FindWindowEx(NULL, NULL, csClass, NULL))
    {
      bShowFlag = FALSE;  // don't show it - a SETUP window is already being displayed
    }
  }

  if(CreateEx(WS_EX_LEFT, csClass, "", WS_POPUP | WS_MAXIMIZE,
              CRect(0, 0, iWidth, iHeight), NULL, NULL))
  {
    if(bShowFlag)
      ShowWindow(SW_SHOWMAXIMIZED);

    return TRUE;
  }

  return FALSE;
}

static BOOL DoRectsIntersect(RECT &rect1, RECT &rect2)
{
  return(rect1.left <= rect2.right &&
         rect1.top <= rect2.bottom &&
         rect1.right >= rect2.left &&
         rect1.bottom >= rect2.top);
}

BOOL CMainFrame::OnEraseBkgnd(HDC hDC) 
{
  if(!GetUpdateRect(m_hWnd, NULL, FALSE))
    return(0);  // don't erase anything

  CDC dcOrig(hDC); // was 'pDC' (make sure I 'Detach()' it
  CDC dc;

  dc.CreateCompatibleDC(&dcOrig);
  dc.SaveDC();

  CRect rct, rctClip;

  if(!GetUpdateRect(m_hWnd, &rctClip, FALSE))
    GetClientRect(&rctClip);      // just use the client rect

  GetClientRect(&rct);  // I also need this for the bitmap stretch...

  dc.SelectObject(&m_bmpBackground);

  CRgn rgnClip;

  rgnClip.CreateRectRgn(rctClip.left, rctClip.top, rctClip.right, rctClip.bottom);
  dcOrig.SelectClipRgn(&rgnClip, RGN_COPY);  // only erases the part I need to erase...

  rgnClip.DeleteObject();  // not needed now


  BITMAP bmp;
  memset(&bmp, 0, sizeof(bmp));

  m_bmpBackground.GetObject(sizeof(bmp), &bmp);

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
//  pDC->StretchBlt(rct.left, rct.top, 
//                  rct.right - rct.left, rct.bottom - rct.top,
//                  &dc, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY);


  m_bmpPalmTree.GetObject(sizeof(bmp), &bmp);

  int iHeight = 2 * (rct.right - rct.left) / 5;
  int iWidth = bmp.bmWidth * iHeight / bmp.bmHeight;
  int iHeight0 = bmp.bmHeight;  // save for later

  CRect rect1((rct.left + rct.right - iWidth) / 2,
              rct.bottom - iHeight,
              (rct.left + rct.right - iWidth) / 2 + iWidth,
              rct.bottom);

  if(DoRectsIntersect(rctClip, rect1))
  {
    dc.SelectObject(&m_bmpPalmTree);  // draw the palm tree silhouette
    dcOrig.SetTextColor(RGB(0,0,0));
    dcOrig.SetBkColor(RGB(255,255,255));

    dcOrig.SetStretchBltMode(BLACKONWHITE);

    dcOrig.StretchBlt((rct.left + rct.right - iWidth) / 2,
                      rct.bottom - iHeight,
                      iWidth, iHeight,
                      &dc, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCAND);
  }

  m_bmpPromo.GetObject(sizeof(bmp), &bmp);

  int iHeight2 = bmp.bmHeight * iHeight / iHeight0;
  int iWidth2 = bmp.bmWidth * iHeight / iHeight0;

  int iHeight3 = (rct.bottom - rct.top) / 48;

  if(iHeight3 > 16)
    iHeight3 = 16;

  CRect rect2(rct.right - iWidth2,
              rct.bottom - iHeight2 - iHeight3,
              rct.right,
              rct.bottom - iHeight3);

  if(DoRectsIntersect(rctClip, rect2))
  {
    dcOrig.SetStretchBltMode(WHITEONBLACK);
//    dcOrig.SetStretchBltMode(HALFTONE);

    dc.SelectObject(&m_bmpPromo);  // draw the promo silhouette

    dcOrig.SetTextColor(RGB(255,255,255));
    dcOrig.SetBkColor(RGB(0,0,0));

    dcOrig.StretchBlt(rct.right - iWidth2,
                      rct.bottom - iHeight2 - iHeight3,
                      iWidth2, iHeight2,
                      &dc, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCAND);

    dcOrig.StretchBlt(rct.right - iWidth2 + 2,
                      rct.bottom - iHeight2 - iHeight3 + 2,
                      iWidth2, iHeight2,
                      &dc, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCAND);

    dcOrig.SetTextColor(RGB(0,0,0));
    dcOrig.SetBkColor(RGB(128,128,128));

    dcOrig.StretchBlt(rct.right - iWidth2,
                      rct.bottom - iHeight2 - iHeight3,
                      iWidth2, iHeight2,
                      &dc, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCPAINT);
  }

  dc.RestoreDC(-1);
  dc.DeleteDC();
  
//  ValidateRect(NULL);  // no need to paint, now

  dcOrig.RestoreDC(-1);
  dcOrig.Detach();

  return TRUE;

//  return CFrameWnd::OnEraseBkgnd(pDC);
}

void CMainFrame::OnPaint() 
{
  if(!GetUpdateRect(m_hWnd, NULL, FALSE))
    return;  // don't paint

  CMyPaintDC dc(m_hWnd); // device context for painting
  
  CDC *pDC = &dc;  // for coding convenience

  CRect rct;
  GetClientRect(&rct);

  // by default use screen height divided by 12 as font height, adjusted for
  // pixel aspect ratio.

  double dFactor = pDC->GetDeviceCaps(HORZSIZE) * pDC->GetDeviceCaps(VERTRES);
  if(!dFactor)
    dFactor = 1.0;
  else
    dFactor = pDC->GetDeviceCaps(VERTSIZE) * pDC->GetDeviceCaps(HORZRES) / dFactor;

  CRect rect2=CRect(rct.left, rct.top, rct.right - 3, rct.bottom - 3);
  CRect rect3=CRect(rct.left + 3, rct.top + 3, rct.right, rct.bottom);

  int iBaseFontHeight = (int)(dFactor * (rct.bottom - rct.top));

  if(DoRectsIntersect(dc.m_ps.rcPaint, rect2) ||
     DoRectsIntersect(dc.m_ps.rcPaint, rect3))
  {
    if(!m_Font.GetGdiObjectHandle())
    {
      m_Font.CreateFont(- iBaseFontHeight / 12, 0, 0, 0, FW_BOLD,
                        TRUE, FALSE, FALSE, DEFAULT_CHARSET,
                        OUT_TT_PRECIS, CLIP_TT_ALWAYS, PROOF_QUALITY,
                        VARIABLE_PITCH | TMPF_TRUETYPE | FF_DONTCARE,
                        "Arial");
    }

    pDC->SaveDC();
    pDC->SelectObject(&m_Font);
    pDC->SetBkMode(TRANSPARENT);  // don't screw up my neato background!

//    CString csTemp = szTitle; // "S.F.T. Inc.\nIntegrated Setup Utility";

    CString csTemp = theApp.m_csAppName + "\n" + theApp.m_csCompanyName;

    if(DoRectsIntersect(dc.m_ps.rcPaint, rect3))
    {
      pDC->SetTextColor(RGB(128,128,128));  // DARK GREY text
      pDC->DrawText(csTemp, &rect3, DT_CENTER | DT_WORDBREAK);
    }

    if(DoRectsIntersect(dc.m_ps.rcPaint, rect2))
    {
      pDC->SetTextColor(RGB(255,255,255));  // WHITE text
      pDC->DrawText(csTemp, &rect2, DT_CENTER | DT_WORDBREAK);
    }

    if(!theApp.m_csAuthCode.GetLength())  // assume it's not valid
    {
      if(!m_Font2.GetGdiObjectHandle())
      {
        m_Font2.CreateFont(- iBaseFontHeight / 32, 0, 0, 0, FW_BOLD,
                           TRUE, FALSE, FALSE, DEFAULT_CHARSET,
                           OUT_TT_PRECIS, CLIP_TT_ALWAYS, PROOF_QUALITY,
                           VARIABLE_PITCH | TMPF_TRUETYPE | FF_DONTCARE,
                           "Times New Roman");
      }

      pDC->SelectObject(&m_Font2);

      CRect rect4(rct.left,
                  rct.bottom - iBaseFontHeight / 32 - 3,
                  rct.right, rct.bottom);

      pDC->SetTextColor(RGB(255,255,0));  // yellow text
      pDC->DrawText("SETUP - Un-Licensed Evaluation Copy",
                    &rect4, DT_LEFT | DT_BOTTOM);
    }
#ifdef _DEBUG
    else if(theApp.m_csAuthCode.GetLength() &&
            !theApp.m_csAuthCodeCompany.GetLength())
    {
      if(!m_Font2.GetGdiObjectHandle())
      {
        m_Font2.CreateFont((rct.top - rct.bottom) / 32, 0, 0, 0, FW_BOLD,
                           TRUE, FALSE, FALSE, DEFAULT_CHARSET,
                           OUT_TT_PRECIS, CLIP_TT_ALWAYS, PROOF_QUALITY,
                           VARIABLE_PITCH | TMPF_TRUETYPE | FF_DONTCARE,
                           "Times New Roman");
      }

      pDC->SelectObject(&m_Font2);

      CRect rect4(rct.left,
                  rct.bottom - iBaseFontHeight / 32 - 3,
                  rct.right, rct.bottom);

      pDC->SetTextColor(RGB(128,255,128));  // LIGHT GREEN text
      pDC->DrawText("SETUP - Authorized Beta Release", &rect4, DT_LEFT | DT_BOTTOM);
    }
#endif // _DEBUG
    else
    {
      if(!m_Font2.GetGdiObjectHandle())
      {
        m_Font2.CreateFont(- iBaseFontHeight / 48, 0, 0, 0, FW_BOLD,
                           TRUE, FALSE, FALSE, DEFAULT_CHARSET,
                           OUT_TT_PRECIS, CLIP_TT_ALWAYS, PROOF_QUALITY,
                           VARIABLE_PITCH | TMPF_TRUETYPE | FF_DONTCARE,
                           "Times New Roman");
      }

      BITMAP bmp;
      m_bmpPalmTree.GetObject(sizeof(bmp), &bmp);  // need bitmap sizes for this...
                                                   // see 'erase background' code above
      int iHeight = 2 * (rct.right - rct.left) / 5;
      int iHeight0 = bmp.bmHeight;  // save for later

      m_bmpPromo.GetObject(sizeof(bmp), &bmp);

      int iWidth2 = bmp.bmWidth * iHeight / iHeight0;

      CRect rect4(rct.right - iWidth2,
                  rct.bottom - iBaseFontHeight / 48 - 1,
                  rct.right - 1, rct.bottom - 1);

      CRect rect5(rct.right - iWidth2 + 1,
                  rct.bottom - iBaseFontHeight / 48,
                  rct.right, rct.bottom);

      pDC->SelectObject(&m_Font2);

//      csTemp = "Licensed To: " + theApp.m_csAuthCodeCompany;
      csTemp = "Licensed Copy";

      if(DoRectsIntersect(dc.m_ps.rcPaint, rect5))
      {
        pDC->SetTextColor(RGB(64,64,64));  // VERY DARK GREY text
        pDC->DrawText(csTemp, &rect5, DT_CENTER | DT_TOP);
      }

      if(DoRectsIntersect(dc.m_ps.rcPaint, rect4))
      {
        pDC->SetTextColor(RGB(128,128,128));  // DARK GREY text
        pDC->DrawText(csTemp, &rect4, DT_CENTER | DT_TOP);
      }
    }

    pDC->RestoreDC(-1);
  }
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
//  if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
//    return -1;
  
  // find out how high the window is, and create an appropriate bitmap for it

  CRect rct;
  GetClientRect(&rct);  // should be same as window

  int iHeight = rct.bottom - rct.top;
  int iWidth = 1;        // > 4096 colors, optimize bitmap for low memory
  int iPenWidth = 1;     // ignore dithering for 4096 colors or more
  int iPenType = PS_SOLID;

  BeginWaitCursor();

  CDC dcOrig(::GetDC(m_hWnd));
  CDC dc;
  dc.CreateCompatibleDC(&dcOrig);
  dc.SaveDC();

  // if 4096 or fewer colors use dithered pen

  if((dcOrig.GetDeviceCaps(BITSPIXEL) + dcOrig.GetDeviceCaps(PLANES)) <= 13)
  {
    // fewer than 4096 colors - make bitmap width of screen

    iWidth = rct.right - rct.left;

    if((dcOrig.GetDeviceCaps(BITSPIXEL) + dcOrig.GetDeviceCaps(PLANES)) < 9)
    {
      // less than 256 colors, use double-width pen
      // this oughta make the 'pink' disappear

      iPenWidth = 8;
    }
    else
    {
      iPenWidth = 4;  // 256 colors and up - 4-bit pen
    }

    iPenType = PS_INSIDEFRAME;
  }

  m_bmpBackground.CreateCompatibleBitmap(&dcOrig, iWidth, iHeight);

  dc.SelectObject(&m_bmpBackground);

  int i1, i2;

  for(i1=0, i2 = iHeight - 1; i1 < (iHeight + iPenWidth); i1+=iPenWidth)
  {
    i2 = iHeight /* - 1 */ - i1;
    if(i2 < 0) i2 = 0;

    // NOTE:  formula for iHeight and i2 is like this:
    //        a * iHeight - b * i2start = 1  // such as 'iHeight / 3'
    //        a * iHeight - b * i2end = 0    // such as 'iHeight / 2'

    CPen cpen;
    if(i2 < iHeight / 8)
    {
      int i3 = 256 * 8 * i2 / iHeight;
      TRACE("SECTION 1:  %d,%d\r\n", i2 ,i3);
      cpen.CreatePen(iPenType, iPenWidth, RGB(0, 0, i3));
    }
    else if(i2 < iHeight / 3)
    {
      int i3 = 255 * (8 * iHeight - 24 * i2) / iHeight / 5;

      TRACE("Section 2:  i2 = %d\r\n", i2);

      // green sweeps from 16 to 192 in calculation, with min at 24
      // blue sweeps from 16 to 32

      int iG = 192 - 11 * i3 / 16; // 224 - 3 * i3 / 4; // 231 - 25 * i3 / 32; //224 - 3 * i3 / 4;
      int iB = 32 - i3 / 16; // 64 - 3 * i3 / 16; // 3 * i3 / 8;

      if(iG < 16)
        iG = 16;
      if(iG > 192)
        iG = 192;

//      if(iB < 32)
//        iB = 32;
//      if(iB > 64)
//        iB = 64;

      int iR = 255;

      BrightnessAdjust(iR, iG, iB, i2, iHeight);

      cpen.CreatePen(iPenType, iPenWidth, RGB(iR, iG, iB));
    }
//    else if(i2 < iHeight / 3)
    else if(i2 < 2 * iHeight / 5)
    {
//      int i3 = 256 * (3 * iHeight - 6 * i2) / iHeight;
      int i3 = 255 * (6 * iHeight - 15 * i2) / iHeight;

      if(i3 > 255)
        i3 = 255;  // this helps fix the 'striping' problem

      TRACE("Section 3:  i2 = %d\r\n", i2);

      int iR = 192 + i3 / 4;
      int iB = 192 - 5 * i3 / 8;

      // red sweeps from 255 down to 192
      // blue sweeps from 32 to 192
      // green stays at 192)

      if(iR < 192)
        iR = 192;
      if(iR > 255)
        iR = 255;

      if(iB < 32)
        iB = 32;
      if(iB > 192)
        iB = 192;

      int iG = 192;

      BrightnessAdjust(iR, iG, iB, i2, iHeight);

      cpen.CreatePen(iPenType, iPenWidth, RGB(iR, iG, iB));
    }
    else if(i2 < 2 * iHeight / 3)
    {
      // on entry the color is RGB(224,224,255)
//      int i3 = 255 * (4 * iHeight - 6 * i2) / iHeight;
      int i3 = 255 * (10 * iHeight - 15 * i2) / iHeight / 4;

      TRACE("Section 4:  i2 = %d\r\n", i2);

      int iRG = 128 + i3 / 4; // 144 + 3 * i3 / 16;
      int iB = 255 - i3 / 4;

      // iRG sweeps down from 192 to 128
      // iB sweeps up from 192 to 255

      if(iRG > 192)
        iRG = 192;
      if(iRG < 128)
        iRG = 128;

      if(iB > 255)
        iB = 255;
      if(iB < 192)
        iB = 192;

      int iR = iRG;
      int iG = iRG;

      BrightnessAdjust(iR, iG, iB, i2, iHeight);

      cpen.CreatePen(iPenType, iPenWidth, RGB(iR, iG, iB));
    }
    else
    {
      int i3 = 256 * (3 * iHeight - 3 * i2) / iHeight;

//      if(i3 > 255)
//        i3 = 255;
//
//      int iRG = i3 / 2;  // I want this to start at 128
//
//      // RG sweeps from 128 down to zero
//      // B sweeps from 255 down to zero
//
//      int iR = iRG;
//      int iG = iRG;
//      int iB = i3;

      // hold colors constant at 128 for RG and 255 for B, then use
      // the brightness adjust to change the hue

      int iR = 128;
      int iG = 128;
      int iB = 255;

      BrightnessAdjust(iR, iG, iB, i2, iHeight);

      cpen.CreatePen(iPenType, iPenWidth, RGB(iR, iG, iB));
    }

    dc.SaveDC();

    dc.SelectObject(&cpen);

    dc.MoveTo(0, i1);
    dc.LineTo(iWidth, i1);

    dc.RestoreDC(-1);

    cpen.DeleteObject();
  }

  dc.RestoreDC(-1);
  dc.DeleteDC();

  ::ReleaseDC(m_hWnd, dcOrig.Detach());

  EndWaitCursor();

  return 0;
}


void CMainFrame::OnEndSession(BOOL bEnding) 
{
  if(bEnding)
  {
    CString csMsg, csTitle;
    csMsg.LoadString(IDS_WARNING15);
    csTitle.LoadString(IDS_WARNING16);

    ::MessageBox(NULL, csMsg, csTitle,
                 MB_SETFOREGROUND | MB_OK | MB_ICONHAND | MB_TOPMOST);
  }

//  CFrameWnd::OnEndSession(bEnding);  // this calls 'ExitInstance' for me
}

BOOL CMainFrame::OnQueryEndSession() 
{
// NOTE:  calling this can cause trouble!!!
//  if(!CFrameWnd::OnQueryEndSession())
//    return FALSE;

  theApp.m_bRebootFlag = TRUE;  // if someone/thing attempts to restart
                                // I most likely need to reboot...

  return FALSE;  // tell 'em I don't want to exit.....  yet.
}
