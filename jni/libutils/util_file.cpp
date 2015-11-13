/**
 * @file   util_file.cpp
 * @author windsome <windsome@windsome-desktop>
 * @date   Tue Jan 12 18:14:16 2010
 * 
 * @brief  
 * 
 * 
 */
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>

#include "util_file.h"
#include "util_log.h"

RTCode UtilFile::cp(const char* src, const char* dest) {
    return RT_OK;
}

RTCode UtilFile::mv(const char* src, const char* dest) {
    char cmd[512];
    sprintf (cmd, "/bin/mv %s %s", src, dest);
    system (cmd);
    
    return RT_OK;
}

RTCode UtilFile::rm(const char* file) {
    if (exist (file)) {
        if (unlink (file) != 0) {
            ERROR ("%s", strerror (errno));
            return RT_UNLINKERROR;
        }
        return RT_OK;
    } else {
        DEBUG ("no file : %s", file);
        return RT_NOSUCHFILE;
    }
}

RTCode UtilFile::info (const char* file) {
    return RT_OK;
}

bool UtilFile::exist (const char* file) {
    struct stat fileinfo;
    if (0 == stat(file, &fileinfo)) {
        return true;
    } else {
        //not exsit.
        return false;
    }
}

RTCode UtilFile::diff (const char* src, const char* dest) {
    return RT_OK;
}

RTCode UtilFile::rename(const char *oldpath, const char *newpath) {
    if (link (oldpath, newpath) != 0) {
        ERROR ("%s", strerror (errno));
        return RT_LINKERROR;
    }
    
    if (unlink (oldpath) != 0) {
        ERROR ("%s", strerror (errno));
        return RT_UNLINKERROR;
    }
    
    return RT_OK;
}
