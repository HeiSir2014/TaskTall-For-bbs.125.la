#include "std_main.h"
#include "DlgPubilcChild.h"

#define WM_SHOWTASK WM_USER+0x87

CDlgPubilcChild::CDlgPubilcChild(void)
{
	m_bRunning = true;
	sHttpPort = "0";
	WM_TASKRESTARTED = (UINT)-1;
	m_MutexHandle = NULL;
}

CDlgPubilcChild::CDlgPubilcChild(CDuiString sSkinFileName,CDuiString sSkinDir):CDirectDialog(sSkinFileName,sSkinDir)
{
	m_bRunning = true;
	sHttpPort = "0";
	WM_TASKRESTARTED = (UINT)-1;
	m_MutexHandle = NULL;
}

CDlgPubilcChild::~CDlgPubilcChild(void)
{
	if (m_MutexHandle)
	{
		CloseHandle( m_MutexHandle);
		m_MutexHandle = NULL;
	}
}

void CDlgPubilcChild::InitWindow()
{
	if( GetSkinFile() == _T("UI_Main.xml"))
	{
		m_bRunning = true;

		_beginthread(RunCheckThread,0,this);

		NOTIFYICONDATA nid ;
		nid.cbSize = (DWORD)sizeof(NOTIFYICONDATA);// 结构的长度，用“位”来做单位。
		nid.hWnd = this->m_hWnd;//消息传送的窗口句柄
		nid.uID = IDI_ICON_MAIN;//在工程中定义的图标ID
		nid.uFlags = NIF_ICON|NIF_MESSAGE|NIF_TIP;
		nid.uCallbackMessage = WM_SHOWTASK;//自定义的消息名称
		nid.hIcon = LoadIcon(CPaintManagerUI::GetInstance() ,MAKEINTRESOURCE(IDI_ICON_MAIN));
		_tcscpy_s(nid.szTip,sizeof(nid.szTip)+1,_T("精易开发者助手"));//信息提示条为“备份客户端”
		Shell_NotifyIcon(NIM_ADD,&nid);//在托盘区添加图标

		WM_TASKRESTARTED = RegisterWindowMessage(_T("TaskbarCreated"));
	}
}

LRESULT CDlgPubilcChild::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (wParam == 0)
	{
		bHandled = TRUE;
		if( GetSkinFile() == _T("UI_Main.xml"))
		{
			if (::MessageBox(this->GetHWND(),_T("是否真的要退出?"),_T("退出"),MB_YESNO|MB_ICONINFORMATION) == IDYES)
			{
				m_bRunning = false;

				NOTIFYICONDATA nid ;
				nid.cbSize = (DWORD)sizeof(NOTIFYICONDATA);// 结构的长度，用“位”来做单位。
				nid.hWnd = this->m_hWnd;//消息传送的窗口句柄
				nid.uID = IDI_ICON_MAIN;//在工程中定义的图标ID
				nid.uFlags = NIF_ICON|NIF_MESSAGE|NIF_TIP;
				nid.uCallbackMessage = WM_SHOWTASK;//自定义的消息名称
				nid.hIcon = NULL;
				Shell_NotifyIcon(NIM_DELETE,&nid);//在托盘区添加图标
				PostQuitMessage(0);
			}
		}
		else
		{
			PostQuitMessage(0);
		}
	}
	else if(wParam == 2)
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

void CDlgPubilcChild::OnClick(TNotifyUI& msg)
{
	CDuiString sName = msg.pSender->GetName();
	CDlgPubilcChild* dlg = NULL;

	if (sName.IsEmpty())
	{
		return;
	}
	else if (sName == _T("BtnExit"))
	{
		Close(1);
	}
	else if(GetSkinFile() == _T("UI_Main.xml"))
	{
		OnRecordClick(msg);
	}
	if (dlg)
	{
		dlg->ShowWnd( *this ,_T("") , true , false );
	}

	CDirectDialog::OnClick(msg);
}


LRESULT CDlgPubilcChild::OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return CDirectDialog::OnMouseMove(uMsg,  wParam,  lParam, bHandled);
}

LRESULT CDlgPubilcChild::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (WM_SHOWTASK == uMsg)
	{

		OnTrayNotification(wParam,lParam);
	}
	else if (WM_TASKRESTARTED == uMsg)
	{
		NOTIFYICONDATA nid ;
		nid.cbSize = (DWORD)sizeof(NOTIFYICONDATA);// 结构的长度，用“位”来做单位。
		nid.hWnd = this->m_hWnd;//消息传送的窗口句柄
		nid.uID = IDI_ICON_MAIN;//在工程中定义的图标ID
		nid.uFlags = NIF_ICON|NIF_MESSAGE|NIF_TIP;
		nid.uCallbackMessage = WM_SHOWTASK;//自定义的消息名称
		nid.hIcon = LoadIcon(CPaintManagerUI::GetInstance() ,MAKEINTRESOURCE(IDI_ICON_MAIN));
		_tcscpy_s(nid.szTip,sizeof(nid.szTip)+1,_T("精易开发者助手"));//信息提示条为“备份客户端”
		Shell_NotifyIcon(NIM_ADD,&nid);//在托盘区添加图标
	}

	return CDirectDialog::HandleMessage(uMsg,wParam,lParam);
}

void CDlgPubilcChild::OnItemSelect( TNotifyUI& msg )
{
	
}

void CDlgPubilcChild::OnValueChanged( TNotifyUI& msg )
{
	
}

void CDlgPubilcChild::OnValueChanging( TNotifyUI& msg )
{
	
}

void CDlgPubilcChild::OnItemActivate(TNotifyUI& msg)
{
	
}

void CDlgPubilcChild::Message(CDuiString sMsg)
{
	CLabelUI* tMsg = (CLabelUI*) m_PaintManager.FindControl(_T("message"));
	if (tMsg)
	{
		tMsg->SetText(sMsg);
	}
}

void CDlgPubilcChild::OnTrayNotification(WPARAM wParam, LPARAM lParam)
{
	if (lParam == WM_LBUTTONDBLCLK)
	{
		::ShowWindow(GetHWND(),SW_SHOW);
		::SetForegroundWindow(GetHWND());
	}
}

LRESULT CDlgPubilcChild::OnSysCommand( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	if (wParam == SC_MINIMIZE)
	{
		::ShowWindow(GetHWND(),SW_HIDE);
		bHandled = true;
		return 0;
	}
	return CDirectDialog::OnSysCommand(uMsg,wParam,lParam,bHandled);
}
