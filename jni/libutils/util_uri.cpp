/**
 * @file   util_uri.cpp
 * @author windsome <windsome@windsome-desktop>
 * @date   Tue Jan 12 18:11:52 2010
 * 
 * @brief  
 * 
 * 
 */
#include "util_uri.h"
#include "util_log.h"

UtilURI::UtilURI (const char* uri) {
    if (uri)
        msURI = uri;
    else
        msURI = "";
    
    Parse ();
}

/** 
 * [scheme]://[authority@][server][:port][/path]
 * 
 * 
 * @return 
 */
int UtilURI::Parse () {
    msScheme = "";
    msAuthority = "";
    msUsername = "";
    msPassword = "";
    msHost="";
    miPort = 0;
    msPath = "";
    msQuery = "";
    msFragment = "";
    msFilename = "";

    string uri = msURI;
    string rest = "";

    int pos = uri.find_first_of ("://");
    if (pos != string::npos) {
        msScheme = uri.substr (0, pos);
        int pos_path = uri.find_first_of ("/", pos + 3);
        if (pos_path != string::npos) {
            //get path.
            msPath = uri.substr (pos_path, uri.size() - pos_path);
            rest = uri.substr (pos + 3, pos_path - pos - 3);
            int pos_filename = msPath.find_last_of ("/");
            msFilename = msPath.substr (pos_filename + 1, msPath.size () - pos_filename - 1);
        } else {
            //don't get path.
            msPath = "";
            rest = uri.substr (pos + 3, uri.size() - pos - 3);
        }

        // rest must be user:pass@server:host
        //INFO ("rest=%s", rest.c_str());

        int pos_host = rest.find_first_of ("@");
        if (pos_host != string::npos) {
            //get authority.
            msAuthority = rest.substr (0, pos_host);
            int pos_password = msAuthority.find_first_of (":");
            if (pos_password != string::npos) {
                //get password.
                msUsername = msAuthority.substr (0, pos_password);
                msPassword = msAuthority.substr (pos_password + 1, msAuthority.size() - pos_password - 1);
            } else {
                msUsername = msAuthority;
                //no password.
            }
            rest = rest.substr (pos_host + 1, rest.size () - pos_host - 1);
        } else {
            //no authority.
        }
        
        // 
        //INFO ("rest must be server[:host]. rest=%s", rest.c_str());

        int pos_port = rest.find_first_of (":");
        if (pos_port != string::npos) {
            //get port.
            msHost = rest.substr (0, pos_port);
            string str_port = rest.substr (pos_port + 1, rest.size () - pos_port - 1);
            miPort = atoi (str_port.c_str());
        } else {
            msHost = rest;
            //no port.
            if (msScheme == "ftp")
                miPort = 21;
        }
        
        DEBUG ("parse ok! uri=%s\n\tschema=%s, auth=%s, username=%s, password=%s, host=%s, port=%d, path=%s", 
               msURI.c_str(), msScheme.c_str(), msAuthority.c_str(), msUsername.c_str(), 
               msPassword.c_str(), msHost.c_str(), miPort, msPath.c_str());
        
        return 0;
    } else {
        ERROR ("invalid uri! uri=%s", msURI.c_str());
        return -1;
    }

    return -1;
}

string UtilURI::GetScheme () {
    return msScheme;
}

string UtilURI::GetAuthority () {
    return msAuthority;
}

string UtilURI::GetUsername () {
    return msUsername;
}

string UtilURI::GetPassword () {
    return msPassword;
}

string UtilURI::GetHost () {
    return msHost;
}

int UtilURI::GetPort () {
    return miPort;
}

string UtilURI::GetPath () {
    return msPath;
}

string UtilURI::GetQuery () {
    return msQuery;
}

string UtilURI::GetFragment () {
    return msFragment;
}

string UtilURI::GetFilename () {
    return msFilename;
}
