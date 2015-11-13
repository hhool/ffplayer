/**
 * @file   util_log4cpp.h
 * @author windsome <windsome@windsome-desktop>
 * @date   Tue Apr 13 14:46:56 2010
 * 
 * @brief  
 * 
 * 
 */
#ifndef __UtilLOG4CPP_H__
#define __UtilLOG4CPP_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#if 0

#include <string>

#include "log4cpp/Portability.hh"
#ifdef LOG4CPP_HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <iostream>
#include "log4cpp/Category.hh"
#include "log4cpp/Appender.hh"
#include "log4cpp/FileAppender.hh"
#include "log4cpp/OstreamAppender.hh"
#ifdef LOG4CPP_HAVE_SYSLOG
#include "log4cpp/SyslogAppender.hh"
#endif
#include "log4cpp/Layout.hh"
#include "log4cpp/BasicLayout.hh"
#include "log4cpp/PatternLayout.hh"
#include "log4cpp/Priority.hh"
#include "log4cpp/NDC.hh"

using namespace std;
using namespace log4cpp;

    typedef enum {
        CLL_EMERG  = Priority::EMERG, 
        CLL_FATAL  = Priority::FATAL,
        CLL_ALERT  = Priority::ALERT,
        CLL_CRIT   = Priority::CRIT,
        CLL_ERROR  = Priority::ERROR, 
        CLL_WARN   = Priority::WARN,
        CLL_NOTICE = Priority::NOTICE,
        CLL_INFO   = Priority::INFO,
        CLL_DEBUG  = Priority::DEBUG,
        CLL_VERBOSE= Priority::NOTSET,
        CLL_NOTSET = Priority::NOTSET
    } eUtilLogLevel;

    typedef enum {
        CLT_STD = 0,
        CLT_SYSLOG,
        CLT_FILE,
        CLT_NONE
    } eUtilLogType;    

    class UtilLog;
    class UtilLogDomain {
        friend class UtilLog;
    public:
        UtilLogDomain (const string& domainName);
        
    protected:
        void SetAppender (Appender* appender) {
            mp_Category->addAppender (appender);
        }

        void SetPriority (Priority::Value value) {
            mp_Category->setPriority (value);
        }

    private:
        Category*            mp_Category;
        Priority::Value      me_Priority;
        
        string               ms_DomainName;
    };
    
    class UtilLog {
        
    public:
        static UtilLog* getInstance ();

        /** 
         * Create a log domain named domaiName with type and levle.
         * 
         * @param log         the return reference
         * @param domainName  the name
         * @param type        the eUtilLogType
         * @param level       the eUtilLogLevel
         * 
         * @return UtilResult codes.
         */
        UtilResult CreateLog (UtilLogDomain*& log, const string& domainName, eUtilLogType type, eUtilLogLevel level);

        /** 
         * Destroy a log, release related resources.
         * 
         * @param log 
         * 
         * @return UtilResult codes. 
         */
        UtilResult DestroyLog (UtilLogDomain* log);
        
        UtilLog (const string& logFile);

        UtilLog ();

        ~UtilLog ();

        void SetLevel (const eUtilLogLevel& level);
        void Timestamp (const bool& btime = false);        
        void Location (const bool& bloc = false);

        void _Log (const eUtilLogLevel& level, const char* file, const char* func, const int line, const char* format, ...);        

    private:
        void InitFileAppender (const string& file);
        void InitStdAppender ();
        void InitSyslogAppernder ();
        void FiniAppenders ();

    private:
        static Appender*       mp_fileAppender;
        static Appender*       mp_stdAppender;
        static SyslogAppender* mp_syslogAppender;
        static Category*       mp_rootCategory;
        static PatternLayout*  mp_patternLayout;
        static UtilLog*        mp_logInstance;
        
        Priority::Value        me_logLevel;
        string                 ms_fileAppenderName;
        
        #define MAX_MSG_BUFFER_SIZE 4096
        char                   ms_msgbuffer[MAX_MSG_BUFFER_SIZE];
        bool                   mb_locInfo;
        bool                   mb_timeInfo;
    };
    
#define UtilLOG_CONV_PATTERN_TIME "[%c] [%d][%p] %m %n"
#define UtilLOG_CONV_PATTERN_LOC  "[%c] [%p] %m %n"
    
#define INFO(fmt... )                                                   \
    do { UtilLog::getInstance()->_Log(CLL_INFO, __FILE__, __FUNCTION__, __LINE__, fmt);} while (0)
    
#define DEBUG(fmt...)                                                   \
    do { UtilLog::getInstance()->_Log(CLL_DEBUG, __FILE__, __FUNCTION__, __LINE__, fmt);} while (0)
    
#define ERROR(fmt...)                                                   \
    do { UtilLog::getInstance()->_Log(CLL_ERROR, __FILE__, __FUNCTION__, __LINE__, fmt);} while (0)
    
#define WARN(fmt...)                                                    \
    do { UtilLog::getInstance()->_Log(CLL_WARN, __FILE__, __FUNCTION__, __LINE__, fmt);} while (0)
    
#define FATAL(fmt...)                                                   \
    do { UtilLog::getInstance()->_Log(CLL_FATAL, __FILE__, __FUNCTION__, __LINE__, fmt);} while (0)
    
#endif

#endif
