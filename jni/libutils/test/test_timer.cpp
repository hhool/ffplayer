/**
 * @file   test_crc32.cpp
 * @author windsome <windsome@windsome-desktop>
 * @date   Mon Jan 18 11:39:45 2010
 * 
 * @brief  
 * 
 * 
 */
#include "util_timer.h"
#include "util_time.h"
#include "util_log.h"
#include "config/stl_config.h"

int TimerFunction (void* cookie) {
    string tt = UtilTime::CurrentTimeString ();
    printf ("timer reached! %s\n", tt.c_str ());
    return 0;
}

int main (int argc, char* argv[]) {
    UtilTimerManager* manager = UtilTimerManager::getInstance ();

    UtilTimer* timer = manager->NewTimer (ONE_SECOND*2, TimerFunction, NULL, true);
    while (1) {
        sleep (10);
    }
    UtilTimerManager::destroyInstance ();
    return 0;
}
