#include "std_main.h"
#include "DlgPubilcChild.h"

CHttpLog g_ZhiBoRecordLog("run.log",true,true);
CHttpLog g_ZhiBoRecordErrorLog("runError.log",true);
#define DEBUG_ g_ZhiBoRecordLog.log(log4cpp::Priority::DEBUG)
#define INFO_ g_ZhiBoRecordLog.log(log4cpp::Priority::INFO)
#define ERROR_ g_ZhiBoRecordErrorLog.log(log4cpp::Priority::ERROR)
#define WARN_ g_ZhiBoRecordLog.log(log4cpp::Priority::WARN)



void CDlgPubilcChild::OnRecordClick(TNotifyUI& msg)
{
	CDuiString sName = msg.pSender->GetName();
	CDlgPubilcChild* dlg = NULL;

	if (sName.IsEmpty())
	{
		return;
	}
	else if (sName == _T("btn_list_item_delete"))
	{
		OnBtnListItemDeleteClick(msg);
	}
	else if (sName == _T("btn_list_item_stop"))
	{
		OnBtnListItemStopClick(msg);
	}
	else if (sName == _T("btn_list_item_open") || sName == _T("btn_list_item_open_qq"))
	{
		OnBtnListItemOpenDirClick(msg);
	}
}


void CDlgPubilcChild::UpdateListItem(CListContainerElementUI *pListItem,Order& tOrder)
{
	if (pListItem)
	{
		CDuiString className = pListItem->GetClass();
		if ( className != _T("ListContainerElementUI"))
		{
			return;
		}

		CLabelUI* pLabelUI = NULL;
		CVerticalLayoutUI* pVerUI = NULL;
		CHorizontalLayoutUI* pHorUI = NULL;

		// �����ǳƣ������ �����ǳ�
		pVerUI = static_cast<CVerticalLayoutUI*>(pListItem->GetItemAt(1));
		if (pVerUI)
		{
			pLabelUI = static_cast<CLabelUI*>(pVerUI->GetItemAt(1));
			if ( pLabelUI )
			{
				pLabelUI->SetText( CHttpPlus::GBKToUNICODE(tOrder.sTitle).c_str() );
			}

			pHorUI = static_cast<CHorizontalLayoutUI*>(pVerUI->GetItemAt(2));
			if ( pHorUI )
			{
				pLabelUI = static_cast<CLabelUI*>(pHorUI->GetItemAt(0));
				if ( pLabelUI )
				{
					pLabelUI->SetText( CHttpPlus::GBKToUNICODE( string("�����ˣ�") + tOrder.sOwner).c_str() );
				}
				pLabelUI = static_cast<CLabelUI*>(pHorUI->GetItemAt(1));
				if ( pLabelUI )
				{
					pLabelUI->SetText( CHttpPlus::GBKToUNICODE( string("QQ��") + tOrder.sOwner ).c_str() );
				}
			}
		}

		pLabelUI = static_cast<CLabelUI*>(pListItem->GetItemAt(2));
		if ( pLabelUI )
		{
			pLabelUI->SetText( CHttpPlus::GBKToUNICODE( tOrder.sPrice).c_str() );
		}

		pVerUI = static_cast<CVerticalLayoutUI*>(pListItem->GetItemAt(3));
		if (pVerUI)
		{
			pLabelUI = static_cast<CLabelUI*>(pVerUI->GetItemAt(1));
			if ( pLabelUI )
			{
				pLabelUI->SetText( CHttpPlus::GBKToUNICODE("������"+ tOrder.sPushedTime ).c_str() );
			}
			pLabelUI = static_cast<CLabelUI*>(pVerUI->GetItemAt(2));
			if ( pLabelUI )
			{
				pLabelUI->SetText( CHttpPlus::GBKToUNICODE( "��ֹ��"+ tOrder.sEndTime  ).c_str() );
			}
		}
	}
}

