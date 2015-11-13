/**
 * @file   util_log4cpp.cpp
 * @author windsome <windsome@windsome-desktop>
 * @date   Tue Apr 13 11:44:05 2010
 * 
 * @brief  
 * 
 * 
 */
#if 0

#include "util_log4cpp.h"
#include <pthread.h>

using namespace log4cpp;
using namespace std;

Appender* UtilLog::mp_fileAppender = NULL;
Appender* UtilLog::mp_stdAppender = NULL;
SyslogAppender* UtilLog::mp_syslogAppender = NULL;
Category* UtilLog::mp_rootCategory = NULL;
PatternLayout* UtilLog::mp_patternLayout = NULL;
UtilLog* UtilLog::mp_logInstance = NULL;

UtilLog* UtilLog::getInstance () {
    if (!mp_logInstance) {
        mp_logInstance = new UtilLog ();
    }

    return mp_logInstance;
}

UtilLogDomain::UtilLogDomain (const string& domainName) {
    Category& ret = Category::getInstance(domainName);
    mp_Category = &ret;
}

UtilResult UtilLog::CreateLog (UtilLogDomain*& log, const string& domainName, eUtilLogType type, eUtilLogLevel level) {
    UtilLogDomain* ret = new UtilLogDomain (domainName);

    switch (type) {
    case CLT_STD:
        ret->SetAppender (mp_stdAppender);
        break;
    case CLT_FILE:
        ret->SetAppender (mp_fileAppender);
        break;
    case CLT_SYSLOG:
        ret->SetAppender (mp_syslogAppender);
        break;
    default:
        break;
    }
    
    ret->SetPriority (level);

    log = ret;    
    return CR_OK;
}

UtilResult UtilLog::DestroyLog (UtilLogDomain* log) {
    delete log;
    log = NULL;

    return CR_OK;
}

UtilLog::UtilLog (const string& logFile) {
    mb_locInfo = false;
    ms_fileAppenderName = logFile;
    InitFileAppender (ms_fileAppenderName);

    Category& root = Category::getRoot();
    mp_rootCategory = &root;
    mp_rootCategory->setPriority (Priority::ERROR);
}

UtilLog::UtilLog () {
    Category& ret = Category::getInstance("UtilTVMW");

    me_logLevel = Priority::DEBUG;

    mp_rootCategory = &ret;
    mp_rootCategory->setPriority (me_logLevel);

    mp_patternLayout = new log4cpp::PatternLayout ();
    
    Location (false);
    Timestamp (false);

    InitStdAppender ();
}

void UtilLog::InitFileAppender (const string& file) {
    mp_fileAppender = new FileAppender("default", file.c_str ());
    mp_fileAppender->setLayout(new BasicLayout());

    mp_rootCategory->addAppender (mp_fileAppender);
}

void UtilLog::InitStdAppender () {
    mp_stdAppender = new OstreamAppender("default", &std::cout);
    
    mp_stdAppender->setLayout(mp_patternLayout);

    mp_rootCategory->addAppender (mp_stdAppender);
}

void UtilLog::InitSyslogAppernder () {
	mp_syslogAppender = new SyslogAppender("syslog", "log4cpp");
    mp_syslogAppender->setLayout(new log4cpp::BasicLayout());

    mp_rootCategory->addAppender (mp_syslogAppender);
}

void UtilLog::FiniAppenders () {

}

void UtilLog::SetLevel (const eUtilLogLevel& level) {
    me_logLevel = level;
    mp_rootCategory->setPriority (me_logLevel);    
}

void UtilLog::Timestamp (const bool& btime) {
    mb_timeInfo = btime;
    if (mb_timeInfo)
        mp_patternLayout->setConversionPattern (UtilLOG_CONV_PATTERN_TIME);
    else
        mp_patternLayout->setConversionPattern (UtilLOG_CONV_PATTERN_LOC);
}

void UtilLog::Location (const bool& bloc) {
    mb_locInfo = bloc;
}

UtilLog::~UtilLog () {
    if (mp_logInstance) {
        if (mp_rootCategory) {
            mp_rootCategory->shutdown ();
            mp_rootCategory = NULL;
        }

        mp_logInstance = NULL;
    }
}

void UtilLog::_Log (const eUtilLogLevel& level, const char* file, const char* func, const int line, const char* format, ...) {
    const char* filename = strrchr (file, '/');
    if (NULL == filename)
        filename = file;
    else 
        filename++;

    memset (ms_msgbuffer, 0, MAX_MSG_BUFFER_SIZE);
            
    if (mb_locInfo)
        sprintf (ms_msgbuffer, "[%s:%d - %s] ", filename, line, func);
    
    va_list va;
    va_start(va, format);
    vsprintf(&ms_msgbuffer[strlen(ms_msgbuffer)], format, va);
    va_end(va);
    
    //printf("%s\n",ms_msgbuffer);
    mp_rootCategory->log (level, ms_msgbuffer);
}

#endif
