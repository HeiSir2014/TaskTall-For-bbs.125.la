#pragma once
namespace DuiLib {

class UILIB_API CMultiLanguage
{
public:
	CMultiLanguage(void);
	~CMultiLanguage(void);
	// 通过 XML 设置多语言解析
	static bool SetLanguageFile(LPCTSTR sFilePath);
	static CDuiString GetMultiValue(LPCTSTR sId);
private:
	
};

} // namespace DuiLib
