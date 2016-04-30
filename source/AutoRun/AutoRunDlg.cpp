//////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                          //
//      _           _          ____                 ____   _                                //
//     / \   _   _ | |_  ___  |  _ \  _   _  _ __  |  _ \ | |  __ _     ___  _ __   _ __    //
//    / _ \ | | | || __|/ _ \ | |_) || | | || '_ \ | | | || | / _` |   / __|| '_ \ | '_ \   //
//   / ___ \| |_| || |_| (_) ||  _ < | |_| || | | || |_| || || (_| | _| (__ | |_) || |_) |  //
//  /_/   \_\\__,_| \__|\___/ |_| \_\ \__,_||_| |_||____/ |_| \__, |(_)\___|| .__/ | .__/   //
//                                                            |___/         |_|    |_|      //
//                                                                                          //
//////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                          //
//               Copyright (c) 1998-2016 by S.F.T. Inc. - All rights reserved               //
//          Use, copying, and distribution of this software are licensed according          //
//            to the GPLv2, LGPLv2, or BSD license, as appropriate (see COPYING)            //
//                                                                                          //
//////////////////////////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "AutoRun.h"
#include "AutoRunDlg.h"
#include "LoginDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAutoRunDlg dialog

CAutoRunDlg::CAutoRunDlg(HWND hParent /*=NULL*/)
: CMyDialogBaseClass(CAutoRunDlg::IDD, hParent)
{
	//{{AFX_DATA_INIT(CAutoRunDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_hIcon = MyGetApp()->LoadIcon(IDR_MAINFRAME);

  HBITMAP hbmpBK = (HBITMAP)::LoadImage(0, "AUTORUN.BMP", IMAGE_BITMAP, 0, 0,
                                        LR_LOADFROMFILE | LR_DEFAULTCOLOR);

  if(hbmpBK)
  {
//    m_bmpBackground.Attach(hbmpBK);
    m_wndBackground.AttachBitmap(hbmpBK);
  }
  else
  {
    DWORD dwErr = GetLastError();

//    m_bmpBackground.LoadBitmap(IDB_BKDEFAULT);
    m_wndBackground.LoadBitmapA(IDB_BKDEFAULT);
  }

//  m_bmpExit.LoadBitmap(IDB_EXIT);
//  m_bmpExplore.LoadBitmap(IDB_EXPLORE);
//  m_bmpInstall.LoadBitmap(IDB_INSTALL);

  m_wndExit.LoadBitmap(IDB_EXIT);
  m_wndExplore.LoadBitmap(IDB_EXPLORE);
  m_wndInstall.LoadBitmap(IDB_INSTALL);
}

void CAutoRunDlg::UpdateData(BOOL bSaveFlag)
{
//	CDialog::DoDataExchange(pDX);
//	//{{AFX_DATA_MAP(CAutoRunDlg)
//	DDX_Control(pDX, IDC_BACKGROUND, m_wndBackground);
//	DDX_Control(pDX, IDRETRY, m_wndExplore);
//	DDX_Control(pDX, IDOK, m_wndInstall);
//	DDX_Control(pDX, IDCANCEL, m_wndExit);
//	//}}AFX_DATA_MAP
}

