LOCAL_PATH:= $(call my-dir)

# slesTest_recBuffQueue

include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := tests

LOCAL_C_INCLUDES:= \
	system/media/opensles/include/SLES/

LOCAL_SRC_FILES:= \
	slesTestRecBuffQueue.cpp

LOCAL_SHARED_LIBRARIES := \
	libutils \
	libOpenSLES

ifeq ($(TARGET_OS),linux)
	LOCAL_CFLAGS += -DXP_UNIX
endif

LOCAL_MODULE:= slesTest_recBuffQueue

include $(BUILD_EXECUTABLE)

# slesTest_playFdPath

include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := tests

LOCAL_C_INCLUDES:= \
	system/media/opensles/include

LOCAL_SRC_FILES:= \
	slesTestPlayFdPath.cpp

LOCAL_SHARED_LIBRARIES := \
	libutils \
	libOpenSLES

ifeq ($(TARGET_OS),linux)
	LOCAL_CFLAGS += -DXP_UNIX
endif

LOCAL_MODULE:= slesTest_playFdPath

include $(BUILD_EXECUTABLE)

# slesTest_feedback

include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := tests

LOCAL_C_INCLUDES:= \
	system/media/opensles/include

LOCAL_SRC_FILES:= \
    slesTestFeedback.cpp

LOCAL_SHARED_LIBRARIES := \
	libutils \
	libOpenSLES

ifeq ($(TARGET_OS),linux)
	LOCAL_CFLAGS += -DXP_UNIX
	#LOCAL_SHARED_LIBRARIES += librt
endif

LOCAL_CFLAGS += -UNDEBUG

LOCAL_MODULE:= slesTest_feedback

include $(BUILD_EXECUTABLE)

# slesTest_sawtoothBufferQueue

include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := tests

LOCAL_C_INCLUDES:= \
	system/media/opensles/include

LOCAL_SRC_FILES:= \
	slesTestSawtoothBufferQueue.cpp

LOCAL_SHARED_LIBRARIES := \
	libutils \
	libOpenSLES

ifeq ($(TARGET_OS),linux)
	LOCAL_CFLAGS += -DXP_UNIX
	#LOCAL_SHARED_LIBRARIES += librt
endif

LOCAL_MODULE:= slesTest_sawtoothBufferQueue

include $(BUILD_EXECUTABLE)

# slesTest_eqFdPath

include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := tests

LOCAL_C_INCLUDES:= \
	system/media/opensles/include

LOCAL_SRC_FILES:= \
	slesTestEqFdPath.cpp

LOCAL_SHARED_LIBRARIES := \
	libutils \
	libOpenSLES

ifeq ($(TARGET_OS),linux)
	LOCAL_CFLAGS += -DXP_UNIX
endif

LOCAL_MODULE:= slesTest_eqFdPath

include $(BUILD_EXECUTABLE)

# slesTest_eqOutputPath

include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := tests

LOCAL_C_INCLUDES:= \
	system/media/opensles/include

LOCAL_SRC_FILES:= \
	slesTestEqOutputPath.cpp

LOCAL_SHARED_LIBRARIES := \
	libutils \
	libOpenSLES

ifeq ($(TARGET_OS),linux)
	LOCAL_CFLAGS += -DXP_UNIX
endif

LOCAL_MODULE:= slesTest_eqOutputPath

include $(BUILD_EXECUTABLE)

# slesTest_bassboostPath

include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := tests

LOCAL_C_INCLUDES:= \
	system/media/opensles/include

LOCAL_SRC_FILES:= \
	slesTestBassBoostPath.cpp

LOCAL_SHARED_LIBRARIES := \
	libutils \
	libOpenSLES

ifeq ($(TARGET_OS),linux)
	LOCAL_CFLAGS += -DXP_UNIX
endif

LOCAL_MODULE:= slesTest_bassboostPath

include $(BUILD_EXECUTABLE)

# slesTest_virtualizer

include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := tests

LOCAL_C_INCLUDES:= \
	system/media/opensles/include

LOCAL_SRC_FILES:= \
	slesTestVirtualizerPath.cpp

LOCAL_SHARED_LIBRARIES := \
	libutils \
	libOpenSLES

ifeq ($(TARGET_OS),linux)
	LOCAL_CFLAGS += -DXP_UNIX
endif

LOCAL_MODULE:= slesTest_virtualizer

include $(BUILD_EXECUTABLE)

# slesTest_effectCapabilities

include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := tests

LOCAL_C_INCLUDES:= \
	system/media/opensles/include

LOCAL_SRC_FILES:= \
	slesTestEffectCapabilities.cpp

LOCAL_SHARED_LIBRARIES := \
	libutils \
	libOpenSLES

ifeq ($(TARGET_OS),linux)
	LOCAL_CFLAGS += -DXP_UNIX
endif

LOCAL_MODULE:= slesTest_effectCapabilities

include $(BUILD_EXECUTABLE)

# slesTest_sendToPresetReverb

include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := tests

LOCAL_C_INCLUDES:= \
	system/media/opensles/include

LOCAL_SRC_FILES:= \
	slesTestSendToPresetReverb.cpp

LOCAL_SHARED_LIBRARIES := \
	libutils \
	libOpenSLES

ifeq ($(TARGET_OS),linux)
	LOCAL_CFLAGS += -DXP_UNIX
endif

LOCAL_MODULE:= slesTest_sendToPresetReverb

include $(BUILD_EXECUTABLE)
