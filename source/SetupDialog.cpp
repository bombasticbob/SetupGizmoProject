////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                            //
//   ____         _                  ____   _         _                                       //
//  / ___|   ___ | |_  _   _  _ __  |  _ \ (_)  __ _ | |  ___    __ _     ___  _ __   _ __    //
//  \___ \  / _ \| __|| | | || '_ \ | | | || | / _` || | / _ \  / _` |   / __|| '_ \ | '_ \   //
//   ___) ||  __/| |_ | |_| || |_) || |_| || || (_| || || (_) || (_| | _| (__ | |_) || |_) |  //
//  |____/  \___| \__| \__,_|| .__/ |____/ |_| \__,_||_| \___/  \__, |(_)\___|| .__/ | .__/   //
//                           |_|                                |___/         |_|    |_|      //
//                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                            //
//                Copyright (c) 1998-2016 by S.F.T. Inc. - All rights reserved                //
//           Use, copying, and distribution of this software are licensed according           //
//             to the GPLv2, LGPLv2, or BSD license, as appropriate (see COPYING)             //
//                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "setupgiz.h"
#include "SetupDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSetupDialog dialog


CSetupDialog::CSetupDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CSetupDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSetupDialog)
	m_csWelcome = _T("");
	//}}AFX_DATA_INIT
}


void CSetupDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSetupDialog)
	DDX_Text(pDX, IDC_WELCOME, m_csWelcome);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSetupDialog, CDialog)
	//{{AFX_MSG_MAP(CSetupDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetupDialog message handlers

BOOL CSetupDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
  SetWindowText(m_csTitle + " (TEST)");
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
