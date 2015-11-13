LOCAL_PATH := $(call my-dir)

AVFORMAT_FILES := 4xm.c \
									aacdec.c \
									ac3dec.c \
									aea.c \
									aiffdec.c \
									allformats.c \
									amr.c \
									anm.c \
									apc.c \
									ape.c \
									apetag.c \
									applehttp.c \
									asf.c \
									asfcrypt.c \
									asfdec.c \
									assdec.c \
									au.c \
									avi.c \
									avidec.c \
									avio.c \
									aviobuf.c \
									avlanguage.c \
									avs.c \
									bethsoftvid.c \
									bfi.c \
									bink.c \
									c93.c \
									caf.c \
									cafdec.c \
									cavsvideodec.c \
									cdg.c \
									cutils.c \
									daud.c \
									dfa.c \
									diracdec.c \
									dnxhddec.c \
									dsicin.c \
									dtsdec.c \
									dv.c \
									dxa.c \
									eacdata.c \
									electronicarts.c \
									ffmdec.c \
									ffmetadec.c \
									file.c \
									filmstripdec.c \
									flacdec.c \
									flic.c \
									flvdec.c \
									gxf.c \
									h261dec.c \
									h263dec.c \
									h264dec.c \
									http.c \
									httpauth.c \
									id3v1.c \
									id3v2.c \
									idcin.c \
									idroqdec.c \
									iff.c \
									img2.c \
									ingenientdec.c \
									ipmovie.c \
									isom.c \
									iss.c \
									iv8.c \
									ivfdec.c \
									jvdec.c \
									lmlm4.c \
									lxfdec.c \
									m4vdec.c \
									matroska.c \
									matroskadec.c \
									metadata.c \
									metadata_compat.c \
									microdvddec.c \
									mm.c \
									mmf.c \
									mms.c \
									mmsh.c \
									mmst.c \
									mov.c \
									mp3dec.c \
									mpc.c \
									mpc8.c \
									mpeg.c \
									mpegts.c \
									mpegvideodec.c \
									msnwc_tcp.c \
									mtv.c \
									mvi.c \
									mxf.c \
									mxfdec.c \
									mxg.c \
									ncdec.c \
									nsvdec.c \
									nut.c \
									nutdec.c \
									nuv.c \
									oggdec.c \
									oggparsecelt.c \
									oggparsedirac.c \
									oggparseflac.c \
									oggparseogm.c \
									oggparseskeleton.c \
									oggparsespeex.c \
									oggparsetheora.c \
									oggparsevorbis.c \
									oma.c \
									options.c \
									os_support.c \
									pcm.c \
									pcmdec.c \
									pmpdec.c \
									psxstr.c \
									pva.c \
									qcp.c \
									r3d.c \
									rawdec.c \
									rawvideodec.c \
									rdt.c \
									riff.c \
									rl2.c \
									rm.c \
									rmdec.c \
									rpl.c \
									rso.c \
									rsodec.c \
									rtp.c \
									rtpdec.c \
									rtpdec_amr.c \
									rtpdec_asf.c \
									rtpdec_h263.c \
									rtpdec_h264.c \
									rtpdec_latm.c \
									rtpdec_mpeg4.c \
									rtpdec_qcelp.c \
									rtpdec_qdm2.c \
									rtpdec_qt.c \
									rtpdec_svq3.c \
									rtpdec_vp8.c \
									rtpdec_xiph.c \
									rtpproto.c \
									rtsp.c \
									rtspdec.c \
									sapdec.c \
									sauce.c \
									sdp.c \
									seek.c \
									segafilm.c \
									sierravmd.c \
									siff.c \
									smacker.c \
									sol.c \
									soxdec.c \
									spdif.c \
									spdifdec.c \
									srtdec.c \
									swfdec.c \
									tcp.c \
									thp.c \
									tiertexseq.c \
									tmv.c \
									tta.c \
									tty.c \
									txd.c \
									udp.c \
									utils.c \
									vc1test.c \
									voc.c \
									vocdec.c \
									vorbiscomment.c \
									vqf.c \
									wav.c \
									wc3movie.c \
									westwood.c \
									wtv.c \
									wtvdec.c \
									wv.c \
									xa.c \
									xwma.c \
									yop.c \
									yuv4mpeg.c \

									

#######################################################
#BUILD avformat_armv5

