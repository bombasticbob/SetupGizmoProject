//////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                          //
//   ____         _    _      ____                                                          //
//  |  _ \  __ _ | |_ | |__  | __ )  _ __  ___ __      __ ___   ___     ___  _ __   _ __    //
//  | |_) |/ _` || __|| '_ \ |  _ \ | '__|/ _ \\ \ /\ / // __| / _ \   / __|| '_ \ | '_ \   //
//  |  __/| (_| || |_ | | | || |_) || |  | (_) |\ V  V / \__ \|  __/ _| (__ | |_) || |_) |  //
//  |_|    \__,_| \__||_| |_||____/ |_|   \___/  \_/\_/  |___/ \___|(_)\___|| .__/ | .__/   //
//                                                                          |_|    |_|      //
//                                                                                          //
//////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                          //
//               Copyright (c) 1998-2016 by S.F.T. Inc. - All rights reserved               //
//          Use, copying, and distribution of this software are licensed according          //
//            to the GPLv2, LGPLv2, or BSD license, as appropriate (see COPYING)            //
//                                                                                          //
//////////////////////////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "shellapi.h"
#include "shlobj.h"
#include "SETUP.h"
#include "PathBrowse.h"

//#ifdef _DEBUG
//#define new DEBUG_NEW
//#undef THIS_FILE
//static char THIS_FILE[] = __FILE__;
//#endif

/////////////////////////////////////////////////////////////////////////////
// CPathBrowse

//IMPLEMENT_DYNAMIC(CPathBrowse, CObject);

CPathBrowse::CPathBrowse(LPCTSTR lpszFileName, DWORD dwFlags,
                         HWND hParentWnd)
: CMyDialogBaseClass(0, hParentWnd)
{
  m_csPath = lpszFileName;
  m_csTitle = "Please select the desired folder";
  m_csRoot = "";
}


//BEGIN_MESSAGE_MAP(CPathBrowse, CWnd)
//  //{{AFX_MSG_MAP(CPathBrowse)
//    // NOTE - the ClassWizard will add and remove mapping macros here.
//  //}}AFX_MSG_MAP
//END_MESSAGE_MAP()


int CPathBrowse::DoModal(void)
{
  CString csTemp;

  LPMALLOC lpMalloc;
  if(SHGetMalloc(&lpMalloc) != NOERROR)
  {
    return(IDCANCEL);
  }

  // NOTE:  code 'stolen' from 'CFileDialog::DoModal()'

//  ASSERT_VALID(this);

  // ** NOTE:  following comment was part of MFC code **
  // WINBUG: This is a special case for the file open/save dialog,
  //  which sometimes pumps while it is coming up but before it has
  //  disabled the main window.
  HWND hWndFocus = ::GetFocus();
  BOOL bEnableParent = FALSE;

  // ** begin my code **

  BROWSEINFO bi;
  bi.hwndOwner = m_hParent;

//  AfxUnhookWindowCreate();

  if(bi.hwndOwner != NULL && ::IsWindowEnabled(bi.hwndOwner))
  {
    bEnableParent = TRUE;
    ::EnableWindow(bi.hwndOwner, FALSE);
  }

  bi.pidlRoot = NULL;


  if(m_csRoot.GetLength())
    m_csRoot = AdjustPathName2(m_csRoot);

  if(m_csRoot.GetLength())
  {
    // get desktop interface and get file IDL for current path

    LPSHELLFOLDER lpDesktop;
    if(SHGetDesktopFolder(&lpDesktop) == NOERROR)
    {
      DWORD cb1, dwAttr;
      LPWSTR lpw1 = (LPWSTR)csTemp.GetBufferSetLength(MAX_PATH * sizeof(WCHAR));

      MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, m_csRoot, -1,
                          lpw1, MAX_PATH);

      if(lpDesktop->ParseDisplayName(NULL, NULL, lpw1, &cb1,
                                    (ITEMIDLIST **)&bi.pidlRoot,
                                    &dwAttr)
        != NOERROR)
      {
        bi.pidlRoot = NULL;
      }

      csTemp.ReleaseBuffer(0);
      lpDesktop->Release();
    }
  }

  bi.pszDisplayName = csTemp.GetBufferSetLength(MAX_PATH);
  bi.lpszTitle = m_csTitle;
  bi.ulFlags = BIF_RETURNONLYFSDIRS; //  | BIF_EDITBOX | BIF_VALIDATE;
  bi.lpfn = PathBrowseCallback;  // TODO:  (COMMDLGPROC)_AfxCommDlgProc???
  bi.lParam = (LPARAM)this;
  bi.iImage = 0;

  // ** end my code **

//  _AFX_THREAD_STATE* pThreadState = AfxGetThreadState();
//  ASSERT(pThreadState->m_pAlternateWndInit == NULL);

////  pThreadState->m_pAlternateWndInit = this;
//  AfxHookWindowCreate(this);

  // ** begin my code **

  LPITEMIDLIST lpIDList = SHBrowseForFolder(&bi);

  // ** end my code **

//  if(lpIDList)
//  {
//    ASSERT(pThreadState->m_pAlternateWndInit == NULL);
//  }
//
//  pThreadState->m_pAlternateWndInit = NULL;

  if(bEnableParent)
    ::EnableWindow(bi.hwndOwner, TRUE);
  if(::IsWindow(hWndFocus))
    ::SetFocus(hWndFocus);

//  PostModal();

  // ** end of "stolen" code adaptation **


  csTemp.ReleaseBuffer(0);  // dont' need it

  if(!lpIDList)
  {
    lpMalloc->Release();
    return(IDCANCEL);
  }

  if(!SHGetPathFromIDList(lpIDList,
                          m_csPath.GetBufferSetLength(MAX_PATH * 2)))
  {
    m_csPath.ReleaseBuffer(0);
  }
  else
  {
    m_csPath.ReleaseBuffer(-1);
  }

  // additional cleanup

  lpMalloc->Free(lpIDList);
  lpMalloc->Release();

  if(m_csPath.GetLength())
    return(IDOK);
  else
    return(IDCANCEL);
}

BOOL CPathBrowse::MessageHandler(UINT, WPARAM, LPARAM)
{
  return FALSE;  // does nothing (just needs to be here)
}

int CALLBACK CPathBrowse::PathBrowseCallback(HWND hWnd, UINT uiMsg, 
                                             LPARAM lParam, LPARAM pData)
{
  CPathBrowse *pThis = (CPathBrowse *)pData;

  ASSERT(pThis);
//  ASSERT(pThis->IsKindOf(RUNTIME_CLASS(CPathBrowse)));

//  ASSERT(!pThis->m_hWnd || pThis->m_hWnd == hWnd ||
//         ::GetParent(hWnd) == pThis->m_hWnd ||
//         ::GetParent(::GetParent(hWnd)) == pThis->m_hWnd);

  if(uiMsg == BFFM_INITIALIZED)  // called after initialization
  {
    ::SendMessage(hWnd, BFFM_SETSELECTION, 1,
                  (LPARAM)(LPCSTR)pThis->m_csPath);
  }


  return(0);
}
