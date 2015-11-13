/**
 * @file   util_network.cpp
 * @author windsome <windsome@windsome-desktop>
 * @date   Tue Jan 12 18:12:40 2010
 * 
 * @brief  
 * 
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>

#include "util_network.h"
#include "util_log.h"

string UtilInterface::msUdhcpcPath = "/sbin/udhcpc";
string UtilInterface::msRoutePath = "/sbin/route";
string UtilInterface::msIfconfigPath = "/sbin/ifconfig";

UtilInterface::UtilInterface (string iface) {
    msIface = iface;
    msMac = "";
    msIp = "";
    msNetmask = "";
    msGateway = "";

    if((miSock = socket (AF_INET, SOCK_STREAM, 0)) >= 0) {
        struct ifreq ifreq;
        strcpy (ifreq.ifr_name, msIface.c_str());
        // mac
        if (ioctl (miSock, SIOCGIFHWADDR, &ifreq) < 0) {
            ERROR("%s", strerror (errno));
        } else {
            char macbuf[20] = {0};
            sprintf(macbuf,"%02x%02x%02x%02x%02x%02x",
                    (unsigned char)ifreq.ifr_hwaddr.sa_data[0],
                    (unsigned char)ifreq.ifr_hwaddr.sa_data[1],
                    (unsigned char)ifreq.ifr_hwaddr.sa_data[2],
                    (unsigned char)ifreq.ifr_hwaddr.sa_data[3],
                    (unsigned char)ifreq.ifr_hwaddr.sa_data[4],
                    (unsigned char)ifreq.ifr_hwaddr.sa_data[5]);
            msMac = macbuf;
        }
        // ip
        if (ioctl(miSock, SIOCGIFADDR, &ifreq) < 0) {
            ERROR ("%s", strerror (errno));
        } else {
            struct sockaddr_in* myaddr = (struct sockaddr_in *)&(ifreq.ifr_addr);
            msIp = inet_ntoa (myaddr->sin_addr);
        }
        // netmask
        if (ioctl(miSock, SIOCGIFNETMASK, &ifreq) < 0) {
            ERROR ("%s", strerror (errno));
        } else {
            struct sockaddr_in* myaddr = (struct sockaddr_in *)&(ifreq.ifr_addr);
            msNetmask = inet_ntoa (myaddr->sin_addr);
        }

    } else {
        ERROR("%s", strerror (errno));
    }

}
UtilInterface::~UtilInterface () {
    if (miSock >= 0) {
        close (miSock);
        miSock = -1;
    }
}

string UtilInterface::GetMacAddress () {
    return msMac;
}
int UtilInterface::SetIp (string ip) {
#if 0
    if (miSock < 0) {
        ERROR ("socket init error!");
        return -1;
    }
    struct ifreq temp;
    strcpy (temp.ifr_name, msIface.c_str ());
    struct sockaddr_in *addr = (struct sockaddr_in *)&(temp.ifr_addr);
    addr->sin_family = AF_INET;
    addr->sin_addr.s_addr = inet_addr (ip.c_str ());
    if (ioctl (miSock, SIOCSIFADDR, &temp) < 0) {
        ERROR ("%s", strerror (errno));
        return -1;
    }
    return 0;
#else
	char cmd[512] = {0};
	sprintf (cmd, "%s %s %s", msIfconfigPath.c_str (), msIface.c_str (), ip.c_str ());
    system (cmd);
	return 0;
#endif
}
int UtilInterface::SetIpDhcp () {
    return SetIpDhcp (msIface);
}
string UtilInterface::GetIp () {
    return msIp;
}
int UtilInterface::SetNetmask (string netmask) {
#if 0	
    if (miSock < 0) {
        ERROR ("socket init error!");
        return -1;
    }
    struct ifreq temp;
    strcpy (temp.ifr_name, msIface.c_str ());
    struct sockaddr_in *addr = (struct sockaddr_in *)&(temp.ifr_addr);
    addr->sin_family = AF_INET;
    addr->sin_addr.s_addr = inet_addr (netmask.c_str ());
    if (ioctl (miSock, SIOCGIFNETMASK, &temp) < 0) {
        ERROR ("%s", strerror (errno));
        return -1;
    }
    return 0;
#else
	char cmd[512] = {0};
	sprintf (cmd, "%s %s netmask %s", msIfconfigPath.c_str (), msIface.c_str (), netmask.c_str ());
    system (cmd);
	return 0;
#endif
}
string UtilInterface::GetNetmask () {
    return msNetmask;
}
int UtilInterface::SetGateway (string gateway) {
    char cmd[512];
    sprintf (cmd, "%s add default gw %s dev %s", msRoutePath.c_str (), gateway.c_str(), msIface.c_str ());
    system (cmd);
    return 0;
}
string UtilInterface::GetGateway () {
    return "";
}
int    UtilInterface::SetDns (string dns1, string dns2) {
    return SetDns (msIface, dns1, dns2);
}
int    UtilInterface::SetDns1 (string dns) {
    return SetDns1 (msIface, dns);
}
int    UtilInterface::SetDns2 (string dns) {
    return SetDns2 (msIface, dns);
}

/*********************************************
 * static functions
 *********************************************/
