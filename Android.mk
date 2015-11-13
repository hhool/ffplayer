LOCAL_PATH:= $(call my-dir)

# Build activity

include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := ffplay

LOCAL_SRC_FILES := $(call all-java-files-under, src)

LOCAL_PACKAGE_NAME := AgouMediaPlayer

LOCAL_JNI_SHARED_LIBRARIES := libwindplayer2_jni

include $(BUILD_PACKAGE)

