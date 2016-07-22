//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//    _                   _         ____   _                                //
//   | |     ___    __ _ (_) _ __  |  _ \ | |  __ _     ___  _ __   _ __    //
//   | |    / _ \  / _` || || '_ \ | | | || | / _` |   / __|| '_ \ | '_ \   //
//   | |___| (_) || (_| || || | | || |_| || || (_| | _| (__ | |_) || |_) |  //
//   |_____|\___/  \__, ||_||_| |_||____/ |_| \__, |(_)\___|| .__/ | .__/   //
//                 |___/                      |___/         |_|    |_|      //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//       Copyright (c) 1998-2016 by S.F.T. Inc. - All rights reserved       //
//  Use, copying, and distribution of this software are licensed according  //
//    to the GPLv2, LGPLv2, or BSD license, as appropriate (see COPYING)    //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "AutoRun.h"
#include "LoginDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLoginDlg dialog
#define _T(X) X /* temporary */

CLoginDlg::CLoginDlg(HWND hParent /*=NULL*/)
: CMyDialogBaseClass(CLoginDlg::IDD, hParent)
{
  //{{AFX_DATA_INIT(CLoginDlg)
  m_csUserName = _T("");
  m_csPassword = _T("");
  m_csDomain = _T("");
  //}}AFX_DATA_INIT
  m_hIcon = MyGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CLoginDlg::UpdateData(BOOL bSaveFlag)
{
//  CDialog::DoDataExchange(pDX);
//  //{{AFX_DATA_MAP(CLoginDlg)
//  DDX_Control(pDX, IDC_DOMAIN, m_wndDomain);
//  DDX_Text(pDX, IDC_USERNAME, m_csUserName);
//  DDX_Text(pDX, IDC_PASSWORD, m_csPassword);
//  DDX_Text(pDX, IDC_DOMAIN, m_csDomain);
//  //}}AFX_DATA_MAP
}

BOOL CLoginDlg::MessageHandler(UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
  switch(uiMsg)
  {
    case WM_PAINT:
      return OnPaint();

    case WM_INITDIALOG:
      return(OnInitDialog());

    case WM_QUERYDRAGICON:
      return((BOOL)OnQueryDragIcon());

    case WM_COMMAND:
      switch(LOWORD(wParam))
      {
        case IDOK:
          if(HIWORD(wParam) == BN_CLICKED)
          {
            OnOK();
            return TRUE;
          }
          break;

        case IDCANCEL:
          if(HIWORD(wParam) == BN_CLICKED)
          {
            OnCancel();
            return TRUE;
          }
          break;
      }

      break;
  }

  return FALSE;
}


//BEGIN_MESSAGE_MAP(CLoginDlg, CDialog)
//  //{{AFX_MSG_MAP(CLoginDlg)
//  ON_WM_PAINT()
//  ON_WM_QUERYDRAGICON()
//  //}}AFX_MSG_MAP
//END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLoginDlg message handlers

BOOL CLoginDlg::OnInitDialog()
{
  SetIcon(m_hIcon, TRUE);     // Set big icon
  SetIcon(m_hIcon, FALSE);    // Set small icon

  // assign a few things based on what I already know

  // the domain name will either be the machine name or the
  // current domain I'm logged into...

  DWORD dwLen, cb1;
  cb1 = GetEnvironmentVariable("USERDOMAIN",m_csDomain.GetBufferSetLength(dwLen = 32767), dwLen);

  if(cb1 == ERROR_ENVVAR_NOT_FOUND)
    cb1 = 0;

  m_csDomain.ReleaseBuffer(cb1);
  m_csDomain.ReleaseBuffer(-1);

  if(!m_csDomain.GetLength())
  {
    GetComputerName(m_csDomain.GetBufferSetLength(dwLen = MAX_PATH * 2), &dwLen);
    m_csDomain.ReleaseBuffer(dwLen);
    m_csDomain.ReleaseBuffer(-1);
  }


  UpdateData(0);

  return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

BOOL CLoginDlg::OnPaint() 
{
  if(IsIconic())
  {
    CMyPaintDC dc(m_hWnd); // device context for painting

    SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

    // Center icon in client rectangle
    int cxIcon = GetSystemMetrics(SM_CXICON);
    int cyIcon = GetSystemMetrics(SM_CYICON);
    CRect rect;
    GetClientRect(&rect);
    int x = (rect.Width() - cxIcon + 1) / 2;
    int y = (rect.Height() - cyIcon + 1) / 2;

    // Draw the icon
    dc.DrawIcon(x, y, m_hIcon);

    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

HCURSOR CLoginDlg::OnQueryDragIcon()
{
  return((HCURSOR) m_hIcon);
}

void CLoginDlg::OnCancel() 
{
  EndDialog(IDCANCEL);
}

void CLoginDlg::OnOK()
{
  EndDialog(IDOK);
}

