#include "HttpPlus.h"
#include <stdlib.h>
#include <string.h>
#include <direct.h>
#include <io.h>
#define ACCESS _access
#define MKDIR(a) _mkdir((a))
CHttpPlusInit global_curl_; // 用於 curl 初始化

CHttpPlus::CHttpPlus()
{
    CHttpPlus::GlobalInit();
	m_nPostSize = 0;
    curl = curl_easy_init();
	bIsStopDownload = NULL;
	MAXRECVBYTEPER = -1;
	m_tLastRecvTime = NULL;
}

CHttpPlus::~CHttpPlus()
{
    if (curl)
	{
        curl_easy_cleanup((CURL*)curl);
        curl = NULL;
    }
//    CHttpPlus::GlobalClean();
}

static bool g_bHttpIsInited = false;

//全局初始化
void CHttpPlus::GlobalInit()
{
    if( !g_bHttpIsInited )
    {
        g_bHttpIsInited = true;
        CURLcode nCode = curl_global_init(CURL_GLOBAL_ALL);
        if(nCode != CURLE_OK)
        {
            cerr  << "curl_global_init fail ,Code" << nCode;
            cerr  << curl_easy_strerror(nCode);
        }
    }
}

//全局清理
void CHttpPlus::GlobalClean()
{
    if(g_bHttpIsInited)
    {
        g_bHttpIsInited = false;
        curl_global_cleanup();
    }
}

void CHttpPlus::ReInit()
{
	if (curl)
	{
		curl_easy_cleanup((CURL*)curl);
		curl = NULL;
	}
	curl = curl_easy_init();
}

string CHttpPlus::GetExecPath()
{
	string sInstancePath( MAX_PATH +1 ,'\0');
	size_t nLen = ::GetModuleFileNameA(NULL, (char*)sInstancePath.c_str(), MAX_PATH);
	size_t sPos = sInstancePath.rfind('\\',nLen);
	if (sPos != string::npos)
	{
		sInstancePath.resize(sPos);
	}
	else
	{
		cerr << "GetModuleFileName Fail,LastError:" << GetLastError() << endl;
		sInstancePath.clear();
	}
	return sInstancePath;
}

char CHttpPlus::Dec2HexChar(short int n)   
{  
	if ( 0 <= n && n <= 9 ) {  
		return char( short('0') + n );  
	} else if ( 10 <= n && n <= 15 ) {  
		return char( short('A') + n - 10 );  
	} else {  
		return char(0);  
	}  
}  

short int CHttpPlus::HexChar2Dec(char c)  
{  
	if ( '0'<=c && c<='9' ) {  
		return short(c-'0');  
	} else if ( 'a'<=c && c<='f' ) {  
		return ( short(c-'a') + 10 );  
	} else if ( 'A'<=c && c<='F' ) {  
		return ( short(c-'A') + 10 );  
	} else {  
		return -1;  
	}  
}  

string CHttpPlus::EncodeURL(const string &URL)  
{  
	string strResult = "";  
	for ( unsigned int i=0; i<URL.size(); i++ )  
	{  
		char c = URL[i];  
		if (  
			( '0'<=c && c<='9' ) ||  
			( 'a'<=c && c<='z' ) ||  
			( 'A'<=c && c<='Z' ) ||  
			c=='/' || c=='.'  
			) {  
				strResult += c;  
		}   
		else   
		{  
			int j = (short int)c;  
			if ( j < 0 )  
			{  
				j += 256;  
			}  
			int i1, i0;  
			i1 = j / 16;  
			i0 = j - i1*16;  
			strResult += '%';  
			strResult += Dec2HexChar(i1);  
			strResult += Dec2HexChar(i0);  
		}  
	}  

	return strResult;  
}  

std::string CHttpPlus::DecodeURL(const std::string &URL)  
{  
	string result = "";  
	for ( unsigned int i=0; i<URL.size(); i++ )   
	{  
		char c = URL[i];  
		if ( c != '%' )   
		{  
			result += c;  
		}   
		else   
		{  
			char c1 = URL[++i];  
			char c0 = URL[++i];  
			int num = 0;  
			num += HexChar2Dec(c1) * 16 + HexChar2Dec(c0);  
			result += char(num);  
		}  
	}  

	return result;  
}

/* Converts a hex character to its integer value */
char CHttpPlus::from_hex(char ch) {
	return isdigit(ch) ? ch - '0' : tolower(ch) - 'a' + 10;
}

/* Converts an integer value to its hex character*/
char CHttpPlus::to_hex(char code) {
	static char hex[] = "0123456789abcdef";
	return hex[code & 15];
}

/* Returns a url-encoded version of str */
/* IMPORTANT: be sure to free() the returned string after use */
string CHttpPlus::url_encode(const string url) {
	string url_;
	char *pstr = (char*)url.c_str(), *buf, *pbuf;
	url_.resize(url.size()*3+1);
	buf = (char*)url_.c_str();
	pbuf = buf;
	while (*pstr) {
		if (isalnum(*pstr) || *pstr == '-' || *pstr == '_' || *pstr == '.' || *pstr == '~') 
			*pbuf++ = *pstr;
		else if (*pstr == ' ')
			*pbuf++ = '+';
		else 
			*pbuf++ = '%', *pbuf++ = to_hex(*pstr >> 4), *pbuf++ = to_hex(*pstr & 15);
		pstr++;
	}
	*pbuf = '\0';
	return url_.c_str();
}