include $(CLEAR_VARS)

LOCAL_MODULE    := avformat_armv5

LOCAL_C_INCLUDES := \
		$(LOCAL_PATH) \
		$(LOCAL_PATH)/..
		
LOCAL_CFLAGS := $(LOCAL_C_INCLUDES:%=-I%)
LOCAL_ARM_MODE := arm

LOCAL_CFLAGS :=-O3

LOCAL_CFLAGS += -include "string.h" -Dipv6mr_interface=ipv6mr_ifindex
LOCAL_CFLAGS +=-DANDROID -DHAVE_AV_CONFIG_H -D__ARMV5__ -DINLINE=inline 
LOCAL_CFLAGS +=-mfloat-abi=softfp -ffast-math
LOCAL_LDLIBS :=-llog -lz

LOCAL_SRC_FILES :=$(AVFORMAT_FILES)

include $(BUILD_STATIC_LIBRARY)

#######################################################
#BUILD avformat_armv6_vfp

include $(CLEAR_VARS)

LOCAL_MODULE    := avformat_armv6_vfp

LOCAL_C_INCLUDES := \
		$(LOCAL_PATH) \
		$(LOCAL_PATH)/..
		
LOCAL_CFLAGS := $(LOCAL_C_INCLUDES:%=-I%)
LOCAL_ARM_MODE := arm

LOCAL_CFLAGS :=-O3
LOCAL_CFLAGS += -include "string.h" -Dipv6mr_interface=ipv6mr_ifindex
LOCAL_CFLAGS +=-DANDROID -DHAVE_AV_CONFIG_H -D__ARMV6_VFP__ -DINLINE=inline 
LOCAL_CFLAGS +=-mfloat-abi=softfp -ffast-math
LOCAL_CFLAGS +=-mfpu=vfp
LOCAL_CFLAGS +=-march=armv6

LOCAL_LDLIBS := -llog -lz


LOCAL_SRC_FILES :=$(AVFORMAT_FILES)

include $(BUILD_STATIC_LIBRARY)

#######################################################
#BUILD avformat_armv7_vfpv3

#include $(CLEAR_VARS)

#LOCAL_MODULE    := avformat_armv7_vfpv3

#LOCAL_C_INCLUDES := \
#		$(LOCAL_PATH) \
#		$(LOCAL_PATH)/..
		
#LOCAL_CFLAGS := $(LOCAL_C_INCLUDES:%=-I%)
#LOCAL_ARM_MODE := arm

#LOCAL_CFLAGS := -O3  
#LOCAL_CFLAGS += -include "string.h" -Dipv6mr_interface=ipv6mr_ifindex
#LOCAL_CFLAGS +=-DANDROID -DHAVE_AV_CONFIG_H -D__ARMV7_VFPV3__ -DINLINE=inline 
#LOCAL_CFLAGS +=-mfloat-abi=softfp -ffast-math
#LOCAL_CFLAGS +=-mfpu=vfpv3-d16
#LOCAL_CFLAGS +=-march=armv7-a
#LOCAL_LDLIBS := -llog -lz

#LOCAL_SRC_FILES :=$(AVFORMAT_FILES)

#include $(BUILD_STATIC_LIBRARY)

#######################################################
#BUILD avformat_armv7_neon

include $(CLEAR_VARS)

LOCAL_MODULE    := avformat_armv7_neon

LOCAL_C_INCLUDES := \
		$(LOCAL_PATH) \
		$(LOCAL_PATH)/..
		
LOCAL_CFLAGS := $(LOCAL_C_INCLUDES:%=-I%)
LOCAL_ARM_MODE := arm

#LOCAL_CFLAGS := -O3
LOCAL_CFLAGS += -include "string.h" -Dipv6mr_interface=ipv6mr_ifindex
LOCAL_CFLAGS +=-DANDROID -DHAVE_AV_CONFIG_H -D__ARMV7_NEON__ -DINLINE=inline 
LOCAL_CFLAGS +=-mfloat-abi=softfp -ffast-math
LOCAL_CFLAGS +=-mfpu=neon
LOCAL_CFLAGS +=-march=armv7-a 
LOCAL_LDLIBS := -llog -lz


LOCAL_SRC_FILES :=$(AVFORMAT_FILES)

include $(BUILD_STATIC_LIBRARY)

