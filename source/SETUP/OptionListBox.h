/////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                         //
//    ___          _    _                _      _       _    ____                  _       //
//   / _ \  _ __  | |_ (_)  ___   _ __  | |    (_) ___ | |_ | __ )   ___ __  __   | |__    //
//  | | | || '_ \ | __|| | / _ \ | '_ \ | |    | |/ __|| __||  _ \  / _ \\ \/ /   | '_ \   //
//  | |_| || |_) || |_ | || (_) || | | || |___ | |\__ \| |_ | |_) || (_) |>  <  _ | | | |  //
//   \___/ | .__/  \__||_| \___/ |_| |_||_____||_||___/ \__||____/  \___//_/\_\(_)|_| |_|  //
//         |_|                                                                             //
//                                                                                         //
/////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                         //
//              Copyright (c) 1998-2016 by S.F.T. Inc. - All rights reserved               //
//          Use, copying, and distribution of this software are licensed according         //
//            to the GPLv2, LGPLv2, or BSD license, as appropriate (see COPYING)           //
//                                                                                         //
/////////////////////////////////////////////////////////////////////////////////////////////


#if !defined(AFX_OPTIONLISTBOX_H__512DA511_BC5D_4DE2_BCD2_2A0426DE2B9E__INCLUDED_)
#define AFX_OPTIONLISTBOX_H__512DA511_BC5D_4DE2_BCD2_2A0426DE2B9E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptionListBox.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COptionListBox window

//#include "DialogHelpers.h"

class COptionListBox : public CMySubClassWindow
{
// Construction
public:
	COptionListBox();

  virtual LRESULT MessageHandler(UINT uiMsg, WPARAM wParam, LPARAM lParam);

// Attributes
public:

// Operations
public:

  // required overrides to support functionality

  int GetCaretIndex() const
  { return((int)SendMessage(LB_GETCARETINDEX, 0, 0)); }

  int SetCaretIndex(int nIndex, BOOL bScroll = TRUE)
  { return((int)SendMessage(LB_SETCARETINDEX, (WPARAM)nIndex, (LPARAM)bScroll)); }

  int GetSel(int nIndex) const;
  int SetSel(int nIndex, BOOL bSelect = TRUE);

  int GetCurSel() const { return((int)SendMessage(LB_GETCURSEL, 0, 0)); }
  DWORD GetItemData(int iSel) const { return((DWORD)SendMessage(LB_GETITEMDATA, (WPARAM)iSel, 0)); }
  int SetItemData(int iSel, DWORD dwVal) { return((int)SendMessage(LB_SETITEMDATA, (WPARAM)iSel, (LPARAM)dwVal)); }

  int GetSelCount() const;
  int GetSelItems(int nMaxItems, LPINT rgIndex);

  int InsertString(int nIndex, LPCTSTR lpszItem);
  int AddString(LPCTSTR lpszItem);
  int DeleteString(UINT nIndex);

  int GetCount() const { return((int)SendMessage(LB_GETCOUNT, 0, 0)); }
  void GetText(int nIndex, CString &csText) const;

protected:


// Implementation
public:
	virtual ~COptionListBox();

	// Generated message map functions
protected:
	LRESULT OnLButtonDown(UINT nFlags, CPoint point);
	LRESULT OnLButtonUp(UINT nFlags, CPoint point);
	LRESULT OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	LRESULT OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTIONLISTBOX_H__512DA511_BC5D_4DE2_BCD2_2A0426DE2B9E__INCLUDED_)
