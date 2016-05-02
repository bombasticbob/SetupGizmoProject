//////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                          //
//   ____         _                   ____  _       ____   _                                //
//  / ___|   ___ | |_  _   _  _ __   / ___|(_) ____|  _ \ | |  __ _     ___  _ __   _ __    //
//  \___ \  / _ \| __|| | | || '_ \ | |  _ | ||_  /| | | || | / _` |   / __|| '_ \ | '_ \   //
//   ___) ||  __/| |_ | |_| || |_) || |_| || | / / | |_| || || (_| | _| (__ | |_) || |_) |  //
//  |____/  \___| \__| \__,_|| .__/  \____||_|/___||____/ |_| \__, |(_)\___|| .__/ | .__/   //
//                           |_|                              |___/         |_|    |_|      //
//                                                                                          //
//////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                          //
//               Copyright (c) 1998-2016 by S.F.T. Inc. - All rights reserved               //
//          Use, copying, and distribution of this software are licensed according          //
//            to the GPLv2, LGPLv2, or BSD license, as appropriate (see COPYING)            //
//                                                                                          //
//////////////////////////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "SetupGiz.h"
#include "SetupGizDlg.h"
#include "NagDialog.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// CSetupGizDlg dialog

IMPLEMENT_DYNAMIC(CSetupGizDlg,CPropertySheet);

CSetupGizDlg::CSetupGizDlg(CWnd* pParentWnd)
	:CPropertySheet("* SFT SetupGiz 'Setup Gizmo' Application", 
                  pParentWnd, 0)  // always pick 1st page
{
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

  AddPage(&m_Page1);
  AddPage(&m_Page2);
  AddPage(&m_Page3);
  AddPage(&m_Page3A);
  AddPage(&m_Page3B);
  AddPage(&m_Page3Cert);
  AddPage(&m_Page4);

  AddPage(&m_Finish);

  SetWizardMode();

  DoClear();
}


CSetupGizDlg *CSetupGizDlg::GetPropertySheet(CPropertyPage *pPropPage)
{
  CWnd *pWnd = pPropPage;

  while(pWnd && !pWnd->IsKindOf(RUNTIME_CLASS(CPropertySheet)))
  {
    pWnd = pWnd->GetParent();
  }

  if(pWnd && !pWnd->IsKindOf(RUNTIME_CLASS(CSetupGizDlg)))
  {
    AfxMessageBox("INTERNAL ERROR - property sheet not 'CSetupGizDlg'",
                  MB_OK | MB_ICONHAND | MB_SETFOREGROUND);
    return(NULL);
  }

  return((CSetupGizDlg *)pWnd);
}

static void DoParseString(CString &csLine, CStringArray &acsColumns)
{
  acsColumns.RemoveAll();

  // parse into columns

  int i1=0, i2;

  while(i1 < csLine.GetLength())
  {
    while(i1 < csLine.GetLength() && csLine[i1] <= ' ')
      i1++;

    if(csLine[i1] == '"' ||
       csLine[i1] == '\'')
    {
      i2 = i1++;

      while(i1 < csLine.GetLength() && csLine[i1] != csLine[i2])
        i1++;

      acsColumns.Add(csLine.Mid(i2 + 1, i1 - i2 - 1));

      if(i1 < csLine.GetLength())
        i1++;

      while(i1 < csLine.GetLength() && csLine[i1] != ',')
      {
        if(csLine[i1] > ' ')
        {
          TRACE("Found bad character '%c' in line\r\n", 
                (char)csLine[i1]);
        }

        i1++;
      }
    }
    else
    {
      i2 = i1;
      while(i1 < csLine.GetLength() && csLine[i1] != ',')
        i1++;

      acsColumns.Add(csLine.Mid(i2, i1 - i2));
    }

    if(i1 < csLine.GetLength() && csLine[i1] == ',')
      i1++;

    while(i1 < csLine.GetLength() && csLine[i1] <= ' ')
      i1++;
  }
}

void CSetupGizDlg::DoClear()
{
  m_csFileName = "";

  m_aFileList.RemoveAll();
  m_aGroupList.RemoveAll();
  m_acsRegistry.RemoveAll();

  m_Page1.DoClear();
  m_Page2.DoClear();
  m_Page3.DoClear();
  m_Page3A.DoClear();
  m_Page3B.DoClear();
  m_Page4.DoClear();

  // put other pages here

  m_Finish.DoClear();
}


