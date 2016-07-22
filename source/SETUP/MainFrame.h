//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//   __  __         _         _____                                 _       //
//  |  \/  |  __ _ (_) _ __  |  ___|_ __  __ _  _ __ ___    ___    | |__    //
//  | |\/| | / _` || || '_ \ | |_  | '__|/ _` || '_ ` _ \  / _ \   | '_ \   //
//  | |  | || (_| || || | | ||  _| | |  | (_| || | | | | ||  __/ _ | | | |  //
//  |_|  |_| \__,_||_||_| |_||_|   |_|   \__,_||_| |_| |_| \___|(_)|_| |_|  //
//                                                                          //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//       Copyright (c) 1998-2016 by S.F.T. Inc. - All rights reserved       //
//  Use, copying, and distribution of this software are licensed according  //
//    to the GPLv2, LGPLv2, or BSD license, as appropriate (see COPYING)    //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CMainFrame frame

//#include "WindowHelpers.h"

class CMainFrame : public CMyWindowBaseClass //CFrameWnd
{
//protected:
//  DECLARE_DYNCREATE(CMainFrame)

public:
  CMainFrame();
  virtual ~CMainFrame();

// Attributes
public:

  CBitmap m_bmpPalmTree;    // 'palm tree' bitmap (shrink to fit)
  CBitmap m_bmpPromo;       // 'SFT Inc.' promo logo in corner
  CBitmap m_bmpBackground;  // background bitmap!  Stretch this to fit
  CFont   m_Font, m_Font2;  // display fonts

// Operations
public:

//// Overrides
//  // ClassWizard generated virtual function overrides
//  //{{AFX_VIRTUAL(CMainFrame)
  public:
  virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CMyWindowBaseClass* pParentWnd, UINT nID /*, CCreateContext* pContext = NULL*/);
  protected:
//  virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
//  //}}AFX_VIRTUAL

// Implementation
protected:

  virtual LRESULT MessageHandler(UINT uiMsg, WPARAM wParam, LPARAM lParam);



//  // Generated message map functions
//  //{{AFX_MSG(CMainFrame)
  BOOL OnEraseBkgnd(HDC hDC);
  int OnCreate(LPCREATESTRUCT lpCreateStruct);
  void OnPaint();
  void OnEndSession(BOOL bEnding);
  BOOL OnQueryEndSession();
//  //}}AFX_MSG
//  DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
