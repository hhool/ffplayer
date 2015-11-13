/**
 * @file   util_folder.h
 * @author windsome <windsome@windsome-desktop>
 * @date   Tue Jan 12 18:13:57 2010
 * 
 * @brief  
 * 
 * 
 */
#ifndef __UtilFOLDER_H__
#define __UtilFOLDER_H__

#include <stdio.h>
#include <stdlib.h>
#include "config/stl_config.h"

class UtilFolder {
public:
    UtilFolder (const char* folder);

    //static int EnumFolder(const char* folder);

    static int GetFileList (const char* folder, list<string>& filelist);

    static int clear (const char* folder);

private:
    
};

#endif