/* Returns a url-decoded version of str */
/* IMPORTANT: be sure to free() the returned string after use */
string CHttpPlus::url_decode(const string url) {
	char *pstr = (char*)url.c_str(), *buf, *pbuf;
	string _url;
	_url.resize(url.size());
	pbuf = buf = (char*)_url.c_str();
	while (*pstr) {
		if (*pstr == '%') {
			if (pstr[1] && pstr[2]) {
				*pbuf++ = from_hex(pstr[1]) << 4 | from_hex(pstr[2]);
				pstr += 2;
			}
		} else if (*pstr == '+') { 
			*pbuf++ = ' ';
		} else {
			*pbuf++ = *pstr;
		}
		pstr++;
	}
	*pbuf = '\0';
	return _url.c_str();
}

wchar_t CHttpPlus::char2hex(char hight,char low)
{
	unsigned char out = 0;
	unsigned char c = hight;
	wchar_t ch = 0;
	if (c >= '0' && c <= '9')
	{
		out += (c-'0')*16;
	}
	else if(c >= 'a' && c <= 'z')
	{
		out += (c-'a'+10)*16;
	}
	else if(c >= 'A' && c <= 'Z')
	{
		out += (c-'A'+10)*16;
	}
	c = low;
	if (c >= '0' && c <= '9')
	{
		out += (c-'0');
	}
	else if(c >= 'a' && c <= 'z')
	{
		out += (c-'a'+10);
	}
	else if(c >= 'A' && c <= 'Z')
	{
		out += (c-'A'+10);
	}
	ch = out;
	return ch;
}

std::string CHttpPlus::usc2Toansi(const string src,bool bWide)
{
	wstring out;
	wchar_t ch_w;
	for(size_t i = 0;i < src.size();++i)
	{
		if (i+5 < src.size() && src[i] == '\\' && src[i+1] == 'u')
		{
			out.push_back(((char2hex(src[i+2],src[i+3]) << 8 ) +  char2hex(src[i+4],src[i+5])));

			i+=5;
		}
		else
		{
			ch_w = 0;
			ch_w = src[i];
			out.push_back(ch_w);
		}
	}
	if (bWide)
	{
		return UnicodeToANSI(out);
	}
	else
	{
		return UnicodeToUTF8(out);
	}
}

string CHttpPlus::UnicodeToANSI( const wstring& str )
{
	char*     pElementText;
	int    iTextLen;
	// wide char to multi char
	iTextLen = WideCharToMultiByte( CP_ACP,
		0,
		str.c_str(),
		-1,
		NULL,
		0,
		NULL,
		NULL );
	pElementText = new char[iTextLen + 1];
	memset( ( void* )pElementText, 0, sizeof( char ) * ( iTextLen + 1 ) );
	::WideCharToMultiByte( CP_ACP,
		0,
		str.c_str(),
		-1,
		pElementText,
		iTextLen,
		NULL,
		NULL );
	string strText;
	strText = pElementText;
	delete[] pElementText;
	return strText;
}


string CHttpPlus::UnicodeToUTF8( const wstring& str )
{
	char*     pElementText;
	int    iTextLen;
	// wide char to multi char
	iTextLen = WideCharToMultiByte( CP_UTF8,
		0,
		str.c_str(),
		-1,
		NULL,
		0,
		NULL,
		NULL );
	pElementText = new char[iTextLen + 1];
	memset( ( void* )pElementText, 0, sizeof( char ) * ( iTextLen + 1 ) );
	::WideCharToMultiByte( CP_UTF8,
		0,
		str.c_str(),
		-1,
		pElementText,
		iTextLen,
		NULL,
		NULL );
	string strText;
	strText = pElementText;
	delete[] pElementText;
	return strText;
}

string CHttpPlus::UTF8ToGBK( const string& str )
{
	wchar_t*     pElementText;
	int    iTextLen;
	// wide char to multi char
	iTextLen = MultiByteToWideChar(CP_UTF8,0,str.c_str(),str.size(),NULL,NULL);
	pElementText = new wchar_t[iTextLen + 1];
	memset( ( void* )pElementText, 0, sizeof( wchar_t ) * ( iTextLen + 1 ) );
	::MultiByteToWideChar( CP_UTF8,
		0,
		str.c_str(),
		str.size(),
		pElementText,
		iTextLen);
	wstring strText;
	strText = pElementText;
	delete[] pElementText;
	return UnicodeToANSI(strText);
}


