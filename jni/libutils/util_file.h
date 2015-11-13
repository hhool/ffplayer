/**
 * @file   util_file.h
 * @author windsome <windsome@windsome-desktop>
 * @date   Tue Jan 12 18:14:05 2010
 * 
 * @brief  
 * 
 * 
 */
#ifndef __UtilFILE_H__
#define __UtilFILE_H__

#include <stdio.h>
#include <stdlib.h>
#include "util_types.h"
#include "config/stl_config.h"

class UtilFile {
public:
    UtilFile ();

    static RTCode cp(const char* src, const char* dest);

    static RTCode mv(const char* src, const char* dest);

    static RTCode rm(const char* file);

    static RTCode info (const char* file);

    static bool exist (const char* file);

    static RTCode diff (const char* src, const char* dest);

    static RTCode rename(const char *oldpath, const char *newpath);
private:
    
};

#endif