string UtilInterface::GetMacAddress (string iface) {
    UtilInterface net(iface);
    return net.GetMacAddress ();
}
int    UtilInterface::SetIp (string iface, string ip) {
    UtilInterface net(iface);
    return net.SetIp (ip);
}
int    UtilInterface::SetIpDhcp (string iface) {
    char cmd[512];
    sprintf (cmd, "%s -i %s -q", msUdhcpcPath.c_str (), iface.c_str ());
    system (cmd);
    return 0;
}
int    UtilInterface::SetIpDhcpCustom (string customCmd) {
    char cmd[512];
    sprintf (cmd, "%s ", customCmd.c_str ());
    system (cmd);
    return 0;
}
string UtilInterface::GetIp (string iface) {
    UtilInterface net(iface);
    return net.GetIp ();
}
int    UtilInterface::SetNetmask (string iface, string netmask) {
    UtilInterface net(iface);
    return net.SetIp (netmask);
}
string UtilInterface::GetNetmask (string iface) {
    UtilInterface net(iface);
    return net.GetNetmask ();
}
int    UtilInterface::SetGateway (string iface, string gateway) {
    char cmd[512];
    sprintf (cmd, "%s add default gw %s dev %s", msRoutePath.c_str (), gateway.c_str (), iface.c_str ());
    system (cmd);
    return 0;
}
string UtilInterface::GetGateway (string iface) {
    return "";
}
int    UtilInterface::SetDns (string iface, string dns1, string dns2) {
    char cmd[512];
    if (dns1 != "")
        sprintf (cmd, "nameserver %s\n", dns1.c_str ());
    if (dns2 != "")
        sprintf (cmd + strlen (cmd), "nameserver %s\n", dns2.c_str ());

    int fd = open ("/etc/resolv.conf", O_CREAT|O_RDWR, 0777);
    if (fd < 0) {
        ERROR ("%s", strerror (errno));
        return -1;
    } else {
        int count = write (fd, cmd, strlen (cmd) + 1);
        if (count < 0) {
            ERROR ("write error!");
        }
        close (fd);
        return 0;
    }
}
int    UtilInterface::SetDns1 (string iface, string dns) {
    if (dns == "") {
        ERROR ("dns == NULL");
        return -1;
    }
    char cmd[512];
    sprintf (cmd, "nameserver %s\n", dns.c_str ());

    int fd = open ("/etc/resolv.conf", O_CREAT|O_RDWR, 0777);
    if (fd < 0) {
        ERROR ("%s", strerror (errno));
        return -1;
    } else {
        int count = write (fd, cmd, strlen (cmd) + 1);
        if (count < 0) {
            ERROR ("write error!");
        }
        close (fd);
        return 0;
    }
}
int    UtilInterface::SetDns2 (string iface, string dns) {
    if (dns == "") {
        ERROR ("dns == NULL");
        return -1;
    }
    char cmd[512];
    sprintf (cmd, "nameserver %s\n", dns.c_str ());

    int fd = open ("/etc/resolv.conf", O_CREAT|O_RDWR, 0777);
    if (fd < 0) {
        ERROR ("%s", strerror (errno));
        return -1;
    } else {
        lseek (fd, 0, SEEK_END);
        int count = write (fd, cmd, strlen (cmd) + 1);
        if (count < 0) {
            ERROR ("write error!");
        }
        close (fd);
        return 0;
    }
}

void UtilInterface::SetUdhcpcPath (string path) {
    msUdhcpcPath = path;
}
void UtilInterface::SetRoutePath (string path) {
    msRoutePath = path;
}
