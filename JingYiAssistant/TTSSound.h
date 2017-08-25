#pragma once

#include "qtts.h"
#include "msp_cmn.h"
#include "msp_errors.h"

#ifdef _WIN64
#pragma comment(lib,"msc_x64.lib")//x64
#else
#pragma comment(lib,"msc.lib")//x86
#endif

class CTTSSound
{
public:
	CTTSSound(void);
	~CTTSSound(void);

	static void init();
	int text_to_speech(string src_text, string &des_data);
	static bool bIsInited;
};

