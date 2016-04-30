//////////////////////////////////////////////////////////////////////////////////
//                                                                              //
//   ____         _                   ____  _       ____   _            _       //
//  / ___|   ___ | |_  _   _  _ __   / ___|(_) ____|  _ \ | |  __ _    | |__    //
//  \___ \  / _ \| __|| | | || '_ \ | |  _ | ||_  /| | | || | / _` |   | '_ \   //
//   ___) ||  __/| |_ | |_| || |_) || |_| || | / / | |_| || || (_| | _ | | | |  //
//  |____/  \___| \__| \__,_|| .__/  \____||_|/___||____/ |_| \__, |(_)|_| |_|  //
//                           |_|                              |___/             //
//                                                                              //
//////////////////////////////////////////////////////////////////////////////////
//                                                                              //
//         Copyright (c) 1998-2016 by S.F.T. Inc. - All rights reserved         //
//    Use, copying, and distribution of this software are licensed according    //
//      to the GPLv2, LGPLv2, or BSD license, as appropriate (see COPYING)      //
//                                                                              //
//////////////////////////////////////////////////////////////////////////////////


#if !defined(AFX_SETUPGIZDLG_H__13778B89_0C34_11D2_A115_004033901FF3__INCLUDED_)
#define AFX_SETUPGIZDLG_H__13778B89_0C34_11D2_A115_004033901FF3__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// CSetupGizDlg dialog

#include "Page1.h"
#include "Page2.h"
#include "Page3.h"
#include "Page3A.h"
#include "Page3B.h"
#include "Page3Cert.h"
#include "Page4.h"

// put others here

#include "FinishPage.h"


// special structures for storing information

struct SetupFileInfo
{
  SetupFileInfo()
  {
    uiIconIndex = 0;
    dwOSFlag = 0;
    iDestPath = 0;
    iShow = SW_SHOWNORMAL;
    bAutoDelete = bExecute = bRegister = bRegDotNet = bOverwrite
                = bShared = bNoUninstall = bAddShortcut = FALSE;

/*    TRACE("SetupFileInfo - CONSTRUCTOR CALLED:  this=%08xH\r\n", (DWORD)this); */
  }
  
  ~SetupFileInfo()
  {
    /*TRACE("SetupFileInfo - DESTRUCTOR CALLED:  this=%08xH\r\n", (DWORD)this); */
  }

  CString csFileName;
  BOOL bAutoDelete,
       bExecute,
       bRegister,
       bRegDotNet,
       bOverwrite,
       bShared,
       bNoUninstall,
       bAddShortcut;

  int iDestPath;

  CString csGroupName;     // non-blank on 'bOptionInstall'
  CString csOtherPathName; // when 'iDestPath' is 'Other'

  CString csExecArgs, csUninstArgs;  // for exec after copy and uninstall

  DWORD dwOSFlag;          // '1' in appropriate bit includes that OS
                           // 1H --> Win 95
                           // 2H --> Win 98
                           // 4H --> Win NT 4.0
                           // 8H --> Win 2000
                           // 16H --> Win ME
                           // 32H --> Win XP
                           // 64H --> Win 2k3 Server
                           // 0x40000000 --> "newer than the above"
                           // a zero implies ALL OS's

  // shortcut info

  CString csShortcutFileName,
          csShortcutDescription,
          csStartPath,
          csArgs,
          csIconFile;

  UINT uiIconIndex;
  int iShow;

  void * operator new(size_t nSize, SetupFileInfo *pOb) { return(pOb); }  // special 'new' operator
  void operator delete(void *pOb, size_t s) { }
  void operator delete(void *pOb, SetupFileInfo *pOb2) { }
};

struct SetupGroupInfo
{
  SetupGroupInfo() { TRACE("SetupGroupInfo - CONSTRUCTOR CALLED:  this=%08xH\r\n", (DWORD)this);  }
  ~SetupGroupInfo() { TRACE("SetupGroupInfo - DESTRUCTOR CALLED:  this=%08xH\r\n", (DWORD)this); }