wstring CHttpPlus::GBKToUNICODE( const string& str )
{
	wstring strTo;
	int    iTextLen;
	// wide char to multi char
	iTextLen = MultiByteToWideChar(CP_ACP,0,str.c_str(),str.size(),NULL,NULL);
	if (iTextLen > 0)
	{
		strTo.resize(iTextLen);
		::MultiByteToWideChar( CP_ACP,
			0,
			str.c_str(),
			str.size(),
			(wchar_t*)strTo.c_str(),
			strTo.size());
	}
	return strTo;
}


string CHttpPlus::GBKToUTF8( const string& str )
{
	return UnicodeToUTF8(GBKToUNICODE(str));
}

int CHttpPlus::CreatDir(const char *pDir)
{
    int i = 0;
    int iRet = 0;
    int iLen;
    char* pszDir;

    if(NULL == pDir)
    {
        return 0;
    }

    pszDir = strdup( pDir );
    iLen = strlen(pszDir);

    // 创建中间目录
    for (i = 0;i < iLen;i ++)
    {
        if (pszDir[i] == '\\' || pszDir[i] == '/' )
        {
            pszDir[i] = '\0';

            //如果不存在,创建
            iRet = ACCESS(pszDir,0);
            if (iRet != 0)
            {
                iRet = MKDIR(pszDir);
                if (iRet != 0)
                {
                    break;
                }
            }
            //支持linux,将所有\换成/
            pszDir[i] = '\\';
        }
    }
	if (iRet == 0)
	{
		iRet = ACCESS(pszDir , 0);
		if (iRet != 0)
		{
			iRet = MKDIR(pszDir);
		}
	}
    free(pszDir);
    return iRet;
}

// 发送HTTP数据包
bool CHttpPlus::httpSend(string sSendUrl,			/*欲访问的URL							*/
    string* pRetData,			/*接受数据的容器						*/
    string sHttpType,				/* false , GET || ture , POST 			*/
    bool bIsAcceptData, /* false , No Accept || ture , Accept 	*/
    string sProxyIP,
    string *sAppendHttpHead,		/*Http 的 附加头						*/
    string *pPostData ,			/*提交的数据							*/
    string *sReturnHead		/*返回的HTTP HEAD						*/
)
{
//    CURL* curl = curl_easy_init();

    CallbackData calldata;
    CURLcode nCode = CURLE_FAILED_INIT;
    long http_code = 0;
    if(pRetData)
        pRetData->clear();

    if(sReturnHead)
        sReturnHead->clear();

    if (curl)
	{
        curl_easy_reset(curl);
        curl_easy_setopt(curl, CURLOPT_URL, sSendUrl.c_str());
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, sHttpType.c_str());
        if(sHttpType == "HEAD")
             curl_easy_setopt(curl,CURLOPT_NOBODY,1);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "aws-sdk-dotnet-45/3.1.1.1 aws-sdk-dotnet-core/3.1.0.1 .NET_Runtime/4.0 .NET_Framework/4.0 OS/Microsoft_Windows_NT_6.1.7601_Service_Pack_1 ClientSync");
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
//        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT_MS,800);
        if(!sProxyIP.empty())
        {
            curl_easy_setopt(curl, CURLOPT_PROXY, sProxyIP.c_str());
        }
//        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT_MS,1000);

        if(bIsAcceptData && pRetData)
        {
            //抓取内容后，回调函数
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, call_wirte_func);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, pRetData);
        }

		//抓取头信息，回调函数
        if(sReturnHead)
        {
            curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, call_headerw_func);
            curl_easy_setopt(curl, CURLOPT_HEADERDATA, sReturnHead);
        }

        vector<string> sHeadList;
        struct curl_slist *chunkHead = NULL;

        /* POST 数据 */
        if (pPostData)
        {
            calldata.pData = pPostData;
            calldata.pHttpPlus = this;
            calldata.nTotalSize = pPostData->size();
            calldata.nIdleSize = 0;

            //curl_easy_setopt(curl, CURLOPT_POST, 1L);
            //curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, pPostData->size());
            //curl_easy_setopt(curl, CURLOPT_POSTFIELDS, pPostData->c_str());
            curl_easy_setopt(curl, CURLOPT_POST, 1L);
            curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, pPostData->size());
            curl_easy_setopt(curl, CURLOPT_READFUNCTION, call_read_string_func);
            curl_easy_setopt(curl, CURLOPT_READDATA, &calldata);
            sHeadList.push_back("Content-Type: application/x-www-form-urlencoded");
        }

        if(sAppendHttpHead)
            SetHttpHead(*sAppendHttpHead, sHeadList);

        if (!sHeadList.empty())
        {
            for (size_t i = 0; i < sHeadList.size(); ++i)
            {
                chunkHead = curl_slist_append(chunkHead, sHeadList[i].c_str());
            }
        }
        if(chunkHead)
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunkHead);

        nCode = curl_easy_perform(curl);

        if(chunkHead)
            curl_slist_free_all(chunkHead);

        if(nCode != CURLE_OK)
        {
            //cerr << sHttpType << '\t' << sSendUrl <<'\t'  << "CURLCode:" << nCode << "\tDetail:" << curl_easy_strerror( nCode );
        }

        curl_easy_getinfo (curl, CURLINFO_RESPONSE_CODE, &http_code);

        if( http_code > 400)
        {
            //cerr << sHttpType << '\t' << sSendUrl <<'\t'  << "RspDatas:" <<( pRetData != NULL ? pRetData->c_str() : "");
        }
        else
        {
            //cout << sHttpType << '\t' << sSendUrl <<'\t'  << "RspDatas:" <<( pRetData != NULL ? pRetData->c_str() : "");
        }

        //curl_easy_cleanup(curl);
	}
    return (CURLE_OK == nCode && http_code < 500);
}