void CDlgPubilcChild::AndItem(Order& order)
{
	for (size_t i = 0 ; m_bRunning && i < m_vAllList.size(); ++i)
	{
		if (order.sPushedTime == m_vAllList[i].sPushedTime && order.sOwner ==  m_vAllList[i].sOwner && order.sHrefUrl ==  m_vAllList[i].sHrefUrl )
		{
			return;
		}
	}

	THIS_GETOBJ(tList,CListUI,"ListResult");
	if (tList)
	{
		CDialogBuilder tBuilder;
		CListContainerElementUI *pListItem = static_cast<CListContainerElementUI*>( tBuilder.Create( _T("UI_ListItem.xml") , 0 , NULL , &m_PaintManager , NULL ) );
		if (pListItem)
		{
			CLabelUI* tId = static_cast<CLabelUI* > (pListItem->GetItemAt(0));
			if (tId)
			{
				ostringstream ss;
				ss << (tList->GetCount()+1);
				tId->SetText( CHttpPlus::GBKToUNICODE(ss.str()).c_str() );
			}
			UpdateListItem(pListItem , order);
			tList->AddAt( pListItem,0);
			m_vAllList.push_back( order );
			pListItem->SetTag((UINT_PTR)m_vAllList.size()-1);

			if (tList->GetCount() >= 30)
			{
				// �����ϳ�
				ostringstream sSoundText;
				sSoundText << "���¶�����" <<  order.sTitle  << ",Ԥ�� " << order.sPrice << "Ԫ";
				//sSoundText << "�ҵı�������û���أ����ڼ���Ӧ���ղ�����,��Ϊ�ʺ������ڷ������л�Ϊ�������ɵ�¼���ʺţ������Ʒ��񡢻�Ϊ�̳ǡ���Ϊ/��ҫ���������۾��ֲ����������";
				PlayTextSound(sSoundText.str());
			}
		}
		else
		{
			// ERROR LOG
		}
	}
}

string CDlgPubilcChild::GetProxyIP(int ListId)
{
	string sProxyIp,sIpStream;
	int nSwitchIp = 1;
	string m_sProxyUrl = "http://www.56pu.com/fetch?orderId=568157143885394794&quantity=&line=tel&region=&regionEx=&beginWith=&ports=&speed=&anonymity=&scheme=";
	if (  vAllProxyIp.empty() )
	{
		if (!m_sProxyUrl.empty())
		{
			CHttpPlus httpGetProxy;
			string sRetIp;
			if (httpGetProxy.httpSend(m_sProxyUrl,&sRetIp))
			{
				CRegexPlus Regex;

				if(Regex.CreateMatch("[0-9.:]{9,21}",sRetIp))
				{
					for (size_t i=0 ; i < Regex.GetMatchedCount() ; ++i)
					{
						vAllProxyIp.push_back(Regex.GetMatchedString(i));
					}
				}
			}
		}
	}

	if (!vAllProxyIp.empty())
	{
		sProxyIp = vAllProxyIp[vAllProxyIp.size()-1];

		if ((ListId + 5) % nSwitchIp == 0)
		{
			vAllProxyIp.pop_back();
		}
	}
	return sProxyIp;
}

