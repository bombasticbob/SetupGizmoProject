//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//           ____                      _  _                                 //
//          |  _ \  __ _   __ _   ___ | || |     ___  _ __   _ __           //
//          | |_) |/ _` | / _` | / _ \| || |_   / __|| '_ \ | '_ \          //
//          |  __/| (_| || (_| ||  __/|__   _|_| (__ | |_) || |_) |         //
//          |_|    \__,_| \__, | \___|   |_| (_)\___|| .__/ | .__/          //
//                        |___/                      |_|    |_|             //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//       Copyright (c) 1998-2016 by S.F.T. Inc. - All rights reserved       //
//  Use, copying, and distribution of this software are licensed according  //
//    to the GPLv2, LGPLv2, or BSD license, as appropriate (see COPYING)    //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "setupgiz.h"
#include "Page4.h"
#include "SetupGizDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPage4 property page

IMPLEMENT_DYNCREATE(CPage4, CPropertyPage)

CPage4::CPage4() : CPropertyPage(CPage4::IDD)
{
  //{{AFX_DATA_INIT(CPage4)
  m_csValue = _T("");
  m_csString = _T("");
  m_csKey = _T("");
  //}}AFX_DATA_INIT

  DoClear();
}

CPage4::~CPage4()
{
}

void CPage4::DoClear()
{
  m_csValue = _T("");
  m_csString = _T("");
  m_csKey = _T("");

  m_iCurSel = -1;
}

void CPage4::DoDataExchange(CDataExchange* pDX)
{
  CPropertyPage::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CPage4)
  DDX_Control(pDX, IDC_REVERT, m_btnRevert);
  DDX_Control(pDX, IDC_VALUE, m_wndValue);
  DDX_Control(pDX, IDC_STRING, m_wndString);
  DDX_Control(pDX, IDC_KEY, m_wndKey);
  DDX_Control(pDX, IDC_ENTRY_LIST, m_wndEntryList);
  DDX_Text(pDX, IDC_VALUE, m_csValue);
  DDX_Text(pDX, IDC_STRING, m_csString);
  DDX_Text(pDX, IDC_KEY, m_csKey);
  //}}AFX_DATA_MAP

  if(pDX->m_bSaveAndValidate)
  {
    m_iCurSel = m_wndEntryList.GetCurSel();
  }
  else
  {
    m_wndEntryList.SetCurSel(m_iCurSel);
  }
}


BEGIN_MESSAGE_MAP(CPage4, CPropertyPage)
  //{{AFX_MSG_MAP(CPage4)
  ON_BN_CLICKED(IDC_ADD_NEW, OnAddNew)
  ON_LBN_SELCHANGE(IDC_ENTRY_LIST, OnSelchangeEntryList)
  ON_BN_CLICKED(IDC_MINI_HELP, OnMiniHelp)
  ON_BN_CLICKED(IDC_REMOVE, OnRemove)
  ON_BN_CLICKED(IDC_REVERT, OnRevert)
  ON_EN_CHANGE(IDC_KEY, OnChangeKey)
  ON_EN_CHANGE(IDC_STRING, OnChangeString)
  ON_EN_CHANGE(IDC_VALUE, OnChangeValue)
  ON_BN_CLICKED(IDC_KEY_HELP, OnKeyHelp)
  ON_BN_CLICKED(IDC_ADD_STANDARD, OnAddStandard)
  ON_BN_CLICKED(IDC_IMPORT_REG, OnImportReg)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPage4 message handlers

BOOL CPage4::OnSetActive() 
{
  UpdateData(0);

  CSetupGizDlg *pSheet = CSetupGizDlg::GetPropertySheet(this);

  if(pSheet)
  {
    pSheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);
  }

  // ensure listbox is properly loaded

  m_wndEntryList.ResetContent();

  CDC *pDC = m_wndEntryList.GetDC();
  pDC->SaveDC();

  HFONT hFont = (HFONT)m_wndEntryList.SendMessage(WM_GETFONT);
  if(hFont)
    pDC->SelectObject(CFont::FromHandle(hFont));

  int i1, iMaxWidth=0;
  for(i1=0; i1 < pSheet->m_acsRegistry.GetSize(); i1++)
  {
    m_wndEntryList.InsertString(i1, pSheet->m_acsRegistry[i1]);

    // for horizontal scrolling I need the width...

    CSize siz1 = pDC->GetTextExtent(pSheet->m_acsRegistry[i1]);
    iMaxWidth = max(iMaxWidth, siz1.cx);
  }

  pDC->RestoreDC(-1);
  m_wndEntryList.ReleaseDC(pDC);

  m_wndEntryList.SetHorizontalExtent(iMaxWidth + 32);

  UpdateData(0);

  OnSelchangeEntryList();

  return CPropertyPage::OnSetActive();
}

