//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//             _           _          ____                    _             //
//            / \   _   _ | |_  ___  |  _ \  _   _  _ __     | |__          //
//           / _ \ | | | || __|/ _ \ | |_) || | | || '_ \    | '_ \         //
//          / ___ \| |_| || |_| (_) ||  _ < | |_| || | | | _ | | | |        //
//         /_/   \_\\__,_| \__|\___/ |_| \_\ \__,_||_| |_|(_)|_| |_|        //
//                                                                          //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//       Copyright (c) 1998-2016 by S.F.T. Inc. - All rights reserved       //
//  Use, copying, and distribution of this software are licensed according  //
//    to the GPLv2, LGPLv2, or BSD license, as appropriate (see COPYING)    //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////


#if !defined(AFX_AUTORUN_H__974C2DC5_275D_4AF2_AD77_25B4A4F4A6BA__INCLUDED_)
#define AFX_AUTORUN_H__974C2DC5_275D_4AF2_AD77_25B4A4F4A6BA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#ifndef __AFXWIN_H__
//	#error include 'stdafx.h' before including this file for PCH
//#endif

#include "resource.h"		// main symbols

// KEY_ALL_ACCESS works only if administrator is accessing HKLM
// KEY_POWERUSER_ACCESS works if not admin

#define KEY_POWERUSER_ACCESS (KEY_READ | KEY_WRITE)


/////////////////////////////////////////////////////////////////////////////
// CAutoRunApp:
// See AutoRun.cpp for the implementation of this class
//

class CAutoRunApp : public CMyApp /*CWinApp*/
{
public:
	CAutoRunApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAutoRunApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CAutoRunApp)
	//}}AFX_MSG
//	DECLARE_MESSAGE_MAP()
};

extern CAutoRunApp theApp;


CString GetRegistryString(HKEY hkeyRoot, LPCSTR szKeyName, LPCSTR szString);
CString GetBugoSoftNotYetVersion();
BOOL ShellExecApp(LPCSTR szApp, LPCSTR szParms);
BOOL FileExists(LPCSTR szFileName);


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AUTORUN_H__974C2DC5_275D_4AF2_AD77_25B4A4F4A6BA__INCLUDED_)
