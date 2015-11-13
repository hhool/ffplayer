#ifndef __CPU_H__
#define __CPU_H__

#define OPTIMIZE_UNKNOWN -1
#define OPTIMIZE_ARMV5 0
#define OPTIMIZE_ARMV6_VFP 1
#define OPTIMIZE_ARMV7_NEON 2

#ifdef __cplusplus
extern "C"{
#endif

int GetCpuFeatures();

#ifdef __cplusplus
}
#endif

#endif
