LOCAL_PATH:= $(call my-dir)

ANDROID16_SYS_INC := $(LOCAL_PATH)/../include/aosp-4
ANDROID22_SYS_INC := $(LOCAL_PATH)/../include/aosp-8
ANDROID23_SYS_INC := $(LOCAL_PATH)/../include/aosp-9
ANDROID40_SYS_INC := $(LOCAL_PATH)/../include/aosp-14
ANDROID41_SYS_INC := $(LOCAL_PATH)/../include/aosp-16
ANDROID43_SYS_INC := $(LOCAL_PATH)/../include/aosp-18

cmd-strip = $(TOOLCHAIN_PREFIX)strip --strip-debug --strip-unneeded $(call cygwin-to-host-path,$1)

ifeq ($(TARGET_ARCH_ABI),armeabi)

#######################################################################
# Build the libffplay-4-jni library

include $(CLEAR_VARS)

LOCAL_MODULE:= libffplay-4-jni

LOCAL_C_INCLUDES := $(LOCAL_PATH) \
					$(LOCAL_PATH)/../ustl-1.0 \
					$(LOCAL_PATH)/../libutils \
					$(LOCAL_PATH)/../libffmpeg \
					$(LOCAL_PATH)/../libffplaycore \
					$(LOCAL_PATH)/../yuv2rgb/include \
					$(LOCAL_PATH)/../libyuv/include \
					$(ANDROID16_SYS_INC)/system/core/include \
					$(ANDROID16_SYS_INC)/frameworks/base/include \
					$(ANDROID16_SYS_INC)/dalvik/libnativehelper/include \
					$(ANDROID16_SYS_INC)/hardware/libhardware/include \
					$(ANDROID16_SYS_INC)/bionic/libc/include \
					$(ANDROID16_SYS_INC)/external/skia/include/ \
					$(ANDROID16_SYS_INC)/external/skia/include/core \
					
LOCAL_CFLAGS := -DPLATFORM_ANDROID=1 -DPLATFORM=4 -D__STDC_CONSTANT_MACROS=1
LOCAL_CFLAGS += -DPLAYER_CLASS_DIR=\"com/pbi/player/LivePlayer\"
LOCAL_CFLAGS += -fPIC -O3 -fvisibility=hidden

LOCAL_CXXFLAGS := -DHAVE_PTHREADS

LOCAL_ARM_MODE := arm

LOCAL_PRELINK_MODULE := false

LOCAL_STATIC_LIBRARIES := libyuv_static libyuv2rgb libwindutils libustl
													
LOCAL_SHARED_LIBRARIES := libffpcore

LOCAL_LDLIBS := -L$(call host-path,$(ANDROID16_SYS_INC)) \
				-lcorecg \
				-lsgl \
               	-lutils \
               	-lcutils \
               	-lui \
               	-landroid_runtime \
               	-lnativehelper \
               	-lsurfaceflinger \
               	-lmedia                 \
               	-ldl                    \
               	-lz	\
               	-llog
               	
LOCAL_SRC_FILES:= \
				liveplayer.cpp \
				android_audiotrack_render.cpp \
				android_videosurface_render.cpp \
				android_stub_render.cpp \
				android_liveplayer.cpp \
				android_liveplayerlistener.cpp

include $(BUILD_SHARED_LIBRARY)

#######################################################################
# Build the libffplay-8-jni library

include $(CLEAR_VARS)

LOCAL_MODULE:= libffplay-8-jni

LOCAL_C_INCLUDES := $(LOCAL_PATH) \
					$(LOCAL_PATH)/../ustl-1.0 \
					$(LOCAL_PATH)/../libutils \
					$(LOCAL_PATH)/../libffmpeg \
					$(LOCAL_PATH)/../libffplaycore \
					$(LOCAL_PATH)/../yuv2rgb/include \
					$(LOCAL_PATH)/../libyuv/include \
					$(ANDROID22_SYS_INC)/system/core/include \
					$(ANDROID22_SYS_INC)/frameworks/base/include \
					$(ANDROID22_SYS_INC)/frameworks/base/native/include \
					$(ANDROID22_SYS_INC)/dalvik/libnativehelper/include \
					$(ANDROID22_SYS_INC)/hardware/libhardware/include \
					$(ANDROID22_SYS_INC)/bionic/libc/include \
					$(ANDROID22_SYS_INC)/external/skia/include/core
					
