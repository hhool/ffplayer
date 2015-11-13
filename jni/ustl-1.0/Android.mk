LOCAL_PATH:= $(call my-dir)

ifeq ($(TARGET_ARCH_ABI),armeabi)
############shared lib#######
####### libustl

include $(CLEAR_VARS)

LOCAL_MODULE := libustl

LOCAL_CFLAGS += -fstrict-aliasing -fomit-frame-pointer -W -O3 -DPLATFORM_ANDROID -DANDROID -DHAVE_ANDROID_OS -fvisibility=hidden

LOCAL_C_INCLUDES := $(LOCAL_PATH) \


LOCAL_ARM_MODE := arm

LOCAL_SRC_FILES := \
							    bktrace.cpp \
							    memblock.cpp \
							    ofstream.cpp \
							    ualgobase.cpp \
							    unew.cpp \
							    cmemlink.cpp \
							    memlink.cpp \
							    sistream.cpp \
							    ubitset.cpp \
							    ustdxept.cpp \
							    fstream.cpp \
							    mistream.cpp \
							    sostream.cpp \
							    uexception.cpp \
							    ustring.cpp \
							    
include $(BUILD_STATIC_LIBRARY)
endif