void GenerateDefaultShortcutInfo(SetupFileInfo &sfi)
{
  if(!sfi.bAddShortcut)
    return;

  if(sfi.csShortcutFileName.GetLength())
    return;

  int i3 = sfi.csFileName.ReverseFind('\\');

  if(i3 >= 0)
  {
    CString csTemp2 = sfi.csFileName.Mid(i3 + 1);
    CString csTemp3 = sfi.csFileName.Left(i3); // not including backslash

    i3 = csTemp2.ReverseFind('.');
    if(i3 <= 0)
    {
      i3 = csTemp2.GetLength();
    }
    else
    {
//        csTemp2.SetAt(i3, '_');  // convert '.' to '_' for unique name
      if(!csTemp2.Mid(i3).CompareNoCase(".EXE"))
        csTemp2 = csTemp2.Left(i3) + " Application";
      else if(!csTemp2.Mid(i3).CompareNoCase(".HLP"))
        csTemp2 = csTemp2.Left(i3) + " Help";
      else if(!csTemp2.Mid(i3).CompareNoCase(".DOC"))
        csTemp2 = csTemp2.Left(i3) + " Document";
      else if(!csTemp2.Mid(i3).CompareNoCase(".RTF"))
        csTemp2 = csTemp2.Left(i3) + " RTF Document";
      else if(!csTemp2.Mid(i3).CompareNoCase(".WRI"))
        csTemp2 = csTemp2.Left(i3) + " MS Write Document";
      else if(!csTemp2.Mid(i3).CompareNoCase(".TXT"))
        csTemp2 = csTemp2.Left(i3) + " Text File";
      else
      {
        csTemp2.SetAt(i3, ' ');  // convert '.' to ' '
        csTemp2 += " File";
      }
    }

    // process the path for unique name
    int i1;
    for(i1=0; i1 < csTemp3.GetLength(); i1++)
    {
      if(csTemp3[i1] == '\\')
        csTemp3.SetAt(i1, ' ');  // change backslashes to spaces!!!
    }

    csTemp3.TrimRight();
    csTemp3.TrimLeft();

    if(csTemp3.GetLength())
      csTemp3 += ' ';


    sfi.csShortcutFileName = csTemp2 + ".LNK";
    sfi.csShortcutDescription = csTemp3 + csTemp2.Left(i3);
  }
  else
  {
    CString csTemp2 = sfi.csFileName;

    i3 = csTemp2.ReverseFind('.');
    if(i3 <= 0)
    {
      i3 = csTemp2.GetLength();
    }
    else
    {
      if(!csTemp2.Mid(i3).CompareNoCase(".EXE"))
        csTemp2 = csTemp2.Left(i3) + " Application";
      else if(!csTemp2.Mid(i3).CompareNoCase(".HLP"))
        csTemp2 = csTemp2.Left(i3) + " Help";
      else if(!csTemp2.Mid(i3).CompareNoCase(".DOC"))
        csTemp2 = csTemp2.Left(i3) + " Document";
      else if(!csTemp2.Mid(i3).CompareNoCase(".RTF"))
        csTemp2 = csTemp2.Left(i3) + " RTF Document";
      else if(!csTemp2.Mid(i3).CompareNoCase(".WRI"))
        csTemp2 = csTemp2.Left(i3) + " MS Write Document";
      else if(!csTemp2.Mid(i3).CompareNoCase(".TXT"))
        csTemp2 = csTemp2.Left(i3) + " Text File";
      else
      {
        csTemp2.SetAt(i3, ' ');  // convert '.' to ' '
        csTemp2 += " File";
      }
    }

    sfi.csShortcutFileName = csTemp2 + ".LNK";
    sfi.csShortcutDescription = csTemp2.Left(i3);
  }

  // NOTE:  leave the other entries 'as-is'
}


