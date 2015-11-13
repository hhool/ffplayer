/**
 * @file   util_network.h
 * @author windsome <windsome@windsome-desktop>
 * @date   Tue Jan 12 18:12:36 2010
 * 
 * @brief  
 * 
 * 
 */
#ifndef __UtilNETWORK_H__
#define __UtilNETWORK_H__

#include "config/stl_config.h"

class UtilInterface {
public:
    UtilInterface (string iface);
    ~UtilInterface ();

    string GetMacAddress ();
    int    SetIp (string ip);
    int    SetIpDhcp ();
    string GetIp ();
    int    SetNetmask (string netmask);
    string GetNetmask ();
    int    SetGateway (string gateway);
    string GetGateway ();
    int    SetDns (string dns1, string dns2);
    int    SetDns1 (string dns);
    int    SetDns2 (string dns);

public:
    static string GetMacAddress (string iface);
    static int    SetIp (string iface, string ip);
    static int    SetIpDhcp (string iface);
    static int    SetIpDhcpCustom (string customCmd);
    static string GetIp (string iface);
    static int    SetNetmask (string iface, string netmask);
    static string GetNetmask (string iface);
    static int    SetGateway (string iface, string gateway);
    static string GetGateway (string iface);
    static int    SetDns (string iface, string dns1, string dns2);
    static int    SetDns1 (string iface, string dns);
    static int    SetDns2 (string iface, string dns);

    static void   SetUdhcpcPath (string path);
    static void   SetRoutePath (string path);

private:
    static string msUdhcpcPath;
    static string msRoutePath;
    static string msIfconfigPath;
    string msIface;
    int    miSock;

    string msMac;
    string msIp;
    string msNetmask;
    string msGateway;
};

#endif