LOCAL_CFLAGS := -DPLATFORM_ANDROID=1 -DPLATFORM=8 -D__STDC_CONSTANT_MACROS=1
LOCAL_CFLAGS += -DPLAYER_CLASS_DIR=\"com/pbi/player/LivePlayer\"
LOCAL_CFLAGS += -fPIC -O3 -fvisibility=hidden

LOCAL_CXXFLAGS := -DHAVE_PTHREADS

LOCAL_ARM_MODE := arm

LOCAL_PRELINK_MODULE := false

LOCAL_STATIC_LIBRARIES := libyuv_static libyuv2rgb libwindutils libustl
													
LOCAL_SHARED_LIBRARIES := libffpcore

LOCAL_LDLIBS := -L$(call host-path,$(ANDROID22_SYS_INC)) \
				-lskia \
               	-lutils \
               	-lcutils \
               	-lui \
               	-landroid_runtime \
               	-lnativehelper \
               	-lsurfaceflinger_client \
               	-lmedia                 \
               	-ldl                    \
               	-lz	\
               	-llog
               	
LOCAL_SRC_FILES:= \
				liveplayer.cpp \
				android_audiotrack_render.cpp \
				android_videosurface_render.cpp \
				android_stub_render.cpp \
				android_liveplayer.cpp \
				android_liveplayerlistener.cpp

include $(BUILD_SHARED_LIBRARY)


#######################################################################
# Build the libffplay-9-jni library

include $(CLEAR_VARS)

LOCAL_MODULE:= libffplay-9-jni

LOCAL_C_INCLUDES := $(LOCAL_PATH) \
					$(LOCAL_PATH)/../ustl-1.0 \
					$(LOCAL_PATH)/../libutils \
					$(LOCAL_PATH)/../libffmpeg \
					$(LOCAL_PATH)/../libffplaycore \
					$(LOCAL_PATH)/../yuv2rgb/include \
					$(LOCAL_PATH)/../libyuv/include \
					$(ANDROID23_SYS_INC)/system/core/include \
					$(ANDROID23_SYS_INC)/system/media/opensles/include \
					$(ANDROID23_SYS_INC)/frameworks/base/include \
					$(ANDROID23_SYS_INC)/frameworks/base/native/include \
					$(ANDROID23_SYS_INC)/dalvik/libnativehelper/include \
					$(ANDROID23_SYS_INC)/hardware/libhardware/include \
					$(ANDROID23_SYS_INC)/bionic/libc/include \
					$(ANDROID23_SYS_INC)/external/skia/include/core
					
LOCAL_CFLAGS := -DPLATFORM_ANDROID=1 -DPLATFORM=9 -D__STDC_CONSTANT_MACROS=1
LOCAL_CFLAGS += -DPLAYER_CLASS_DIR=\"com/pbi/player/LivePlayer\"
LOCAL_CFLAGS += -fPIC -O3 -fvisibility=hidden

LOCAL_CXXFLAGS := -DHAVE_PTHREADS

LOCAL_ARM_MODE := arm

LOCAL_PRELINK_MODULE := false

LOCAL_STATIC_LIBRARIES := libyuv_static libyuv2rgb libwindutils libustl
													
LOCAL_SHARED_LIBRARIES := libffpcore

LOCAL_LDLIBS := -L$(call host-path,$(ANDROID23_SYS_INC)) \
				-lskia \
               	-lutils \
               	-lcutils \
               	-lui \
               	-landroid_runtime \
               	-lnativehelper \
               	-lsurfaceflinger_client \
               	-lOpenSLES	\
               	-ldl                    \
               	-lz	\
               	-llog
               	
LOCAL_SRC_FILES:= \
				liveplayer.cpp \
				android_audioopensles_render.cpp \
				android_videosurface_render.cpp \
				android_stub_render.cpp \
				android_liveplayer.cpp \
				android_liveplayerlistener.cpp
				
include $(BUILD_SHARED_LIBRARY)