BOOL CPage4::OnKillActive() 
{
  OnSelchangeEntryList();

  CSetupGizDlg *pSheet = CSetupGizDlg::GetPropertySheet(this);

  if(pSheet)
  {
    pSheet->m_acsRegistry.RemoveAll();

    int i1;
    CString csTemp;
    for(i1=0; i1 < m_wndEntryList.GetCount(); i1++)
    {
      m_wndEntryList.GetText(i1, csTemp);

      if(csTemp.GetLength() && csTemp[0] == '[')
        pSheet->m_acsRegistry.Add(csTemp);
    }
  }
  
  return CPropertyPage::OnKillActive();
}

LRESULT CPage4::OnWizardBack() 
{
  // if there are no groups, go to 'Page3'; else, 'Page3A'

  CSetupGizDlg *pSheet = CSetupGizDlg::GetPropertySheet(this);

  CPropertyPage::OnWizardBack();

  if(pSheet)
  {
    int i1;

    // check for '3 cert' first
    if(pSheet->m_acsCertFileNameList.GetSize())
    {
      return IDD_PAGE_3_CERT; // 3 Cert - certificates
    }

    // check for 'add shortcut' (page 3B)
    for(i1=0; i1 < pSheet->m_aFileList.GetSize(); i1++)
    {
      if(pSheet->m_aFileList[i1].bAddShortcut)
      {
        return(IDD_PAGE_3B);
      }
    }

    // check for 3A

    if(pSheet->m_aGroupList.GetSize())
    {
      return(IDD_PAGE_3A);  // go to 'page 3' and not 'page 3a'
    }
  }
  
  return IDD_PAGE_3; // where I go when I can't determine where else to go
}

LRESULT CPage4::OnWizardNext() 
{
  // TODO: Add your specialized code here and/or call the base class
  
  return CPropertyPage::OnWizardNext();
}

