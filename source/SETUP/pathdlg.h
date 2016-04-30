//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//                          _    _          _  _            _               //
//            _ __    __ _ | |_ | |__    __| || |  __ _    | |__            //
//           | '_ \  / _` || __|| '_ \  / _` || | / _` |   | '_ \           //
//           | |_) || (_| || |_ | | | || (_| || || (_| | _ | | | |          //
//           | .__/  \__,_| \__||_| |_| \__,_||_| \__, |(_)|_| |_|          //
//           |_|                                  |___/                     //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//       Copyright (c) 1998-2016 by S.F.T. Inc. - All rights reserved       //
//  Use, copying, and distribution of this software are licensed according  //
//    to the GPLv2, LGPLv2, or BSD license, as appropriate (see COPYING)    //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// CPathDialog dialog

//#include "DialogHelpers.h"

class CPathDialog : public CMyDialogBaseClass
{
// Construction
public:
	CPathDialog(HWND hParent = NULL);   // standard constructor

	enum { IDD = IDD_PATH_DIALOG };
	CString	m_csPathName;
	CString	m_csPathCaption;

  CString m_csTitle;

// Implementation
protected:

  void UpdateData(BOOL bSaveFlag);
  BOOL MessageHandler(UINT uiMsg, WPARAM wParam, LPARAM lParam);

	void OnOK();
	BOOL OnInitDialog();
	void OnBrowse();
};
