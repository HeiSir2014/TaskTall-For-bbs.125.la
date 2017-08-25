#pragma once
#include <iostream>
#include <string>
#include <sstream>
using namespace std;
#include <log4cpp/Category.hh>
#include <log4cpp/Appender.hh>
#include <log4cpp/OstreamAppender.hh>
#include <log4cpp/PatternLayout.hh>
#include <log4cpp/Layout.hh>
#include <log4cpp/BasicLayout.hh>
#include <log4cpp/Priority.hh>
#include <log4cpp/FileAppender.hh>
#include <log4cpp/RollingFileAppender.hh>
#include <log4cpp/CategoryStream.hh>

class CHttpLog
{
public:
    CHttpLog(std::string instaceName = "http_.log",std::string pp = "%d; %6p %m%n", bool isOutRollFile = true, bool isAppedCOUT = true);
    CHttpLog(std::string instaceName,bool isOutRollFile = true, bool isAppedCOUT = true);
    ~CHttpLog();;
    void initlog(const char* sPriority = NULL);;
    void exitlog();;
    void log(log4cpp::Priority::PriorityLevel Level,string& message );

    log4cpp::CategoryStream log(int Level);

private:
    log4cpp::RollingFileAppender* m_RollfileAppender;
    log4cpp::FileAppender* m_fileAppender;
    log4cpp::OstreamAppender* m_osAppender;
    std::string m_InstcanceName;
    std::string m_sFileOutPattern;
    bool m_bIsInited;
    bool m_bIsOutRollFile;
    bool m_bIsAppedCOUT;
};
