#pragma once

class CDirectDialog :
	public WindowImplBase
{
public:
	CDirectDialog(void);
	CDirectDialog::CDirectDialog(CDuiString sSkinFileName , CDuiString sSkinDir = _T("skin"));
	~CDirectDialog(void);

	
	virtual void OnFinalMessage(HWND hWnd);
	virtual void InitWindow();
	virtual LRESULT ResponseDefaultKeyEvent(WPARAM wParam);
	virtual LPCTSTR GetWindowClassName() const;
	virtual CDuiString GetSkinFile();
	virtual CDuiString GetSkinFolder();
	virtual UILIB_RESOURCETYPE GetResourceType() const;
	virtual LPCTSTR GetResourceID() const;
	virtual CControlUI* CreateControl(LPCTSTR pstrClass);
	//virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual void OnClick(TNotifyUI& msg);
	//virtual LRESULT OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	//virtual LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual void OnItemActivate(TNotifyUI& msg);
	virtual void OnItemSelect(TNotifyUI& msg);

	int ShowWnd(LPCTSTR sTilte,bool bIsShowModal = false, bool bIsShowInTaskbar = true);
	int ShowWnd(HWND hParent,LPCTSTR sTilte,bool bIsShowModal = false, bool bIsShowInTaskbar = true);

	void CopyHdc(HDC hdc,RECT rect);
protected:	
	virtual void Notify(TNotifyUI& msg);
	virtual void OnPrepare(TNotifyUI& msg);
	virtual void OnValueChanged( TNotifyUI& msg );
	virtual void OnValueChanging( TNotifyUI& msg );
private:
	static int m_nClass;
	CDuiString m_sSkinDir;
	CDuiString m_sSkinFileName; //窗口的皮肤文件
};