// 发送HTTP数据包
bool CHttpPlus::httpSendSaveFile(string sSendUrl,			/*欲访问的URL							*/
    string pFilePath,			/*接受数据的容器						*/
    string sHttpType,				/* false , GET || ture , POST 			*/
    bool bIsAcceptData,			/* false , No Accept || ture , Accept 	*/
    string sProxyIP   ,
    string *sAppendHttpHead,		/*Http 的 附加头						*/
    string *pPostData ,			/*提交的数据							*/
    string *sReturnHead		/*返回的HTTP HEAD						*/
)
{
//  CURL* curl = curl_easy_init();

    if(sReturnHead)
        sReturnHead->clear();
    long http_code = 0;
    CURLcode nCode = CURLE_FAILED_INIT;
    cout << sHttpType << '\t' << sSendUrl <<'\t' << (pPostData != NULL ? *pPostData : "") << "\nFile:" << pFilePath << endl;

    CallbackData calldata;
    FILE* pFile = NULL;
    if ( curl )
    {
        curl_easy_reset(curl);
        curl_easy_setopt(curl, CURLOPT_URL, sSendUrl.c_str());
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, sHttpType.c_str());
        if(sHttpType == "HEAD")
             curl_easy_setopt(curl,CURLOPT_NOBODY,1);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "aws-sdk-dotnet-45/3.1.1.1 aws-sdk-dotnet-core/3.1.0.1 .NET_Runtime/4.0 .NET_Framework/4.0 OS/Microsoft_Windows_NT_6.1.7601_Service_Pack_1 ClientSync");
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
//        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT_MS,800);
        if(!sProxyIP.empty())
        {
            curl_easy_setopt(curl, CURLOPT_PROXY, sProxyIP.c_str());
        }

//        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT_MS,1000);

		size_t Pos = pFilePath.rfind('\\');
		if (Pos != std::string::npos)
		{
			CreatDir( string(pFilePath,Pos).c_str() );//Linux下有问题
		}
        
        pFile = fopen(pFilePath.c_str(),"wb+");
        if(bIsAcceptData && pFile)
        {
            calldata.pHttpPlus = this;
            calldata.pData = pFile;
            calldata.nTotalSize = 0;

            //抓取内容后，回调函数
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, call_wirte_file_func);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &calldata);
        }
        else
        {
            cerr << "fopen Fail:" << pFilePath.c_str();
            if(sReturnHead)
            {
                *sReturnHead = "HTTP/1.1 400 Error\r\nServer: BWS/1.0\r\nContent-Type: text/html;charset=UTF-8\r\nReason: File Open Failed\r\n";
            }
            curl_easy_reset(curl);
            return false;
        }

        //抓取头信息，回调函数
        if(sReturnHead)
        {
            curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, call_headerw_func);
            curl_easy_setopt(curl, CURLOPT_HEADERDATA, sReturnHead);
        }

        vector<string> sHeadList;
        struct curl_slist *chunkHead = NULL;

        /* POST 数据 */
        if (pPostData)
        {
            curl_easy_setopt(curl, CURLOPT_POST, 1L);
            curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, pPostData->size());
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, pPostData->c_str());
            sHeadList.push_back("Content-Type: application/octet-stream");
        }

        if(sAppendHttpHead)
            SetHttpHead(*sAppendHttpHead, sHeadList);

        if (!sHeadList.empty())
        {
            for (size_t i = 0; i < sHeadList.size(); ++i)
            {
                chunkHead = curl_slist_append(chunkHead, sHeadList[i].c_str());
            }
        }
        if(chunkHead)
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunkHead);

        nCode = curl_easy_perform(curl);

        if(chunkHead)
            curl_slist_free_all(chunkHead);

        if(pFile)
            fclose(pFile);
        //curl_easy_cleanup(curl);

        curl_easy_getinfo (curl, CURLINFO_RESPONSE_CODE, &http_code);
    }
    return (CURLE_OK == nCode && http_code < 500);
}


