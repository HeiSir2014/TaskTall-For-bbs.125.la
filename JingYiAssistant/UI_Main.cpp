#include "std_main.h"
#include <exdisp.h>
#include <comdef.h>
#include <Dbghelp.h>

const std::string GetTime()
{
	time_t  timev;
	time(&timev);
	struct tm* tm = localtime(&timev);
	char time[255]={0};
	strftime(time, 255, "%Y-%m-%d %H.%M.%S", tm);
	return time;
}

long __stdcall except_call(_EXCEPTION_POINTERS* excp)
{
	char tszModule[MAX_PATH + 1] = { 0 };
	::GetModuleFileNameA(NULL, tszModule, MAX_PATH);
	string sInstancePath = tszModule;
	sInstancePath.append(".");
	sInstancePath.append(GetTime());
	string sInstancePathMin(sInstancePath);
	string sInstancePathFull(sInstancePath);
	sInstancePathMin.append("_Normal.dmp");
	sInstancePathFull.append("_Full.dmp");

	HANDLE hFile = ::CreateFileA( sInstancePathMin.c_str() , GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if( hFile != INVALID_HANDLE_VALUE)
	{
		MINIDUMP_EXCEPTION_INFORMATION einfo;
		einfo.ThreadId = ::GetCurrentThreadId();
		einfo.ExceptionPointers = excp;
		einfo.ClientPointers = FALSE;

		::MiniDumpWriteDump(::GetCurrentProcess(), ::GetCurrentProcessId(), hFile, MiniDumpNormal, &einfo, NULL, NULL);
		::CloseHandle(hFile);
		hFile = INVALID_HANDLE_VALUE;
	}

	hFile = ::CreateFileA( sInstancePathFull.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if( hFile != INVALID_HANDLE_VALUE)
	{
		MINIDUMP_EXCEPTION_INFORMATION einfo;
		einfo.ThreadId = ::GetCurrentThreadId();
		einfo.ExceptionPointers = excp;
		einfo.ClientPointers = FALSE;

		::MiniDumpWriteDump(::GetCurrentProcess(), ::GetCurrentProcessId(), hFile, MiniDumpWithFullMemory, &einfo, NULL, NULL);
		::CloseHandle(hFile);
	}
	return EXCEPTION_EXECUTE_HANDLER;
}

HANDLE hMapFile = NULL;  
PVOID pView = NULL;  

bool IsLoadInstance()
{
	const char* FULL_MAP_NAME = "HeiSirJingYiJieDan";
	hMapFile = OpenFileMappingA(  
		FILE_MAP_READ,          // Read access  
		FALSE,                  // Do not inherit the name  
		FULL_MAP_NAME           // File mapping name   
		);  
	if (hMapFile == NULL)   
	{
		hMapFile = CreateFileMappingA(
			INVALID_HANDLE_VALUE,   // Use paging file - shared memory  
			NULL,                   // Default security attributes  
			PAGE_READWRITE,         // Allow read and write access  
			0,                      // High-order DWORD of file mapping max size  
			4,               // Low-order DWORD of file mapping max size  
			FULL_MAP_NAME           // Name of the file mapping object  
			);
		if (hMapFile == NULL)   
		{  
			return false;
		}
		pView = MapViewOfFile(
			hMapFile,               // Handle of the map object  
			FILE_MAP_ALL_ACCESS,    // Read and write access  
			0,                      // High-order DWORD of the file offset   
			0,            // Low-order DWORD of the file offset   
			4               // The number of bytes to map to view  
			);
		return false;
	}
	else
	{
		pView = MapViewOfFile(  
			hMapFile,               // Handle of the map object  
			FILE_MAP_READ,          // Read access  
			0,                      // High-order DWORD of the file offset   
			0,            // Low-order DWORD of the file offset  
			4               // The number of bytes to map to view  
			);  
		if (pView == NULL)  
		{
			if (hMapFile)  
			{
				if (pView)
				{
					UnmapViewOfFile(pView);
					pView = NULL;
				}
				CloseHandle(hMapFile);
				hMapFile = NULL;
			}
			return true;
		}
		HWND hwnd = *((HWND*)pView);
		if (hwnd != 0)
		{
			::ShowWindow(hwnd,SW_NORMAL);
			::SetForegroundWindow(hwnd);
		}
		return true;
	}
}
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int nCmdShow)
{
	SetUnhandledExceptionFilter(except_call);

#ifndef _DEBUG
	if (IsLoadInstance())
	{
		return -1;
	}
#endif // _DEBUG
	/*
	int argc = __argc;
	char** argv = __argv;

	if (__argc > 1)
	{
		for (int i = 1 ; i < argc;++i)
		{
			string sFileName = argv[i];
			string sData = CYouKuSdk::ReadFromFile(sFileName);
			if (!sData.empty())
			{
				unsigned char* p = (unsigned char*)sData.c_str();
				for(size_t j = 0; j < sData.size(); ++j)
				{
					p[j] ^= 0x21;
				}
				sFileName.append(".zh");
				CYouKuSdk::WriteToFile(sFileName,sData);
			}
		}
		return 0;
	}
	return 1;
	*/
	
	/*
	CTTSSound s;
	string sData;
	s.text_to_speech("报警，列表中有域名被微信拉黑了。",sData);
	CYouKuSdk::WriteToFile("weixin.wav",sData);
	s.text_to_speech("报警，列表中有域名被QQ管家拉黑了。",sData);
	CYouKuSdk::WriteToFile("qq.wav",sData);
	*/

	DWORD ErrorCode = 0;
	::SetLastError(NO_ERROR);
	::CreateMutex(NULL,FALSE,_T("JingYiAssistantSingle"));
	if (::GetLastError() == ERROR_ALREADY_EXISTS)
	{
		HWND hWnd = ::FindWindow(_T("JingYiAssistantClass"), NULL);
		if (hWnd)
		{
			::ShowWindow(hWnd,SW_SHOW);
			::SetForegroundWindow(hWnd);
		}
		return 0;
	}

	HRESULT Hr = ::CoInitialize(NULL);
	if( FAILED(Hr) ) return 0;

	CPaintManagerUI::SetInstance(hInstance);
	CPaintManagerUI::SetResourcePath( CPaintManagerUI::GetInstancePath()+ _T("Skin\\"));
	//CPaintManagerUI::SetResourcePath( _T("E:\\project\\duilib_pro\\test\\") );
	//CYouKuSdk Youku;
	//CMultiLanguage::SetLanguageFile( CDuiString( _T("localization\\") ) + CGlobalConfig::g_sLangFileName );
	CDlgPubilcChild* Dlg = NULL;
#ifndef VERIYF_DISABLE
	Dlg = new CDlgPubilcChild(_T("UI_Login.xml"));
	if( Dlg->ShowWnd( _T("登录授权") ,true) == 2 )
#endif
	{
		Dlg = new CDlgPubilcChild(_T("UI_Main.xml"));
		Dlg->ShowWnd( _T("精易开发者助手") );

		if (Dlg && hMapFile && pView)
		{
			HWND hwnd = *Dlg;
			memcpy(pView,&hwnd,sizeof(HWND));
		}

		CPaintManagerUI::MessageLoop();
		CPaintManagerUI::Term();
	}
	::CoUninitialize();
	if (hMapFile) // 释放共享内存。
	{
		if (pView)
		{
			UnmapViewOfFile(pView);
			pView = NULL;
		}
		CloseHandle(hMapFile);
		hMapFile = NULL;
	}
	return 0;
}