void CPage4::OnSelchangeEntryList() 
{
  int i1, i2, iOldSel = m_iCurSel;

  UpdateData(1);

  CString csTemp;

  if(iOldSel >= 0 && iOldSel < m_wndEntryList.GetCount())
  {
    // load values into here if key is NOT BLANK; else, delete it
    // NOTE:  quotes need to be doubled

    if(m_csKey.Find('[')>=0 || m_csKey.Find(']')>=0)
    {
      AfxMessageBox("Invalid character in key - entry not updated!",
                    MB_OK | MB_ICONHAND | MB_SETFOREGROUND);
      m_wndEntryList.SetCurSel(iOldSel);

      PostMessage(WM_NEXTDLGCTL, (WPARAM)m_wndKey.m_hWnd, 1);

      return;
    }

    m_csKey.TrimRight();
    m_csKey.TrimLeft();
    m_csString.TrimRight();
    m_csString.TrimLeft();
    m_csValue.TrimRight();
    m_csValue.TrimLeft();

    if(m_csKey.GetLength())
    {
      CString csString, csValue;

      // double any embedded quote marks
      for(i1 = 0; i1 < m_csValue.GetLength(); i1++)
      {
        if(m_csValue[i1] == '\"')
        {
          csValue += "\"\"";
        }
        else
        {
          csValue += m_csValue[i1];
        }
      }

      for(i1 = 0; i1 < m_csString.GetLength(); i1++)
      {
        if(m_csString[i1] == '\"')
        {
          csString += "\"\"";
        }
        else
        {
          csString += m_csString[i1];
        }
      }

      if(m_csString.GetLength())
      {
        csTemp = "[" 
               + m_csKey 
               + "]\""
               + csString
               + "\"=\""
               + csValue
               + "\"";
      }
      else
      {
        csTemp = "[" 
               + m_csKey 
               + "]@=\""
               + csValue
               + "\"";
      }

      m_wndEntryList.DeleteString(iOldSel);
      m_wndEntryList.InsertString(iOldSel, csTemp);

      m_wndEntryList.SetCurSel(m_iCurSel);

      // *****************************************************
      // update text extent to support horizontal scrolling...
      // *****************************************************

      CDC *pDC = m_wndEntryList.GetDC();
      pDC->SaveDC();

      HFONT hFont = (HFONT)m_wndEntryList.SendMessage(WM_GETFONT);
      if(hFont)
        pDC->SelectObject(CFont::FromHandle(hFont));

      int iMaxWidth=0;
      for(i1=0; i1 < m_wndEntryList.GetCount(); i1++)
      {
        m_wndEntryList.GetText(i1, csTemp);

        // for horizontal scrolling I need the width...

        CSize siz1 = pDC->GetTextExtent(csTemp);
        iMaxWidth = max(iMaxWidth, siz1.cx);
      }

      pDC->RestoreDC(-1);
      m_wndEntryList.ReleaseDC(pDC);

      m_wndEntryList.SetHorizontalExtent(iMaxWidth + 32);
    }
    else
    {
      if(iOldSel != m_iCurSel)
      {
        m_wndEntryList.DeleteString(iOldSel);
        
        m_iCurSel = m_wndEntryList.GetCurSel();
        iOldSel = -1;

        if(m_iCurSel == iOldSel)
          iOldSel = 0;
      }
    }
  }

  if(iOldSel == m_iCurSel)
    return;  // I am done now


  if(m_iCurSel >= 0 && m_iCurSel < m_wndEntryList.GetCount())
  {
    m_wndEntryList.GetText(m_iCurSel, csTemp);

    // parse it - very strict

    if(csTemp[0] != '[')  // assume "new entry"
    {
      m_csKey = "";
      m_csString = "";
      m_csValue = "";
    }
    else
    {
      i1 = csTemp.Find(']');

      m_csKey = csTemp.Mid(1, i1 - 1);
      csTemp = csTemp.Mid(i1 + 1);

      // parse through quote marks to find '='

      if(csTemp[0] == '\"')  // most likely
      {
        for(i1=1; i1 < csTemp.GetLength(); i1++)
        {
          if(csTemp[i1] == '\"' &&
             ((i1 + 1) >= csTemp.GetLength() ||
              csTemp[i1 + 1] != '\"'))
          {
            break;  // found the ending quote mark
          }
        }

        m_csString = csTemp.Mid(1, i1 - 1);

        // un-double quote marks

        for(i2 = 0; (i2 + 1) < m_csString.GetLength(); i2++)
        {
          if(m_csString[i2] == '\"' &&
             m_csString[i2 + 1] == '\"')
          {
            m_csString = m_csString.Left(i2)
                       + m_csString.Mid(i2 + 1);
          }
        }

        if(i1 < csTemp.GetLength())
          i1++;
      }
      else
      {
        i1 = csTemp.Find('=');

        m_csString = csTemp.Left(i1);
        m_csString.TrimLeft();
        m_csString.TrimRight();

        if(m_csString[0] == '@')
        {
          m_csString = "";
        }
      }

      if(i1 < csTemp.GetLength() &&
         csTemp[i1] == '=')  // only if it's working properly
      {
        csTemp = csTemp.Mid(i1 + 1);
        csTemp.TrimRight();
        csTemp.TrimLeft();  // just in case

        // strip outer quotes off (if any)

        if(csTemp[0] == '\"' && csTemp[csTemp.GetLength()-1] == '\"')
        {
          m_csValue = csTemp.Mid(1, csTemp.GetLength() - 2);

          // un-double quote marks

          for(i2 = 0; (i2 + 1) < m_csValue.GetLength(); i2++)
          {
            if(m_csValue[i2] == '\"' &&
               m_csValue[i2 + 1] == '\"')
            {
              m_csValue = m_csValue.Left(i2)
                         + m_csValue.Mid(i2 + 1);
            }
          }
        }
        else
        {
          m_csValue = csTemp;  // just because
        }
      }
      else
      {
        m_csValue = "";  // blank value otherwise
      }
    }
  }
  else
  {
    m_csKey = "";
    m_csString = "";
    m_csValue = "";
  }

  UpdateData(0);
}

void CPage4::OnAddNew() 
{
  m_wndEntryList.InsertString(-1, "{New Entry}");
  m_wndEntryList.SetCurSel(m_wndEntryList.GetCount() - 1);

  OnSelchangeEntryList();

  PostMessage(WM_NEXTDLGCTL, (WPARAM)m_wndKey.m_hWnd, 1);
}

