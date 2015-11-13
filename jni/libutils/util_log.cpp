/**
 * @file   util_log.cpp
 * @author windsome <windsome@windsome-desktop>
 * @date   Tue Jan 12 18:13:36 2010
 * 
 * @brief  
 * 
 * 
 */
#ifdef PLATFORM_ANDROID
#include "util_log_android.cpp"
#else
#include "util_log_v2.cpp"
#endif
