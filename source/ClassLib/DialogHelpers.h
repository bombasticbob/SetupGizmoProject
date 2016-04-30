////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
//   ____   _         _                _   _        _                             _       //
//  |  _ \ (_)  __ _ | |  ___    __ _ | | | |  ___ | | _ __    ___  _ __  ___    | |__    //
//  | | | || | / _` || | / _ \  / _` || |_| | / _ \| || '_ \  / _ \| '__|/ __|   | '_ \   //
//  | |_| || || (_| || || (_) || (_| ||  _  ||  __/| || |_) ||  __/| |   \__ \ _ | | | |  //
//  |____/ |_| \__,_||_| \___/  \__, ||_| |_| \___||_|| .__/  \___||_|   |___/(_)|_| |_|  //
//                              |___/                 |_|                                 //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
//                 Copyright (c) 2016 by S.F.T. Inc. - All rights reserved                //
//         Use, copying, and distribution of this software are licensed according         //
//           to the GPLv2, LGPLv2, or BSD license, as appropriate (see COPYING)           //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////


#ifndef _DIALOGHELPERS_H_INCLUDED_
#define _DIALOGHELPERS_H_INCLUDED_

#pragma once

#include "WindowHelpers.h"




class CMyDialogBaseClass /* : public CObject */
{
public:

  CMyDialogBaseClass();
  CMyDialogBaseClass(UINT idDlg, HWND hParent = NULL);
  virtual ~CMyDialogBaseClass();

  virtual BOOL MessageHandler(UINT, WPARAM, LPARAM) = 0;  // derived class must override

  BOOL Create(UINT idDlg, HWND hParent = NULL);  // modeless type
  virtual int DoModal(); // modal type

  // a few common inlines

  __forceinline operator HWND() const { return(m_hWnd); }

  __forceinline LRESULT SendMessage(UINT Msg, WPARAM wParam, LPARAM lParam) { return(::SendMessage(m_hWnd, Msg, wParam, lParam)); }
  __forceinline void GetClientRect(CRect *pRct) { ::GetClientRect(m_hWnd, (LPRECT)pRct); }
  __forceinline void SetWindowText(LPCSTR szText) { ::SetWindowText(m_hWnd, szText); }
  __forceinline HWND GetDlgItem(UINT idItem) { return(::GetDlgItem(m_hWnd, idItem)); }
  __forceinline void SetDlgItemText(UINT idItem, LPCSTR szText) { ::SetDlgItemText(m_hWnd, idItem, szText); }
  __forceinline void SetIcon(HICON hIcon, BOOL bLarge) { ::SendMessage(m_hWnd, WM_SETICON, (WPARAM)(bLarge ? ICON_BIG : ICON_SMALL), (LPARAM)hIcon); }
  __forceinline BOOL IsIconic() { return(::IsIconic(m_hWnd)); }
  __forceinline BOOL EndDialog(INT_PTR nResult) { return(::EndDialog(m_hWnd, nResult)); }
  __forceinline BOOL UpdateWindow() { return(::UpdateWindow(m_hWnd)); }
  __forceinline BOOL ShowWindow(int nCmdShow) { return(::ShowWindow(m_hWnd, nCmdShow)); }
  __forceinline BOOL SetForegroundWindow() { return(::SetForegroundWindow(m_hWnd)); }
  __forceinline BOOL DestroyWindow() { return(::DestroyWindow(m_hWnd)); }
  __forceinline BOOL CloseWindow() { return(::CloseWindow(m_hWnd)); }


  void GetWindowText(CString &csText);
  void GetDlgItemText(UINT idItem, CString &csText);


public:
  HWND m_hWnd, m_hParent;

//  MSG m_msgLast;  // so I know what I'm dealing with


protected:
  UINT m_idDlg;

  static BOOL WINAPI DialogProc(HWND hDlg, UINT uiMsg, WPARAM wParam, LPARAM lParam);

};

#endif // _DIALOGHELPERS_H_INCLUDED_

