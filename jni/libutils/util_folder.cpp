/**
 * @file   util_folder.cpp
 * @author windsome <windsome@windsome-desktop>
 * @date   Tue Jan 12 18:14:01 2010
 * 
 * @brief  
 * 
 * 
 */
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>

#include "util_folder.h"
#include "util_log.h"

int UtilFolder::GetFileList (const char* folder, list<string>& filelist)
{
    filelist.clear ();

    if (folder == NULL)
        return -1;

    DIR              *pDir = NULL;
    struct dirent    *ent  = NULL;  

    pDir=opendir(folder);
    if (pDir)
    {
        while((ent = readdir(pDir)) != NULL)
        {
            if(strcmp(ent->d_name,".")==0 || strcmp(ent->d_name,"..")==0)
                continue;
            string pathname = folder;
            pathname += "/";
            pathname += ent->d_name;
            struct stat buf;
            if (stat (pathname.c_str(), &buf) < 0)
            {
                ERROR ("%s!", strerror (errno));
                continue;
            }

            switch (buf.st_mode & S_IFMT) {
            case S_IFBLK:
                break;
            case S_IFCHR:
                break;
            case S_IFDIR:
                //printf("directory\n");
                INFO ("directory %s, not return to filelist", ent->d_name);
                break;
            case S_IFIFO:
                break;
            case S_IFLNK:
                break;
            case S_IFREG:
                //printf("regular file\n");
                filelist.push_back(ent->d_name);
                break;
            case S_IFSOCK:
                break;
            default:       
                break;
            }
        }
        closedir (pDir);
        return 0;
    }
    else
    {
        ERROR ("opendir error(%s)!", folder);
        return -1;
    }
}

int UtilFolder::clear(const char* folder) {
    char cmd[512];
    sprintf (cmd, "/bin/rm %s/* -rf", folder);
    system (cmd);
    
    return 0;
}
