/**
 * @file   util_fsinfo.h
 * @author windsome <windsome@windsome-desktop>
 * @date   Mon Jan 18 12:00:00 2010
 * 
 * @brief  
 * 
 * 
 */
#ifndef __UTILFSINFO_H__
#define __UTILFSINFO_H__

#include <stdio.h>
#include <stdlib.h>
#include "config/stl_config.h"

class UtilFsInfo {
public:
    UtilFsInfo (const char* mountPoint);
    int Init ();

    string GetFormat ();
    float GetTotalCapacity ();
    float GetFreeCapacity ();
    string GetPermissions ();

private:
    unsigned long mscale(unsigned long b, unsigned long bs);
    
private:
    string msMountPoint;
    string msFsFormat;
    float mfTotalCapacity;
    float mfFreeCapacity;
    string msFsPermissions;
};

#endif
