//////////////////////////////////////////////////////////////////////////////////
//                                                                              //
//      _           _          ____                 ____   _            _       //
//     / \   _   _ | |_  ___  |  _ \  _   _  _ __  |  _ \ | |  __ _    | |__    //
//    / _ \ | | | || __|/ _ \ | |_) || | | || '_ \ | | | || | / _` |   | '_ \   //
//   / ___ \| |_| || |_| (_) ||  _ < | |_| || | | || |_| || || (_| | _ | | | |  //
//  /_/   \_\\__,_| \__|\___/ |_| \_\ \__,_||_| |_||____/ |_| \__, |(_)|_| |_|  //
//                                                            |___/             //
//                                                                              //
//////////////////////////////////////////////////////////////////////////////////
//                                                                              //
//         Copyright (c) 1998-2016 by S.F.T. Inc. - All rights reserved         //
//    Use, copying, and distribution of this software are licensed according    //
//      to the GPLv2, LGPLv2, or BSD license, as appropriate (see COPYING)      //
//                                                                              //
//////////////////////////////////////////////////////////////////////////////////


#if !defined(AFX_AUTORUNDLG_H__78AB131B_0586_4A13_B1B6_90520438279F__INCLUDED_)
#define AFX_AUTORUNDLG_H__78AB131B_0586_4A13_B1B6_90520438279F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CAutoRunDlg dialog

class CAutoRunDlg : public CMyDialogBaseClass /*CDialog*/
{
// Construction
public:
	CAutoRunDlg(HWND hParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CAutoRunDlg)
  enum { IDD = IDD_AUTORUN_DIALOG };
  CStatic	m_wndBackground;
  CButton	m_wndExplore;
  CButton	m_wndInstall;
  CButton	m_wndExit;
  //}}AFX_DATA

  CBitmap m_bmpBackground, m_bmpInstall, m_bmpExplore, m_bmpExit;

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAutoRunDlg)
protected:
	void UpdateData(BOOL bSaveFlag);
  virtual BOOL MessageHandler(UINT, WPARAM, LPARAM);
	//}}AFX_VIRTUAL

  BOOL RunApplication(LPCSTR szCommand);


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CAutoRunDlg)
	virtual BOOL OnInitDialog();
	BOOL OnPaint();
	HCURSOR OnQueryDragIcon();
	virtual void OnCancel();
	void OnInstall();
	void OnExplore();
	//}}AFX_MSG
//	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AUTORUNDLG_H__78AB131B_0586_4A13_B1B6_90520438279F__INCLUDED_)
