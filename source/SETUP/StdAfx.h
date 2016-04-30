//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//              ____   _       _     _      __           _                  //
//             / ___| | |_  __| |   / \    / _|__  __   | |__               //
//             \___ \ | __|/ _` |  / _ \  | |_ \ \/ /   | '_ \              //
//              ___) || |_| (_| | / ___ \ |  _| >  <  _ | | | |             //
//             |____/  \__|\__,_|/_/   \_\|_|  /_/\_\(_)|_| |_|             //
//                                                                          //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//       Copyright (c) 1998-2016 by S.F.T. Inc. - All rights reserved       //
//  Use, copying, and distribution of this software are licensed according  //
//    to the GPLv2, LGPLv2, or BSD license, as appropriate (see COPYING)    //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////


#if !defined(AFX_STDAFX_H__B3134EED_0D1F_11D2_A115_004033901FF3__INCLUDED_)
#define AFX_STDAFX_H__B3134EED_0D1F_11D2_A115_004033901FF3__INCLUDED_

#define _AFX_NO_OCC_SUPPORT
#define _AFX_NO_RICHEDIT_SUPPORT
#define _AFX_NO_AFXCMN_SUPPORT
#define _WIN32_IE 0x0501 /* lowest compatibility for SHELL DLL's */
#define _WIN32_WINNT 0x0501 /* lowest compatibility for WINDOWS API */

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// USED TO USE MFC, but not any more.  It *MUST* be statically linked, and
// JUST having MFC linked adds ~1.5Mb to the image size.  This was NOT the
// case with DevStudio '98, but *IS* the case in DevStudio 2010.  "progress"

// now using 'ClassLib' that I wrote to behave similar to MFC where it matters

#include "../ClassLib/ClassLib.h"
#include "../ClassLib/WindowHelpers.h"
#include "../ClassLib/DialogHelpers.h"

#include "objbase.h"        // COM stuff
#include "shlguid.h"        // required for 'sftsetup' stuff
#include "shlobj.h"
#include "windowsx.h"
#include "dde.h"
#include "ddeml.h"



//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__B3134EED_0D1F_11D2_A115_004033901FF3__INCLUDED_)
