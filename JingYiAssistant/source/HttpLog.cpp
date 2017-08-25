#include "HttpLog.h"
#include "HttpPlus.h"

CHttpLog::CHttpLog(std::string instaceName ,std::string pp, bool isOutRollFile, bool isAppedCOUT )
{
	m_osAppender = NULL;
	m_RollfileAppender = NULL;
	m_fileAppender = NULL;
	m_InstcanceName = instaceName;
	m_sFileOutPattern = pp;
	m_bIsInited = false;
	m_bIsOutRollFile = isOutRollFile;
	m_bIsAppedCOUT = isAppedCOUT;
}

CHttpLog::CHttpLog( std::string instaceName,bool isOutRollFile /*= true*/, bool isAppedCOUT /*= true*/ )
{
	m_osAppender = NULL;
	m_RollfileAppender = NULL;
	m_fileAppender = NULL;
	m_InstcanceName = instaceName;
	m_sFileOutPattern = "%d{%Y-%m-%d %H:%M:%S:%l} %6p %m%n";
	m_bIsInited = false;
	m_bIsOutRollFile = isOutRollFile;
	m_bIsAppedCOUT = isAppedCOUT;
}

CHttpLog::~CHttpLog()
{
	exitlog();
	m_osAppender = NULL;
	m_RollfileAppender = NULL;
	m_fileAppender = NULL;
}

void CHttpLog::initlog( const char* sPriority /*= NULL*/ )
{
	m_bIsInited = true;
	int argc = 0;
	char** argv = NULL;

	int g_PriorityLevel = log4cpp::Priority::INFO;
	if(sPriority)
	{
		if (strcmp(sPriority,"NOTSET") == 0)
			g_PriorityLevel = log4cpp::Priority::NOTSET  ;
		else if (strcmp(sPriority,"DEBUG") == 0)
			g_PriorityLevel = log4cpp::Priority::DEBUG  ;
		else if(strcmp(sPriority,"INFO") == 0)
			g_PriorityLevel = log4cpp::Priority::INFO  ;
		else if(strcmp(sPriority,"NOTICE") == 0)
			g_PriorityLevel = log4cpp::Priority::NOTICE  ;
		else if(strcmp(sPriority,"WARN") == 0)
			g_PriorityLevel = log4cpp::Priority::WARN  ;
		else if(strcmp(sPriority,"ERROR") == 0)
			g_PriorityLevel = log4cpp::Priority::ERROR  ;
		else if(strcmp(sPriority,"CRIT") == 0)
			g_PriorityLevel = log4cpp::Priority::CRIT  ;
		else if(strcmp(sPriority,"ALERT") == 0)
			g_PriorityLevel = log4cpp::Priority::ALERT  ;
	}
	log4cpp::Category& tCategory_ = log4cpp::Category::getInstance(m_InstcanceName);
	if(m_osAppender)
	{
		tCategory_.removeAppender(m_osAppender);
		m_osAppender = NULL;
	}
	if(m_RollfileAppender)
	{
		tCategory_.removeAppender(m_RollfileAppender);
		m_RollfileAppender = NULL;
	}
	if(m_fileAppender)
	{
		tCategory_.removeAppender(m_fileAppender);
		m_fileAppender = NULL;
	}

	if(m_bIsAppedCOUT)
	{
		m_osAppender = new log4cpp::OstreamAppender(m_InstcanceName+"_cout", &cout);
		log4cpp::PatternLayout* pattenOSLayout = new log4cpp::PatternLayout();
		pattenOSLayout->setConversionPattern("%d{%Y-%m-%d %H:%M:%S:%l} %6p %m%n");
		m_osAppender->setLayout(pattenOSLayout);
		m_osAppender->setThreshold( g_PriorityLevel );
	}

	char tszModule[MAX_PATH + 1] = { 0 };
	::GetModuleFileNameA(NULL, tszModule, MAX_PATH);
	string sInstancePath = tszModule;
	size_t sPos = sInstancePath.rfind('\\');
	if (sPos != string::npos)
	{
		sInstancePath = sInstancePath.substr(0,sPos);
	}
	


	string sFileName = sInstancePath;
	sFileName += "\\log";
	
	CHttpPlus::CreatDir(sFileName.c_str());
	sFileName += "\\";
	sFileName += m_InstcanceName.c_str();
	if(m_bIsOutRollFile)
	{
		m_RollfileAppender = new log4cpp::RollingFileAppender(m_InstcanceName + "_RollingFileAppender" ,sFileName.c_str(), 50 * 1024 *1024, 200);
		log4cpp::PatternLayout* pattenLayout = new log4cpp::PatternLayout();
		pattenLayout->setConversionPattern( m_sFileOutPattern );
		m_RollfileAppender->setLayout( pattenLayout );
		m_RollfileAppender->setThreshold( log4cpp::Priority::DEBUG );
	}
	else
	{
		m_fileAppender = new log4cpp::FileAppender(m_InstcanceName + "_FileAppender" ,sFileName.c_str());
		log4cpp::PatternLayout* pattenLayout = new log4cpp::PatternLayout();
		pattenLayout->setConversionPattern( m_sFileOutPattern );
		m_fileAppender->setLayout( pattenLayout );
		m_fileAppender->setThreshold( log4cpp::Priority::DEBUG );
	}
	log4cpp::Category& tCategory = log4cpp::Category::getInstance(m_InstcanceName);
	if(m_osAppender)
		tCategory.addAppender( m_osAppender );
	if(m_RollfileAppender)
		tCategory.addAppender( m_RollfileAppender );
	if(m_fileAppender)
		tCategory.addAppender( m_fileAppender );

	tCategory.setPriority( log4cpp::Priority::DEBUG );
}

void CHttpLog::exitlog()
{
	if(false && m_bIsInited)
	{
		m_bIsInited = false;
		log4cpp::Category& tCategory_ = log4cpp::Category::getInstance(m_InstcanceName);
		tCategory_.removeAllAppenders();
		tCategory_.shutdown();
	}
}

void CHttpLog::log( log4cpp::Priority::PriorityLevel Level,string& message )
{
	if(!m_bIsInited)
		initlog();
	log4cpp::Category& tCategory = log4cpp::Category::getInstance(m_InstcanceName);
	tCategory.log(Level,message );
}

log4cpp::CategoryStream CHttpLog::log( int Level )
{
	if(!m_bIsInited)
		initlog();
	log4cpp::Category& tCategory = log4cpp::Category::getInstance(m_InstcanceName);
	return tCategory.getStream(Level);
}
