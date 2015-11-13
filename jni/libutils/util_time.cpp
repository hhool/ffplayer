/**
 * @file   util_time.cpp
 * @author windsome <windsome@windsome-desktop>
 * @date   Tue Jan 12 18:12:11 2010
 * 
 * @brief  
 * 
 * 
 */
#include "util_time.h"
#include "util_log.h"

UtilTime::UtilTime (int sec) {
    if (sec < 0) {
        //get current time.
        if(time(&miTime) == -1) {
            ERROR ("get current time error!");
        }
    }
}

string UtilTime::GetFormatString () {
    string timestr = "";
    struct tm* gmt = localtime(&miTime);
    
    if(gmt) {
        char timebuf[50] = {0};
        sprintf (timebuf, "%d-%d-%d %d:%d:%d", (1900 + gmt->tm_year), gmt->tm_mon+1, gmt->tm_mday, gmt->tm_hour, gmt->tm_min, gmt->tm_sec);
        timestr = timebuf;
    }
    
    return timestr;
}

string UtilTime::CurrentTimeString () {
    string timestr = "";
    time_t currTime;

    if(time(&currTime) == -1) {
        ERROR ("get current time error!");
    } else {
        struct tm* gmt = localtime(&currTime);
        if(gmt) {
            char timebuf[50] = {0};
            sprintf (timebuf, "%d-%d-%d %d:%d:%d", (1900 + gmt->tm_year), gmt->tm_mon+1, gmt->tm_mday, gmt->tm_hour, gmt->tm_min, gmt->tm_sec);
            timestr = timebuf;
        }
    }

    return timestr;
}
