LOCAL_PATH := $(call my-dir)

AVUTIL_FILES := \
									adler32.c \
									aes.c \
									audioconvert.c \
									avstring.c \
									base64.c \
									cpu.c \
									crc.c \
									des.c \
									dict.c \
									error.c \
									eval.c \
									fifo.c \
									file.c \
									imgutils.c \
									intfloat_readwrite.c \
									inverse.c \
									lfg.c \
									lls.c \
									log.c \
									lzo.c \
									mathematics.c \
									md5.c \
									mem.c \
									opt.c \
									parseutils.c \
									pixdesc.c \
									random_seed.c \
									rational.c \
									rc4.c \
									samplefmt.c \
									sha.c \
									tree.c \
									utils.c \
									arm/cpu.c
		

#######################################################
#BUILD avutil_armv5

include $(CLEAR_VARS)

LOCAL_MODULE    := avutil_armv5

LOCAL_C_INCLUDES := \
		$(LOCAL_PATH) \
		$(LOCAL_PATH)/..
		
LOCAL_CFLAGS := $(LOCAL_C_INCLUDES:%=-I%)
LOCAL_ARM_MODE := arm

LOCAL_CFLAGS :=-O3
LOCAL_CFLAGS +=-DANDROID -DHAVE_AV_CONFIG_H -D__ARMV5__ -DINLINE=inline 
LOCAL_CFLAGS +=-mfloat-abi=softfp -ffast-math
LOCAL_LDLIBS :=-llog -lz


LOCAL_SRC_FILES :=$(AVUTIL_FILES)

include $(BUILD_STATIC_LIBRARY)

#######################################################
#BUILD avutil_armv6_vfp

include $(CLEAR_VARS)

LOCAL_MODULE    := avutil_armv6_vfp

LOCAL_C_INCLUDES := \
		$(LOCAL_PATH) \
		$(LOCAL_PATH)/..
		
LOCAL_CFLAGS := $(LOCAL_C_INCLUDES:%=-I%)
LOCAL_ARM_MODE := arm

LOCAL_CFLAGS :=-O3
LOCAL_CFLAGS +=-DANDROID -DHAVE_AV_CONFIG_H -D__ARMV6_VFP__ -DINLINE=inline 
LOCAL_CFLAGS +=-mfloat-abi=softfp -ffast-math
LOCAL_CFLAGS +=-mfpu=vfp
LOCAL_CFLAGS +=-march=armv6

LOCAL_LDLIBS := -llog -lz


LOCAL_SRC_FILES :=$(AVUTIL_FILES)

include $(BUILD_STATIC_LIBRARY)

#######################################################
#BUILD avutil_armv7_vfpv3

#include $(CLEAR_VARS)

#LOCAL_MODULE    := avutil_armv7_vfpv3

#LOCAL_C_INCLUDES := \
#		$(LOCAL_PATH) \
#		$(LOCAL_PATH)/..
		
#LOCAL_CFLAGS := $(LOCAL_C_INCLUDES:%=-I%)
#LOCAL_ARM_MODE := arm

#LOCAL_CFLAGS :=-O3
#LOCAL_CFLAGS +=-DANDROID -DHAVE_AV_CONFIG_H -D__ARMV7_VFPV3__ -DINLINE=inline 
#LOCAL_CFLAGS +=-mfloat-abi=softfp -ffast-math
#LOCAL_CFLAGS +=-mfpu=vfpv3-d16
#LOCAL_CFLAGS +=-march=armv7-a
#LOCAL_LDLIBS := -llog -lz

#LOCAL_SRC_FILES :=$(AVUTIL_FILES)

#include $(BUILD_STATIC_LIBRARY)

#######################################################
#BUILD avutil_armv7_neon

include $(CLEAR_VARS)

LOCAL_MODULE    := avutil_armv7_neon

LOCAL_C_INCLUDES := \
		$(LOCAL_PATH) \
		$(LOCAL_PATH)/..
		
LOCAL_CFLAGS := $(LOCAL_C_INCLUDES:%=-I%)
LOCAL_ARM_MODE := arm

LOCAL_CFLAGS :=-O3
LOCAL_CFLAGS +=-DANDROID -DHAVE_AV_CONFIG_H -D__ARMV7_NEON__ -DINLINE=inline 
LOCAL_CFLAGS +=-mfloat-abi=softfp -ffast-math
LOCAL_CFLAGS +=-mfpu=neon
LOCAL_CFLAGS +=-march=armv7-a 
LOCAL_LDLIBS := -llog -lz


LOCAL_SRC_FILES :=$(AVUTIL_FILES)

include $(BUILD_STATIC_LIBRARY)

