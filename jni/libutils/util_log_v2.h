/**
 * @file   util_log_v2.h
 * @author windsome <windsome@windsome-desktop>
 * @date   Tue Apr 13 14:46:32 2010
 * 
 * @brief  
 * 
 * 
 */
#ifndef __UtilLOG_V2_H__
#define __UtilLOG_V2_H__

#define ATOM_LOG 1

#include "config/stl_config.h"

#ifdef ATOM_LOG
#include <pthread.h>
#endif

typedef enum {
    CLL_EMERG  = 0,
    CLL_FATAL,
    CLL_ALERT,
    CLL_CRIT,
    CLL_ERROR,
    CLL_WARN,
    CLL_NOTICE,
    CLL_INFO,
    CLL_DEBUG,
    CLL_VERBOSE,
    CLL_NOTSET
} LogLevel;

class UtilLog {
private:
    static UtilLog *mpLogger; // the singleton implementing logging

    LogLevel mLogLevel;
    string msPrefix;
    string msLogFilePath;
    bool mbLogLocation;

    FILE* mpLogFile;

#ifdef ATOM_LOG
    // lock mode.
    pthread_mutex_t mMutex;
#endif

public:

    UtilLog ();
    ~UtilLog ();

    static UtilLog* getInstance ();
    static void destroyInstance ();

    void SetLogFile (const char* logPath);
    void Reset ();

    void SetLevel (LogLevel level) { mLogLevel = level; }
    LogLevel GetLevel () { return mLogLevel; }
    bool IsLoggable (LogLevel level) { return level <= mLogLevel; }

    void _Log (const LogLevel& level, const char* file, const char* func, const int line, const char* format, ...);

    //void setPrefix (const char *prefix) { msPrefix = prefix;}
    //size_t getLogSize ();
};

# define LOGGER UtilLog::getInstance()

#define VERBOSE(fmt...)                                                   \
    do { LOGGER->_Log(CLL_VERBOSE, __FILE__, __FUNCTION__, __LINE__, fmt);} while (0)
    
#define DEBUG(fmt...)                                                   \
    do { LOGGER->_Log(CLL_DEBUG, __FILE__, __FUNCTION__, __LINE__, fmt);} while (0)
    
#define INFO(fmt... )                                                   \
    do { LOGGER->_Log(CLL_INFO, __FILE__, __FUNCTION__, __LINE__, fmt);} while (0)
    
#define ERROR(fmt...)                                                   \
    do { LOGGER->_Log(CLL_ERROR, __FILE__, __FUNCTION__, __LINE__, fmt);} while (0)
    
#define WARN(fmt...)                                                    \
    do { LOGGER->_Log(CLL_WARN, __FILE__, __FUNCTION__, __LINE__, fmt);} while (0)
    
#define FATAL(fmt...)                                                   \
    do { LOGGER->_Log(CLL_FATAL, __FILE__, __FUNCTION__, __LINE__, fmt);} while (0)
    
#endif
