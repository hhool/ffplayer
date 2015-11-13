# Build the unit tests.
ifneq ($(TARGET_SIMULATOR),true)

LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := tests

LOCAL_C_INCLUDES:= \
    bionic \
    bionic/libstdc++/include \
    external/gtest/include \
    system/media/opensles/include \
    external/stlport/stlport

LOCAL_SRC_FILES:= \
    BufferQueue_test.cpp

LOCAL_SHARED_LIBRARIES := \
	libutils \
	libOpenSLES \
    libstlport

LOCAL_STATIC_LIBRARIES := \
    libOpenSLESUT \
    libgtest

ifeq ($(TARGET_OS),linux)
	LOCAL_CFLAGS += -DXP_UNIX
	#LOCAL_SHARED_LIBRARIES += librt
endif

LOCAL_MODULE:= BufferQueue_test

LOCAL_MODULE_PATH := $(TARGET_OUT_DATA)/nativetest

include $(BUILD_EXECUTABLE)

endif
# Build the manual test programs.
include $(call all-subdir-makefiles)
