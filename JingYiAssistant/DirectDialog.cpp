#include "std_main.h"
#include "DirectDialog.h"

#define kSettingButtonControlName _T("settingbtn")
#define kMinButtonControlName _T("minbtn")
#define kMaxButtonControlName _T("maxbtn")
#define kRestoreButtonControlName _T("restorebtn")
#define kCloseButtonControlName _T("closebtn")


int CDirectDialog::m_nClass = 0;

CDirectDialog::CDirectDialog(void)
{
	m_sSkinFileName.Empty();
}

CDirectDialog::CDirectDialog(CDuiString sSkinFileName,CDuiString sSkinDir)
{
	m_sSkinFileName.Empty();
	m_sSkinFileName = sSkinFileName;
	m_sSkinDir		= sSkinDir;
}

CDirectDialog::~CDirectDialog(void)
{
	CDuiString log=_T("Close & Delete ") ;
	log += m_sSkinFileName;
	LOGDEBUG(log.GetData())
}

LPCTSTR CDirectDialog::GetWindowClassName() const
{
	return _T("JingYiAssistantClass");
}

CControlUI* CDirectDialog::CreateControl(LPCTSTR pstrClass)
{
	return NULL;
}

void CDirectDialog::OnFinalMessage(HWND hWnd)
{
	WindowImplBase::OnFinalMessage(hWnd);	
	delete this;
}


CDuiString CDirectDialog::GetSkinFile()
{
	return m_sSkinFileName;
}

CDuiString CDirectDialog::GetSkinFolder()
{
	return CDuiString(CPaintManagerUI::GetInstancePath()) + m_sSkinDir;
}

UILIB_RESOURCETYPE CDirectDialog::GetResourceType() const
{
	//return UILIB_FILE;
	return UILIB_ZIPRESOURCE;
}

LPCTSTR CDirectDialog::GetResourceID() const
{
	return  MAKEINTRESOURCE(IDR_ZIPRE_SKIN) ;
}

void CDirectDialog::InitWindow()
{

}

void CDirectDialog::OnPrepare(TNotifyUI& msg)
{

}

//LRESULT CDirectDialog::OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
//{
//	bHandled = TRUE;
//	return 0;
//}

int CDirectDialog::ShowWnd(LPCTSTR sTilte,bool bIsShowModal, bool bIsShowInTaskbar)
{
	return ShowWnd(NULL,sTilte,bIsShowModal,bIsShowInTaskbar);
}

int CDirectDialog::ShowWnd(HWND hParent,LPCTSTR sTilte,bool bIsShowModal, bool bIsShowInTaskbar)
{
	//DWORD Style =UI_WNDSTYLE_DIALOG;
	//Style &= ~WS_DLGFRAME;
	CDuiString log = _T("Load & Create ") ;
	log += m_sSkinFileName;
	LOGDEBUG(log.GetData());
	
	DWORD Style ;
	
	//Style = bIsShowInTaskbar ? UI_WNDSTYLE_FRAME : ( UI_WNDSTYLE_DIALOG & ~WS_DLGFRAME) ;
	//Style = bIsShowInTaskbar ? UI_WNDSTYLE_FRAME : ( WS_THICKFRAME | UI_WNDSTYLE_CHILD ) ;
	Style = bIsShowInTaskbar ? UI_WNDSTYLE_FRAME : ( WS_THICKFRAME ) ;

	HWND tHwnd = Create(hParent, sTilte == NULL ? _T("HeiSir") : sTilte , Style  , bIsShowInTaskbar ? WS_EX_STATICEDGE | WS_EX_APPWINDOW : NULL, 0, 0, 0, 0);
	if (::IsWindow(tHwnd) )
	{
		SetIcon( IDI_ICON_MAIN );

		CenterWindow();

		if (bIsShowModal)
		{
			return ShowModal();
		}
		else
		{
			ShowWindow();
			return 0;
		}
	}
	return -1;
}

LRESULT CDirectDialog::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (wParam == 0)
	{
		bHandled = TRUE;
		PostQuitMessage(0);
	}
	else
	{
		bHandled = FALSE;
	}
	return 0;
}

