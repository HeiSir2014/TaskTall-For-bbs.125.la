#include "std_main.h"
#include "TTSSound.h"

bool CTTSSound::bIsInited = false;
/* wav音频头部格式 */
typedef struct _wave_pcm_hdr
{
	char            riff[4];                // = "RIFF"
	int				size_8;                 // = FileSize - 8
	char            wave[4];                // = "WAVE"
	char            fmt[4];                 // = "fmt "
	int				fmt_size;				// = 下一个结构体的大小 : 16

	short int       format_tag;             // = PCM : 1
	short int       channels;               // = 通道数 : 1
	int				samples_per_sec;        // = 采样率 : 8000 | 6000 | 11025 | 16000
	int				avg_bytes_per_sec;      // = 每秒字节数 : samples_per_sec * bits_per_sample / 8
	short int       block_align;            // = 每采样点字节数 : wBitsPerSample / 8
	short int       bits_per_sample;        // = 量化比特数: 8 | 16

	char            data[4];                // = "data";
	int				data_size;              // = 纯数据长度 : FileSize - 44 
} wave_pcm_hdr;

/* 默认wav音频头部数据 */
wave_pcm_hdr default_wav_hdr = 
{
	{ 'R', 'I', 'F', 'F' },
	0,
	{'W', 'A', 'V', 'E'},
	{'f', 'm', 't', ' '},
	16,
	1,
	1,
	16000,
	32000,
	2,
	16,
	{'d', 'a', 't', 'a'},
	0  
};



CTTSSound::CTTSSound(void)
{
	init();
}


CTTSSound::~CTTSSound(void)
{
}

void CTTSSound::init()
{
	if (bIsInited)
	{
		return;
	}

	const char* login_params         = "appid = 5514e686, work_dir = .";
	int ret = MSPLogin(NULL, NULL, login_params); //第一个参数是用户名，第二个参数是密码，第三个参数是登录参数，用户名和密码可在http://www.xfyun.cn注册获取
	if (MSP_SUCCESS != ret)
	{
		return;
	}

	bIsInited = true;
}

int CTTSSound::text_to_speech(string src_text, string &des_data)
{
	if (src_text.empty())
	{
		return -1;
	}
	des_data.clear();
	const char* session_begin_params = "voice_name = xiaoyan, text_encoding = gb2312,sample_rate = 16000, speed = 55, volume = 100, pitch = 50, rdn = 1";
	int          ret          = -1;
	const char*  sessionID    = NULL;
	unsigned int audio_len    = 0;
	wave_pcm_hdr wav_hdr      = default_wav_hdr;
	int          synth_status = MSP_TTS_FLAG_STILL_HAVE_DATA;

	/* 开始合成 */
	sessionID = QTTSSessionBegin(session_begin_params, &ret);
	if (MSP_SUCCESS != ret)
	{
		printf("QTTSSessionBegin failed, error code: %d.\n", ret);
		return ret;
	}
	ret = QTTSTextPut(sessionID, src_text.c_str(), src_text.size(), NULL);
	if (MSP_SUCCESS != ret)
	{
		printf("QTTSTextPut failed, error code: %d.\n",ret);
		QTTSSessionEnd(sessionID, "TextPutError");
		return ret;
	}

	
	while ( true ) 
	{
		/* 获取合成音频 */
		const void* data = QTTSAudioGet(sessionID, &audio_len, &synth_status, &ret);
		if (MSP_SUCCESS != ret)
			break;
		if (NULL != data)
		{
			des_data += string((const char*)data, audio_len);
			wav_hdr.data_size += audio_len; //计算data_size大小
		}
		if (MSP_TTS_FLAG_DATA_END == synth_status)
			break;
		Sleep(1); //防止频繁占用CPU
	}//合成状态synth_status取值请参阅《讯飞语音云API文档》
	if (MSP_SUCCESS != ret)
	{
		QTTSSessionEnd(sessionID, "AudioGetError");
		return ret;
	}
	/* 修正wav文件头数据的大小 */
	wav_hdr.size_8 += wav_hdr.data_size + (sizeof(wav_hdr) - 8);

	des_data = string((const char*)&wav_hdr,sizeof(wav_hdr)) + des_data;//添加wav音频头，使用采样率为16000
	/* 合成完毕 */
	ret = QTTSSessionEnd(sessionID, "Normal");
	if (MSP_SUCCESS != ret)
	{
		printf("QTTSSessionEnd failed, error code: %d.\n",ret);
	}

	return ret;
}