BOOL CAutoRunDlg::MessageHandler(UINT uiMsg, WPARAM wParam, LPARAM lParam)
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
            OnInstall();
            return TRUE;
          }
          break;

        case IDRETRY:
          if(HIWORD(wParam) == BN_CLICKED)
          {
            OnExplore();
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


//BEGIN_MESSAGE_MAP(CAutoRunDlg, CDialog)
//	//{{AFX_MSG_MAP(CAutoRunDlg)
//	ON_WM_PAINT()
//	ON_WM_QUERYDRAGICON()
//	ON_BN_CLICKED(IDOK, OnInstall)
//	ON_BN_CLICKED(IDRETRY, OnExplore)
//	//}}AFX_MSG_MAP
//END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAutoRunDlg message handlers

BOOL CAutoRunDlg::OnInitDialog()
{
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

  m_wndExit.Attach(::GetDlgItem(m_hWnd, IDCANCEL));
  m_wndExplore.Attach(::GetDlgItem(m_hWnd, IDRETRY));
  m_wndInstall.Attach(::GetDlgItem(m_hWnd, IDOK));
  m_wndBackground.Attach(::GetDlgItem(m_hWnd, IDC_BACKGROUND));
//	DDX_Control(pDX, IDC_BACKGROUND, m_wndBackground);
//	DDX_Control(pDX, IDRETRY, m_wndExplore);
//	DDX_Control(pDX, IDOK, m_wndInstall);
//	DDX_Control(pDX, IDCANCEL, m_wndExit);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

BOOL CAutoRunDlg::OnPaint() 
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

HCURSOR CAutoRunDlg::OnQueryDragIcon()
{
	return((HCURSOR) m_hIcon);
}

void CAutoRunDlg::OnCancel() 
{
	EndDialog(IDCANCEL);
}

BOOL CAutoRunDlg::RunApplication(LPCSTR szCommand)
{
  HMODULE hAdvAPI32 = GetModuleHandle("ADVAPI32.DLL");
  HMODULE hKernel32 = GetModuleHandle("KERNEL32.DLL");

  LPVOID (WINAPI *pGetEnvironmentStringsW)(void) = NULL;
  BOOL (WINAPI *pCreateProcessWithLogonW)(LPCWSTR,LPCWSTR,LPCWSTR,DWORD,LPCWSTR,
                                          LPWSTR,DWORD,LPVOID,LPCWSTR,LPSTARTUPINFOW,
                                          LPPROCESS_INFORMATION) = NULL;

  if(hAdvAPI32)
    (FARPROC &)pCreateProcessWithLogonW = GetProcAddress(hAdvAPI32, "CreateProcessWithLogonW");

  if(hKernel32)
    (FARPROC &)pGetEnvironmentStringsW = GetProcAddress(hKernel32, "GetEnvironmentStringsW");


  if(!(GetVersion() & 0x80000000L))  // windows NT/2k/XP/2k3
  {
    // Check user access rights to run SETUP.  If I can't get write access to HKLM, 
    // the user has "insufficient privileges" and can't run SETUP.

    HKEY hkeyTest = NULL;

    if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE", 0, KEY_POWERUSER_ACCESS, &hkeyTest)
      != ERROR_SUCCESS
#ifdef _DEBUG
      || MyMessageBox("DEBUG:  pretend I have to log in?", MB_YESNO | MB_ICONQUESTION | MB_SETFOREGROUND)
          == IDYES
#endif // _DEBUG
      )
    {
#ifdef _DEBUG
      if(hkeyTest)
        RegCloseKey(hkeyTest);  // only for debug
#endif // _DEBUG

      if((GetVersion() & 0xff) <= 4 || !pCreateProcessWithLogonW)
      {
        MyMessageBox("ERROR:  You do not have sufficient privileges on this system to run "
                      "either the SETUP or UNINSTALL applications.\r\n"
                      "If you want to run SETUP, you must log in as a different user first.\r\n"
                      "This application will now close.\r\n",
                      MB_OK | MB_ICONEXCLAMATION | MB_SETFOREGROUND);
      }
      else
      {
        CLoginDlg dlgLogin;

        if(dlgLogin.DoModal() == IDOK)
        {
          CString csAppName, csCurDir;
          GetModuleFileName(NULL, csAppName.GetBufferSetLength(MAX_PATH * 2), MAX_PATH * 2);
          csAppName.ReleaseBuffer(-1);

          GetCurrentDirectory(MAX_PATH * 2, csCurDir.GetBufferSetLength(MAX_PATH * 2));
          csCurDir.ReleaseBuffer(-1);

//          csAppName = '"' + csAppName + '"';  // in case it's got spaces in the name

          // convert to wide char using 'BSTR'
          BSTR bstrUser = NULL, bstrPass = NULL, bstrDomain = NULL,
              bstrModule = NULL, bstrArgs = NULL, bstrCurDir = NULL;

          bstrUser = dlgLogin.m_csUserName.AllocSysString();
          bstrPass = dlgLogin.m_csPassword.AllocSysString();

          if(dlgLogin.m_csDomain.GetLength())
            bstrDomain = dlgLogin.m_csDomain.AllocSysString();

          bstrArgs = ((CString)szCommand).AllocSysString();
          bstrCurDir = csCurDir.AllocSysString();

          STARTUPINFOW si;
          memset(&si, 0, sizeof(si));
          si.cb = sizeof(si);

          PROCESS_INFORMATION pi;
          memset(&pi, 0, sizeof(pi));

          LPWSTR lpwEnv = pGetEnvironmentStringsW ? (LPWSTR)pGetEnvironmentStringsW()
                                                  : NULL;

          BOOL bSuccess = FALSE;

          if(pCreateProcessWithLogonW(bstrUser, bstrDomain, bstrPass,
                                     0, NULL, bstrArgs,
                                     (lpwEnv ? CREATE_NEW_PROCESS_GROUP | CREATE_UNICODE_ENVIRONMENT
                                             : CREATE_NEW_PROCESS_GROUP),
                                     lpwEnv, bstrCurDir, &si, &pi))
          {
            bSuccess = TRUE;

            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
          }
          else
          {
            DWORD dwErr = GetLastError();  // 1326 is 'logon failure'

            TRACE("ERROR in 'CreateProcessWithLogonW' - %ld (%08xH)\r\n", dwErr, dwErr);

            MyMessageBox("Unable to log in with the specifed user/password",
                          MB_OK | MB_ICONHAND | MB_SETFOREGROUND);
          }

          if(bstrUser)
            SysFreeString(bstrUser);
          if(bstrPass)
            SysFreeString(bstrPass);
          if(bstrDomain)
            SysFreeString(bstrDomain);
          if(bstrModule)
            SysFreeString(bstrModule);
          if(bstrArgs)
            SysFreeString(bstrArgs);
          if(bstrCurDir)
            SysFreeString(bstrCurDir);

          return(bSuccess);
        }
      }

      return(FALSE);
    }

    RegCloseKey(hkeyTest);  // now that I'm done with it...
  }

  return(WinExec(szCommand, SW_SHOWNORMAL) > 32);
}

void CAutoRunDlg::OnInstall() 
{
CMyWaitCursor wait;

  CString csAppPath, csTemp, csTemp2, csTemp3;
  int i1;

  GetModuleFileName(NULL, csAppPath.GetBufferSetLength(MAX_PATH), MAX_PATH);
  csAppPath.ReleaseBuffer(-1);

  if(!csAppPath.GetLength())
  {
    MessageBeep(0);
    return;
  }

  i1 = csAppPath.ReverseFind('\\');
  csAppPath = csAppPath.Left(i1 + 1);  // includes backslash

  if(theApp.m_lpCmdLine && *theApp.m_lpCmdLine)
  {
    if(RunApplication(theApp.m_lpCmdLine))
    {
      EndDialog(IDOK);
      return;
    }

    MyMessageBox("Unable to execute the specified command.  Your SETUP CD may be corrupt.\n"
                  "Please contact the software author to correct this problem.");
    return;
  }

  // BLANK COMMAND LINE - default behavior

  // STEP 1:  see if I'm doing "BugoSoft Not Yet"

  if(GetPrivateProfileString(".Net", "VERSION", "",
                             csTemp.GetBufferSetLength(MAX_PATH * 4),
                             MAX_PATH * 4, csAppPath + "autorun.inf")
     > 0)
  {
    csTemp.ReleaseBuffer(-1);

    if(csTemp.GetLength())
    {
      csTemp2 = GetBugoSoftNotYetVersion();

      if(!csTemp2.GetLength() || stricmp(csTemp, csTemp2) > 0)
      {
        if(csTemp2.GetLength())
        {
          csTemp2 = "  Currently you have version "
                  + csTemp2
                  + " installed.  SETUP can upgrade this to version "
                  + csTemp
                  + " at this time.  ";
        }
        else
        {
          csTemp2 = "  Currently Microsoft '.Net' Framework is not installed on your computer.  ";
        }

        csTemp2 +=  "One or more applications or components may not function or even install "
                    "properly without first installing the Microsoft '.Net' Framework.";

        if(GetPrivateProfileString(".Net", "SETUP", "",
                                   csTemp3.GetBufferSetLength(MAX_PATH * 4),
                                   MAX_PATH * 4, csAppPath + "autorun.inf")
          > 0)
        {
          csTemp3.ReleaseBuffer(-1);

          if(csTemp3.GetLength())
          {
            i1 = MyMessageBox("This application requires that version "
                               + csTemp
                               + " of the Microsoft '.Net' Framework be installed."
                               + csTemp2
                               + "\nTo install the Microsoft '.Net' Framework, press 'Yes'.  "
                                 "To skip installing the '.Net' Framework and proceed with SETUP, "
                                 "press 'No'.  To exit SETUP at this time, press 'Cancel'",
                                 MB_YESNOCANCEL | MB_ICONEXCLAMATION | MB_SETFOREGROUND);

            if(i1 == IDCANCEL)
            {
              EndDialog(IDCANCEL);
              return;
            }
            else if(i1 == IDYES)
            {
              // attempt to run the application

              if(!ShellExecApp(csAppPath + csTemp3, ""))  // no 'quiet' mode
              {
                EndDialog(IDCANCEL);
                return;
              }

              // TODO:  check for reboot ???
            }
          }
        }
      }
    }
  }
  else
  {
    csTemp.ReleaseBuffer(0);
  }


  // first, try '..\disk1\SETUP.EXE', and 'disk1\setup.exe' and '..\setup\setup.exe' and
  // 'setup\setup.exe'.  If these don't work, it's an error and I can't recover

  LPCSTR lpc1;
  if((FileExists(lpc1 = "..\\disk1\\SETUP.EXE") && RunApplication(lpc1)) ||
     (FileExists(lpc1 = "disk1\\SETUP.EXE") && RunApplication(lpc1)) ||
     (FileExists(lpc1 = "..\\setup\\SETUP.EXE") && RunApplication(lpc1)) ||
     (FileExists(lpc1 = "setup\\SETUP.EXE") && RunApplication(lpc1)) ||
     (FileExists(lpc1 = "..\\SETUP.EXE") && RunApplication(lpc1)) ||
     (FileExists(lpc1 = ".\\SETUP.EXE") && RunApplication(lpc1)))
  {
    // success, but don't do anything - this is easier than any other method
  }
  else
//  if(WinExec(csAppPath + "..\\disk1\\SETUP.EXE", SW_SHOWNORMAL) <= 32 &&
//     WinExec(csAppPath + "disk1\\SETUP.EXE", SW_SHOWNORMAL) <= 32 &&
//     WinExec(csAppPath + "..\\setup\\SETUP.EXE", SW_SHOWNORMAL) <= 32 &&
//     WinExec(csAppPath + "setup\\SETUP.EXE", SW_SHOWNORMAL) <= 32 &&
//     WinExec(csAppPath + ".\\SETUP.EXE", SW_SHOWNORMAL) <= 32)
  {
    MyMessageBox("Unable to locate 'SETUP.EXE'.  Your SETUP CD may be corrupt.\n"
                  "Please contact the software author to correct this problem.");
    return;
  }

  EndDialog(IDOK);  // assume it worked ok, and exit the front-end
  return;
}

void CAutoRunDlg::OnExplore() 
{
  CString csAppPath;
  GetModuleFileName(NULL, csAppPath.GetBufferSetLength(MAX_PATH), MAX_PATH);
  csAppPath.ReleaseBuffer(-1);

  if(!csAppPath.GetLength())
  {
    MessageBeep(0);
    return;
  }

  int i1 = csAppPath.ReverseFind('\\');

  if(i1 > 3)
    csAppPath = csAppPath.Left(i1);
  else
    csAppPath = csAppPath.Left(i1 + 1);

  ShellExecute(NULL, "explore", csAppPath, "", csAppPath, SW_SHOWNORMAL);

  EndDialog(IDCANCEL);
}
