////////////////////////////////////////////////////////////////////////////////////////
//                                                                                    //
//    ___          _    _                     ____   _                                //
//   / _ \  _ __  | |_ (_)  ___   _ __   ___ |  _ \ | |  __ _     ___  _ __   _ __    //
//  | | | || '_ \ | __|| | / _ \ | '_ \ / __|| | | || | / _` |   / __|| '_ \ | '_ \   //
//  | |_| || |_) || |_ | || (_) || | | |\__ \| |_| || || (_| | _| (__ | |_) || |_) |  //
//   \___/ | .__/  \__||_| \___/ |_| |_||___/|____/ |_| \__, |(_)\___|| .__/ | .__/   //
//         |_|                                          |___/         |_|    |_|      //
//                                                                                    //
////////////////////////////////////////////////////////////////////////////////////////
//                                                                                    //
//            Copyright (c) 1998-2016 by S.F.T. Inc. - All rights reserved            //
//       Use, copying, and distribution of this software are licensed according       //
//         to the GPLv2, LGPLv2, or BSD license, as appropriate (see COPYING)         //
//                                                                                    //
////////////////////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "setup.h"
#include "OptionsDlg.h"

//#ifdef _DEBUG
//#define new DEBUG_NEW
//#undef THIS_FILE
//static char THIS_FILE[] = __FILE__;
//#endif

/////////////////////////////////////////////////////////////////////////////
// COptionsDlg dialog

#define _T(X) X /* temporary */

COptionsDlg::COptionsDlg(CStringArray *pList, const CStringArray *pPreSel)
	: CMyDialogBaseClass(COptionsDlg::IDD, NULL)
{
	m_csOptionDesc = _T("");

  m_pList = pList;      // source for data, destination for results
  m_pPreSel = pPreSel;  // pre-selected options list
  m_dwHeight = 0;       // initial value - is fixed up when listbox is first drawn
}

void COptionsDlg::UpdateData(BOOL bSaveFlag)
{
  if(bSaveFlag)
  {
    GetDlgItemText(IDC_OPTION_DESC, m_csOptionDesc);
  }
  else
  {
    SetDlgItemText(IDC_OPTION_DESC, m_csOptionDesc);
  }
}