void CPage4::OnRemove() 
{
  int iCurSel = m_wndEntryList.GetCurSel();

  if(iCurSel < 0)
  {
    MessageBeep(0);
    return;
  }

  m_wndEntryList.DeleteString(iCurSel);

  m_csKey = "";
  m_csString = "";
  m_csValue = "";

  m_iCurSel = -1;

  UpdateData(0);

  if(iCurSel < m_wndEntryList.GetCount())
    m_wndEntryList.SetCurSel(iCurSel);
  else
    m_wndEntryList.SetCurSel(iCurSel - 1);

  OnSelchangeEntryList();  // that'll fix it...

  PostMessage(WM_NEXTDLGCTL, (WPARAM)m_wndEntryList.m_hWnd, 1);
}

void CPage4::OnRevert() 
{
  m_iCurSel = -1;

  OnSelchangeEntryList();  // that'll fix it...

  PostMessage(WM_NEXTDLGCTL, (WPARAM)m_wndKey.m_hWnd, 1);
}

void CPage4::OnChangeKey() 
{
  // TODO: If this is a RICHEDIT control, the control will not
  // send this notification unless you override the CPropertyPage::OnInitDialog()
  // function to send the EM_SETEVENTMASK message to the control
  // with the ENM_CHANGE flag ORed into the lParam mask.
  
  // TODO: Add your control notification handler code here
  
}

void CPage4::OnChangeString() 
{
  // TODO: If this is a RICHEDIT control, the control will not
  // send this notification unless you override the CPropertyPage::OnInitDialog()
  // function to send the EM_SETEVENTMASK message to the control
  // with the ENM_CHANGE flag ORed into the lParam mask.
  
  // TODO: Add your control notification handler code here
  
}

void CPage4::OnChangeValue() 
{
  // TODO: If this is a RICHEDIT control, the control will not
  // send this notification unless you override the CPropertyPage::OnInitDialog()
  // function to send the EM_SETEVENTMASK message to the control
  // with the ENM_CHANGE flag ORed into the lParam mask.
  
  // TODO: Add your control notification handler code here
  
}

void CPage4::OnKeyHelp() 
{
  AfxMessageBox("The following root key 'shorthand' values are valid:\r\n"
                "HKCR -  HKEY_CLASSES_ROOT\r\n"
                "HKCU -  HKEY_CURRENT_USER\r\n"
                "HKLM -  HKEY_LOCAL_MACHINE\r\n"
                "HKU  -  HKEY_USERS\r\n"
                "HKR  -  'relative' key {reserved}\r\n"
                "Format is {root key}\\key1\\key2\\key3\\...\\keyN\r\n"
                "BLANK value deletes string; blank string name and value deletes key",
                MB_OK | MB_ICONINFORMATION | MB_SETFOREGROUND);
}

void CPage4::OnMiniHelp() 
{
  AfxMessageBox("The following string substitions are valid at install time:\r\n"
                "%windir%\t\tWINDOWS directory\r\n"
                "%sysdir%\t\tSYSTEM directory\r\n"
                "%apppath%\tApplication (root) path\r\n"
                "%tmpdir%\t\tTemporary Files Directory\r\n"
                "%themedir%\t\tThe directory where 'Themes' are located\r\n"
                "%appcommon%\tApplication 'Common' files\r\n"
                "%%\t\tdoubled '%' indicates a literal '%'\r\n"
                "'apppath' and 'appcommon' will be prompted for when used",
                MB_OK | MB_ICONINFORMATION | MB_SETFOREGROUND);
}


