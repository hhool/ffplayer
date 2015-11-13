#include "util_cmdline.h"
#include "util_log.h"
#include <stdio.h>
#include <string.h>

#ifdef MAX_CMD_LENGTH
#undef MAX_CMD_LENGTH
#endif
#define MAX_CMD_LENGTH 1000

UtilCmdLine::UtilCmdLine (string prefix) {
    msPrefix = prefix;
}

string UtilCmdLine::WaitCmdLine () {
    static char cmdline[MAX_CMD_LENGTH] = {0};

    printf("%s", msPrefix.c_str ());
    fflush(stdout);

    if (NULL == fgets(cmdline, MAX_CMD_LENGTH, stdin) ) {
        ERROR ("read from cmdline fail!");
        return "";
    } else {
        int len = strlen (cmdline);
        if (len>0 && cmdline[len-1]=='\n')
            cmdline[len-1] = 0;
        return cmdline;
    }
}

