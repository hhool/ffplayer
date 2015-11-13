/**
 * @file   util_eventloop.cpp
 * @author windsome <windsome@windsome-desktop>
 * @date   Tue Jan 12 18:14:32 2010
 * 
 * @brief  
 * 
 * 
 */
#include "util_eventloop.h"

UtilEventLoop::UtilEventLoop(string name) {
    mName = name;
    mbExit = false;
}

void UtilEventLoop::ThreadEntry () {
    EventLoopProcess ();
}

void UtilEventLoop::NotifyExit(bool bExit) {
    mbExit = bExit;
}

bool UtilEventLoop::NeedExit() {
    return mbExit;
}

string UtilEventLoop::GetName() {
    return mName;
}