void CPage4::OnAddStandard() 
{
  m_wndEntryList.SetCurSel(-1);  // select nothing
  OnSelchangeEntryList();        // updates everything correctly

  // standard entries:

  // HKLM\SOFTWARE\{Company Name}\{Product Name}  @={Product Description}

  // for each EXE file
  // HKLM\SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths\FileName.EXE @=%apppath%\FileName.EXE
  // HKLM\SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths\FileName.EXE Path=%apppath%


  // NOTE:  don't bother checking - just add stuff

  CSetupGizDlg *pSheet = CSetupGizDlg::GetPropertySheet(this);

  if(!pSheet)
    return;

  CString csTemp = "[HKLM\\SOFTWARE\\"
                 + pSheet->m_Page1.m_csCompanyName
                 + "\\"
                 + pSheet->m_Page1.m_csAppName
                 + "]@=\""
                 + pSheet->m_Page1.m_csAppName
                 + "\"";

  m_wndEntryList.AddString(csTemp);

  int i1;
  for(i1=0; i1 < pSheet->m_aFileList.GetSize(); i1++)
  {
    csTemp = pSheet->m_aFileList[i1].csFileName;

    if(!csTemp.Right(4).CompareNoCase(".EXE"))
    {
      // .exe files - register them

      CString csPath = "%apppath%";

      if(pSheet->m_aFileList[i1].iDestPath == 0)
        csPath = "%sysdir%";
      else if(pSheet->m_aFileList[i1].iDestPath == 2)
        csPath = "%tmpdir%";
      else if(pSheet->m_aFileList[i1].iDestPath == 3 &&
              pSheet->m_aFileList[i1].csOtherPathName.GetLength()) 
      {
        csPath = pSheet->m_aFileList[i1].csOtherPathName;
      }

      CString csPath0 = csPath;

      if(csPath.GetLength() && 
         csPath[csPath.GetLength() - 1] != '\\')
        csPath += '\\';

      csTemp = "HKLM\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\"
             + csTemp;

      m_wndEntryList.AddString("[" + csTemp + "]@=\"" + csPath +
                               pSheet->m_aFileList[i1].csFileName + "\"");

      m_wndEntryList.AddString("[" + csTemp + "]\"Path\"=\"" + csPath0 + "\"");
    }
  }

  m_wndEntryList.SetCurSel(m_wndEntryList.GetCount() - 1);
  OnSelchangeEntryList();  // selects last item & fixes the rest
}

