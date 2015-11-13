/**
 * @file   util_uri.h
 * @author windsome <windsome@windsome-desktop>
 * @date   Tue Jan 12 18:11:29 2010
 * 
 * @brief  
 * 
 * 
 */
#ifndef __UtilURI_H__
#define __UtilURI_H__

#include <stdio.h>
#include <stdlib.h>
#include "config/stl_config.h"

class UtilURI {
public:
    UtilURI (const char* uri);

    string GetScheme ();
    string GetAuthority ();
    string GetUsername ();
    string GetPassword ();
    string GetHost ();
    int    GetPort ();
    string GetQuery ();
    string GetPath ();
    string GetFragment ();
    string GetFilename ();

private:
    int Parse ();
    string msURI;

    string msScheme;
    string msAuthority;
    string msUsername;
    string msPassword;
    string msHost;
    int    miPort;
    string msPath;
    string msQuery;
    string msFragment;
    string msFilename;
};

#endif
