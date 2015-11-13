LOCAL_PATH:= $(call my-dir)

cmd-strip = $(TOOLCHAIN_PREFIX)strip --strip-debug --strip-unneeded $(call cygwin-to-host-path,$1)

ifeq ($(TARGET_ARCH_ABI),armeabi)

###############################################
# Build the libffpcore library

include $(CLEAR_VARS)

LOCAL_MODULE:= libffpcore
									
LOCAL_C_INCLUDES := $(LOCAL_PATH) \
					$(LOCAL_PATH)/../ustl-1.0 \
					$(LOCAL_PATH)/../libutils \
					$(LOCAL_PATH)/../libffmpeg \
					$(LOCAL_PATH)/../libcpuinfo \
										
LOCAL_CFLAGS := -DPLATFORM_ANDROID=1 \
				-D__STDC_CONSTANT_MACROS=1 

LOCAL_CFLAGS += -fPIC -O3 #-fvisibility=hidden

LOCAL_ARM_MODE := arm

LOCAL_PRELINK_MODULE := false

LOCAL_STATIC_LIBRARIES := libwindutils libustl libcpuinfo

LOCAL_LDLIBS := -llog -lz

LOCAL_SRC_FILES :=packetList.cpp \
				  demux.cpp \
  				  basedecoder.cpp \
				  decoder_impl.cpp \
				  masterclock.cpp \
				  adecoder.cpp \
				  vdecoder.cpp \
				  baseplayer.cpp \
				  cpu.cpp \
				  playcore.cpp	\
				  audioresample.cpp \
				  baseaudiorender.cpp \
				  timeshift.cpp


include $(BUILD_SHARED_LIBRARY)

endif