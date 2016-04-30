/////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                             //
//   _____        _                      _   _____  _                            _     _       //
//  | ____|__  __| |_  _ __  __ _   ___ | |_|_   _|| |__   _ __  ___   __ _   __| |   | |__    //
//  |  _|  \ \/ /| __|| '__|/ _` | / __|| __| | |  | '_ \ | '__|/ _ \ / _` | / _` |   | '_ \   //
//  | |___  >  < | |_ | |  | (_| || (__ | |_  | |  | | | || |  |  __/| (_| || (_| | _ | | | |  //
//  |_____|/_/\_\ \__||_|   \__,_| \___| \__| |_|  |_| |_||_|   \___| \__,_| \__,_|(_)|_| |_|  //
//                                                                                             //
//                                                                                             //
/////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                             //
//                Copyright (c) 1998-2016 by S.F.T. Inc. - All rights reserved                 //
//            Use, copying, and distribution of this software are licensed according           //
//              to the GPLv2, LGPLv2, or BSD license, as appropriate (see COPYING)             //
//                                                                                             //
/////////////////////////////////////////////////////////////////////////////////////////////////


#if !defined(AFX_EXTRACTTHREAD_H__B3134F01_0D1F_11D2_A115_004033901FF3__INCLUDED_)
#define AFX_EXTRACTTHREAD_H__B3134F01_0D1F_11D2_A115_004033901FF3__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ExtractThread.h : header file
//



/////////////////////////////////////////////////////////////////////////////
// CExtractThread thread

class CExtractThread : public CMyThread /* CWinThread */
{
//	DECLARE_DYNCREATE(CExtractThread)
protected:
	CExtractThread();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:
  static CExtractThread * BeginThread(void);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExtractThread)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CExtractThread();

	// Generated message map functions
	//{{AFX_MSG(CExtractThread)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

//	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EXTRACTTHREAD_H__B3134F01_0D1F_11D2_A115_004033901FF3__INCLUDED_)
