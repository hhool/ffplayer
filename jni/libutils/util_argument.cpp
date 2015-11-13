/**
 * @file   util_uri.cpp
 * @author windsome <windsome@windsome-desktop>
 * @date   Tue Jan 12 18:11:52 2010
 * 
 * @brief  
 * 
 * 
 */
#include "util_argument.h"
#include "util_log.h"

UtilArgument::UtilArgument (string argument) {
    msArgument = argument;
    Parse ();
}

string UtilArgument::Argv (int index) {
    if (index < 0 || index > mArgs.size ()) {
        DEBUG ("index %d not exist! size is %d!", index, mArgs.size ());
        return "";
    }
    return mArgs[index];
}
int UtilArgument::Argc () {
    return mArgs.size ();
}

int UtilArgument::Parse () {
    mArgs.clear ();

    if (msArgument == "") {
        DEBUG ("argument == NULL!");
        return -1;
    }

    string strset = " \r\n";
    string temp;
    
    int last = 0;
    int next = 0;
    do {
        next = msArgument.find_first_of (strset, last);
        if (next == string::npos) {
            temp = msArgument.substr (last, msArgument.size() - last);
            if (temp != "")
                mArgs.push_back (temp);
            break;
        } else {
            temp = msArgument.substr (last, next - last);
            if (temp != "")
                mArgs.push_back (temp);
            last = next+1;
            if (last > msArgument.size() - 1) {
                break;
            }
        }
    }while (1);

    DEBUG ("get Argument (%d)", mArgs.size ());
    string argvs = "--";
    for (int i = 0; i < mArgs.size (); i++) {
        argvs += mArgs[i];
        argvs += "--";
    }
    DEBUG ("argv:%s", argvs.c_str ());
    return 0;
}
