#include <stdio.h>
#include <errno.h>
#include <sys/stat.h>

#include "util_file.h"
#include "util_log.h"

int UtilFile::cp(const char* src, const char* dest) {
    return 0;
}

int UtilFile::mv(const char* src, const char* dest) {
    return 0;
}

int UtilFile::info (const char* file) {
    return 0;
}

int UtilFile::exist (const char* file) {
    struct stat fileinfo;
    if (0 == stat(file, &fileinfo)) {
        return 1;
    } else {
        //not exsit.
        return 0;
    }
}

int UtilFile::diff (const char* src, const char* dest) {
    return 0;
}

int UtilFile::rename(const char *oldpath, const char *newpath) {
    if (link (oldpath, newpath) != 0) {
        ERROR ("%s", strerror (errno));
        return -1;
    }
    
    if (unlink (oldpath) != 0) {
        ERROR ("%s", strerror (errno));
        return -1;
    }

    return 0;
}
