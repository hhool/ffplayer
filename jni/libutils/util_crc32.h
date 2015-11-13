/**
 * @file   util_crc32.h
 * @author windsome <windsome@windsome-desktop>
 * @date   Tue Jan 12 18:14:38 2010
 * 
 * @brief  
 * 
 * 
 */
#ifndef __UTIL_CRC32_H__
#define __UTIL_CRC32_H__

typedef unsigned int UINT32;

class UtilCrc32
{
public:
    UtilCrc32 ();
    UINT32 UpdateCrc (char *buf, int len);
    UINT32 GetResult ();
private:
    UINT32 muValue;

public:
    static UINT32 crc32(unsigned char* bytes, int len);
private:
    static UINT32 crcTable[256];
};
#endif