// 发送HTTP数据包
bool CHttpPlus::httpSendSaveFile_(string sSendUrl,			/*欲访问的URL							*/
	string pFilePath,			/*接受数据的容器						*/
	string sHttpType,				/* false , GET || ture , POST 			*/
	bool bIsAcceptData,			/* false , No Accept || ture , Accept 	*/
	string sProxyIP   ,
	string *sAppendHttpHead,		/*Http 的 附加头						*/
	string *pPostData ,			/*提交的数据							*/
	string *sReturnHead		/*返回的HTTP HEAD						*/
	)
{
	//  CURL* curl = curl_easy_init();

	if(sReturnHead)
		sReturnHead->clear();
	long http_code = 0;
	CURLcode nCode = CURLE_FAILED_INIT;
	cout << sHttpType << '\t' << sSendUrl <<'\t' << (pPostData != NULL ? *pPostData : "") << "\nFile:" << pFilePath << endl;

	CallbackData calldata;
	FILE* pFile = NULL;
	if ( curl )
	{
		curl_easy_reset(curl);
		curl_easy_setopt(curl, CURLOPT_URL, sSendUrl.c_str());
		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, sHttpType.c_str());
		if(sHttpType == "HEAD")
			curl_easy_setopt(curl,CURLOPT_NOBODY,1);
		curl_easy_setopt(curl, CURLOPT_USERAGENT, "aws-sdk-dotnet-45/3.1.1.1 aws-sdk-dotnet-core/3.1.0.1 .NET_Runtime/4.0 .NET_Framework/4.0 OS/Microsoft_Windows_NT_6.1.7601_Service_Pack_1 ClientSync");
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
		//        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT_MS,800);
		if(!sProxyIP.empty())
		{
			curl_easy_setopt(curl, CURLOPT_PROXY, sProxyIP.c_str());
		}

		//        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT_MS,1000);

		size_t Pos = pFilePath.rfind('\\');
		if (Pos != std::string::npos)
		{
			CreatDir( string(pFilePath,Pos).c_str() );//Linux下有问题
		}

		pFile = fopen(pFilePath.c_str(),"wb+");
		if(bIsAcceptData && pFile)
		{
			calldata.pHttpPlus = this;
			calldata.pData = pFile;
			calldata.nTotalSize = 0;

			//抓取内容后，回调函数
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, call_wirte_file_func);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &calldata);
		}
		else
		{
			cerr << "fopen Fail:" << pFilePath.c_str();
			if(sReturnHead)
			{
				*sReturnHead = "HTTP/1.1 400 Error\r\nServer: BWS/1.0\r\nContent-Type: text/html;charset=UTF-8\r\nReason: File Open Failed\r\n";
			}
			curl_easy_reset(curl);
			return false;
		}

		//抓取头信息，回调函数
		if(sReturnHead)
		{
			curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, call_headerw_func);
			curl_easy_setopt(curl, CURLOPT_HEADERDATA, sReturnHead);
		}

		vector<string> sHeadList;
		struct curl_slist *chunkHead = NULL;

		/* POST 数据 */
		if (pPostData)
		{
			curl_easy_setopt(curl, CURLOPT_POST, 1L);
			curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, pPostData->size());
			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, pPostData->c_str());
			sHeadList.push_back("Content-Type: application/octet-stream");
		}

		if(sAppendHttpHead)
			SetHttpHead(*sAppendHttpHead, sHeadList);

		if (!sHeadList.empty())
		{
			for (size_t i = 0; i < sHeadList.size(); ++i)
			{
				chunkHead = curl_slist_append(chunkHead, sHeadList[i].c_str());
			}
		}
		if(chunkHead)
			curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunkHead);


		CURLM* multi = curl_multi_init();
		if ( multi != NULL )
		{
			curl_multi_add_handle(multi,curl);
			int Run = 0;
			int calc = 0;
			do 
			{
				Sleep(1);
				curl_multi_perform(multi,&Run);
				if ( bIsStopDownload != NULL && *bIsStopDownload )
				{
					break;
				}
				calc++;
				if (calc > 1000)
				{
					time_t t;
					time(&t);
					if (m_tLastRecvTime && t - m_tLastRecvTime >= 30)
					{
						break;
					}
					calc = 0;
				}
			} while (Run);
			curl_multi_remove_handle(multi,curl);
			curl_multi_cleanup(multi);
		}

		nCode = CURLE_OK;

		if(chunkHead)
			curl_slist_free_all(chunkHead);

		if(pFile)
			fclose(pFile);
		//curl_easy_cleanup(curl);

		curl_easy_getinfo (curl, CURLINFO_RESPONSE_CODE, &http_code);
	}
	return (CURLE_OK == nCode && http_code < 500);
}



bool CHttpPlus::httpSendFile(string sSendUrl,			/*欲访问的URL							     */
                             string* pRetData,			/*接受数据的容器						     */
                             string sHttpType,			/* false , GET || ture , POST 			 */
                             bool bIsAcceptData,		/* false , No Accept || ture , Accept 	 */
                             string sProxyIP,
                             string* sAppendHttpHead,	/*Http 的 附加头                           */
                             string  sFileName ,			/*Set Send File Name                     */
                             string* sReturnHead		/*返回的HTTP HEAD                          */
                         )
{
//    CURL* curl = curl_easy_init();

    CallbackData calldata;
    CURLcode nCode = CURLE_FAILED_INIT;
    long http_code = 0;

    FILE* pFile = NULL;
    if(pRetData)
        pRetData->clear();

    if(sReturnHead)
        sReturnHead->clear();

    if (curl)
    {
        curl_easy_reset(curl);
        curl_easy_setopt(curl, CURLOPT_URL, sSendUrl.c_str());
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, sHttpType.c_str());
        if(sHttpType == "HEAD")
             curl_easy_setopt(curl,CURLOPT_NOBODY,1);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "aws-sdk-dotnet-45/3.1.1.1 aws-sdk-dotnet-core/3.1.0.1 .NET_Runtime/4.0 .NET_Framework/4.0 OS/Microsoft_Windows_NT_6.1.7601_Service_Pack_1 ClientSync");
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
        curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1L);