void CDlgPubilcChild::RunCheck()
{
	Sleep(1000);
	string sData;
	bool bResult = false;
	while(m_bRunning)
	{
		{
			CHttpPlus http;
			INFO_ << "httpSend ...";
			string sProxyIp = "";
			bResult = http.httpSend("https://bbs.125.la/plugin.php?id=e3600%3Atask&mod=show&type=1&a=",&sData,"GET",true,sProxyIp);
		}
		if(bResult)
		{
			INFO_ << "httpSendFinish ... data size:" << sData.size();
			try
			{
				Json::Reader reader;
				Json::Value root;
				string sHtml;
				if(reader.parse(sData,root) && !root.isNull() && root.isObject() && !root["data"].isNull() && root["data"].isString())
				{
					sHtml = root["data"].asString();
					if (!sHtml.empty())
					{
						vector<string> lTr;
						CHttpPlus::split(sHtml,"</tr><tr>",&lTr);
						for(size_t i = lTr.size()-1; i > 0; --i)
						{
							vector<string> lTd;
							Order order;
							string sTemp;
							CHttpPlus::split(lTr[i],"</td><td>",&lTd);
							for(size_t j = 0; j < lTd.size(); ++j)
							{
								switch(j)
								{
								case 0:
									if(CHttpPlus::Getmiddletext(lTd[j],"href=\"","\"",sTemp))
									{
										order.sHrefUrl = "http://bbs.125.la/" + sTemp;
									}
									if(CHttpPlus::Getmiddletext(lTd[j],"target=\"_blank\">","</a>",sTemp))
									{
										order.sTitle = sTemp;
									}
									break;
								case 1:
									if(CHttpPlus::Getmiddletext(lTd[j],"<b>��","</b>",sTemp))
									{
										order.sPrice =sTemp;
									}
									break;
								case 2:
									break;
								case 3:
									if(CHttpPlus::Getmiddletext(lTd[j],"title=\"","\"",sTemp))
									{
										order.sPushedTime = sTemp;
									}

									break;
								case 4:
									order.sEndTime = lTd[j];
									break;
								case 5:
									if(CHttpPlus::Getmiddletext(lTd[j],"href=\"","\"",sTemp))
									{
										order.sOwnerUrl = "http://bbs.125.la/"+sTemp;
									}

									if(CHttpPlus::Getmiddletext(lTd[j],"target=\"_blank\">","</a>",sTemp))
									{
										order.sOwner = sTemp;
									}
									break;
								}
							}
							if (!order.sTitle.empty())
							{
								AndItem(order);
							}
						}
					}
					else{
						Sleep(5000);
						continue;
					}
				}
			}
			catch (...)
			{

			}
			for (int i = 0 ;m_bRunning && i < 60; i++)
			{
				Sleep(1000);
			}
		}
		else
		{
			ERROR_ << "httpSendFinish ... data size:" << sData.size() << "data:" << sData;
		}
	}
}

void CDlgPubilcChild::RunCheckThread(void* pDlg)
{
	CDlgPubilcChild* pThis = (CDlgPubilcChild*)pDlg;
	if (pThis)
	{
		pThis->RunCheck();
	}
	_endthread();
}

void CDlgPubilcChild::OnBtnListItemDeleteClick( TNotifyUI& msg )
{

}

void CDlgPubilcChild::OnBtnListItemStopClick( TNotifyUI& msg )
{

}

void CDlgPubilcChild::OnBtnListItemOpenDirClick( TNotifyUI& msg )
{
	CButtonUI* Btn = static_cast<CButtonUI* >(msg.pSender);
	if (Btn)
	{
		CControlUI* pCtrl = Btn;
		for (int i = 0; i < 3;++i)
		{
			if (pCtrl)
			{
				pCtrl = pCtrl->GetParent();
			}
			else
			{
				break;
			}
		}
		if (pCtrl)
		{
			size_t index = (size_t)(pCtrl->GetTag());
			if (index > 0 && index < m_vAllList.size())
			{
				string url((Btn->GetName() == _T("btn_list_item_open") ? m_vAllList[index].sHrefUrl:m_vAllList[index].sOwnerUrl));
				ShellExecuteA(GetHWND(),"open",url.c_str(),NULL,NULL,SW_SHOW);
			}
		}
	}
}

void CDlgPubilcChild::PlayTextSound(string sText)
{
	CTTSSound TTSSound;
	string sData;
	TTSSound.text_to_speech(sText,sData);
	if (!sData.empty())
	{
		/*
		FILE* fp = fopen("./sound.wav","wb");
		if(fp)
		{
			fwrite(sData.c_str(),1,sData.size(),fp);
			fclose(fp);
		}*/

		PlaySoundA(sData.c_str(),CPaintManagerUI::GetInstance() , SND_MEMORY|SND_SYNC );
	}
}