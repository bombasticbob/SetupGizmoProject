////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
//   ____                            _    ____          _    _                    _       //
//  |  _ \  ___  _ __    ___   __ _ | |_ | __ )  _   _ | |_ | |_  ___   _ __     | |__    //
//  | |_) |/ _ \| '_ \  / _ \ / _` || __||  _ \ | | | || __|| __|/ _ \ | '_ \    | '_ \   //
//  |  _ <|  __/| |_) ||  __/| (_| || |_ | |_) || |_| || |_ | |_| (_) || | | | _ | | | |  //
//  |_| \_\\___|| .__/  \___| \__,_| \__||____/  \__,_| \__| \__|\___/ |_| |_|(_)|_| |_|  //
//              |_|                                                                       //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
//              Copyright (c) 1998-2016 by S.F.T. Inc. - All rights reserved              //
//         Use, copying, and distribution of this software are licensed according         //
//           to the GPLv2, LGPLv2, or BSD license, as appropriate (see COPYING)           //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////


#if !defined(AFX_REPEATBUTTON_H__D016BB0B_3B53_4015_AC36_D14BF5B51E8C__INCLUDED_)
#define AFX_REPEATBUTTON_H__D016BB0B_3B53_4015_AC36_D14BF5B51E8C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RepeatButton.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRepeatButton window

class CRepeatButton : public CButton
{
// Construction
public:
	CRepeatButton();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRepeatButton)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CRepeatButton();

	// Generated message map functions
protected:
	//{{AFX_MSG(CRepeatButton)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REPEATBUTTON_H__D016BB0B_3B53_4015_AC36_D14BF5B51E8C__INCLUDED_)