BOOL COptionsDlg::MessageHandler(UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
  WORD wCtrl = LOWORD(wParam);
  WORD wNotify = HIWORD(wParam);

  switch(uiMsg)
  {
    case WM_INITDIALOG:
      return(OnInitDialog());

    case WM_COMMAND:
      switch(wCtrl)
      {
        case IDOK:
          OnOK();
          return TRUE;

        case IDCANCEL:
          OnCancel();
          return TRUE;

        case IDC_OPTION_LIST:
          if(wNotify == LBN_SELCHANGE)
          {
            OnSelchangeOptionList();
            return TRUE;
          }

          break;
      }
      break;

    case WM_DRAWITEM:
      return(OnDrawItem((int)wParam, (LPDRAWITEMSTRUCT)lParam));

    case WM_MEASUREITEM:
      return(OnMeasureItem((int)wParam, (LPMEASUREITEMSTRUCT)lParam));
  }

  return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// COptionsDlg message handlers

void COptionsDlg::OnOK() 
{
  UpdateData(1);

  // remove anything that isn't "checked" from the string array passed in the constructor

  int i1;
  for(i1=m_pList->GetSize() - 1; i1 >= 0; i1--)
  {
    if(m_wndOptionList.GetSel(i1) <= 0)  // it's NOT selected, in other words
    {
      m_pList->RemoveAt(i1);
    }
  }

	EndDialog(IDOK);
}

void COptionsDlg::OnCancel() 
{
  if(m_pList)
    m_pList->RemoveAll();  // by convention, do this

	EndDialog(IDCANCEL);
}

void COptionsDlg::OnSelchangeOptionList() 
{
  UpdateData(1);

  // get the description for the currently selected item

  int iCurSel = m_wndOptionList.GetCaretIndex();

  if(iCurSel != LB_ERR && iCurSel < m_acsDesc.GetSize())
  {
    m_csOptionDesc = m_acsDesc[iCurSel];
  }
  else
  {
    m_csOptionDesc = "";
  }

  UpdateData(0);
}

BOOL COptionsDlg::OnInitDialog() 
{
  m_wndOptionList.Attach(GetDlgItem(IDC_OPTION_LIST));

	UpdateData(0);
	
  int i1, i2;
  CStringArray acsTemp;

  for(i1=0; i1 < m_pList->GetSize(); i1++)
  {
    DoParseString((*m_pList)[i1], acsTemp);
    m_acsDesc.SetAtGrow(i1, acsTemp[2]);  // the long description
    
    m_wndOptionList.InsertString(i1, acsTemp[1]);  // the short description

    // if I find it in 'm_pPreSel' set it as "selected"

    for(i2=0; m_pPreSel && i2 < (*m_pPreSel).GetSize(); i2++)
    {
      if(!((*m_pPreSel)[i2].CompareNoCase(acsTemp[0])))
      {
        m_wndOptionList.SetSel(i1, TRUE);
        break;
      }
    }
  }

  if(m_wndOptionList.GetCount() > 0)  // set up 'caret' on 1st item when possible
  {
    m_wndOptionList.SetCaretIndex(0);
    OnSelchangeOptionList();
  }
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL COptionsDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
  if(nIDCtl != IDC_OPTION_LIST)
  {
    return FALSE;  // NOT handled
  }

  // do it simple.  If selected, check the box.  If not selected, don't check the box.
  // if highlighted, draw a focus rectangle around it.

  CDC dc(lpDrawItemStruct->hDC); // make sure I Detach() it
  dc.SaveDC();

  HFONT hFont = (HFONT)SendMessage(WM_GETFONT, 0, 0);
  if(hFont)
  {
    CFont font(hFont);

    dc.SelectObject(&font);

    font.Detach();
  }

  dc.SetBkColor(RGB(255,255,255));
  dc.SetTextColor(RGB(0,0,0));  // always black on white

  BOOL bChecked = lpDrawItemStruct->itemID >= 0
                ? m_wndOptionList.GetSel(lpDrawItemStruct->itemID)
                : 0;  // lpDrawItemStruct->itemState & ODS_SELECTED;

  BOOL bHasFocus = lpDrawItemStruct->itemState & ODS_FOCUS;

  CRect rcItem = lpDrawItemStruct->rcItem;
  CPen penB, penG1, penG2;
  CBrush brushBK;

  brushBK.CreateSolidBrush(RGB(255,255,255));  // always WHITE background brush

  dc.FillRect(&rcItem, &brushBK);

  penB.CreatePen(PS_SOLID, 1, RGB(0,0,0));         // black pen
  penG1.CreatePen(PS_SOLID, 1, RGB(192,192,192));  // light grey pen
  penG2.CreatePen(PS_SOLID, 1, RGB(128,128,128));  // dark grey pen

  // draw a checkbox, vertically centered

  CRect rect1 = rcItem;  // make the checkbox 2/3 of the character height
  int iBoxSize = ((rcItem.bottom - rcItem.top) * 2 / 3);

  rect1.left += 2;  // always 2 pixels
  rect1.right = rect1.left + iBoxSize;
  rect1.top = ((rect1.bottom + rect1.top) - iBoxSize) / 2;
  rect1.bottom = rect1.top + iBoxSize;

  dc.SelectObject(&penB);  // border
  dc.MoveTo(rect1.left, rect1.top);
  dc.LineTo(rect1.right - 1, rect1.top);
  dc.LineTo(rect1.right - 1, rect1.bottom - 1);
  dc.LineTo(rect1.left, rect1.bottom - 1);
  dc.LineTo(rect1.left, rect1.top);

  dc.SelectObject(&penG1); // light shadow
  dc.MoveTo(rect1.right - 2, rect1.top + 1);
  dc.LineTo(rect1.right - 2, rect1.bottom - 2);
  dc.LineTo(rect1.left, rect1.bottom - 2);

  dc.SelectObject(&penG2); // dark shadow
  dc.MoveTo(rect1.left + 1, rect1.bottom - 2);
  dc.MoveTo(rect1.left + 1, rect1.top + 1);
  dc.MoveTo(rect1.right - 1, rect1.top + 1);

  if(bChecked)
  {
    // draw the 'x' in the middle of the box

    dc.SelectObject(&penB);

    dc.MoveTo(rect1.left + 2, rect1.top + 2);
    dc.LineTo(rect1.right - 3, rect1.bottom - 3);
    dc.MoveTo(rect1.left + 2, rect1.top + 3);
    dc.LineTo(rect1.right - 2, rect1.bottom - 2);

    dc.MoveTo(rect1.right - 3, rect1.top + 2);
    dc.LineTo(rect1.left + 1, rect1.bottom - 3);
    dc.MoveTo(rect1.right - 3, rect1.top + 3);
    dc.LineTo(rect1.left + 1, rect1.bottom - 2);
  }

  // now draw the text

  rcItem.left += 8 + iBoxSize;  // box size + 8 pixels (always)

  CString csText;
  m_wndOptionList.GetText(lpDrawItemStruct->itemID, csText);

  dc.DrawText(csText, &rcItem, DT_LEFT | DT_VCENTER | DT_NOPREFIX | DT_SINGLELINE);

  // finally, a focus rectangle (when needed)

  if(bHasFocus)
  {
    dc.DrawFocusRect(&lpDrawItemStruct->rcItem);
  }

  dc.RestoreDC(-1);
  dc.Detach();

  return TRUE;  // handled
}

BOOL COptionsDlg::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct) 
{
  if(nIDCtl != IDC_OPTION_LIST)
  {
    return FALSE;
  }

  if(!m_dwHeight)
  {
    // note:  this will be called *BEFORE* anything draws...

    m_dwHeight = HIWORD(GetDialogBaseUnits());

	  // get the default dialog box's font
    HFONT hFont = (HFONT)SendMessage(WM_GETFONT, 0, 0);
    if(hFont)
    {
      TEXTMETRIC tm;

      CDC dc(::GetDC(m_hWnd));
      CFont font(hFont);

      dc.SaveDC();
      dc.SelectObject(&font);
      dc.GetTextMetrics(&tm);
      dc.RestoreDC(-1);

      ::ReleaseDC(m_hWnd, dc.Detach());
      font.Detach(); // also detach here, don't want to delete the GDI object

      m_dwHeight = tm.tmHeight;
    }

    m_dwHeight += 2; // 2 pixels for spacing
  }

  lpMeasureItemStruct->itemHeight = m_dwHeight;  // other parameters don't matter
  return TRUE;  // handled
}