//        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT_MS,800);
        if(!sProxyIP.empty())
        {
            curl_easy_setopt(curl, CURLOPT_PROXY, sProxyIP.c_str());
        }
//        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT_MS,1000);

        if(bIsAcceptData && pRetData)
        {
            //抓取内容后，回调函数
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, call_wirte_func);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, pRetData);
        }

        //抓取头信息，回调函数
        if(sReturnHead)
        {
            curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, call_headerw_func);
            curl_easy_setopt(curl, CURLOPT_HEADERDATA, sReturnHead);
        }


        vector<string> sHeadList;
        struct curl_slist *chunkHead = NULL;
        pFile = fopen( sFileName.c_str() ,"rb");  // 支持中文
        if (!sFileName.empty() && pFile )
        {
            fseek(pFile,0,SEEK_END); //定位到文件末
            long nFileLen = ftell(pFile); //文件长度
            rewind(pFile);

            calldata.pHttpPlus = this;
            calldata.pData = pFile;
            calldata.nTotalSize = nFileLen;

            curl_easy_setopt(curl, CURLOPT_POST, 1L);
            curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, nFileLen);
            curl_easy_setopt(curl, CURLOPT_READFUNCTION, call_read_file_func);
            curl_easy_setopt(curl, CURLOPT_READDATA, &calldata);
            sHeadList.push_back("Content-Type: application/octet-stream");
        }

        if(sAppendHttpHead)
            SetHttpHead(*sAppendHttpHead, sHeadList);

        if (!sHeadList.empty())
        {
            for (size_t i = 0; i < sHeadList.size(); ++i)
            {
                chunkHead = curl_slist_append(chunkHead, sHeadList[i].c_str());
            }
        }
        if(chunkHead)
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunkHead);

        nCode = curl_easy_perform(curl);

        if(chunkHead)
            curl_slist_free_all(chunkHead);

//        curl_easy_cleanup(curl);

        if(pFile)
            fclose(pFile);

        if(nCode != CURLE_OK)
        {
            //cerr << sHttpType << '\t' << sSendUrl <<'\t'  << "CURLCode:" << nCode << "\tDetail:" << curl_easy_strerror( nCode );
        }

        curl_easy_getinfo (curl, CURLINFO_RESPONSE_CODE, &http_code);

        if( http_code > 400)
        {
            //cerr << sHttpType << '\t' << sSendUrl <<'\t'  << "RspDatas:" <<( pRetData != NULL ? pRetData->c_str() : "");
        }
        else
        {
            //cout << sHttpType << '\t' << sSendUrl <<'\t'  << "RspDatas:" <<( pRetData != NULL ? pRetData->c_str() : "");
        }
    }

    if(nCode != CURLE_OK && bIsAcceptData && pRetData)
    {
        pRetData->append("<Error>");
    }

    return (CURLE_OK == nCode && http_code < 400);
}

//注意：当字符串为空时，也会返回一个空字符串
void CHttpPlus::split(std::string& s, std::string delim,std::vector< std::string >* ret)
{
    size_t last = 0;
    size_t index=s.find(delim,last);
    while (index != std::string::npos)
    {
        ret->push_back(s.substr(last,index-last));
        last=index+delim.size();
        index=s.find(delim,last);
    }
	ret->push_back(s.substr(last,s.length() - last));
}

std::string CHttpPlus::DNSParse(const std::string& domain)
{
	string ip;
	int a,b,c,d;  
	int ret=0;
	ret = sscanf(domain.c_str(),"%d.%d.%d.%d",&a,&b,&c,&d);   
	if( ret == 4 &&(a>=0&&a<=255)&&(b>=0&&b<=255)&&(c>=0&&c<=255)&&(d>=0&&d<=255))   
	{
		return domain;  
	}

	if (curl)
	{
		curl_easy_reset(curl);
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);
		curl_easy_setopt(curl, CURLOPT_URL, domain.c_str());
		if ( curl_easy_perform(curl) != CURLE_COULDNT_RESOLVE_HOST)
		{
			char* ipstr = NULL;
			if( curl_easy_getinfo(curl, CURLINFO_PRIMARY_IP, &ipstr ) == CURLE_OK)
				ip = ipstr;
		}
	}
	return ip;
}

// 附加 Http Head .
bool CHttpPlus::SetHttpHead(string sHttpHead , vector<string> & sHeadList)
{
    split(sHttpHead,"\n",&sHeadList);
	return true;
}

