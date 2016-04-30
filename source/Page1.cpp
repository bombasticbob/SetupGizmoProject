//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//             ____                      _                                  //
//            |  _ \  __ _   __ _   ___ / |    ___  _ __   _ __             //
//            | |_) |/ _` | / _` | / _ \| |   / __|| '_ \ | '_ \            //
//            |  __/| (_| || (_| ||  __/| | _| (__ | |_) || |_) |           //
//            |_|    \__,_| \__, | \___||_|(_)\___|| .__/ | .__/            //
//                          |___/                  |_|    |_|               //
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
#include "Page1.h"
#include "setupgizdlg.h"
#include "SetupDialog.h"
#include "PathBrowse.h"
#include "Page1AdvancedDlg.h"
#include "LicenseDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// friend myself when needed
class CMyFileDialog : public CFileDialog
{
  friend class CPage1;
};


/////////////////////////////////////////////////////////////////////////////
// CPage1 property page

IMPLEMENT_DYNCREATE(CPage1, CPropertyPage)

CPage1::CPage1() : CPropertyPage(CPage1::IDD)
{
	//{{AFX_DATA_INIT(CPage1)
	m_csAppName = _T("");
	m_csCompanyName = _T("");
	m_csSourcePath = _T("");
	m_csStartMessage = _T("");
	m_csDestPath = _T("");
	m_csLicenseFile = _T("");
  m_csCertFiles = _T("");
	//}}AFX_DATA_INIT

  DoClear();
}

CPage1::~CPage1()
{
}

void CPage1::DoClear()
{
  m_csAppName = "{ Your Application }";

  if(theApp.m_csAuthCompany.GetLength())
    m_csCompanyName = theApp.m_csAuthCompany;
  else
    m_csCompanyName = "{ Your Company Name }";

  m_csStartMessage = "Welcome to SETUP!";

	m_csSourcePath = _T("");
	m_csDestPath = _T("");
	m_csLicenseFile = _T("");


  // ADVANCED SETTINGS

  m_csAppPath = "%programfiles%\\%appname%";
  m_csAppCommon = "%programfiles%\\Common Files";
  m_csStartMenu = "%appname%";

  m_acsUpgrade.RemoveAll();
}

void CPage1::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPage1)
	DDX_Text(pDX, IDC_APP_NAME, m_csAppName);
	DDX_Text(pDX, IDC_COMPANY_NAME, m_csCompanyName);
	DDX_Text(pDX, IDC_SOURCE_PATH, m_csSourcePath);
	DDX_Text(pDX, IDC_START_MESSAGE, m_csStartMessage);
	DDX_Text(pDX, IDC_DEST_PATH, m_csDestPath);
	DDX_Text(pDX, IDC_LICENSE_FILE, m_csLicenseFile);
	DDX_Text(pDX, IDC_CERT_FILES, m_csCertFiles);
	//}}AFX_DATA_MAP

  CWnd *pCtrl = GetDlgItem(IDC_TEST_LIC);

  if(pCtrl &&
     m_csLicenseFile.GetLength() &&
     !pCtrl->IsWindowEnabled())
  {
    pCtrl->EnableWindow(1);
  }
  else if(pCtrl &&
          !m_csLicenseFile.GetLength() &&
          pCtrl->IsWindowEnabled())
  {
    pCtrl->EnableWindow(0);
  }
}


BEGIN_MESSAGE_MAP(CPage1, CPropertyPage)
	//{{AFX_MSG_MAP(CPage1)
	ON_BN_CLICKED(IDC_BROWSE_DEST, OnBrowseDest)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	ON_BN_CLICKED(IDC_BROWSE_LICENSE, OnBrowseLicense)
	ON_BN_CLICKED(IDC_BROWSE_CERT, OnBrowseCert)
	ON_BN_CLICKED(IDC_TEST, OnTest)
	ON_BN_CLICKED(IDC_OPEN, OnOpen)
	ON_BN_CLICKED(IDC_ADVANCED, OnAdvanced)
	//}}AFX_MSG_MAP
  ON_BN_CLICKED(IDC_TEST_LIC, OnTestLic)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPage1 message handlers


