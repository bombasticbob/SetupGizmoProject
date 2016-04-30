/////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                     //
//    ___          _    _                _      _       _    ____                                      //
//   / _ \  _ __  | |_ (_)  ___   _ __  | |    (_) ___ | |_ | __ )   ___ __  __    ___  _ __   _ __    //
//  | | | || '_ \ | __|| | / _ \ | '_ \ | |    | |/ __|| __||  _ \  / _ \\ \/ /   / __|| '_ \ | '_ \   //
//  | |_| || |_) || |_ | || (_) || | | || |___ | |\__ \| |_ | |_) || (_) |>  <  _| (__ | |_) || |_) |  //
//   \___/ | .__/  \__||_| \___/ |_| |_||_____||_||___/ \__||____/  \___//_/\_\(_)\___|| .__/ | .__/   //
//         |_|                                                                         |_|    |_|      //
//                                                                                                     //
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                     //
//                    Copyright (c) 1998-2016 by S.F.T. Inc. - All rights reserved                     //
//                Use, copying, and distribution of this software are licensed according               //
//                  to the GPLv2, LGPLv2, or BSD license, as appropriate (see COPYING)                 //
//                                                                                                     //
/////////////////////////////////////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "setup.h"
#include "OptionListBox.h"

//#ifdef _DEBUG
//#define new DEBUG_NEW
//#undef THIS_FILE
//static char THIS_FILE[] = __FILE__;
//#endif

/////////////////////////////////////////////////////////////////////////////
// COptionListBox

COptionListBox::COptionListBox()
{
}

COptionListBox::~COptionListBox()
{
}

LPARAM COptionListBox::MessageHandler(UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
  switch(uiMsg)
  {
    case WM_LBUTTONDOWN:
      return(OnLButtonDown((UINT)wParam, CPoint(LOWORD(lParam),HIWORD(lParam))));

    case WM_LBUTTONUP:
      return(OnLButtonUp((UINT)wParam, CPoint(LOWORD(lParam),HIWORD(lParam))));

    case WM_KEYDOWN:
      return(OnKeyDown((UINT)wParam, LOWORD(lParam), HIWORD(lParam)));
    case WM_KEYUP:
      return(OnKeyUp((UINT)wParam, LOWORD(lParam), HIWORD(lParam)));
  }

  return(DefWindowProc(uiMsg, wParam, lParam));
}


/////////////////////////////////////////////////////////////////////////////
// COptionListBox message handlers

LRESULT COptionListBox::OnLButtonDown(UINT nFlags, CPoint point) 
{
  return(DefWindowProc(WM_LBUTTONDOWN, (WPARAM)nFlags, MAKELPARAM(point.x,point.y)));
}

LRESULT COptionListBox::OnLButtonUp(UINT nFlags, CPoint point) 
{
  // if 'point.x' is less than 16, g'head and toggle the selection

  LRESULT lRval = DefWindowProc(WM_LBUTTONUP, (WPARAM)nFlags, MAKELPARAM(point.x,point.y));

  if(point.x < 16)
  {
    int iSel = GetCurSel();

    if(iSel != LB_ERR)
    {
      SetItemData(iSel, !GetItemData(iSel));
      InvalidateRect(NULL, TRUE);
    }
  }

  return(lRval);
}


int COptionListBox::GetSel(int nIndex) const
{
  if(nIndex >= 0 && nIndex < GetCount())
  {
    if(GetItemData(nIndex))
      return(1);
    else
      return(0);
  }

  return(LB_ERR);
}

int COptionListBox::SetSel(int nIndex, BOOL bSelect /*= TRUE*/)
{
  if(nIndex >= 0 && nIndex < GetCount())
  {
    return(SetItemData(nIndex, bSelect ? 1 : 0));
  }

  return(LB_ERR);
}

int COptionListBox::GetSelCount() const
{
  int i1, iCount = 0;
  for(i1=0; i1 < GetCount(); i1++)
  {
    if(GetItemData(i1))
      iCount++;
  }

  return(iCount);
}

int COptionListBox::GetSelItems(int nMaxItems, LPINT rgIndex)
{
  if(!rgIndex)
    return(LB_ERR);

  int i1, iCount = 0;
  for(i1=0; i1 < GetCount(); i1++)
  {
    if(GetItemData(i1))
    {
      if(iCount >= nMaxItems)
        return(iCount);

      rgIndex[iCount++] = i1;
    }
  }

  return(iCount);  // # of items actually placed into array
}

int COptionListBox::InsertString(int nIndex, LPCTSTR lpszItem)
{
  int iRval = (int)SendMessage(LB_INSERTSTRING, (WPARAM)nIndex, (LPARAM)lpszItem);

  if(iRval != LB_ERR && iRval != LB_ERRSPACE)
    SetItemData(iRval, 0);

  return(iRval);
}

int COptionListBox::AddString(LPCTSTR lpszItem)
{
  int iRval = (int)SendMessage(LB_ADDSTRING, 0, (LPARAM)lpszItem);

  if(iRval != LB_ERR && iRval != LB_ERRSPACE)
    SetItemData(iRval, 0);

  return(iRval);
}

int COptionListBox::DeleteString(UINT nIndex)
{
  return((int)SendMessage(LB_DELETESTRING, (WPARAM)nIndex, 0));
}

void COptionListBox::GetText(int nIndex, CString &csText) const
{
  int nLen = (int)SendMessage(LB_GETTEXTLEN, (WPARAM)nIndex, 0);

  if(nLen == LB_ERR || !nLen)
    csText = "";
  else
  {
    LPSTR lp1 = csText.GetBufferSetLength(nLen + 1);

    if(!lp1)
      csText = "";
    else
    {
      SendMessage(LB_GETTEXT, (WPARAM)nIndex, (LPARAM)lp1);
      csText.ReleaseBuffer(-1);
    }
  }
}


LRESULT COptionListBox::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
  if(nChar == VK_SPACE &&
     !(GetKeyState(VK_MENU) & 0x80000000) &&
     !(GetKeyState(VK_CONTROL) & 0x80000000) &&
     !(GetKeyState(VK_SHIFT) & 0x80000000))
  {
    // simulate clicking in the area of the box

    int iSel = GetCurSel();

    if(iSel != LB_ERR)
    {
      SetSel(iSel, !GetSel(iSel));
      InvalidateRect(NULL, TRUE);
    }
  }
	
	return(DefWindowProc(WM_KEYDOWN, (WPARAM)nChar, MAKELPARAM(nRepCnt, nFlags)));
}

LRESULT COptionListBox::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
  if(nChar == VK_SPACE &&
     !(GetKeyState(VK_MENU) & 0x80000000) &&
     !(GetKeyState(VK_CONTROL) & 0x80000000) &&
     !(GetKeyState(VK_SHIFT) & 0x80000000))
  {
    return(0);  // eat it (already processed)
  }

	return(DefWindowProc(WM_KEYUP, (WPARAM)nChar, MAKELPARAM(nRepCnt, nFlags)));
}