#######################################################################
# Build the libffplay-14-jni library

include $(CLEAR_VARS)

LOCAL_MODULE:= libffplay-14-jni

LOCAL_C_INCLUDES := $(LOCAL_PATH) \
					$(LOCAL_PATH)/../ustl-1.0 \
					$(LOCAL_PATH)/../libutils \
					$(LOCAL_PATH)/../libffmpeg \
					$(LOCAL_PATH)/../libffplaycore \
					$(LOCAL_PATH)/../yuv2rgb/include \
					$(LOCAL_PATH)/../libyuv/include \
					$(ANDROID40_SYS_INC)/system/core/include \
					$(ANDROID40_SYS_INC)/system/media/wilhelm/include \
					$(ANDROID40_SYS_INC)/frameworks/base/include \
					$(ANDROID40_SYS_INC)/frameworks/base/native/include \
					$(ANDROID40_SYS_INC)/frameworks/base/opengl/include \
					$(ANDROID40_SYS_INC)/dalvik/libnativehelper/include \
					$(ANDROID40_SYS_INC)/hardware/libhardware/include \
					$(ANDROID40_SYS_INC)/bionic/libc/include \
					$(ANDROID40_SYS_INC)/external/skia/include/core
					
LOCAL_CFLAGS := -DPLATFORM_ANDROID=1 -DPLATFORM=14 -D__STDC_CONSTANT_MACROS=1
LOCAL_CFLAGS += -DPLAYER_CLASS_DIR=\"com/pbi/player/LivePlayer\"
LOCAL_CFLAGS += -fPIC -O3 -fvisibility=hidden

LOCAL_CXXFLAGS := -DHAVE_PTHREADS

LOCAL_ARM_MODE := arm

LOCAL_PRELINK_MODULE := false

LOCAL_STATIC_LIBRARIES := libyuv_static libyuv2rgb libwindutils libustl
													
LOCAL_SHARED_LIBRARIES := libffpcore

LOCAL_LDLIBS := -L$(call host-path,$(ANDROID40_SYS_INC)) \
				-lskia \
               	-lutils \
               	-lcutils \
               	-lui \
               	-landroid_runtime \
               	-lnativehelper \
               	-lgui \
               	-lOpenSLES \
               	-ldl                    \
               	-lz	\
               	-llog
               	
LOCAL_SRC_FILES:= \
				liveplayer.cpp \
				android_audioopensles_render.cpp \
				android_videosurface_render.cpp \
				android_stub_render.cpp \
				android_liveplayer.cpp \
				android_liveplayerlistener.cpp

include $(BUILD_SHARED_LIBRARY)

#######################################################################
# Build the libffplay-16-jni library

include $(CLEAR_VARS)

LOCAL_MODULE:= libffplay-16-jni

LOCAL_C_INCLUDES := $(LOCAL_PATH) \
					$(LOCAL_PATH)/../ustl-1.0 \
					$(LOCAL_PATH)/../libutils \
					$(LOCAL_PATH)/../libffmpeg \
					$(LOCAL_PATH)/../libffplaycore \
					$(LOCAL_PATH)/../yuv2rgb/include \
					$(LOCAL_PATH)/../libyuv/include \
					$(ANDROID41_SYS_INC)/system/core/include \
					$(ANDROID41_SYS_INC)/frameworks/av/include \
					$(ANDROID41_SYS_INC)/frameworks/base/include \
					$(ANDROID41_SYS_INC)/frameworks/base/native/include \
					$(ANDROID41_SYS_INC)/frameworks/base/opengl/include \
					$(ANDROID41_SYS_INC)/frameworks/native/include \
					$(ANDROID41_SYS_INC)/frameworks/native/opengl/include \
					$(ANDROID41_SYS_INC)/frameworks/wilhelm/include \
					$(ANDROID41_SYS_INC)/libnativehelper/include \
					$(ANDROID41_SYS_INC)/hardware/libhardware/include \
					$(ANDROID41_SYS_INC)/bionic/libc/include \
					$(ANDROID41_SYS_INC)/external/skia/include/core
					
