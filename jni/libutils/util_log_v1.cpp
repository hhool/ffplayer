/**
 * @file   util_log_v1.cpp
 * @author windsome <windsome@windsome-desktop>
 * @date   Tue Apr 13 14:49:37 2010
 * 
 * @brief  
 * 
 * 
 */
#include "util_log_v1.h"

#define MAX_MSG_BUFFER_SIZE 32 * 1024
static char gsLogBuffer[MAX_MSG_BUFFER_SIZE] = {0};
static eUtilLogLevel gsCurrentLogLevel = CLL_DEBUG; //todo.
static int gsLogLocation = 1; // todo.
static const char* logmod = "";

void _LogSetLevel (const eUtilLogLevel level, int bLocation)
{
    gsCurrentLogLevel = level;
    gsLogLocation = bLocation;
}

void _Log (const eUtilLogLevel& level, const char* file, const char* func, const int line, const char* format, ...)
{
    if (level > gsCurrentLogLevel)
        return;

    const char* filename = strrchr (file, '/');
    if (NULL == filename)
        filename = file;
    else 
        filename++;

    const char* levelstring = "";
    switch (level)
    {
    case CLL_EMERG:  levelstring = "EMERG";     break;
    case CLL_FATAL:  levelstring = "FATAL";     break;
    case CLL_ALERT:  levelstring = "ALERT";     break;
    case CLL_CRIT:   levelstring = "CRIT ";     break;
    case CLL_ERROR:  levelstring = "ERROR";     break;
    case CLL_WARN:   levelstring = "WARN ";     break;
    case CLL_NOTICE: levelstring = "NOTIC";     break;
    case CLL_INFO:   levelstring = "INFO ";     break;
    case CLL_DEBUG:  levelstring = "DEBUG";     break;
    case CLL_NOTSET: levelstring = "NTSET";     break;
    }
    memset (gsLogBuffer, 0, MAX_MSG_BUFFER_SIZE);

    if (gsLogLocation)
        sprintf (gsLogBuffer, "[%s %s %s:%d - %s] ", levelstring, logmod, filename, line, func);
    else
        sprintf (gsLogBuffer, "[%s] ", levelstring); 
   
    va_list va;
    va_start(va, format);
    vsprintf(&gsLogBuffer[strlen(gsLogBuffer)], format, va);
    va_end(va);
    
    printf("%s\n",gsLogBuffer);
}

