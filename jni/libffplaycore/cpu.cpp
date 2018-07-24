#include <cpu-features.h>
#include "cpu.h"

int GetCpuFeatures() {
  AndroidCpuFamily  CpuFamily = ANDROID_CPU_FAMILY_UNKNOWN;
  uint64_t features;
  CpuFamily = android_getCpuFamily();
  features = android_getCpuFeatures();

  if (CpuFamily == ANDROID_CPU_FAMILY_ARM && (features & ANDROID_CPU_ARM_FEATURE_ARMv7)) {
    if ((features & ANDROID_CPU_ARM_FEATURE_NEON)) {
      return OPTIMIZE_ARMV7_NEON;
    } else if ((features & ANDROID_CPU_ARM_FEATURE_VFPv3)) {
      return OPTIMIZE_ARMV6_VFP;
    } else
      return OPTIMIZE_ARMV5;
  } else if (CpuFamily == ANDROID_CPU_FAMILY_ARM && (features & ANDROID_CPU_ARM_FEATURE_ARMv6)) {
    if ((features & ANDROID_CPU_ARM_FEATURE_VFP)) {
      return OPTIMIZE_ARMV6_VFP;
    } else
      return OPTIMIZE_ARMV5;
  } else if (CpuFamily == ANDROID_CPU_FAMILY_ARM && (features & ANDROID_CPU_ARM_FEATURE_ARMv5TE)) {
    return OPTIMIZE_ARMV5;
  }

  return OPTIMIZE_UNKNOWN;
}