BOOL CPage1::OnSetActive() 
{
  CSetupGizDlg *pSheet = CSetupGizDlg::GetPropertySheet(this);

  if(pSheet)
  {
    pSheet->SetWizardButtons(PSWIZB_NEXT);  // only 'next'
//    pSheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);
//    pSheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_FINISH);
  }

  UpdateData(0);

	return CPropertyPage::OnSetActive();
}

BOOL CPage1::OnKillActive() 
{
  UpdateData(1);
	
	return CPropertyPage::OnKillActive();
}


BOOL IsValidDirectory(LPCSTR szDirName)
{
  CString csPath = szDirName;

  if(csPath.GetLength() == 3 &&
     toupper(csPath[0]) >= 'A' &&
     toupper(csPath[0]) <= 'Z' &&
     csPath[1] == ':' &&
     csPath[2] == '\\')
  {
    // TODO:  validate drive letter???

    return(TRUE);  // for now, assume valid
  }

  WIN32_FIND_DATA fd;
  HANDLE hFF = FindFirstFile(csPath, &fd);

  if(hFF == INVALID_HANDLE_VALUE)
  {
    return(FALSE);  // not valid
  }

  BOOL bIsDir = fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;

  FindClose(hFF);

  if(!bIsDir)
  {
    // don't give up, it might be a 'share'

    if(csPath.GetLength() > 2 &&
       csPath[0] == '\\' && csPath[1] == '\\')
    {
      // so far so good... see if there's a share name

      LPCSTR lp1 = strchr((LPCSTR)csPath + 2, '\\');

      if(lp1)  // has a share name
      {
        lp1 = strchr(lp1 + 1, '\\');  // find next one

        if(!lp1 || !*(lp1 + 1))  // end of string or no more '\'
        {
          bIsDir = TRUE;
        }
      }
    }
  }

  return(bIsDir);
}

BOOL FileExists(LPCSTR szFileName)
{
  CString csPath = szFileName;

  if(csPath.GetLength() == 3 &&
     toupper(csPath[0]) >= 'A' &&
     toupper(csPath[0]) <= 'Z' &&
     csPath[1] == ':' &&
     csPath[2] == '\\')
  {
    // TODO:  validate drive letter???

    return(TRUE);  // for now, assume valid
  }

  WIN32_FIND_DATA fd;
  HANDLE hFF = FindFirstFile(csPath, &fd);

  if(hFF == INVALID_HANDLE_VALUE)
  {
    return(FALSE);  // not valid
  }

  FindClose(hFF);

  return TRUE;
}


