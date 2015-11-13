#ifdef __ARMV5__
#include "config_armv5.h"
#elif defined(__ARMV6_VFP__)
#include "config_armv6_vfp.h"
#elif defined(__ARMV7_VFPV3__)
#include "config_armv7_vfpv3.h"
#elif defined(__ARMV7_NEON__)
#include "config_armv7_neon.h"
#else
#error "unsupport"
#endif