void CSetupGizDlg::DoOpen()
{
CString csTemp, csTemp2;
LPSTR lp1, lp2;


  if(!m_csFileName)
    return;  // do nothing if file name is blank

  CString csFileName = m_csFileName;  // make backup of name

  DoClear();  // make sure I *EMPTY* everything first!

  m_csFileName = csFileName;

  if(m_csFileName.Find('\\') >= 0 || m_csFileName.Find(':') >= 0 ||
     !SearchPath(NULL, m_csFileName, ".giz", MAX_PATH,
                 csTemp.GetBufferSetLength(MAX_PATH), &lp1))
  {
    csTemp.ReleaseBuffer(0);

    // partially qualified name or an error using 'SearchPath'

    GetFullPathName(m_csFileName, MAX_PATH,
                    csTemp.GetBufferSetLength(MAX_PATH), &lp1);
    csTemp.ReleaseBuffer(-1);

    if(csTemp.GetLength())
      m_csFileName = csTemp;
  }
  else
  {
    // "SearchPath" worked?

    if(csTemp.GetLength())
      m_csFileName = csTemp;
  }

  // NOW, set default directory to match the file name

  if(m_csFileName.Find('\\') >= 0)
  {
    csTemp = m_csFileName.Left(m_csFileName.ReverseFind('\\') + 1);  // include the backslash

    if(csTemp.GetLength() > 3)
      csTemp = csTemp.Left(csTemp.GetLength() - 1);

    SetCurrentDirectory(csTemp);  // this should fix everything
  }


  // ** PAGE 1 stuff **

  GetPrivateProfileString("Page1","AppName", "",
                          m_Page1.m_csAppName.GetBufferSetLength(MAX_PATH),
                          MAX_PATH, m_csFileName);

  m_Page1.m_csAppName.ReleaseBuffer(-1);

  GetPrivateProfileString("Page1","CompanyName", "",
                          m_Page1.m_csCompanyName.GetBufferSetLength(MAX_PATH),
                          MAX_PATH, m_csFileName);

  m_Page1.m_csCompanyName.ReleaseBuffer(-1);

  GetPrivateProfileString("Page1","SourcePath", "",
                          m_Page1.m_csSourcePath.GetBufferSetLength(MAX_PATH),
                          MAX_PATH, m_csFileName);

  m_Page1.m_csSourcePath.ReleaseBuffer(-1);

  GetPrivateProfileString("Page1","DestPath", "",
                          m_Page1.m_csDestPath.GetBufferSetLength(MAX_PATH),
                          MAX_PATH, m_csFileName);

  m_Page1.m_csDestPath.ReleaseBuffer(-1);

  GetPrivateProfileString("Page1","LicenseFile", "",
                          m_Page1.m_csLicenseFile.GetBufferSetLength(MAX_PATH),
                          MAX_PATH, m_csFileName);

  m_Page1.m_csLicenseFile.ReleaseBuffer(-1);

  GetPrivateProfileString("Page1","CertFiles", "",
                          m_Page1.m_csCertFiles.GetBufferSetLength((MAX_PATH + 4) * MAX_NUM_CERT_FILES),
                          (MAX_PATH + 4) * MAX_NUM_CERT_FILES, m_csFileName);

  m_Page1.m_csCertFiles.ReleaseBuffer(-1);

  // hack - if it starts with '\"', trim the '\'
  // this is because 'GetPrivateProfilString()' trims the quote
  // and if I'm quoting multiple files, I want it left ALONE
  if(m_Page1.m_csCertFiles.GetLength() >= 2 &&
     m_Page1.m_csCertFiles.GetAt(0) == '\\' &&
     m_Page1.m_csCertFiles.GetAt(1) == '"')
  {
    m_Page1.m_csCertFiles = m_Page1.m_csCertFiles.Mid(1);
  }

  // update 'm_acsCertFileNameList' and 'm_acsCertRepositoryList'

  UpdatePage1Stuff(TRUE);

  GetPrivateProfileString("Page1","StartMessage", "",
                          m_Page1.m_csStartMessage.GetBufferSetLength(MAX_PATH),
                          MAX_PATH, m_csFileName);

  m_Page1.m_csStartMessage.ReleaseBuffer(-1);

  GetPrivateProfileString("Page1","AppPath", "%programfiles%\\%appname%",
                          m_Page1.m_csAppPath.GetBufferSetLength(MAX_PATH),
                          MAX_PATH, m_csFileName);

  m_Page1.m_csAppPath.ReleaseBuffer(-1);

  GetPrivateProfileString("Page1","AppCommon", "%programfiles%\\Common Files",
                          m_Page1.m_csAppCommon.GetBufferSetLength(MAX_PATH),
                          MAX_PATH, m_csFileName);

  m_Page1.m_csAppCommon.ReleaseBuffer(-1);

  GetPrivateProfileString("Page1","StartMenuFolderName", "%appname%",
                          m_Page1.m_csStartMenu.GetBufferSetLength(MAX_PATH),
                          MAX_PATH, m_csFileName);

  m_Page1.m_csStartMenu.ReleaseBuffer(-1);

  GetPrivateProfileString("Page1","Upgrade","",
                          csTemp2.GetBufferSetLength(65536),
                          65535, m_csFileName);

  csTemp2.ReleaseBuffer(-1);

  if(csTemp2.GetLength() && csTemp2[0] != '"')  // this should ALWAYS be...
    csTemp2 = '"' + csTemp2 + '"';              // quote the thing

  // parse out 'csTemp2'

  DoParseString(csTemp2, m_Page1.m_acsUpgrade);


  // ** FILES **

  // get a list of keys, first

  lp1 = csTemp2.GetBufferSetLength(65536);  // max buffer for these funcitons

  GetPrivateProfileString("FileList",NULL,"",
                          lp1, 65535, m_csFileName);

  // for each key, get the information

  for(lp2=lp1; *lp2; lp2 += lstrlen(lp2) + 1)
  {
    csTemp = "";

    GetPrivateProfileString("FileList",lp2, "",
                            csTemp.GetBufferSetLength(MAX_PATH),
                            MAX_PATH, m_csFileName);

    csTemp.ReleaseBuffer(-1);

    CStringArray acsColumns;

    DoParseString(csTemp, acsColumns);


    if(acsColumns.GetSize() < 10)  // TODO:  change to '13'
    {
      AfxMessageBox("WARNING:  too few columns in '.giz' file for "
                    + (CString)lp2,
                    MB_OK | MB_ICONHAND | MB_SETFOREGROUND);
      continue;  // skip it
    }

// TEMPORARY
    if(acsColumns.GetSize() <= 10)
      acsColumns.SetAtGrow(10,"0");  // default is '0' 

    if(acsColumns.GetSize() <= 11)
      acsColumns.SetAtGrow(11,"");

    if(acsColumns.GetSize() <= 12)
      acsColumns.SetAtGrow(12,"");

// TEMPORARY

    SetupFileInfo sfi;

    sfi.csFileName = lp2;
    sfi.bAutoDelete = atoi(acsColumns[0]) != 0;
    sfi.bExecute    = atoi(acsColumns[1]) != 0;
    sfi.bOverwrite  = atoi(acsColumns[2]) != 0;
    sfi.bShared     = atoi(acsColumns[3]) != 0;
    sfi.iDestPath   = atoi(acsColumns[4]);
    sfi.csGroupName = acsColumns[5];
    sfi.csOtherPathName = acsColumns[6];
    sfi.dwOSFlag    = atol(acsColumns[7]);
    sfi.bNoUninstall= atoi(acsColumns[8]) != 0;
    sfi.bRegister   = atoi(acsColumns[9]) != 0;
    sfi.bRegDotNet  = atoi(acsColumns[9]) > 1;  // typically, 2
    sfi.bAddShortcut= atoi(acsColumns[10]) != 0;

    if(sfi.bExecute)
    {
      sfi.csExecArgs = acsColumns[11];

      if(sfi.bNoUninstall)
        sfi.csUninstArgs = "";
      else
        sfi.csUninstArgs = acsColumns[12];
    }
    else
    {
      sfi.csExecArgs = "";
      sfi.csUninstArgs = "";
    }

    // NEXT, load the shortcut info (if present)

    if(sfi.bAddShortcut)
    {
      // [AddShortcut]
      // srcfilename=Description,ShortcutName[,StartPath,Args,IconFile,IconIndex,nCMDShow]

      csTemp = "";

      GetPrivateProfileString("AddShortcut",lp2, "",
                              csTemp.GetBufferSetLength(MAX_PATH * 4),
                              MAX_PATH * 4, m_csFileName);

      csTemp.ReleaseBuffer(-1);

      if(csTemp.GetLength())
      {
        DoParseString(csTemp, acsColumns);

        if(acsColumns.GetSize() < 7)
        {
          AfxMessageBox("WARNING:  too few columns in '.giz' file ([AddShortcut] section) for "
                        + (CString)lp2,
                        MB_OK | MB_ICONHAND | MB_SETFOREGROUND);
        }

        sfi.csShortcutDescription = acsColumns[0];
        sfi.csShortcutFileName = acsColumns[1];
        sfi.csStartPath = acsColumns[2];
        sfi.csArgs = acsColumns[3];
        sfi.csIconFile = acsColumns[4];

        if(acsColumns[5].GetLength())
          sfi.uiIconIndex = atoi(acsColumns[5]);
        else
          sfi.uiIconIndex = (UINT)-1;

        if(acsColumns[6].GetLength())
          sfi.iShow = atoi(acsColumns[6]);
        else
          sfi.iShow = SW_SHOWNORMAL;
      }
    }

    // NOW, generate the "default" shortcut information (as applicable) - this
    // automatically converts 'old style .giz' files or assigns missing info

    GenerateDefaultShortcutInfo(sfi);

    m_aFileList.Add(sfi);
  }

  csTemp2.ReleaseBuffer(0);


  // NEXT, do 'groups' (same way)

  // get a list of keys, first

  lp1 = csTemp2.GetBufferSetLength(65536);  // max buffer for these funcitons

  GetPrivateProfileString("GroupList",NULL,"",
                          lp1, 65535, m_csFileName);

  // for each key, get the information

  for(lp2=lp1; *lp2; lp2 += lstrlen(lp2) + 1)
  {
    csTemp = "";

    GetPrivateProfileString("GroupList",lp2, "",
                            csTemp.GetBufferSetLength(MAX_PATH),
                            MAX_PATH, m_csFileName);

    csTemp.ReleaseBuffer(-1);

    // NOTE:  quote marks get stripped off.... boo!

    if(csTemp.GetLength() && csTemp[0] != '"')
      csTemp = '"' + csTemp;

    if(csTemp.GetLength() && csTemp[csTemp.GetLength() - 1] != '"')
      csTemp = csTemp + '"';

    CStringArray acsColumns;

    DoParseString(csTemp, acsColumns);


    if(acsColumns.GetSize() < 2)
    {
      AfxMessageBox("WARNING:  too few columns in '.giz' file for "
                    + (CString)lp2,
                    MB_OK | MB_ICONHAND | MB_SETFOREGROUND);
      continue;  // skip it
    }

    SetupGroupInfo sgi;

    sgi.csGroupName = lp2;
    sgi.csDesc = acsColumns[0];
    sgi.csLongDesc = acsColumns[1];

    m_aGroupList.Add(sgi);
  }

  csTemp2.ReleaseBuffer(0);


  // Finally, do 'registry keys' (similar method)

  // get a list of keys, first

  lp1 = csTemp2.GetBufferSetLength(65536);  // max buffer for these funcitons

  GetPrivateProfileString("RegistryList",NULL,"",
                          lp1, 65535, m_csFileName);

  // for each key, get the information

  for(lp2=lp1; *lp2; lp2 += lstrlen(lp2) + 1)
  {
    csTemp = "";

    GetPrivateProfileString("RegistryList",lp2, "",
                            csTemp.GetBufferSetLength(MAX_PATH),
                            MAX_PATH, m_csFileName);

    csTemp.ReleaseBuffer(-1);

    if(csTemp.GetLength())
    {
      m_acsRegistry.Add(csTemp);
    }
  }

  csTemp2.ReleaseBuffer(0);


  // TODO:  any other stuff goes here


}

