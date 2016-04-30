//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//   ___                       _    ____   _                                //
//  |_ _| _ __   _ __   _   _ | |_ |  _ \ | |  __ _     ___  _ __   _ __    //
//   | | | '_ \ | '_ \ | | | || __|| | | || | / _` |   / __|| '_ \ | '_ \   //
//   | | | | | || |_) || |_| || |_ | |_| || || (_| | _| (__ | |_) || |_) |  //
//  |___||_| |_|| .__/  \__,_| \__||____/ |_| \__, |(_)\___|| .__/ | .__/   //
//              |_|                           |___/         |_|    |_|      //
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
#include "InputDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInputDlg dialog


CInputDlg::CInputDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CInputDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CInputDlg)
	m_csCaption = _T("");
	m_csInput = _T("");
	//}}AFX_DATA_INIT
}


void CInputDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInputDlg)
	DDX_Text(pDX, IDC_CAPTION, m_csCaption);
	DDX_Text(pDX, IDC_INPUT, m_csInput);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CInputDlg, CDialog)
	//{{AFX_MSG_MAP(CInputDlg)
	ON_BN_CLICKED(IDHELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInputDlg message handlers

BOOL CInputDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
  SetWindowText(m_csTitle);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CInputDlg::OnOK() 
{
  UpdateData(1);

  // TODO:  validate?
	
	CDialog::OnOK();
}

void CInputDlg::OnCancel() 
{
	CDialog::OnCancel();
}

void CInputDlg::OnHelp() 
{
  CDialog::OnHelp();
}
