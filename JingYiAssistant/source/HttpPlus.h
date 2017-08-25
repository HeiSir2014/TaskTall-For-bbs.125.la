#pragma once
#include "curl\curl.h"
#include <iostream>
#include <string>
#include <vector>
#define  SKIP_PEER_VERIFICATION 1
//#define  SKIP_HOSTNAME_VERFICATION 1
//#ifndef DWORD
typedef unsigned long DWORD;
typedef void* HANDLE;
typedef void* LPVOID;
#ifdef Q_OS_WIN
#define WINAPI __stdcall
#endif
using namespace std;

class CHttpPlus
{
private:
    CURL* curl;
    int  m_Code;
	string m_Url;
	bool m_bHttpType; /* false GET | true POST*/
    unsigned int m_nPostSize;
	string m_Cookie;
	string m_RedirctUrl;
	string m_IpProxy;
public:
	CHttpPlus();
	~CHttpPlus();

    struct CallbackData {
        CallbackData();
        CHttpPlus* pHttpPlus;
        void *pData;
        __int64 nIdleSize;
        __int64 nTotalSize;
    };
    static void GlobalInit();
    static void GlobalClean();

	void ReInit();
    bool httpSendFile(string sSendUrl, string *pRetData, string sHttpType = "GET", bool bIsAcceptData = true, string sProxyIP = string(), string *sAppendHttpHead = NULL, string sFileName = NULL, string *sReturnHead = NULL);
    bool httpSend(string sSendUrl, string *pRetData, string sHttpType = "GET", bool bIsAcceptData = true, string sProxyIP = string(), string *sAppendHttpHead = NULL, string *pPostData = NULL, string *sReturnHead = NULL);
    bool httpSendSaveFile(string sSendUrl, string pFilePath, string sHttpType = "GET", bool bIsAcceptData = true, string sProxyIP = string(), string *sAppendHttpHead = NULL, string *pPostData = NULL, string *sReturnHead = NULL);
	bool httpSendSaveFile_(string sSendUrl, /*欲访问的URL */ string pFilePath, /*接受数据的容器 */ string sHttpType = "GET", bool bIsAcceptData = true, string sProxyIP = string(), string *sAppendHttpHead = NULL, string *pPostData = NULL, string *sReturnHead = NULL);
	bool HttpGetMp4ByM3u8(const string url,FILE* pFile);

    static int CreatDir(const char *pDir);
    virtual void WriteProgress(__int64 nWriteCount,__int64 nTotalSize);
	virtual void ReadProgress(__int64 nReadCount,__int64 nTotalSize);
	std::string DNSParse(const std::string& domain);

	__int64 MAXRECVBYTEPER;
	//common funcation
	static string GetExecPath();
	static std::string EncodeURL(const string &URL);
	static std::string DecodeURL(const std::string &URL);
	static string url_encode(const string url);
	static string url_decode(const string url);
	static string url_encode_(const string sUrl);
	static string UTF8ToGBK( const string& str );
	static string GBKToUTF8( const string& str );
	static string UnicodeToANSI( const wstring& str );
	static string UnicodeToUTF8( const wstring& str );
	static wstring GBKToUNICODE( const string& str );
	static wchar_t char2hex(char hight,char low);
	static std::string usc2Toansi(const string src,bool bWide = true);

	static const std::string GetTime();

	bool M3u8Parse(const string& m3u8Context,vector<string>& sListUrl,size_t nTotalTime);
	static bool Getmiddletext(string src, char *left, char *right,string & str_ret);
	static void replaceAll(std::string& str, const std::string& from, const std::string& to);

	static void split(std::string &s, string delim, std::vector<std::string> *ret);

	bool* bIsStopDownload;
	void HttpAbort();
	time_t m_tLastRecvTime;
private:
    bool SetHttpHead(string sHttpHead, vector<string> &sHeadList);
	static size_t call_wirte_func(const char *ptr, size_t size, size_t nmemb, string *stream);
    static size_t call_wirte_file_func(const char *ptr, size_t size, size_t nmemb, CallbackData *pData);
    static size_t call_headerw_func(const char *ptr, size_t size, size_t nmemb, string *stream);
    static size_t call_read_file_func(void *ptr, size_t size, size_t nmemb, CallbackData *pData);
    static size_t call_read_string_func(void *ptr, size_t size, size_t nmemb, CallbackData *pData);
	static char to_hex(char code);
	static char from_hex(char ch);
	static char Dec2HexChar(short int n);
	static short int HexChar2Dec(char c);
};


class CHttpPlusInit
{
public:
    CHttpPlusInit()
    {
        //CHttpPlus::GlobalInit();
    }
    ~CHttpPlusInit()
    {
        CHttpPlus::GlobalClean();
    }
};
