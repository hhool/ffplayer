/**
 * @file   util_fsinfo.cpp
 * @author windsome <windsome@windsome-desktop>
 * @date   Mon Jan 18 12:01:20 2010
 * 
 * @brief  
 * 
 * 
 */
#include <stdio.h>
#include <errno.h>
#include <sys/vfs.h>

#include "util_fsinfo.h"
#include "util_log.h"

UtilFsInfo::UtilFsInfo (const char* mountPoint) {
    msMountPoint = mountPoint;
    msFsFormat = "";
    mfTotalCapacity = 0.0f;
    mfFreeCapacity = 0.0f;
    msFsPermissions = "";
}
int UtilFsInfo::Init () {
    struct statfs buf;

    if (statfs(msMountPoint.c_str (), &buf) < 0) {
        ERROR ("get disk infomation faild");
        return -1;
    }

    switch(buf.f_type)
    {
        case 0x4d44:
            msFsFormat = "FAT";
            break;

        case 0x5346544e:
        case 0X65735546:
            msFsFormat = "NTFS";
            break;

        case 0xEF53:
        case 0xEF51:
            msFsFormat = "EXT2";
            break;

        default:
            msFsFormat = "unknown";
            break;
    }

    mfTotalCapacity = (float)(mscale(buf.f_blocks, buf.f_bsize));
    mfFreeCapacity = (float)(mscale(buf.f_bfree, buf.f_bsize));
    msFsPermissions = "rw";
    return 0;
}

unsigned long UtilFsInfo::mscale(unsigned long b, unsigned long bs)
{
    return (b * (unsigned long long) bs + 1024/2) / 1024 / 1024;
}

string UtilFsInfo::GetFormat () {
    return msFsFormat;
}
float UtilFsInfo::GetTotalCapacity () {
    return mfTotalCapacity;
}
float UtilFsInfo::GetFreeCapacity () {
    return mfFreeCapacity;
}
string UtilFsInfo::GetPermissions () {
    return msFsPermissions;
}
