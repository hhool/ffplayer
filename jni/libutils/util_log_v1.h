/**
 * @file   util_log_v1.h
 * @author windsome <windsome@windsome-desktop>
 * @date   Tue Apr 13 14:49:21 2010
 * 
 * @brief  
 * 
 * 
 */
#ifndef __UtilLOG_V1_H__
#define __UtilLOG_V1_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

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
} eUtilLogLevel;

//static const char* logmod = "";
void _LogSetLevel (const eUtilLogLevel level, int bLocation);
void _Log (const eUtilLogLevel& level, const char* file, const char* func, const int line, const char* format, ...);

#define LOGLEVEL(level, bLoc)                        \
    do { _LogSetLevel(level, bLoc);} while (0)

//#define LOGMODULE(module)  logmod = module;

//#define LOGMODULE(module)                     
//    do { logmod = module;} while (0)

#define VERBOSE(fmt...)                                                   \
    do { _Log(CLL_VERBOSE, __FILE__, __FUNCTION__, __LINE__, fmt);} while (0)
    
#define DEBUG(fmt...)                                                   \
    do { _Log(CLL_DEBUG, __FILE__, __FUNCTION__, __LINE__, fmt);} while (0)
    
#define INFO(fmt... )                                                   \
    do { _Log(CLL_INFO, __FILE__, __FUNCTION__, __LINE__, fmt);} while (0)
    
#define ERROR(fmt...)                                                   \
    do { _Log(CLL_ERROR, __FILE__, __FUNCTION__, __LINE__, fmt);} while (0)
    
#define WARN(fmt...)                                                    \
    do { _Log(CLL_WARN, __FILE__, __FUNCTION__, __LINE__, fmt);} while (0)
    
#define FATAL(fmt...)                                                   \
    do { _Log(CLL_FATAL, __FILE__, __FUNCTION__, __LINE__, fmt);} while (0)
    
#endif
