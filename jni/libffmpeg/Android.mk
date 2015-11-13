LOCAL_PATH := $(call my-dir)


#######################################################
#BUILD ffmpeg_armv5

include $(CLEAR_VARS)

LOCAL_STATIC_LIBRARIES := libavformat_armv5 libavcodec_armv5  libavutil_armv5  

LOCAL_SRC_FILES := settings.c

LOCAL_MODULE := ffmpeg_armv5

LOCAL_LDLIBS := -llog -lz

LOCAL_ARM_MODE := arm

include $(BUILD_SHARED_LIBRARY)


#######################################################
#BUILD ffmpeg_armv6_vfp

include $(CLEAR_VARS)

LOCAL_STATIC_LIBRARIES := libavformat_armv6_vfp libavcodec_armv6_vfp  libavutil_armv6_vfp  

LOCAL_SRC_FILES := settings.c

LOCAL_MODULE := ffmpeg_armv6_vfp

LOCAL_LDLIBS := -llog -lz

LOCAL_ARM_MODE := arm

include $(BUILD_SHARED_LIBRARY)

#######################################################
#BUILD ffmpeg_armv7_vfpv3

#include $(CLEAR_VARS)

#LOCAL_STATIC_LIBRARIES := libavformat_armv7_vfpv3 libavcodec_armv7_vfpv3  libavutil_armv7_vfpv3  

#LOCAL_SRC_FILES := settings.c

#LOCAL_MODULE := ffmpeg_armv7_vfpv3

#LOCAL_LDLIBS := -llog -lz

#LOCAL_ARM_MODE := arm

#include $(BUILD_SHARED_LIBRARY)

#######################################################
#BUILD ffmpeg_armv7_neon

include $(CLEAR_VARS)

LOCAL_STATIC_LIBRARIES := libavformat_armv7_neon libavcodec_armv7_neon  libavutil_armv7_neon  

LOCAL_SRC_FILES := settings.c

LOCAL_MODULE := ffmpeg_armv7_neon

LOCAL_LDLIBS := -llog -lz

LOCAL_ARM_MODE := arm

include $(BUILD_SHARED_LIBRARY)

include $(call all-makefiles-under,$(LOCAL_PATH))
