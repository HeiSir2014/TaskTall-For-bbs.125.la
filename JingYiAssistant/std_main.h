
#if !defined(AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_)
#define AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_

#pragma once

#ifdef _WIN32
#include <direct.h>
#include <io.h>
#elif _LINUX
#include <stdarg.h>
#include <sys/stat.h>
#endif

#ifdef _WIN32
#define ACCESS _access
#define MKDIR(a) _mkdir((a))
#elif _LINUX
#define ACCESS access
#define MKDIR(a) mkdir((a),0755)
#endif

#pragma comment(lib, "DuiLib.lib")
#pragma comment(lib, "Dbghelp.lib")

#include <windows.h>
#include <objbase.h>
#include <zmouse.h>
#include <cmath>
#include <time.h>
#include <stdlib.h>
#include <iomanip>
#include <process.h>

#include "resource.h"
#include <UIlib.h>
using namespace DuiLib;

using DuiLib::CContainerUI;
using DuiLib::CPaintManagerUI;
using DuiLib::CWindowWnd;
using DuiLib::TNotifyUI;
using DuiLib::IMessageFilterUI;
using DuiLib::CStdPtrArray;
using DuiLib::CStdValArray;
using DuiLib::CStdStringPtrMap;
using DuiLib::CDuiString;
using DuiLib::TListInfoUI;
using DuiLib::CRenderEngine;
using DuiLib::CDialogBuilder;
using DuiLib::CControlUI;
using DuiLib::CButtonUI;
using DuiLib::CEditUI;
using DuiLib::CLabelUI;
using DuiLib::CTextUI;
using DuiLib::COptionUI;
using DuiLib::CComboUI;
using DuiLib::CListUI;
using DuiLib::CSliderUI;
using DuiLib::CProgressUI;
using DuiLib::CActiveXUI;
using DuiLib::CWebBrowserUI;
using DuiLib::CContainerUI;
using DuiLib::CChildLayoutUI;
using DuiLib::CVerticalLayoutUI;
using DuiLib::CHorizontalLayoutUI;
using DuiLib::CTabLayoutUI;
using DuiLib::CTileLayoutUI;
using DuiLib::CListHeaderUI;
using DuiLib::CListElementUI;
using DuiLib::CListHeaderItemUI;
using DuiLib::CListTextElementUI;
using DuiLib::CListLabelElementUI;
using DuiLib::CListContainerElementUI;
using DuiLib::CScrollBarUI;
using DuiLib::IContainerUI;
using DuiLib::STRINGorID;
//using DuiLib::TRelativePosUI;
using DuiLib::TImageInfo;
using DuiLib::TFontInfo;
using DuiLib::TEventUI;
using DuiLib::TDrawInfo;
using DuiLib::EVENTTYPE_UI;

#include "HttpLog.h"
#include "HttpPlus.h"
#include "md5lib.h"
#include "lib_json/include/json/json.h"

#include "DirectDialog.h"
#include "DlgPubilcChild.h"

#define LOGDEBUG(X) OutputDebugString(X);
#define THIS_GETOBJ(OBJNAME,OBJCLASS,OBJ_ID) \
	OBJCLASS* OBJNAME = (OBJCLASS*)this->m_PaintManager.FindControl(_T(OBJ_ID))

#define PTR_GETOBJ(PTRTHIS,OBJNAME,OBJCLASS,OBJ_ID) \
	OBJCLASS* OBJNAME = (OBJCLASS*)PTRTHIS->m_PaintManager.FindControl(_T(OBJ_ID))

#include "TTSSound.h"
#pragma comment(lib,"winmm.lib")

#define  VERIYF_DISABLE 1

#include "RegexPlus.h"
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_)
