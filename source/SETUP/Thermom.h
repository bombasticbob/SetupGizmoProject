//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//      _____  _                                                  _         //
//     |_   _|| |__    ___  _ __  _ __ ___    ___   _ __ ___     | |__      //
//       | |  | '_ \  / _ \| '__|| '_ ` _ \  / _ \ | '_ ` _ \    | '_ \     //
//       | |  | | | ||  __/| |   | | | | | || (_) || | | | | | _ | | | |    //
//       |_|  |_| |_| \___||_|   |_| |_| |_| \___/ |_| |_| |_|(_)|_| |_|    //
//                                                                          //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//       Copyright (c) 1998-2016 by S.F.T. Inc. - All rights reserved       //
//  Use, copying, and distribution of this software are licensed according  //
//    to the GPLv2, LGPLv2, or BSD license, as appropriate (see COPYING)    //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////

#ifndef _THERMOMETER_H_INCLUDED_
#define _THERMOMETER_H_INCLUDED_

/////////////////////////////////////////////////////////////////////////////
// CThermometer window

//#include "DialogHelpers.h"

class CThermometer : public CMySubClassWindow
{
// Construction
public:
	CThermometer();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CThermometer)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CThermometer();

	// Generated message map functions
protected:
	//{{AFX_MSG(CThermometer)
	BOOL OnEraseBkgnd(HDC hDC);
	void OnPaint();
	//}}AFX_MSG

  LRESULT OnSetText(WPARAM wParam, LPARAM lParam);

  virtual LRESULT MessageHandler(UINT uiMsg, WPARAM wParam, LPARAM lParam);
};

/////////////////////////////////////////////////////////////////////////////

#endif // _THERMOMETER_H_INCLUDED_
