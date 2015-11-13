/**
 * @file   test_disk.cpp
 * @author windsome <windsome@windsome-desktop>
 * @date   Mon Jan 18 11:39:45 2010
 * 
 * @brief  
 * 
 * 
 */
#include "util_fsinfo.h"
#include "util_log.h"

int main (int argc, char* argv[]) {
    UtilFsInfo* fsinfo = new UtilFsInfo("/");
    if (fsinfo->Init() == 0) {
        DEBUG ("fs=%s, total=%f, free=%f", fsinfo->GetFormat ().c_str(), fsinfo->GetTotalCapacity(), fsinfo->GetFreeCapacity ());
        return 0;
    } else {
        ERROR ("init UtilFsInfo fail!");
        return -1;
    }
}