LRESULT CPage1::OnWizardNext() 
{
  CString csTemp;

  UpdateData(1);

  // validate data
	
  if(!m_csSourcePath.GetLength())
  {
    AfxMessageBox("You must enter a valid source path",
                  MB_OK | MB_ICONHAND | MB_SETFOREGROUND);
    return -1;
  }

  if(!m_csDestPath.GetLength())
  {
    AfxMessageBox("You must enter a valid destination path",
                  MB_OK | MB_ICONHAND | MB_SETFOREGROUND);
    return -1;
  }

  csTemp = m_csSourcePath;
  if(GetFullPathName(csTemp, MAX_PATH * 2,
                     m_csSourcePath.GetBufferSetLength(MAX_PATH * 2),
                     NULL))
  {
    m_csSourcePath.ReleaseBuffer(-1);
    UpdateData(0);
  }
  else
  {
    m_csSourcePath = csTemp;
  }

  if(!IsValidDirectory(m_csSourcePath))
  {
    AfxMessageBox("You must enter a valid directory for the source path",
                  MB_OK | MB_ICONHAND | MB_SETFOREGROUND);

    return -1;
  }

  csTemp = m_csDestPath;
  if(GetFullPathName(csTemp, MAX_PATH * 2,
                     m_csDestPath.GetBufferSetLength(MAX_PATH * 2),
                     NULL))
  {
    m_csDestPath.ReleaseBuffer(-1);
    UpdateData(0);
  }
  else
  {
    m_csDestPath = csTemp;
  }

  if(!IsValidDirectory(m_csDestPath))
  {
    if(!FileExists(m_csDestPath))
    {
      if(AfxMessageBox("Source path \"" + m_csDestPath + "\" does not exist, create it now?",
                       MB_YESNO | MB_ICONHAND | MB_SETFOREGROUND)
         != IDYES)
      {
        return -1;
      }
      else if(!CreateDirectory(m_csDestPath, NULL))
      {
        DWORD dwErr = GetLastError();

        csTemp.Format("Unable to create directory \"%s\", error code %d (%08xH)",
                      (LPCSTR)m_csDestPath, dwErr, dwErr);

        AfxMessageBox(csTemp);
        return -1;
      }
    }
    else
    {
      AfxMessageBox("You must enter a valid directory for the destination path",
                    MB_OK | MB_ICONHAND | MB_SETFOREGROUND);
      return -1;
    }
  }

  // make sure license file is either BLANK or a real file

  m_csLicenseFile.TrimRight();
  m_csLicenseFile.TrimLeft();

  if(m_csLicenseFile.GetLength())
  {
    // TODO:  make this a utility to test any kind of file with

    WIN32_FIND_DATA fd;
    HANDLE hFF = FindFirstFile(m_csLicenseFile, &fd);

    // check if file exists
    if(hFF == INVALID_HANDLE_VALUE)
    {
      AfxMessageBox("You must enter a valid (or blank) path for the license file",
                    MB_OK | MB_ICONHAND | MB_SETFOREGROUND);

      return -1;
    }

    // check if file is directory
    BOOL bIsDir = fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;

    FindClose(hFF);

    if(bIsDir)
    {
      AfxMessageBox("You must enter a valid (or blank) path for the license file that is not the name of a sub-directory",
                    MB_OK | MB_ICONHAND | MB_SETFOREGROUND);

      return -1;
    }
  }

  // UPDATING THINGS IN THE PROPERTY SHEET CONTAINER

  CSetupGizDlg *pSheet = CSetupGizDlg::GetPropertySheet(this);

  if(pSheet)
  {
    pSheet->UpdatePage1Stuff(); // updates from internal arrays
  }

	return(CPropertyPage::OnWizardNext());
}

void CPage1::OnBrowseDest() 
{
  UpdateData(1);

  CPathBrowse dlg(m_csDestPath);
  dlg.m_csTitle = "Select the 'destination' path for the current SETUP project";

  if(dlg.DoModal() == IDOK)
  {
    m_csDestPath = dlg.GetPathName();
    UpdateData(0);
  }
}

void CPage1::OnBrowse() 
{
  UpdateData(1);

  CPathBrowse dlg(m_csSourcePath);
  dlg.m_csTitle = "Select the 'source' path for the current SETUP project";

  if(dlg.DoModal() == IDOK)
  {
    m_csSourcePath = dlg.GetPathName();
    UpdateData(0);
  }
}

void CPage1::OnBrowseLicense() 
{
  UpdateData(1);

  CString csDefault = m_csLicenseFile;

  if(!csDefault.GetLength())
  {
    csDefault = "license.txt";
  }
  else
  {
    int i1 = csDefault.ReverseFind('\\');

    if(i1 >= 0)
    {
      if(i1 > 3)
        SetCurrentDirectory(csDefault.Left(i1));
      else
        SetCurrentDirectory(csDefault.Left(i1 + 1));

      csDefault = csDefault.Mid(i1 + 1);  // trim path off of name
    }
  }

  // license file is read-only - get the file name
  CFileDialog dlg(TRUE, "txt", csDefault, 0,
                  "Text Files (*.txt)|*.txt|RTF Files (*.rtf)|*.rtf|All Files (*.*)|*.*||",
                  this);

  if(dlg.DoModal() == IDOK)
  {
    m_csLicenseFile = dlg.GetPathName();

    UpdateData(0);
  }
}

