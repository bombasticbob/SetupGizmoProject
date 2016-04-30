///////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                   //
//  __        __ _             _                  _   _        _                             _       //
//  \ \      / /(_) _ __    __| |  ___ __      __| | | |  ___ | | _ __    ___  _ __  ___    | |__    //
//   \ \ /\ / / | || '_ \  / _` | / _ \\ \ /\ / /| |_| | / _ \| || '_ \  / _ \| '__|/ __|   | '_ \   //
//    \ V  V /  | || | | || (_| || (_) |\ V  V / |  _  ||  __/| || |_) ||  __/| |   \__ \ _ | | | |  //
//     \_/\_/   |_||_| |_| \__,_| \___/  \_/\_/  |_| |_| \___||_|| .__/  \___||_|   |___/(_)|_| |_|  //
//                                                               |_|                                 //
//                                                                                                   //
///////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                   //
//                      Copyright (c) 2016 by S.F.T. Inc. - All rights reserved                      //
//               Use, copying, and distribution of this software are licensed according              //
//                 to the GPLv2, LGPLv2, or BSD license, as appropriate (see COPYING)                //
//                                                                                                   //
///////////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef _WINDOWHELPERS_H_INCLUDED_
#define _WINDOWHELPERS_H_INCLUDED_

#pragma once

#include "ClassLib.h" // make sure

class CMasterWindowClassList /*: public CObject */
{
public:

  CMasterWindowClassList() { }
  ~CMasterWindowClassList()
  {
    int i1;
    for(i1=m_acsClasses.GetSize() - 1; i1 >= 0; i1--)
      UnregisterClass(m_acsClasses[i1], MyGetApp()->m_hInstance);
  }

  CString GetClassName(UINT uiStyle, HICON hIcon = NULL, HCURSOR hCursor = NULL,
                       HBRUSH hbrBackground = NULL, LPCSTR lpszMenuName = NULL,
                       LPCSTR csClassName = NULL);

  CString GetClassName(UINT uiStyle, int cbClsExtra, int cbWndExtra,
                       HICON hIcon, HICON hIconSm, HCURSOR hCursor,
                       HBRUSH hbrBackground, LPCSTR lpszMenuName,
                       LPCSTR csClassName = NULL);
    // this one creates a unique class name based on the entries and
    // registers it if needed.

  CStringArray m_acsClasses;  // a record of window classes I've registered

};

extern CMasterWindowClassList theMasterWindowClassList;

class CMySubClassWindow /* : public CObject */
{
public:
  CMySubClassWindow();
  ~CMySubClassWindow();

  void Attach(HWND hWnd);  // call once to attach object to window
  HWND Detach();           // call to detach; automatic on 'WM_NCDESTROY'

  virtual LRESULT MessageHandler(UINT uiMsg, WPARAM wParam, LPARAM lParam) = 0;

  LRESULT DefWindowProc(UINT uiMsg, WPARAM wParam, LPARAM lParam);
    // must call 'DefWindowProc()' if not handled

  // a few common inlines

  __forceinline operator HWND() const { return(m_hWnd); }

  __forceinline LRESULT SendMessage(UINT Msg, WPARAM wParam, LPARAM lParam) const { return(::SendMessage(m_hWnd, Msg, wParam, lParam)); }
  __forceinline void GetClientRect(CRect *pRct) const { ::GetClientRect(m_hWnd, (LPRECT)pRct); }
  __forceinline void SetWindowText(LPCSTR szText) { ::SetWindowText(m_hWnd, szText); }
  __forceinline BOOL InvalidateRect(CONST RECT *lpRect, BOOL bErase) const { return(::InvalidateRect(m_hWnd, lpRect, bErase)); }
  __forceinline BOOL UpdateWindow() const { return(::UpdateWindow(m_hWnd)); }
  __forceinline BOOL ShowWindow(int nCmdShow) const { return(::ShowWindow(m_hWnd, nCmdShow)); }
  __forceinline BOOL SetForegroundWindow() const { return(::SetForegroundWindow(m_hWnd)); }
  __forceinline BOOL DestroyWindow() const { return(::DestroyWindow(m_hWnd)); }
  __forceinline BOOL CloseWindow() const { return(::CloseWindow(m_hWnd)); }


  void GetWindowText(CString &csText);

  static CMySubClassWindow *FromHandle(HWND hWnd);

public:

  HWND m_hWnd;  // my window handle

private:

