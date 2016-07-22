/////////////////////////////////////////////////////////////////////////////////////
//                                                                                 //
//   _____  _                                                                      //
//  |_   _|| |__    ___  _ __  _ __ ___    ___   _ __ ___      ___  _ __   _ __    //
//    | |  | '_ \  / _ \| '__|| '_ ` _ \  / _ \ | '_ ` _ \    / __|| '_ \ | '_ \   //
//    | |  | | | ||  __/| |   | | | | | || (_) || | | | | | _| (__ | |_) || |_) |  //
//    |_|  |_| |_| \___||_|   |_| |_| |_| \___/ |_| |_| |_|(_)\___|| .__/ | .__/   //
//                                                                 |_|    |_|      //
//                                                                                 //
/////////////////////////////////////////////////////////////////////////////////////
//                                                                                 //
//          Copyright (c) 1998-2016 by S.F.T. Inc. - All rights reserved           //
//      Use, copying, and distribution of this software are licensed according     //
//        to the GPLv2, LGPLv2, or BSD license, as appropriate (see COPYING)       //
//                                                                                 //
/////////////////////////////////////////////////////////////////////////////////////


// This is a dialog control that draws the 'progress thermometer' with the percentage

#include "stdafx.h"
#include "setup.h"
#include "Thermom.h"

//#ifdef _DEBUG
//#define new DEBUG_NEW
//#undef THIS_FILE
//static char THIS_FILE[] = __FILE__;
//#endif

/////////////////////////////////////////////////////////////////////////////
// CThermometer

CThermometer::CThermometer()
{
}

CThermometer::~CThermometer()
{
}

/////////////////////////////////////////////////////////////////////////////
// CThermometer message handlers

BOOL CThermometer::OnEraseBkgnd(HDC hDC) 
{
  // TODO: Add your message handler code here and/or call default
  
//  return CStatic::OnEraseBkgnd(pDC);

  return TRUE;  // erased (supposedly)
}

LRESULT CThermometer::MessageHandler(UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
  switch(uiMsg)
  {
    case WM_PAINT:
      OnPaint();
      return(0);

    case WM_ERASEBKGND:
      return OnEraseBkgnd((HDC)wParam); //CDC::FromHandle((HDC)wParam)));

    case WM_SETTEXT:
      return OnSetText(wParam, lParam);
  }

  return(DefWindowProc(uiMsg, wParam, lParam));
}

void CThermometer::OnPaint() 
{
  CMyPaintDC dc(m_hWnd); // device context for painting
  
  CString csText;
  GetWindowText(csText);
  
  CRect rct;
  GetClientRect(&rct);

  int iVal = atoi(csText);  // value from 0 to 100

  int iBorder = iVal * (rct.right - rct.left) / 100 + rct.left;

  CRgn rgnLeft, rgnRight;
  rgnLeft.CreateRectRgn(rct.left, rct.top, iBorder, rct.bottom);
  rgnRight.CreateRectRgn(iBorder, rct.top, rct.right, rct.bottom);

  CBrush cbr;

  dc.SaveDC();

  // paint the "blue half"

  dc.SelectObject(&rgnLeft);

  cbr.CreateSolidBrush(RGB(0,0,255));  // blue brush
  dc.FillRect(&rct, &cbr);
  cbr.DeleteObject();

  dc.SetTextColor(RGB(255,255,255));  // white text
  dc.SetBkColor(RGB(0,0,255));  // blue background (like rect)

  dc.DrawText(csText + "%", &rct, DT_SINGLELINE | DT_VCENTER | DT_CENTER);

  // do it again for "white half"

  dc.SelectObject(&rgnRight);

  cbr.CreateSolidBrush(RGB(255,255,255));  // white brush
  dc.FillRect(&rct, &cbr);
  cbr.DeleteObject();

  dc.SetTextColor(RGB(0,0,255));  // blue text
  dc.SetBkColor(RGB(255,255,255));  // white background (like rect)

  dc.DrawText(csText + "%", &rct, DT_SINGLELINE | DT_VCENTER | DT_CENTER);


  dc.RestoreDC(-1);  // restore original values

  rgnLeft.DeleteObject();
  rgnRight.DeleteObject();  // cleanup  (I am done)
}


LRESULT CThermometer::OnSetText(WPARAM wParam, LPARAM lParam)
{
  InvalidateRect(NULL, TRUE);

  return(DefWindowProc(WM_SETTEXT, wParam, lParam));
}