/*
	ptr是指向存储数据的指针，
	size是每个块的大小，
	nmemb是指块的数目，
	stream是用户参数。
	所以根据以上这些参数的信息可以知道，ptr中的数据的总长度是size*nmemb
*/
size_t CHttpPlus::call_wirte_func(const char *ptr, size_t size, size_t nmemb, string *stream)
{
	size_t len = size * nmemb;
    if(ptr && len > 0)
        stream->append(ptr, len);
    return len;
}

size_t CHttpPlus::call_wirte_file_func(const char *ptr, size_t size, size_t nmemb, CallbackData* pData)
{
    FILE *pFile = (FILE *)pData->pData;
    CHttpPlus *pHttp =pData->pHttpPlus;
    size_t nWrite = fwrite(ptr,size,nmemb,pFile);
    pData->nIdleSize += nWrite;
	if ( pHttp )
	{
		time(&pHttp->m_tLastRecvTime);
		pHttp->WriteProgress(pData->nIdleSize,pData->nTotalSize);

		if (pHttp->MAXRECVBYTEPER != -1 && pData->nIdleSize >= pHttp->MAXRECVBYTEPER)
		{
			return -1;
		}
		else if ( pHttp->bIsStopDownload != NULL && *pHttp->bIsStopDownload )
		{
			return -1;
		}
	}
    return nWrite;
}

//size_t CHttpPlus::call_wirte_file_func_thread(const char *ptr, size_t size, size_t nmemb, ThreadTask* pThreadTask)
//{
//    return size*nmemb;

//    if(pThreadTask->m_nOffset > pThreadTask->m_nRangeSize)
//        return 0;

//    pThreadTask->m_Mutex->lock();

//    fseek(pThreadTask->m_pFileMemStart,pThreadTask->m_nRangePos + pThreadTask->m_nOffset,SEEK_SET);

//    size_t len = fwrite(ptr,size,nmemb,pThreadTask->m_pFileMemStart);

//    pThreadTask->m_nOffset += len;

//    pThreadTask->m_Mutex->unlock();

//    return len;
//}

size_t CHttpPlus::call_read_file_func(void *ptr, size_t size, size_t nmemb, CallbackData* pData)
{
    FILE *pFile =(FILE *)pData->pData;
    CHttpPlus *pHttp =pData->pHttpPlus;
    size_t nRead = fread(ptr,size,nmemb,pFile);
    pData->nIdleSize += nRead;
    pHttp->ReadProgress(nRead , pData->nTotalSize);

    return nRead;
}

size_t CHttpPlus::call_read_string_func(void *ptr, size_t size, size_t nmemb, CallbackData* pData)
{
//    qDebug()<<"call_read_file_funcp kg";
    string *pStr =(string *)pData->pData;
    CHttpPlus *pHttp =pData->pHttpPlus;
    size_t nRead = size*nmemb;
    if(pStr->size() < (pData->nIdleSize + nRead))
        nRead = pStr->size() - pData->nIdleSize;
    if(pData->nIdleSize >= pStr->size())
    {
        return 0;
    }
    memcpy(ptr,pStr->c_str() + pData->nIdleSize,nRead);
    pData->nIdleSize += nRead;
    pHttp->ReadProgress(nRead , pData->nTotalSize);
    return nRead;
}

// 返回http header回调函数  
size_t CHttpPlus::call_headerw_func(const char  *ptr, size_t size, size_t nmemb, string *stream)
{
    size_t len = size * nmemb;
    if(ptr && len > 0 && stream)
        stream->append(ptr, len);
	return len;
}

void CHttpPlus::WriteProgress(__int64 nWriteCount,__int64 nTotalSize)
{
	
}

void CHttpPlus::ReadProgress(__int64 nReadCount,__int64 nTotalSize)
{

}

void CHttpPlus::HttpAbort()
{
	if (curl)
	{
		curl_easy_cleanup(curl);
		curl = NULL;
	}
}


CHttpPlus::CallbackData::CallbackData()
{
    this->nIdleSize = 0;
    this->nTotalSize = 0;
    this->pData = NULL;
    this->pHttpPlus = NULL;
}


string CHttpPlus::url_encode_(const string sUrl)
{
	return curl_escape(sUrl.c_str(),sUrl.size());
}

