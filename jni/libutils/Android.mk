LOCAL_PATH:= $(call my-dir)

ifeq ($(TARGET_ARCH_ABI),armeabi)

include $(CLEAR_VARS)

LOCAL_MODULE := libwindutils

LOCAL_C_INCLUDES :=	$(LOCAL_PATH) \
										$(LOCAL_PATH)/../ustl-1.0 \
    								
LOCAL_CFLAGS += -W -O3 -DPLATFORM_ANDROID -fvisibility=hidden
    							 
LOCAL_ARM_MODE := arm

LOCAL_SRC_FILES :=  \
									util_log.cpp \
									util_lock.cpp \
									util_thread.cpp \
									util_time.cpp \
									util_timer.cpp \
									util_uri.cpp \
									util_network.cpp \
									util_file.cpp \
									util_fsinfo.cpp \
									util_folder.cpp \
									util_eventloop.cpp \
									util_md5sum.cpp \
									util_crc32.cpp \
									util_format.cpp \
									util_ringbuffer.cpp \
									util_argument.cpp \
									util_cmdline.cpp
									
include $(BUILD_STATIC_LIBRARY)

endif