BOOL CSetupGizDlg::DoSave()
{
  // Write output to an INI file

  CString csOldPath = GetCurrentDirectory();

  CString csTemp = "";

  if(m_csFileName.GetLength())
  {
    // get the long file name for this one...
    csTemp = MyGetLongPathName(m_csFileName);

    if(!csTemp.GetLength())  // an error
    {
      csTemp = m_csFileName;  // use the 'short name'
    }
  }
  
  if(csTemp.GetLength())
  {
    int i1 = csTemp.ReverseFind('\\');
    if(i1 >= 0)
    {
      if(i1 > 3)
      {
        SetCurrentDirectory(csTemp.Left(i1));
      }
      else
      {
        SetCurrentDirectory(csTemp.Left(i1 + 1));
      }

      csTemp = csTemp.Mid(i1 + 1);
    }
  }

  if(!csTemp.GetLength())
  {
    csTemp = "noname.giz";
  }


  CFileDialog dlg(FALSE, "giz", csTemp,
                  OFN_HIDEREADONLY,
                  "Setup Gizmo File (*.giz)|*.giz|All Files (*.*)|*.*||");

  CString csCurDir = GetCurrentDirectory();

  if(csCurDir.GetLength())
    dlg.m_ofn.lpstrInitialDir = (LPCSTR)csCurDir;

  dlg.m_ofn.lpstrTitle = "SELECT SetupGiz SAVE FILE";

  BOOL bOK = (dlg.DoModal() == IDOK);

  SetCurrentDirectory(csOldPath);

  if(bOK)
  {
    CString csPath = dlg.GetPathName();
    int i1, i2;

    DeleteFile(csPath);  // in case it exists already

    // page 1 stuff

    WritePrivateProfileString("Page1","AppName",
                              m_Page1.m_csAppName,
                              csPath);

    WritePrivateProfileString("Page1","CompanyName",
                              m_Page1.m_csCompanyName,
                              csPath);

    WritePrivateProfileString("Page1","SourcePath",
                              m_Page1.m_csSourcePath,
                              csPath);

    WritePrivateProfileString("Page1","DestPath",
                              m_Page1.m_csDestPath,
                              csPath);

    WritePrivateProfileString("Page1","LicenseFile",
                              m_Page1.m_csLicenseFile,
                              csPath);

    // hack - if m_Page1.m_csCertFiles starts with '"', prefix a '\'
    // this is because 'GetPrivateProfilString()' trims the quote
    // and if I'm quoting multiple files, I want it left ALONE

    if(m_Page1.m_csCertFiles.GetLength() > 0 &&
       m_Page1.m_csCertFiles.GetAt(0) == '"')
    {
      WritePrivateProfileString("Page1","CertFiles",
                                (CString)'\\' + m_Page1.m_csCertFiles,
                                csPath);
    }
    else
    {
      WritePrivateProfileString("Page1","CertFiles",
                                m_Page1.m_csCertFiles,
                                csPath);
    }

    WritePrivateProfileString("Page1","StartMessage",
                              m_Page1.m_csStartMessage,
                              csPath);

    WritePrivateProfileString("Page1","AppPath",
                              m_Page1.m_csAppPath,
                              csPath);

    WritePrivateProfileString("Page1","AppCommon",
                              m_Page1.m_csAppCommon,
                              csPath);

    WritePrivateProfileString("Page1","StartMenuFolderName",
                              m_Page1.m_csStartMenu,
                              csPath);

    // cert file info
    for(i1=0; i1 < m_acsCertFileNameList.GetSize(); i1++)
    {
      WritePrivateProfileString("CertFiles",m_acsCertFileNameList[i1],
                                m_acsCertRepositoryList[i1],
                                csPath);
    }

    csTemp = "";

    for(i1=0; i1 < m_Page1.m_acsUpgrade.GetSize(); i1++)
    {
      if(csTemp.GetLength())
        csTemp += ',';

      csTemp += '"';

      for(i2=0; i2 < m_Page1.m_acsUpgrade[i1].GetLength(); i2++)
      {
        if(m_Page1.m_acsUpgrade[i1].GetAt(i2) == '"')
          csTemp += "\"\"";
        else
          csTemp += m_Page1.m_acsUpgrade[i1].GetAt(i2);
      }

      csTemp += '"';
    }

    WritePrivateProfileString("Page1","Upgrade",
                              csTemp,
                              csPath);



    // files

    for(i1=0; i1 < m_aFileList.GetSize(); i1++)
    {
      // TODO:  double embedded quotes... ?

      csTemp.Format("%c,%c,%c,%c,%d,\"%s\",\"%s\",%ld,%c,%c,%c,%s,%s",
                    (char)(m_aFileList[i1].bAutoDelete ? '1' : '0'),
                    (char)(m_aFileList[i1].bExecute ? '1' : '0'),
                    (char)(m_aFileList[i1].bOverwrite ? '1' : '0'),
                    (char)(m_aFileList[i1].bShared ? '1' : '0'),
                    m_aFileList[i1].iDestPath,
                    (LPCSTR)m_aFileList[i1].csGroupName,
                    (LPCSTR)m_aFileList[i1].csOtherPathName,
                    m_aFileList[i1].dwOSFlag,
                    (char)(m_aFileList[i1].bNoUninstall ? '1' : '0'),
                    (char)(m_aFileList[i1].bRegister ?
                           (m_aFileList[i1].bRegDotNet ? '2' : '1')
                           : '0'),
                    (char)(m_aFileList[i1].bAddShortcut ? '1' : '0'),
                    (LPCSTR)(m_aFileList[i1].bExecute ?
                             (LPCSTR)m_aFileList[i1].csExecArgs : ""),
                    (LPCSTR)(m_aFileList[i1].bExecute &&
                             !m_aFileList[i1].bNoUninstall ?
                             (LPCSTR)m_aFileList[i1].csUninstArgs : ""));

      WritePrivateProfileString("FileList",
                                m_aFileList[i1].csFileName,
                                csTemp, csPath);

      if(!m_aFileList[i1].bAddShortcut)
      {
        WritePrivateProfileString("AddShortcut", m_aFileList[i1].csFileName,
                                  NULL, csPath);  // this should delete it
      }
      else
      {
        // [AddShortcut]
        // srcfilename=Description,ShortcutName[,StartPath,Args,IconFile,IconIndex,nCMDShow]

        csTemp.Format("%ld,%d", m_aFileList[i1].uiIconIndex, m_aFileList[i1].iShow);

        csTemp = "\""
               + m_aFileList[i1].csShortcutDescription
               + "\",\""
               + m_aFileList[i1].csShortcutFileName
               + "\",\""
               + m_aFileList[i1].csStartPath
               + "\",\""
               + m_aFileList[i1].csArgs
               + "\",\""
               + m_aFileList[i1].csIconFile
               + "\","
               + csTemp;

        WritePrivateProfileString("AddShortcut", m_aFileList[i1].csFileName,
                                  csTemp, csPath);
      }
    }

    // groups

    for(i1=0; i1 < m_aGroupList.GetSize(); i1++)
    {
      // TODO:  double embedded quotes...

      csTemp = "\"" 
             + m_aGroupList[i1].csDesc 
             + "\",\""
             + m_aGroupList[i1].csLongDesc 
             + "\"";

      WritePrivateProfileString("GroupList",
                                m_aGroupList[i1].csGroupName,
                                csTemp, csPath);
    }

    // registry keys

    for(i1=0; i1 < m_acsRegistry.GetSize(); i1++)
    {
      csTemp.Format("%03d", i1);

      WritePrivateProfileString("RegistryList", csTemp,
                                m_acsRegistry[i1],
                                csPath);
    }

    m_csFileName = csPath;  // save the file name for later
  }

  return(bOK);
}


