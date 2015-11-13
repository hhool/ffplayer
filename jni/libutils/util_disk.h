#ifndef __UtilFILE_H__
#define __UtilFILE_H__

#include <stdio.h>
#include <stdlib.h>
#include "config/stl_config.h"

class UtilFile {
public:
    UtilFile ();

    static int cp(const char* src, const char* dest);

    static int mv(const char* src, const char* dest);

    static int info (const char* file);

    static int exist (const char* file);

    static int diff (const char* src, const char* dest);

    static int rename(const char *oldpath, const char *newpath);
private:
    
};

#endif
