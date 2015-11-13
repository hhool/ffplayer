/**
 * @file   util_log_v2.cpp
 * @author windsome <windsome@windsome-desktop>
 * @date   Tue Apr 13 13:36:22 2010
 * 
 * @brief  
 * 
 * 
 */
#include "util_log_v2.h"
#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <string.h>

UtilLog* UtilLog::mpLogger = NULL;
UtilLog::UtilLog ()
    : mLogLevel(CLL_DEBUG), msPrefix(""), mbLogLocation(true) {
    mpLogFile = NULL;
    msLogFilePath = "";
#ifdef ATOM_LOG
    pthread_mutex_init(&mMutex, 0);
#endif
}
UtilLog::~UtilLog () {
    if (mpLogFile) {
        fclose (mpLogFile);
        mpLogFile = NULL;
    }
#ifdef ATOM_LOG
    pthread_mutex_destroy(&mMutex);
#endif
}

UtilLog* UtilLog::getInstance () {
    if (mpLogger == NULL)
        mpLogger = new UtilLog ();
    return mpLogger;
}
void UtilLog::destroyInstance () {
    if (mpLogger) {
        delete mpLogger;
        mpLogger = NULL;
    }
}

void UtilLog::SetLogFile (const char *logPath) {
    if (logPath == msLogFilePath) {
        printf ("same log file path (%s), ignore it!\n", logPath);
        return;
    }

    if (mpLogFile != NULL) {
        fclose(mpLogFile);
        mpLogFile = NULL;
    }

    msLogFilePath = logPath;
    mpLogFile = fopen(logPath, "w" );
    if (mpLogFile == NULL) {
        printf ("open log file fail! %s\n", msLogFilePath.c_str ());
    }

}
void UtilLog::Reset () {
    if (mpLogFile) {
        ftruncate(fileno(mpLogFile), 0);
    }
}

void UtilLog::_Log (const LogLevel& level, const char* file, const char* func, const int line, const char* format, ...)
{
    if (level > mLogLevel)
        return;

#ifdef ATOM_LOG
    pthread_mutex_lock(&mMutex);
#endif

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

    if (mbLogLocation) {
        fprintf (stdout, "[%s %s:%d - %s] ", levelstring, filename, line, func);
        if (mpLogFile)
            fprintf (mpLogFile, "[%s %s:%d - %s] ", levelstring, filename, line, func);
    } else {
        fprintf (stdout, "[%s] ", levelstring);
        if (mpLogFile)
            fprintf (mpLogFile, "[%s] ", levelstring);
    }
   
    va_list va;
    va_start(va, format);
    vfprintf (stdout, format, va);
    if (mpLogFile)
        vfprintf (mpLogFile, format, va);
    va_end(va);
    
    fprintf (stdout, "\n");
    if (mpLogFile)
        fprintf (mpLogFile, "\n");

#ifdef ATOM_LOG
    pthread_mutex_unlock(&mMutex);
#endif
}

