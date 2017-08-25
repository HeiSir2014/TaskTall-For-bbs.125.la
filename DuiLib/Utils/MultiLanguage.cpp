#include "StdAfx.h"
#include "MultiLanguage.h"

namespace DuiLib {

CMultiLanguage::CMultiLanguage(void)
{
}


CMultiLanguage::~CMultiLanguage(void)
{
}


// 通过 XML 设置多语言解析
bool CMultiLanguage::SetLanguageFile(LPCTSTR sFilePath)
{
	CPaintManagerUI::RemoveAllMultiLanguageString();
	CMarkup xml;
	if( !xml.LoadFromFile(sFilePath) ) return false;
	CMarkupNode root = xml.GetRoot();
	if (!root.IsValid())	return false;
	LPCTSTR pstrClass = NULL;
	int nAttributes = 0;
	LPCTSTR pstrName = NULL;
	LPCTSTR pstrValue = NULL;
	LPTSTR pstr = NULL;
	for( CMarkupNode node = root.GetChild() ; node.IsValid(); node = node.GetSibling() ) 
	{
		pstrClass = node.GetName();
		if( pstrClass !=NULL && _tcsicmp(pstrClass, _T("Item")) == 0 )
		{
			nAttributes = node.GetAttributeCount();
			LPCTSTR pId = NULL;
			LPCTSTR pMultiLanguage = NULL;
			for( int i = 0; i < nAttributes; i++ ) {
				pstrName = node.GetAttributeName(i);
				pstrValue = node.GetAttributeValue(i);
				if( _tcsicmp(pstrName, _T("id")) == 0 ) {
					pId = pstrValue;
				}
				else if( _tcsicmp(pstrName, _T("value")) == 0 ) {
					pMultiLanguage = pstrValue;
				}
			}
			if (pId >= 0 && pMultiLanguage ) {
				CPaintManagerUI::AddMultiLanguageString(pId, pMultiLanguage);
			}
		}
	}
	return false;
}

// 通过 XML 设置多语言解析
CDuiString CMultiLanguage::GetMultiValue(LPCTSTR sId)
{
	if (sId == NULL)
	{
		return _T("");
	}

	LPCTSTR sValue = CPaintManagerUI::GetMultiLanguageString(sId);

	return sValue ? sValue : sId;
}

} // namespace DuiLib