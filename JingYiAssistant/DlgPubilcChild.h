#pragma once
class CDirectDialog;
typedef struct{
	string sTitle;
	string sHrefUrl;
	string sOwner;
	string sOwnerUrl;
	string sPushedTime;
	string sEndTime;
	string sPrice;

}Order;


class CDlgPubilcChild :
	public CDirectDialog
{
public:
	CDlgPubilcChild(void);
	CDlgPubilcChild(CDuiString sSkinFileName,CDuiString sSkinDir = _T("skin"));
	~CDlgPubilcChild(void);

	virtual void InitWindow();
	virtual void OnClick(TNotifyUI& msg);
	virtual void OnItemActivate(TNotifyUI& msg);
	virtual void OnItemSelect(TNotifyUI& msg);
	virtual void OnValueChanged( TNotifyUI& msg );
	virtual void OnValueChanging( TNotifyUI& msg );
	virtual LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	void Message(CDuiString sMsg);
	void InitLoginWindow();

	void OnBtnListItemDeleteClick(TNotifyUI& msg);
	void OnBtnListItemStopClick(TNotifyUI& msg);
	void OnBtnListItemOpenDirClick(TNotifyUI& msg);
	void OnRecordClick(TNotifyUI& msg);
	static void RunCheckThread(void* pDlg);
	void RunCheck();
	void AndItem(Order& order);
	void UpdateListItem(CListContainerElementUI *pListItem,Order& tOrder);
	void PlayTextSound(string sText);
	void OnTrayNotification(WPARAM wParam, LPARAM lParam);
	string GetProxyIP(int ListId);
	bool m_bRunning;
	string sHttpPort;
	string m_sHttpProxyPort;
	HANDLE m_MutexHandle;

	UINT WM_TASKRESTARTED;
	vector<Order> m_vAllList;
	vector<string> vAllProxyIp;
private:
	POINT m_LastPos;
};