  void AddToObjectsArray();
  void InnerAddToObjectsArray();
  void RemoveFromObjectsArray();
  void InnerRemoveFromObjectsArray();

  static CArray<CMySubClassWindow *,CMySubClassWindow *> m_aObjects;
  static volatile LONG m_lSpinLock;

  static void SpinLock();
  static void SpinUnlock();

  WNDPROC m_pSuperWndProc;

protected:
  static LRESULT WINAPI WindowProc(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam);

  friend class CMasterWindowClassList;
};


class CMyWindowBaseClass : public CMySubClassWindow
{
public:

  CMyWindowBaseClass();
  ~CMyWindowBaseClass();

  // this one differs from the others in that I must create it

  BOOL CreateEx(DWORD dwExStyle = 0, LPCSTR lpszWindowClass = NULL,
                LPCSTR lpszWindowText = NULL, DWORD dwStyle = WS_OVERLAPPED,
                CRect &rctWnd = rectDefault,
                HWND hwndParent = NULL, HMENU hMenu = NULL,
                int nIDIcon = 101 /*IDR_MAINFRAME*/);

  static CRect rectDefault;

  void BeginWaitCursor() { ::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_WAIT))); }
  void EndWaitCursor() { ::SetCursor((HCURSOR)::GetClassLongPtr(m_hWnd, GCLP_HCURSOR)); }

protected:
  static LRESULT WINAPI WindowProc(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam);

  friend class CMasterWindowClassList;
};


// KNOWN control types

class CEdit : public CMySubClassWindow
{
public:

  CEdit() { }
  virtual ~CEdit() { }

  virtual LRESULT MessageHandler(UINT uiMsg, WPARAM wParam, LPARAM lParam);

};


class CButton : public CMySubClassWindow
{
public:
  CButton() { }
  virtual ~CButton() { }

  // for owner draw controls, uses this to paint it
  void LoadBitmap(UINT nID) { m_Bitmap.LoadBitmap(nID); SetBitmap(); }
  void LoadBitmap(LPCSTR szID) { m_Bitmap.LoadBitmap(szID); SetBitmap(); }
  void AttachBitmap(HBITMAP hBitmap) { m_Bitmap.Attach(hBitmap); SetBitmap(); }

  void Attach(HWND hWnd) { CMySubClassWindow::Attach(hWnd); if(m_Bitmap.GetGdiObjectHandle() != NULL) { SetBitmap(); } }

  const CBitmap * GetBitmap(void) { return &m_Bitmap; }

  virtual LRESULT MessageHandler(UINT uiMsg, WPARAM wParam, LPARAM lParam);

protected:
  CBitmap m_Bitmap; // loaded bitmap stored here

  void SetBitmap(void) { if(m_hWnd) { ::SendMessage(m_hWnd, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)(m_Bitmap.GetGdiObjectHandle())); } }
};

class CStatic : public CMySubClassWindow
{
public:
  CStatic() { }
  virtual ~CStatic() { }

  // for owner draw controls, uses this to paint background
  void LoadBitmap(UINT nID) { m_Bitmap.LoadBitmap(nID); }
  void LoadBitmap(LPCSTR szID) { m_Bitmap.LoadBitmap(szID); }
  void AttachBitmap(HBITMAP hBitmap) { m_Bitmap.Attach(hBitmap); }

  const CBitmap * GetBitmap(void) { return &m_Bitmap; }

  virtual LRESULT MessageHandler(UINT uiMsg, WPARAM wParam, LPARAM lParam);

protected:
  CBitmap m_Bitmap; // loaded bitmap stored here

};

// other 'helper' things

class CMyPaintDC : public CDC
{
public:
  CMyPaintDC(HWND hWnd); // constructor calls BeginPaint() and attaches DC
  ~CMyPaintDC();         // destructor calls 'EndPaint()'

  HWND m_hWnd;
  PAINTSTRUCT m_ps;
};

class CMyWaitCursor
{
public:
  __forceinline CMyWaitCursor()
  {
    m_hWait = ::LoadCursor(NULL, IDC_WAIT);
    m_hOldCursor = ::SetCursor(m_hWait);
  }
  __forceinline ~CMyWaitCursor()
  {
    ::SetCursor(m_hOldCursor);
  }

  __forceinline void Restore()
  {
    ::SetCursor(m_hWait);
  }

  HCURSOR m_hOldCursor, m_hWait;
};

#endif // _WINDOWHELPERS_H_INCLUDED_
