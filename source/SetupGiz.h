//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//          ____         _                   ____  _          _             //
//         / ___|   ___ | |_  _   _  _ __   / ___|(_) ____   | |__          //
//         \___ \  / _ \| __|| | | || '_ \ | |  _ | ||_  /   | '_ \         //
//          ___) ||  __/| |_ | |_| || |_) || |_| || | / /  _ | | | |        //
//         |____/  \___| \__| \__,_|| .__/  \____||_|/___|(_)|_| |_|        //
//                                  |_|                                     //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//       Copyright (c) 1998-2016 by S.F.T. Inc. - All rights reserved       //
//  Use, copying, and distribution of this software are licensed according  //
//    to the GPLv2, LGPLv2, or BSD license, as appropriate (see COPYING)    //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////


#if !defined(AFX_SETUPGIZ_H__13778B87_0C34_11D2_A115_004033901FF3__INCLUDED_)
#define AFX_SETUPGIZ_H__13778B87_0C34_11D2_A115_004033901FF3__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

// KEY_ALL_ACCESS works only if administrator is accessing HKLM
// KEY_POWERUSER_ACCESS works if not admin

#define KEY_POWERUSER_ACCESS (KEY_READ | KEY_WRITE)


/////////////////////////////////////////////////////////////////////////////
// CSetupGizApp:
// See SetupGiz.cpp for the implementation of this class
//

class CSetupGizApp : public CWinApp
{
public:
	CSetupGizApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSetupGizApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

  CString m_csAuthCode, m_csAuthCompany;

  HKEY GetRegistryKey(BOOL bReadOnly = FALSE);
     // returns HKLM key for this application's settings, default being a 'read only' version
     // settings are global for all users (HKLM)

  BOOL ValidateAuthCode(LPCSTR szAuthCode = NULL, LPCSTR szAuthCompany = NULL);

	//{{AFX_MSG(CSetupGizApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

extern CSetupGizApp theApp;

// utilities

CString MyGetLongPathName(LPCSTR szShortName);
LPCSTR GetBaseName(LPCSTR szFullName);
void InPlaceConvertMultStringToStringList(char *pData, unsigned int cbData);
void InPlaceEscapeQuotes(CString &csString);
CString GetCertFileFriendlyName(LPCSTR szCertFile);
CString GetCurrentDirectory(void);


// base-31 stuff (for auth code)

void CreateRadix(char szRadix[32]);
DWORD Base31(LPCSTR szSource, LPCSTR szRadix);
CString Base31(DWORD dwValue, LPCSTR szRadix);
CString AuthCodeEncrypt(LPCSTR szCompany, LPCSTR szRadix);


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETUPGIZ_H__13778B87_0C34_11D2_A115_004033901FF3__INCLUDED_)