LRESULT CDirectDialog::OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	
	DUITRACE(_T("OnSysCommand: %d,%d,%d"),uMsg,wParam,lParam);


	return WindowImplBase::OnSysCommand(uMsg, wParam,  lParam, bHandled);
}
//
//LRESULT CDirectDialog::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
//{
//	bHandled = TRUE;
//	return 0;
//}

LRESULT CDirectDialog::ResponseDefaultKeyEvent(WPARAM wParam)
{
	if (wParam == VK_RETURN)
	{
		return FALSE;
	}
	else if (wParam == VK_ESCAPE)
	{
		return TRUE;
	}
	return FALSE;
}

void CDirectDialog::OnClick(TNotifyUI& msg)
{
	if (msg.pSender->GetName() == kCloseButtonControlName )
	{
		Close(0);
	}
	else if (msg.pSender->GetName() == kSettingButtonControlName )
	{
		
	}
	else if (msg.pSender->GetName() == kMinButtonControlName )
	{
		SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
	}
	else if (msg.pSender->GetName() == kMaxButtonControlName )
	{
		SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);
	}
	else if (msg.pSender->GetName() == kRestoreButtonControlName )
	{
		SendMessage(WM_SYSCOMMAND, SC_RESTORE, 0);
	}
	else if (msg.pSender->GetName() == _T("BtnExit"))
	{
		Close(1);
	}
	else if(msg.pSender->GetName() == _T("BtnLogin"))
	{
		Close(2);
	}
}

void CDirectDialog::CopyHdc(HDC hdc,RECT rcw)
{
	CButtonUI* btn = (CButtonUI*)m_PaintManager.FindControl(_T("BtnLogin"));
	
	//::StretchBlt(hdc, rcw.left, rcw.top,rcw.right -  rcw.left, rcw.bottom - rcw.top, \
	//	m_PaintManager.GetPaintDC(),btn->GetPos().left, btn->GetPos().top, \
	//	btn->GetWidth(), btn->GetHeight(), SRCCOPY);
	
	::StretchBlt(hdc, rcw.left, rcw.top,rcw.right -  rcw.left, rcw.bottom - rcw.top, \
		m_PaintManager.GetPaintDC(),0, 0, \
		m_PaintManager.GetClientSize().cx, m_PaintManager.GetClientSize().cy, SRCCOPY);
}


void CDirectDialog::Notify(TNotifyUI& msg)
{
	if (_tcsicmp(msg.sType, _T("windowinit")) == 0)
	{
		OnPrepare(msg);
	}
//	else if (_tcsicmp(msg.sType, _T("killfocus")) == 0)
//	{
//	}
//	else if (_tcsicmp(msg.sType, _T("click")) == 0)
//	{
//		OnClick(msg);
//	}
//	else if( _tcsicmp(msg.sType, _T("return")) == 0 ) 
//	{
//		
//	}
//	else if (_tcsicmp(msg.sType, _T("timer")) == 0)
//	{
//		
//	}
//	else if (_tcsicmp(msg.sType, _T("selectchanged")) == 0)
//	{
//		
//	}
//	else if (_tcsicmp(msg.sType, _T("itemselect")) == 0)
//	{
//
	//	}
	else if (_tcsicmp(msg.sType, _T("itemactivate")) == 0)
	{
		OnItemActivate(msg);
	}
	else if (_tcsicmp(msg.sType, _T("itemselect")) == 0)
	{
		OnItemSelect(msg);
	}
	else if (_tcsicmp(msg.sType, DUI_MSGTYPE_VALUECHANGED ) == 0)
	{
		OnValueChanged(msg);
	}
	else if (_tcsicmp(msg.sType, DUI_MSGTYPE_VALUECHANGING ) == 0)
	{
		OnValueChanging(msg);
	}

	WindowImplBase::Notify(msg);
}
//
//
//LRESULT CDirectDialog::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
//{
//	return __super::HandleMessage(uMsg, wParam, lParam);
//}

void CDirectDialog::OnItemActivate(TNotifyUI& msg)
{

}

void CDirectDialog::OnItemSelect( TNotifyUI& msg )
{

}

void CDirectDialog::OnValueChanged( TNotifyUI& msg )
{

}

void CDirectDialog::OnValueChanging( TNotifyUI& msg )
{

}