BEGIN_MESSAGE_MAP(CSetupGizDlg, CPropertySheet)
	//{{AFX_MSG_MAP(CSetupGizDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetupGizDlg message handlers

BOOL CSetupGizDlg::OnInitDialog()
{
  CPropertySheet::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);
	ASSERT((IDM_REGISTER & 0xFFF0) == IDM_REGISTER);
	ASSERT(IDM_REGISTER < 0xF000);

  // also 'IDM_REGISTER' - need that to make sure users can open
  // the 'registration' dialog box and re-register the product

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu, strRegisterMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		strRegisterMenu.LoadString(IDS_REGISTER);

    if(theApp.m_csAuthCompany.GetLength())
      strRegisterMenu.LoadString(IDS_REREGISTER);

		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
      pSysMenu->AppendMenu(MF_STRING, IDM_REGISTER, strRegisterMenu);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CSetupGizDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else if ((nID & 0xFFF0) == IDM_REGISTER)
	{
    if(theApp.m_csAuthCompany.GetLength())
    {
      CString csTemp, csTemp2;
      csTemp.LoadString(IDS_REREGINFO1);
      csTemp2.LoadString(IDS_REREGINFO2);
      csTemp = csTemp + theApp.m_csAuthCompany + csTemp2;

//      if(AfxMessageBox("You are already registered for this product as \""
//                       + theApp.m_csAuthCompany + "\".\n"
//                       "If you want to re-register this product using a different name, please obtain a\n"
//                       "new authorization code first.\n\n"
//                       "You can obtain a new authorization code by sending e-mail to:\n\n"
//                       "    SetupGizmo@mrp3.com\n\n"
//                       "with 'RE-REGISTER SETUP GIZMO' in the subject line.  You must include your existing\n"
//                       "registration info (name and authorization code) in the message body, as well as\n"
//                       "the new name under which you want to register the product.\n\n"
//                       "Press 'Yes' to continue, 'No' to return to the application.",
      if(AfxMessageBox(csTemp,
                       MB_YESNO | MB_ICONEXCLAMATION | MB_SETFOREGROUND)
         == IDYES)
      {
        CNagDialog dlgNag;

        dlgNag.m_csAuthCompany = theApp.m_csAuthCompany;
//        dlgNag.m_csAuthCode = theApp.m_csAuthCode;

        dlgNag.DoModal();
      }
    }
	}
	else
	{
		CPropertySheet::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CSetupGizDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CPropertySheet::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CSetupGizDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CSetupGizDlg::WinHelp(DWORD dwData, UINT nCmd) 
{
//  if(nCmd != HELP_CONTEXT)
//	  TRACE("WINHELP called with dwData = %08xH,%ld and nCmd=%08xH,%ld\n",
//          dwData,dwData,nCmd,nCmd);
//  else
//    TRACE("WINHELP called with dwData = %08xH,%ld and nCmd=HELP_CONTEXT\n",
//          dwData,dwData);
//	
//  CPropertySheet::WinHelp(dwData, nCmd);
  CPropertySheet::WinHelp(0, HELP_CONTENTS);
}

void CSetupGizDlg::UpdatePage1Stuff(BOOL bLoadFromFile /*= FALSE*/)
{
int i1, i2, i3, i4;
CString csTemp;
CStringArray acsFileNames, acsRepository;
LPSTR lp1, lp2;


  if(bLoadFromFile) // reading from a GIZ file
  {
    m_acsCertFileNameList.RemoveAll();
    m_acsCertRepositoryList.RemoveAll();

    // parse 'm_csCertFiles' into 'm_acsCertFileNameList'

    csTemp = m_Page1.m_csCertFiles;

    lp1 = csTemp.GetBuffer(csTemp.GetLength() + 2);
    lp1[csTemp.GetLength()] = 0; // make sure
    lp1[csTemp.GetLength() + 1] = 0; // make sure

    InPlaceConvertMultStringToStringList(lp1, csTemp.GetLength() + 2);

    for(lp2=lp1; *lp2; lp2 += strlen(lp2) + 1)
    {
      m_acsCertFileNameList.Add(lp2);
    }

    csTemp.ReleaseBuffer(0); // clears it

    for(i1=0, i2 = m_acsCertFileNameList.GetSize(); i1 < i2; i1++)
    {
      csTemp = "";
      if(m_acsCertFileNameList[i1].GetLength()) // just in case
      {
        GetPrivateProfileString("CertRepo", m_acsCertFileNameList[i1], "",
                                csTemp.GetBufferSetLength(MAX_PATH),
                                MAX_PATH, m_csFileName);

        csTemp.ReleaseBuffer(-1);

        if(!csTemp.GetLength())
        {
          csTemp = "ROOT"; // the default
        }
      }

      m_acsCertRepositoryList.SetAtGrow(i1, csTemp);
    }

    return;
  }

  // manage temp arrays by copying relevant data from original
  // list, and using 'ROOT' for the default repository on new items

  csTemp = m_Page1.m_csCertFiles;

  lp1 = csTemp.GetBuffer(csTemp.GetLength() + 2);
  lp1[csTemp.GetLength()] = 0; // make sure
  lp1[csTemp.GetLength() + 1] = 0; // make sure

  InPlaceConvertMultStringToStringList(lp1, csTemp.GetLength() + 2);

  for(lp2=lp1; *lp2; lp2 += strlen(lp2) + 1)
  {
    acsFileNames.Add(lp2);
  }

  csTemp.ReleaseBuffer(0); // clears it

  // TODO:  should I cache the 'friendly name' of the cert??

  for(i1=0, i2=acsFileNames.GetSize(); i1 < i2; i1++)
  {
    csTemp = acsFileNames[i1];
    for(i3=0, i4=m_acsCertFileNameList.GetSize(); i3 < i4; i3++)
    {
      if(!csTemp.CompareNoCase(m_acsCertFileNameList[i3]))
      {
        break; // file name match
      }
    }

    if(i3 < i4)
    {
      acsRepository.SetAtGrow(i1, m_acsCertRepositoryList[i3]);
    }
    else
    {
      acsRepository.SetAtGrow(i1, "ROOT");
    }
  }

  // copy temp arrays back into the main ones

  m_acsCertFileNameList.RemoveAll();
  m_acsCertRepositoryList.RemoveAll();

  for(i1=0; i1 < i2; i1++)
  {
    m_acsCertFileNameList.SetAtGrow(i1, acsFileNames[i1]);
    m_acsCertRepositoryList.SetAtGrow(i1, acsRepository[i1]);
  }

  // done now
}
