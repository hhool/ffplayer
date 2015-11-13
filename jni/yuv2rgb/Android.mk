LOCAL_PATH := $(call my-dir)

ifeq ($(TARGET_ARCH_ABI),armeabi)

#the yuv2rgb library

include $(CLEAR_VARS)

LOCAL_ALLOW_UNDEFINED_SYMBOLS=false

LOCAL_MODULE := yuv2rgb

LOCAL_C_INCLUDES := $(LOCAL_PATH) \
					$(LOCAL_PATH)/include 

LOCAL_CFLAGS := -D__STDC_CONSTANT_MACROS  -fvisibility=hidden
LOCAL_CFLAGS += $(CC_OPTIMIZE_FLAG) 

LOCAL_ARM_MODE := arm

LOCAL_SRC_FILES := src/yuv2rgb16tab.c src/yuv420rgb8888.S src/yuv420rgb565.S 

ifeq ($(TARGET_ARCH_ABI),x86)
   LOCAL_SRC_FILES := src/yuv2rgb16tab.c src/yuv420rgb8888c.c src/yuv420rgb565c.c
endif

ifeq ($(TARGET_ARCH_ABI),mips)
   LOCAL_SRC_FILES := src/yuv2rgb16tab.c src/yuv420rgb8888c.c src/yuv420rgb565c.c
endif

LOCAL_SHARED_LIBRARIES := 
LOCAL_STATIC_LIBRARIES := 
LOCAL_LDLIBS := -ldl -llog 

include $(BUILD_STATIC_LIBRARY)

endif