void CPage1::OnBrowseCert() 
{
  CStringArray acsTemp;
  CString csTemp;
  int i1;
  char *lp1;
  DWORD cb1 = (MAX_PATH + 2) * MAX_NUM_CERT_FILES;

  UpdateData(1);

  CFileDialog dlg(TRUE, "cer", NULL,
                  OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,
                  "Cert Files (*.cer)|*.cer|All Files (*.*)|*.*||",
//                  "Cert Files (*.cer,*.pem)|*.cer;*.pem|All Files (*.*)|*.*||",
                  this);

  // TODO:  allow PEM files but convert them automatically
  
  csTemp = m_csCertFiles + '\0'; // make sure extra NULL byte there

  // need to make an OFN buffer for the multi-select
  dlg.m_ofn.Flags |= OFN_ALLOWMULTISELECT;
  dlg.m_ofn.lpstrFile = lp1 = csTemp.GetBufferSetLength(cb1 + 1);
  dlg.m_ofn.nMaxFile = cb1;

  // convert "string" "string2" to string\0string2\0\0
  InPlaceConvertMultStringToStringList(lp1, cb1);

  if(dlg.DoModal() == IDOK)
  {
    POSITION pos = dlg.GetStartPosition();

    while(pos)
    {
      acsTemp.Add(dlg.GetNextPathName(pos));
    }

    if(acsTemp.GetSize() <= 1)
    {
      if(acsTemp.GetSize() == 1)
      {
        m_csCertFiles = acsTemp[0];
      }
    }
    else
    {
      m_csCertFiles = "";

      for(i1=0; i1 < acsTemp.GetSize(); i1++)
      {
        if(i1 > 0)
        {
          m_csCertFiles += " ";
        }

        m_csCertFiles += '"' + acsTemp[i1] + '"';
      }
    }

    UpdateData(0);
  }
}


CString TranslateEscapeCodes(CString &csSource)
{
  CString csRval = "";

  int i1;
  for(i1=0; i1 < csSource.GetLength(); i1++)
  {
    char c1 = csSource[i1];

    if(c1 == '\\' && 
       (i1 + 1) < csSource.GetLength())
    {
      i1++;
      c1 = toupper(csSource[i1]);
      switch(c1)
      {
        case 'A':
          c1 = '\a';
          break;
        case 'B':
          c1 = '\b';
          break;
        case 'F':
          c1 = '\f';
          break;
        case 'N':
          c1 = '\n';
          break;
        case 'R':
          c1 = '\r';
          break;
        case 'T':
          c1 = '\t';
          break;
        case 'V':
          c1 = '\v';
          break;
        case '?':
          c1 = '?';
          break;
        case '\'':
          c1 = '\'';
          break;
        case '"':
          c1 = '"';
          break;
        case '\\':
          c1 = '\\';
          break;
        case '0':
          c1 = '\0';
          break;
        default:
          if(c1 == 'X')
          {
            c1 = 0;

            if((i1 + 1) < csSource.GetLength())
            {
              char c2 = toupper(csSource[i1 + 1]);
              if((c2 >= '0' && c2 <= '9') ||
                 (c2 >= 'A' && c2 <= 'F'))
              {
                i1++;

                if(c2 >= 'A' && c2 <= 'F')
                {
                  c1 += 10 + c2 - 'A';
                }
                else
                {
                  c1 += c2 - '0';
                }

                if((i1 + 1) < csSource.GetLength())
                {
                  c2 = toupper(csSource[i1 + 1]);

                  if((c2 >= '0' && c2 <= '9') ||
                     (c2 >= 'A' && c2 <= 'F'))
                  {
                    i1++;

                    c1 *= 16;

                    if(c2 >= 'A' && c2 <= 'F')
                    {
                      c1 += 10 + c2 - 'A';
                    }
                    else
                    {
                      c1 += c2 - '0';
                    }
                  }
                }
              }
            }
          }
          else
          {
            c1 = 0;

            if((i1 + 1) < csSource.GetLength())
            {
              char c2 = toupper(csSource[i1 + 1]);
              if(c2 >= '0' && c2 <= '7')
              {
                i1++;
                c1 += c2 - '0';

                if((i1 + 1) < csSource.GetLength())
                {
                  c2 = toupper(csSource[i1 + 1]);

                  if(c2 >= '0' && c2 <= '7')
                  {
                    i1++;

                    c1 *= 8;
                    c1 += c2 - '0';

                    if((i1 + 1) < csSource.GetLength())
                    {
                      c2 = toupper(csSource[i1 + 1]);

                      if(c2 >= '0' && c2 <= '7')
                      {
                        i1++;

                        c1 *= 8;
                        c1 += c2 - '0';
                      }
                    }
                  }
                }
              }
            }
          }
      }
    }

    csRval += c1;
  }

  return(csRval);
}  

