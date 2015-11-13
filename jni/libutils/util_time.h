/**
 * @file   util_time.h
 * @author windsome <windsome@windsome-desktop>
 * @date   Tue Jan 12 18:12:05 2010
 * 
 * @brief  
 * 
 * 
 */
#ifndef __UtilTIME_H__
#define __UtilTIME_H__

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "config/stl_config.h"

class UtilTime {
public:
    UtilTime (int sec = -1);

    string GetFormatString ();
    static string CurrentTimeString ();

private:
    time_t miTime;
};

#endif