  CString csGroupName;
  CString csDesc;
  CString csLongDesc;

  void * operator new(size_t nSize, SetupGroupInfo *pOb) { return((LPVOID)pOb); }  // special 'new' operator
  void operator delete(void *pOb, size_t s) { }
  void operator delete(void *pOb, SetupGroupInfo *pOb2) { }
};

//template <> void AFXAPI ConstructElements < SetupFileInfo > ( SetupFileInfo* pNew, int nCount );
//template <> void AFXAPI DestructElements < SetupFileInfo > ( SetupFileInfo* pOld, int nCount );
//template <> void AFXAPI ConstructElements < SetupGroupInfo > ( SetupGroupInfo* pNew, int nCount );
//template <> void AFXAPI DestructElements < SetupGroupInfo > ( SetupGroupInfo* pOld, int nCount );

//template <> static void AFXAPI ConstructElements < SetupFileInfo > ( SetupFileInfo* pNew, int nCount )
//{
//    for ( int i = 0; i < nCount; i++, pNew++ )
//    {
//        // call SetupFileInfo default constructor directly
//        new(pNew) SetupFileInfo;
//    }
//}

//template <> static void AFXAPI DestructElements < SetupFileInfo > ( SetupFileInfo* pOld, int nCount )
//{
//    for ( int i = 0; i < nCount; i++, pOld++ )
//    {
//        // call SetupFileInfo default constructor directly
//        delete(pOld,pOld);  // call the destructor before killing the 'pointer'
//    }
//}

//template <> static void AFXAPI ConstructElements < SetupGroupInfo > ( SetupGroupInfo* pNew, int nCount )
//{
//    for ( int i = 0; i < nCount; i++, pNew++ )
//    {
//        // call SetupGroupInfo default constructor directly
//        new(pNew) SetupGroupInfo;
//    }
//}

//template <> static void AFXAPI DestructElements < SetupGroupInfo > ( SetupGroupInfo* pOld, int nCount )
//{
//    for ( int i = 0; i < nCount; i++, pOld++ )
//    {
//        // call SetupGroupInfo default constructor directly
//        delete(pOld,pOld);  // call the destructor before killing the 'pointer'
//    }
//}

class CSetupGizDlg : public CPropertySheet
{
// Construction
public:

  DECLARE_DYNAMIC(CSetupGizDlg);

	CSetupGizDlg(CWnd* pParent = NULL);	// standard constructor

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSetupGizDlg)
	public:
	virtual BOOL OnInitDialog();
	virtual void WinHelp(DWORD dwData, UINT nCmd = HELP_CONTEXT);
	//}}AFX_VIRTUAL


  CPage1  m_Page1;
  CPage2  m_Page2;
  CPage3  m_Page3;
  CPage3A m_Page3A;
  CPage3B m_Page3B;
  CPage3Cert m_Page3Cert;
  CPage4  m_Page4;

  // put other pages here

  CFinishPage m_Finish;

  CString m_csFileName;  // for open/save only

  void DoOpen();
  BOOL DoSave();  // rudimentary open/save capability

  void DoClear();  // re-initialization of all sheets

  void UpdatePage1Stuff(BOOL bLoadFromFile = FALSE);
    // this is called by 'page 1' on 'wizard next' and from this class
    // when I load from a GIZ file

  static CSetupGizDlg *GetPropertySheet(CPropertyPage *pPropPage);

  CArray<SetupFileInfo,SetupFileInfo> m_aFileList;
  CArray<SetupGroupInfo,SetupGroupInfo> m_aGroupList;

  CStringArray m_acsRegistry;  // registry entries in 'listbox' format
  CStringArray m_acsCertFileNameList;   // created by page 1 on exit
  CStringArray m_acsCertRepositoryList; // corresponds to cert file name list

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CSetupGizDlg)
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


void GenerateDefaultShortcutInfo(SetupFileInfo &sfi);


//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETUPGIZDLG_H__13778B89_0C34_11D2_A115_004033901FF3__INCLUDED_)