void CPage1::OnTest() 
{
  UpdateData(1);

  CSetupDialog dlg(this);

  dlg.m_csTitle = m_csAppName + " SETUP";
  dlg.m_csWelcome = TranslateEscapeCodes(m_csStartMessage);

  dlg.DoModal();
}

void CPage1::OnOpen() 
{
  UpdateData(1);

  CSetupGizDlg *pSheet = CSetupGizDlg::GetPropertySheet(this);

  if(!pSheet)
  {
    AfxMessageBox("INTERNAL ERROR - unable to continue operation",
                  MB_OK | MB_ICONHAND | MB_SETFOREGROUND);
    return;
  }

  CFileDialog dlg(TRUE,".giz",NULL,OFN_HIDEREADONLY,
                  "Setup Gizmo Files (*.giz)|*.giz|All Files(*.*)|*.*||",
                  pSheet);

  if(dlg.DoModal() == IDOK)
  {
    pSheet->m_csFileName = dlg.GetPathName();
    pSheet->DoOpen();
  }

  UpdateData(0);
}

void CPage1::OnAdvanced() 
{
  UpdateData(1);

  CPage1AdvancedDlg dlg;

  dlg.m_csAppPath = m_csAppPath;
  dlg.m_csAppCommon = m_csAppCommon;
  dlg.m_csStartMenu = m_csStartMenu;

  int i1;
  for(i1=0; i1 < m_acsUpgrade.GetSize(); i1++)
  {
    dlg.m_acsUpgrade.Add(m_acsUpgrade[i1]);
  }

  if(dlg.DoModal() == IDOK)
  {
    m_csAppPath = dlg.m_csAppPath;
    m_csAppCommon = dlg.m_csAppCommon;
    m_csStartMenu = dlg.m_csStartMenu;

    m_acsUpgrade.RemoveAll();

    for(i1=0; i1 < dlg.m_acsUpgrade.GetSize(); i1++)
    {
      m_acsUpgrade.Add(dlg.m_acsUpgrade[i1]);
    }
  }
}

void CPage1::OnTestLic()
{
  UpdateData(1);

  m_csLicenseFile.TrimRight();
  m_csLicenseFile.TrimLeft();

  UpdateData(0);  // to make sure

  if(!m_csLicenseFile.GetLength())
  {
    MessageBeep(0);
    return;
  }

  HANDLE hFile = CreateFile(m_csLicenseFile,
                            GENERIC_READ, 0, NULL, OPEN_EXISTING,
                            FILE_ATTRIBUTE_NORMAL, NULL);

  if(hFile == INVALID_HANDLE_VALUE)
  {
    AfxMessageBox("WARNING:  " + m_csLicenseFile + " does not exist.", MB_OK | MB_ICONHAND | MB_SETFOREGROUND);
    return;
  }
  else
  {
    CLicenseDlg dlg;

    // find out size of file, then read the whole thing into 'm_csINFFile'
    // NOTE:  empty file is the same as non-existent file

    DWORD cb1, cbFile = GetFileSize(hFile, NULL);

    if(cbFile &&
       !ReadFile(hFile, dlg.m_csLicense.GetBufferSetLength(cbFile),
                 cbFile, &cb1, NULL)
       || cb1 != cbFile)
    {
      dlg.m_csLicense.ReleaseBuffer(0);
      CloseHandle(hFile);

      AfxMessageBox("Read error on license file", MB_OK | MB_ICONHAND | MB_SETFOREGROUND);
      return;
    }
              
    dlg.m_csLicense.ReleaseBuffer(cbFile);
    CloseHandle(hFile);

    if(!cbFile)
      AfxMessageBox("WARNING:  license file is empty!", MB_OK | MB_ICONEXCLAMATION | MB_SETFOREGROUND);

    dlg.DoModal();
  }
}
