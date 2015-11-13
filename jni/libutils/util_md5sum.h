/**
 * @file   util_md5sum.h
 * @author windsome <windsome@windsome-desktop>
 * @date   Tue Jan 12 18:12:44 2010
 * 
 * @brief  
 * 
 * 
 */
#ifndef _UTIL_MD5SUM_H
#define _UTIL_MD5SUM_H

#include "config/stl_config.h"

#define MD5_HASH_LEN 16
#ifndef U32
#define U32 unsigned int
#define U8 unsigned char
#endif

class UtilMd5
{
public:
    UtilMd5 ();
    void Begin ();
    void Hash (const void *buffer, size_t length);
    void End ();
    string GetMd5String ();
    int GetMd5BinBuffer (unsigned char* buf);
    
    static string HashFd (int srcFd, const size_t size);
    static string HashFile(const char *filename);
    static string HashBuffer(unsigned char *buf, const size_t len);

private:
    void md5_hash_bytes (const void *buffer, size_t len);
    void md5_hash_block (const void *buffer, size_t len);
    static string Bin2Hex (unsigned char *hash_value, unsigned char hash_length);

private:
    U32 muA;
    U32 muB;
    U32 muC;
    U32 muD;
    U32 muTotal[2];
    U32 miBuflen;
    char mBuffer[128];

    unsigned char mMd5Buf[MD5_HASH_LEN];
    string mMd5string;
};
    
#endif
