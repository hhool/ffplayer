LOCAL_PATH:= $(call my-dir)

#########################
# Build the libcpu_detect library

ifeq ($(TARGET_ARCH_ABI),armeabi)

include $(CLEAR_VARS)

LOCAL_MODULE:= libcpuinfo

LOCAL_C_INCLUDES := $(LOCAL_PATH)
         
LOCAL_CFLAGS :=-O3 -fvisibility=hidden

LOCAL_ARM_MODE := arm
      	
LOCAL_SRC_FILES:= cpu-features.c

include $(BUILD_STATIC_LIBRARY)

endif