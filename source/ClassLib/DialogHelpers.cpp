////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                    //
//   ____   _         _                _   _        _                                                 //
//  |  _ \ (_)  __ _ | |  ___    __ _ | | | |  ___ | | _ __    ___  _ __  ___     ___  _ __   _ __    //
//  | | | || | / _` || | / _ \  / _` || |_| | / _ \| || '_ \  / _ \| '__|/ __|   / __|| '_ \ | '_ \   //
//  | |_| || || (_| || || (_) || (_| ||  _  ||  __/| || |_) ||  __/| |   \__ \ _| (__ | |_) || |_) |  //
//  |____/ |_| \__,_||_| \___/  \__, ||_| |_| \___||_|| .__/  \___||_|   |___/(_)\___|| .__/ | .__/   //
//                              |___/                 |_|                             |_|    |_|      //
//                                                                                                    //
////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                    //
//                       Copyright (c) 2016 by S.F.T. Inc. - All rights reserved                      //
//               Use, copying, and distribution of this software are licensed according               //
//                 to the GPLv2, LGPLv2, or BSD license, as appropriate (see COPYING)                 //
//                                                                                                    //
////////////////////////////////////////////////////////////////////////////////////////////////////////


#include "DialogHelpers.h"


//#ifdef _DEBUG
//#define new DEBUG_NEW
//#undef THIS_FILE
//static char THIS_FILE[] = __FILE__;
//#endif


// SEMI-GLOBAL HELPERS

static void GetWindowText(HWND hWnd, CString &csText)
{
  if(!hWnd || !::IsWindow(hWnd))
  {
    csText = "";
    return;
  }

  int iLen = ::GetWindowTextLength(hWnd);

  if(iLen > 0)
  {
    LPSTR lp1 = csText.GetBufferSetLength(iLen + 1);

    if(lp1)
      ::GetWindowText(hWnd, lp1, iLen + 1);

    csText.ReleaseBuffer(iLen);
    csText.ReleaseBuffer(-1);
  }
  else
    csText = "";
}




/////////////////////////////////////////////////////////////////////////////
// CMyDialogBaseClass

CMyDialogBaseClass::CMyDialogBaseClass()
{
  m_idDlg = 0;
  m_hParent = NULL;
  m_hWnd = NULL;
}

CMyDialogBaseClass::CMyDialogBaseClass(UINT idDlg, HWND hParent /*=NULL*/)
{
  m_idDlg = idDlg;
  m_hParent = hParent;
  m_hWnd = NULL;
}

CMyDialogBaseClass::~CMyDialogBaseClass()
{
}

BOOL WINAPI CMyDialogBaseClass::DialogProc(HWND hDlg, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
  // a pointer to the class, please (stored in DWLP_USER)

  CMyDialogBaseClass *pOwner = NULL;

  if(uiMsg == WM_INITDIALOG)
  {
    SetWindowLong(hDlg, DWLP_USER, (LONG)lParam);

    pOwner = (CMyDialogBaseClass *)lParam;
    if(pOwner && !pOwner->m_hWnd)
    {
      pOwner->m_hWnd = hDlg;
    }

    ASSERT(pOwner && pOwner->m_hWnd == hDlg);
  }
  else
  {
    pOwner = (CMyDialogBaseClass *)(INT_PTR)GetWindowLongPtr(hDlg, DWLP_USER);
  }

  if(!pOwner)
    return FALSE;  // skip if owner not known

//  pOwner->m_msgLast.hwnd = hDlg;
//  pOwner->m_msgLast.message = uiMsg;
//  pOwner->m_msgLast.wParam = wParam;
//  pOwner->m_msgLast.lParam = lParam;
//  (DWORD &)(pOwner->m_msgLast.pt) = ::GetMessagePos();
//  pOwner->m_msgLast.time = ::GetMessageTime();

  // call class's message handler (yay)

  BOOL bRval = pOwner->MessageHandler(uiMsg, wParam, lParam);

  if(uiMsg == WM_NCDESTROY)
    pOwner->m_hWnd = NULL;

  return(bRval);
}

BOOL CMyDialogBaseClass::Create(UINT idDlg, HWND hParent /* = NULL */)
{
  if(m_hWnd)
    return FALSE;  // already created (ignore)

  m_idDlg = idDlg;
  m_hParent = hParent;

  // determine parent window if not specified using current thread

  if(!m_hParent && MyGetCurrentThread() && MyGetCurrentThread()->m_hWnd)
    m_hParent = MyGetCurrentThread()->m_hWnd;

  HWND hDlg = ::CreateDialogParam(MyGetApp()->m_hInstance, MAKEINTRESOURCE(m_idDlg),
                                  m_hParent, DialogProc, (LPARAM)this);

  if(hDlg && !m_hWnd)
  {
    m_hWnd = hDlg;

    ::PostMessage(m_hWnd, WM_INITDIALOG, 0, (LPARAM)this);
  }

  return(hDlg != NULL);
}

int CMyDialogBaseClass::DoModal()
{
  // if no parent, I'll need one for the call to 'DialogBoxParam'
  if(!m_hParent && MyGetCurrentThread() && MyGetCurrentThread()->m_hWnd)
    m_hParent = MyGetCurrentThread()->m_hWnd;

  return((int)::DialogBoxParam(MyGetApp()->m_hInstance, MAKEINTRESOURCE(m_idDlg),
                               m_hParent, DialogProc, (LPARAM)this));
}

void CMyDialogBaseClass::GetWindowText(CString &csText)
{
  ::GetWindowText(m_hWnd, csText);
}

void CMyDialogBaseClass::GetDlgItemText(UINT idItem, CString &csText)
{
  ::GetWindowText(::GetDlgItem(m_hWnd, idItem), csText);
}

