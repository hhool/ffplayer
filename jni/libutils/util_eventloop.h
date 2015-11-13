/**
 * @file   util_eventloop.h
 * @author windsome <windsome@windsome-desktop>
 * @date   Tue Jan 12 18:14:20 2010
 * 
 * @brief  
 * 
 * 
 */
#ifndef __UtilEVENTLOOP_H__
#define __UtilEVENTLOOP_H__

#include "util_thread.h"
#include "config/stl_config.h"

class UtilEventLoop : public UtilThread {
public:
    UtilEventLoop(string name);

    void ThreadEntry ();
    void NotifyExit (bool bExit = true);
    bool NeedExit ();
    string GetName ();

    virtual void EventLoopProcess () = 0;

private:
    string mName;//for identify.
    bool mbExit;
};

#endif
