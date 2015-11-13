/**
 * @file   util_format.cpp
 * @author windsome <windsome@windsome-desktop>
 * @date   Tue Jan 12 18:13:53 2010
 * 
 * @brief  
 * 
 * 
 */
#include "util_format.h"
#include "util_log.h"

string UtilFormat::Int2String (int value)
{
    string ret = "";
    char buffer[50] = {0};
    sprintf (buffer, "%d", value);
    ret = buffer;
    return ret;
}