void CPage4::OnImportReg() 
{
  int i1;

  // [KEYNAME]
  // @="something"
  // "value"="something"

  CFileDialog dlg(TRUE, ".reg", "*.reg", OFN_HIDEREADONLY,
                  "Registry Files (*.reg)|*.reg|Theme File (*.theme)|*.theme|All Files (*.*)|*.*||");

  dlg.m_ofn.lpstrTitle = "Open Registry File";

  CString csCurDir = GetCurrentDirectory();

  if(csCurDir.GetLength())
    dlg.m_ofn.lpstrInitialDir = (LPCSTR)csCurDir;

  if(dlg.DoModal() != IDOK)
    return;


  HANDLE hFile = CreateFile(dlg.GetPathName(), GENERIC_READ, FILE_SHARE_READ,
                            NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

  if(hFile == INVALID_HANDLE_VALUE)
  {
    AfxMessageBox("Unable to load registry file",
                  MB_OK | MB_ICONHAND | MB_SETFOREGROUND);
    return;
  }

  BOOL bIsTheme = dlg.GetPathName().Right(6).CompareNoCase(".theme");

  long lSize = GetFileSize(hFile, NULL);

  if(!lSize)
  {
    AfxMessageBox("Registry file is empty",
                  MB_OK | MB_ICONHAND | MB_SETFOREGROUND);

    CloseHandle(hFile);
    return;
  }

  CString csBuf;
  LPSTR lpBuf = csBuf.GetBufferSetLength(lSize + 1);

  if(!lpBuf)
  {
    AfxMessageBox("Not enough memory to read registry file",
                  MB_OK | MB_ICONHAND | MB_SETFOREGROUND);

    CloseHandle(hFile);
    return;
  }

  DWORD cb1;

  if(!ReadFile(hFile, lpBuf, lSize, &cb1, NULL) ||
      (long)cb1 != lSize)
  {
    AfxMessageBox("Read error on registry file",
                  MB_OK | MB_ICONHAND | MB_SETFOREGROUND);

    CloseHandle(hFile);
    return;
  }

  CloseHandle(hFile);  // not needed now (I got it all)

  LPCSTR lpc1 = lpBuf, lpc2;

  lpBuf[lSize] = 0;  // do this to make things easier

  CString csTemp, csLine, csKey;  // csKey is the current key

  CStringArray acsKeys;

  while(*lpc1)
  {
    for(lpc2 = lpc1; *lpc2 && *lpc2 != '\n'; *lpc2++)
      ;  // find the "newline"

    csLine = "";
    if(lpc2 > lpc1)
      memcpy(csLine.GetBufferSetLength(lpc2 - lpc1), lpc1, lpc2 - lpc1);

    csLine.ReleaseBuffer(lpc2 - lpc1);
    csLine.ReleaseBuffer(-1);

    if(*lpc2)
      lpc2++;

    lpc1 = lpc2;  // points to NEXT entry (so I can use 'continue')

    csLine.TrimRight();
    csLine.TrimLeft();

    if(!csLine.GetLength())
      continue;

    if(!csLine.CompareNoCase("REGEDIT4"))
    {
      bIsTheme = FALSE;  // it's a REGEDIT file
      continue;          // in all other ways, ignore this
    }

    if(csLine[0] == ';')
      continue;  // comment

    if(csLine[0] == '[')
    {
      // a KEY!

      lpc2 = (LPCSTR)csLine + 1;

      while(*lpc2 && *lpc2 != ']')
        lpc2++;

      if(*lpc2)
      {
        lpc2++;

        csKey = csLine.Left(lpc2 - (LPCSTR)csLine);

        if(bIsTheme)
        {
          csKey = "[HKCU\\" + csKey.Mid(1);
        }
      }
      else
      {
        AfxMessageBox("SYNTAX ERROR in input file\r\n\n" + csLine,
                      MB_OK | MB_ICONHAND | MB_SETFOREGROUND); // indicate error, reject file
        return;
      }
    }
    else if(csKey.GetLength())
    {
      // search for an '='

      lpc2 = (LPCSTR)csLine + 1;

      while(*lpc2 && *lpc2 != '=')
      {
        if(*lpc2 == '"')
        {
          lpc2++;
          while(*lpc2 && (*lpc2 != '"' || lpc2[1] == '"'))
          {
            if(*lpc2 == '\\')
              lpc2++;

            if(*lpc2)
              lpc2++;
          }
        }

        if(*lpc2)
          lpc2++;
      }

      if(*lpc2 == '=')  // found!
      {
        csTemp = csLine.Left(lpc2 - (LPCSTR)csLine);
        csTemp.TrimRight();

        csLine = csLine.Mid(lpc2 - (LPCSTR)csLine + 1); // exclude the '='

        // trim 'csLine' and make sure it's quoted

        csLine.TrimRight();
        csLine.TrimLeft();

        csTemp.TrimRight();
        csTemp.TrimLeft();

        if(!csLine.GetLength() || csLine[0] != '"')  // unquoted
        {
          // assume backslashes aren't doubled, but double the quote marks (if any)

          for(i1=0; i1 < csLine.GetLength(); i1++)
          {
            if(csLine[i1] == '"')
            {
              csLine = csLine.Left(i1) + '"' + csLine.Mid(i1);
              i1++;
            }
          }

          csLine = '"' + csLine + '"';
        }
        else
        {
          // un-double any backslashes, translate any '\"' to '""'

          for(i1=0; i1 < (csLine.GetLength() - 1); i1++)
          {
            if(csLine[i1] == '\\' && csLine[i1 + 1] == '\\')
            {
              csLine = csLine.Left(i1) + csLine.Mid(i1 + 1);
            }
            else if(csLine[i1] == '\\' && csLine[i1 + 1] == '"')
            {
              csLine = csLine.Left(i1) + '"' + csLine.Mid(i1 + 1);
              i1++;
            }
          }
        }

        // NOTE:  it is not necessary to quote the key, unless it has spaces in it.
        //        but by convention the value *IS* quoted.

        if(bIsTheme && !csTemp.CompareNoCase("DefaultValue")) // it's assumed to be '@'
          csLine = "@=" + csLine;
        else
          csLine = csTemp + "=" + csLine;

        // at this point, 'csLine' is equal to "key=value" with quotes inserted where needed

        csTemp = csKey + csLine;

#ifdef _DEBUG
        afxDump << "ADDING REG ENTRY " + csTemp + "\r\n";
#endif // _DEBUG

        acsKeys.Add(csTemp);
      }
    }
    else
    {
      AfxMessageBox("WARNING - the following entry was rejected because it had no key\r\n\n"
                    + csLine,
                    MB_OK | MB_ICONASTERISK | MB_SETFOREGROUND);
    }
  }

  csBuf.ReleaseBuffer();

  if(!acsKeys.GetSize())
  {
    AfxMessageBox("WARNING:  no registry entries have been added",
                  MB_OK | MB_ICONHAND | MB_SETFOREGROUND);
  }
  else
  {
    for(i1=0; i1 < acsKeys.GetSize(); i1++)
    {
      m_wndEntryList.AddString(acsKeys[i1]);
    }
  }

  m_wndEntryList.SetCurSel(m_wndEntryList.GetCount() - 1);
  OnSelchangeEntryList();  // selects last item & fixes the rest
}
