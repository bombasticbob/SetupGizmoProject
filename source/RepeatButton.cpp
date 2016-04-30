////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                    //
//   ____                            _    ____          _    _                                        //
//  |  _ \  ___  _ __    ___   __ _ | |_ | __ )  _   _ | |_ | |_  ___   _ __      ___  _ __   _ __    //
//  | |_) |/ _ \| '_ \  / _ \ / _` || __||  _ \ | | | || __|| __|/ _ \ | '_ \    / __|| '_ \ | '_ \   //
//  |  _ <|  __/| |_) ||  __/| (_| || |_ | |_) || |_| || |_ | |_| (_) || | | | _| (__ | |_) || |_) |  //
//  |_| \_\\___|| .__/  \___| \__,_| \__||____/  \__,_| \__| \__|\___/ |_| |_|(_)\___|| .__/ | .__/   //
//              |_|                                                                   |_|    |_|      //
//                                                                                                    //
////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                    //
//                    Copyright (c) 1998-2016 by S.F.T. Inc. - All rights reserved                    //
//               Use, copying, and distribution of this software are licensed according               //
//                 to the GPLv2, LGPLv2, or BSD license, as appropriate (see COPYING)                 //
//                                                                                                    //
////////////////////////////////////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "setupgiz.h"
#include "RepeatButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRepeatButton

CRepeatButton::CRepeatButton()
{
}

CRepeatButton::~CRepeatButton()
{
}


BEGIN_MESSAGE_MAP(CRepeatButton, CButton)
	//{{AFX_MSG_MAP(CRepeatButton)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_KILLFOCUS()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRepeatButton message handlers

void CRepeatButton::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CButton::OnLButtonDown(nFlags, point);

  SetTimer(101, 500, NULL);  // 1/2 second timer
}

void CRepeatButton::OnLButtonUp(UINT nFlags, CPoint point) 
{
  KillTimer(101);
  KillTimer(102);

	CButton::OnLButtonUp(nFlags, point);
}

void CRepeatButton::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
  // this *MUST* be ignored in 'normal' processing

  CWnd *pwndParent = GetParent();

  if(pwndParent)  // posts an extra message
    pwndParent->PostMessage(WM_COMMAND,
                            MAKELPARAM((WORD)GetWindowLong(m_hWnd, GWL_ID),
                                       BN_CLICKED),
                            (LPARAM)m_hWnd);
}

void CRepeatButton::OnKillFocus(CWnd* pNewWnd) 
{
	CButton::OnKillFocus(pNewWnd);
	
  KillTimer(101);
  KillTimer(102);
}

void CRepeatButton::OnTimer(UINT nIDEvent) 
{
  if(nIDEvent == 101)
  {
    KillTimer(101);
    SetTimer(102, 50, NULL);

    return;
  }
  else if(nIDEvent == 102)
  {
    if(GetKeyState(VK_LBUTTON) < 0)  // key is down
    {
      CWnd *pwndParent = GetParent();

      if(pwndParent)
        pwndParent->PostMessage(WM_COMMAND,
                                MAKELPARAM((WORD)GetWindowLong(m_hWnd, GWL_ID),
                                           BN_CLICKED),
                                (LPARAM)m_hWnd);
    }
    else
    {
      KillTimer(102);
    }

    return;
  }

	CButton::OnTimer(nIDEvent);
}
