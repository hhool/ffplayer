#ifndef __UTIL_ARGUMENT_H__
#define __UTIL_ARGUMENT_H__

#include <stdio.h>
#include <stdlib.h>
#include "config/stl_config.h"
class UtilArgument {
public:
    UtilArgument (string argument);

    string Argv (int index);
    int Argc ();

private:
    int Parse ();

private:
    string msArgument;
    deque<string> mArgs;
};

#endif
