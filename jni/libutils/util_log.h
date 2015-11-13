/**
 * @file   util_log.h
 * @author windsome <windsome@windsome-desktop>
 * @date   Tue Jan 12 18:13:30 2010
 * 
 * @brief  
 * 
 * 
 */
#ifndef __UtilLOG_H__
#define __UtilLOG_H__

#ifdef VERBOSE
#undef VERBOSE
#endif

#ifdef DEBUG
#undef DEBUG
#endif

#ifdef INFO
#undef INFO
#endif

#ifdef ERROR
#undef ERROR
#endif

#ifdef WARN
#undef WARN
#endif

#ifdef FATAL
#undef FATAL
#endif

#ifdef PLATFORM_ANDROID
#include "util_log_android.h"
#else
#include "util_log_v2.h"
#endif

#endif