LOCAL_CFLAGS := -DPLATFORM_ANDROID=1 -DPLATFORM=16 -D__STDC_CONSTANT_MACROS=1
LOCAL_CFLAGS += -DPLAYER_CLASS_DIR=\"com/pbi/player/LivePlayer\"
LOCAL_CFLAGS += -fPIC -O3 -fvisibility=hidden

LOCAL_CXXFLAGS := -DHAVE_PTHREADS

LOCAL_ARM_MODE := arm

LOCAL_PRELINK_MODULE := false

LOCAL_STATIC_LIBRARIES := libyuv_static libyuv2rgb libwindutils libustl
													
LOCAL_SHARED_LIBRARIES := libffpcore

LOCAL_LDLIBS := -L$(call host-path,$(ANDROID41_SYS_INC)) \
				-lskia \
               	-lutils \
               	-lcutils \
               	-lui \
               	-landroid_runtime \
               	-lnativehelper \
               	-lgui \
               	-lOpenSLES \
               	-ldl                    \
               	-lz	\
               	-llog
               	
LOCAL_SRC_FILES:= \
				liveplayer.cpp \
				android_audioopensles_render.cpp \
				android_videosurface_render.cpp \
				android_stub_render.cpp \
				android_liveplayer.cpp \
				android_liveplayerlistener.cpp

include $(BUILD_SHARED_LIBRARY)

#######################################################################
# Build the libffplay-18-jni library

include $(CLEAR_VARS)

LOCAL_MODULE:= libffplay-18-jni

LOCAL_C_INCLUDES := $(LOCAL_PATH) \
					$(LOCAL_PATH)/../ustl-1.0 \
					$(LOCAL_PATH)/../libutils \
					$(LOCAL_PATH)/../libffmpeg \
					$(LOCAL_PATH)/../libffplaycore \
					$(LOCAL_PATH)/../yuv2rgb/include \
					$(LOCAL_PATH)/../libyuv/include \
					$(ANDROID43_SYS_INC)/system/core/include \
					$(ANDROID43_SYS_INC)/frameworks/av/include \
					$(ANDROID43_SYS_INC)/frameworks/base/include \
					$(ANDROID43_SYS_INC)/frameworks/base/native/include \
					$(ANDROID43_SYS_INC)/frameworks/base/opengl/include \
					$(ANDROID43_SYS_INC)/frameworks/native/include \
					$(ANDROID43_SYS_INC)/frameworks/native/opengl/include \
					$(ANDROID43_SYS_INC)/frameworks/wilhelm/include \
					$(ANDROID43_SYS_INC)/libnativehelper/include \
					$(ANDROID43_SYS_INC)/hardware/libhardware/include \
					$(ANDROID43_SYS_INC)/bionic/libc/include \
					$(ANDROID43_SYS_INC)/external/skia/include/core
					
LOCAL_CFLAGS := -DPLATFORM_ANDROID=1 -DPLATFORM=18 -D__STDC_CONSTANT_MACROS=1
LOCAL_CFLAGS += -DPLAYER_CLASS_DIR=\"com/pbi/player/LivePlayer\"
LOCAL_CFLAGS += -fPIC -O3 -fvisibility=hidden

LOCAL_CXXFLAGS := -DHAVE_PTHREADS

LOCAL_ARM_MODE := arm

LOCAL_PRELINK_MODULE := false

LOCAL_STATIC_LIBRARIES := libyuv_static libyuv2rgb libwindutils libustl
													
LOCAL_SHARED_LIBRARIES := libffpcore

LOCAL_LDLIBS := -L$(call host-path,$(ANDROID43_SYS_INC)) \
				-landroid \
				-lskia \
               	-lutils \
               	-lcutils \
               	-lui \
               	-landroid_runtime \
               	-lnativehelper \
               	-lgui \
               	-lOpenSLES \
               	-ldl                    \
               	-lz	\
               	-llog
               	
LOCAL_SRC_FILES:= \
				liveplayer.cpp \
				android_audioopensles_render.cpp \
				android_nativewindow_render.cpp \
				android_stub_render.cpp \
				android_liveplayer.cpp \
				android_liveplayerlistener.cpp

include $(BUILD_SHARED_LIBRARY)
endif