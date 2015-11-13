#ifndef __UTIL_CMDLINE_H__
#define __UTIL_CMDLINE_H__

#include "config/stl_config.h"

class UtilCmdLine {
public:
    UtilCmdLine (string prefix = "cmd> ");

    string WaitCmdLine ();

private:
    string msPrefix;

};

#endif