bool CHttpPlus::M3u8Parse(const string& m3u8Context,vector<string>& sListUrl,size_t nTotalTime)
{
	const char* p = m3u8Context.c_str();
	const char* sLineStart = NULL;
	size_t nRead,nLineCount,nTotalCount,nTime;
	nRead = nLineCount = nTotalCount = 0;
	nTotalCount = m3u8Context.size();
	string sLine;
	sListUrl.clear();
	while( p && nRead <= nTotalCount)
	{
		if ( *p == '\r' || *p == '\n' )
		{
			if( nLineCount > 0 )
			{
				sLine = string(sLineStart,nLineCount);
				if (!sLine.empty() && sLine[0] == '#' && sLine.substr(0,8) == "#EXTINF:")
				{
					nTime = strtod( (sLine.c_str()+8),NULL );
					if ( nTime > 0 )
					{
						nTotalTime += nTime;
					}
				}
				else if(!sLine.empty() && sLine[0] != '#' && sLine.find(".ts") != string::npos)
				{
					sListUrl.push_back(sLine);
				}
				sLineStart = NULL;
				nLineCount = 0;
			}
		}
		else
		{
			if (nLineCount == 0)
			{
				sLineStart = p;
			}
			nLineCount++;
		}
		p++;
		nRead++;
	}
	if( nLineCount > 0 )
	{
		sLine = string(sLineStart,nLineCount);
		if (!sLine.empty() && sLine[0] == '#' && sLine.substr(0,8) == "#EXTINF:")
		{
			nTime = strtod( (sLine.c_str()+8),NULL );
			if ( nTime > 0 )
			{
				nTotalTime += nTime;
			}
		}
		else if(sLine.find(".ts") != string::npos)
		{
			sListUrl.push_back(sLine);
		}
	}
	return (nTotalTime>0 && sListUrl.size());
}

bool CHttpPlus::Getmiddletext(string src, char *left, char *right,string & str_ret)
{
	size_t site_start,site_end;
	string str_left(left),str_right(right);
	if ((site_start=src.find(str_left.c_str()))!=string::npos)
	{
		if ((site_end=src.find(str_right.c_str(),site_start+str_left.size()))!=string::npos)
		{
			str_ret=src.substr(site_start+str_left.size(),site_end-site_start-str_left.size());
			return true;
		}
	}
	return false;
}


const std::string CHttpPlus::GetTime()
{
	time_t  timev;
	time(&timev);
	struct tm* tm = localtime(&timev);
	char time[255];
	strftime(time, 255, "%Y-%m-%d %H.%M.%S", tm);
	return time;
}

void CHttpPlus::replaceAll(std::string& str, const std::string& from, const std::string& to) {
	if(from.empty())
		return;
	size_t start_pos = 0;
	while((start_pos = str.find(from, start_pos)) != std::string::npos) {
		str.replace(start_pos, from.length(), to);
		start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
	}
}

bool CHttpPlus::HttpGetMp4ByM3u8(const string url,FILE* pFile)
{
	if (pFile == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	size_t nIdleSize = 0;
	string sRet,sData,sHost,sMp4Url,url_t,url_;
	size_t pos = url.rfind('/');
	if (pos != string::npos)
	{
		sHost = url.substr(0,pos+1);
	}
	if (sHost.empty())
	{
		return false;
	}

	// 移动到文件尾

	string sLastUrl;
	vector<string> sListUrl;
	vector<string> sDownloadUrl;
	size_t nTotalTime = 0;
	bool bFind = false;
	url_ = url;
	do 
	{
		nTotalTime = 5000;
		bFind = false;
		sDownloadUrl.clear();

		if( httpSend(url_,&sRet) )
		{
			if (sRet.find("Access Denied.") != string::npos )
			{
				break;
			}
			else if(sRet.substr(0U,7) == "#EXTM3U")
			{
				if(M3u8Parse(sRet,sListUrl,nTotalTime))
				{
					if (!sLastUrl.empty())
					{
						for ( size_t i = 0; i < sListUrl.size(); ++i )
						{
							if (sListUrl[i] == sLastUrl)
							{
								bFind = true;
							}
							else if(bFind && sListUrl[i] != sLastUrl)
							{
								sDownloadUrl.push_back(sListUrl[i]);
							}
						}
					}
					if (!bFind)
					{
						sDownloadUrl = sListUrl;
					}
					if (!sDownloadUrl.empty())
					{
						sLastUrl = sDownloadUrl[sDownloadUrl.size()-1];
					}

					for (size_t i = 0;i < sDownloadUrl.size(); ++i)
					{
						sMp4Url.clear();
						if (sDownloadUrl[i].substr(0,4) != "http")
						{
							sMp4Url = sHost;
						}
						sMp4Url.append(sDownloadUrl[i]);
						size_t calc = 0;
						while(true)
						{
							string header;
							httpSend(sMp4Url,&sData,"GET",true,"",NULL,NULL,&header);
							if(Getmiddletext(header,"Location: ","\r\n",sMp4Url))
							{
								if (!sMp4Url.empty())
								{
									httpSend(sMp4Url,&sData);
								}
							}
							
							if (!sData.empty() && sData.size() > 1024 && sData[0] == 0x47 && sData[1] == 0x40)
							{
								//cout << sDownloadUrl[i] <<endl;
								fwrite(sData.c_str(),sizeof(char),sData.size(),pFile);
								break;
							}
							else
							{
								if (calc++ > 20)
								{
									break;
								}
							}
						}
					}
				}
			}
			else
			{
				break;
			}
		}

		if (MAXRECVBYTEPER != -1 && ftell(pFile) >= MAXRECVBYTEPER)
		{
			return false;
		}
		else if ( bIsStopDownload != NULL && *bIsStopDownload )
		{
			return false;
		}
		Sleep(5000);
	} while ( true );

	return true;
}