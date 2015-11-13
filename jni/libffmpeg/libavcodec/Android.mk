LOCAL_PATH := $(call my-dir)

AVCODEC_FILES	:=  4xm.c \
									8bps.c \
									8svx.c \
									aac_ac3_parser.c \
									aac_parser.c \
									aacadtsdec.c \
									aacdec.c \
									aacps.c \
									aacsbr.c \
									aactab.c \
									aandcttab.c \
									aasc.c \
									ac3.c \
									ac3_parser.c \
									ac3dec.c \
									ac3dec_data.c \
									ac3dsp.c \
									ac3tab.c \
									acelp_filters.c \
									acelp_pitch_delay.c \
									acelp_vectors.c \
									adpcm.c \
									adxdec.c \
									alac.c \
									allcodecs.c \
									alsdec.c \
									amrnbdec.c \
									amrwbdec.c \
									anm.c \
									ansi.c \
									apedec.c \
									ass.c \
									ass_split.c \
									assdec.c \
									asv1.c \
									atrac.c \
									atrac1.c \
									atrac3.c \
									audioconvert.c \
									aura.c \
									avfft.c \
									avpacket.c \
									avs.c \
									bethsoftvideo.c \
									bfi.c \
									bgmc.c \
									bink.c \
									binkaudio.c \
									binkidct.c \
									bitstream.c \
									bitstream_filter.c \
									bmp.c \
									c93.c \
									cabac.c \
									cavs.c \
									cavs_parser.c \
									cavsdec.c \
									cavsdsp.c \
									cdgraphics.c \
									celp_filters.c \
									celp_math.c \
									cga_data.c \
									cinepak.c \
									cljr.c \
									cook.c \
									cscd.c \
									cyuv.c \
									dca.c \
									dca_parser.c \
									dcadsp.c \
									dct.c \
									dct32_fixed.c \
									dct32_float.c \
									dfa.c \
									dirac.c \
									dirac_parser.c \
									dnxhd_parser.c \
									dnxhddata.c \
									dnxhddec.c \
									dpcm.c \
									dpx.c \
									dsicinav.c \
									dsputil.c \
									dv.c \
									dvbsub_parser.c \
									dvbsubdec.c \
									dvdata.c \
									dvdsub_parser.c \
									dvdsubdec.c \
									dwt.c \
									dxa.c \
									eac3dec.c \
									eac3dec_data.c \
									eacmv.c \
									eaidct.c \
									eamad.c \
									eatgq.c \
									eatgv.c \
									eatqi.c \
									error_resilience.c \
									escape124.c \
									faanidct.c \
									faxcompr.c \
									fft_fixed.c \
									fft_float.c \
									ffv1.c \
									flac.c \
									flac_parser.c \
									flacdata.c \
									flacdec.c \
									flashsv.c \
									flicvideo.c \
									flvdec.c \
									fmtconvert.c \
									fraps.c \
									frwu.c \
									g722.c \
									g726.c \
									gifdec.c \
									golomb.c \
									gsmdec.c \
									gsmdec_data.c \
									h261.c \
									h261_parser.c \
									h261dec.c \
									h263.c \
									h263_parser.c \
									h263dec.c \
									h264.c \
									h264_cabac.c \
									h264_cavlc.c \
									h264_direct.c \
									h264_loopfilter.c \
									h264_parser.c \
									h264_ps.c \
									h264_refs.c \
									h264_sei.c \
									h264dsp.c \
									h264idct.c \
									h264pred.c \
									huffman.c \
									huffyuv.c \
									idcinvideo.c \
									iff.c \
									imc.c \
									imgconvert.c \
									indeo2.c \
									indeo3.c \
									indeo5.c \
									intelh263dec.c \
									interplayvideo.c \
									intrax8.c \
									intrax8dsp.c \
									inverse.c \
									ituh263dec.c \
									ivi_common.c \
									ivi_dsp.c \
									j2k.c \
									j2k_dwt.c \
									j2kdec.c \
									jpegls.c \
									jpeglsdec.c \
									jrevdct.c \
									jvdec.c \
									kbdwin.c \
									kgv1dec.c \
									kmvc.c \
									lagarith.c \
									lagarithrac.c \
									latm_parser.c \
									lcldec.c \
									loco.c \
									lsp.c \
									lzw.c \
									mace.c \
									mdct_fixed.c \
									mdct_float.c \
									mdec.c \
									mimic.c \
									mjpeg.c \
									mjpeg_parser.c \
									mjpegbdec.c \
									mjpegdec.c \
									mlp.c \
									mlp_parser.c \
									mlpdec.c \
									mlpdsp.c \
									mmvideo.c \
									motionpixels.c \
									mpc.c \
									mpc7.c \
									mpc8.c \
									mpeg12.c \
									mpeg12data.c \
									mpeg4audio.c \
									mpeg4video.c \
									mpeg4video_parser.c \
									mpeg4videodec.c \
									mpegaudio.c \
									mpegaudio_parser.c \
									mpegaudiodata.c \
									mpegaudiodec.c \
									mpegaudiodec_float.c \
									mpegaudiodecheader.c \
									mpegaudiodsp.c \
									mpegaudiodsp_fixed.c \
									mpegaudiodsp_float.c \
									mpegvideo.c \
									mpegvideo_parser.c \
									mqc.c \
									mqcdec.c \
									msgsmdec.c \
									msmpeg4.c \
									msmpeg4data.c \
									msrle.c \
									msrledec.c \
									msvideo1.c \
									mxpegdec.c \
									nellymoser.c \
									nellymoserdec.c \
									nuv.c \
									opt.c \
									options.c \
									parser.c \
									pcm-mpeg.c \
									pcm.c \
									pcx.c \
									pgssubdec.c \
									pictordec.c \
									png.c \
									pngdec.c \
									pnm.c \
									pnm_parser.c \
									pnmdec.c \
									pthread.c \
									ptx.c \
									qcelpdec.c \
									qdm2.c \
									qdrw.c \
									qpeg.c \
									qtrle.c \
									r210dec.c \
									ra144.c \
									ra144dec.c \
									ra288.c \
									rangecoder.c \
									raw.c \
									rawdec.c \
									rdft.c \
									resample.c \
									resample2.c \
									rl2.c \
									roqvideo.c \
									roqvideodec.c \
									rpza.c \
									rtjpeg.c \
									rv10.c \
									rv30.c \
									rv30dsp.c \
									rv34.c \
									rv40.c \
									rv40dsp.c \
									s302m.c \
									s3tc.c \
									sgidec.c \
									shorten.c \
									simple_idct.c \
									sinewin.c \
									sipr.c \
									sipr16k.c \
									smacker.c \
									smc.c \
									snow.c \
									sonic.c \
									sp5xdec.c \
									srtdec.c \
									sunrast.c \
									svq1.c \
									svq1dec.c \
									svq3.c \
									synth_filter.c \
									targa.c \
									tiertexseqv.c \
									tiff.c \
									tmv.c \
									truemotion1.c \
									truemotion2.c \
									truespeech.c \
									tscc.c \
									tta.c \
									twinvq.c \
									txd.c \
									ulti.c \
									utils.c \
									v210dec.c \
									v210x.c \
									vb.c \
									vc1.c \
									vc1_parser.c \
									vc1data.c \
									vc1dec.c \
									vc1dsp.c \
									vcr1.c \
									vmdav.c \
									vmnc.c \
									vorbis.c \
									vorbis_data.c \
									vorbisdec.c \
									vp3.c \
									vp3_parser.c \
									vp3dsp.c \
									vp5.c \
									vp56.c \
									vp56data.c \
									vp56dsp.c \
									vp56rac.c \
									vp6.c \
									vp6dsp.c \
									vp8.c \
									vp8_parser.c \
									vp8dsp.c \
									vqavideo.c \
									wavpack.c \
									wma.c \
									wmadec.c \
									wmaprodec.c \
									wmavoice.c \
									wmv2.c \
									wmv2dec.c \
									wnv1.c \
									ws-snd1.c \
									xan.c \
									xiph.c \
									xl.c \
									xsubdec.c \
									xxan.c \
									yop.c \
									zmbv.c
AVCODEC_ARMV5_FILES:= \
									arm/ac3dsp_arm.S \
									arm/ac3dsp_init_arm.c \
									arm/dcadsp_init_arm.c \
									arm/dsputil_arm.S \
									arm/dsputil_init_arm.c \
									arm/dsputil_init_armv5te.c \
									arm/fft_fixed_init_arm.c \
									arm/fft_init_arm.c \
									arm/fmtconvert_init_arm.c \
									arm/h264dsp_init_arm.c \
									arm/h264pred_init_arm.c \
									arm/jrevdct_arm.S \
									arm/mpegaudiodsp_init_arm.c \
									arm/mpegvideo_arm.c \
									arm/mpegvideo_armv5te.c \
									arm/mpegvideo_armv5te_s.S \
									arm/simple_idct_arm.S \
									arm/simple_idct_armv5te.S \
									arm/vp56dsp_init_arm.c \
									arm/vp8dsp_init_arm.c
AVCODEC_VFP_FILES := \
									arm/dsputil_init_vfp.c \
									arm/dsputil_vfp.S \
									arm/fmtconvert_vfp.S
AVCODEC_ARMV6_FILES:=\
									arm/ac3dsp_armv6.S \
									arm/dsputil_armv6.S \
									arm/dsputil_init_armv6.c \
									arm/mpegaudiodsp_fixed_armv6.S \
									arm/simple_idct_armv6.S \
									arm/vp8_armv6.S	
AVCODEC_NEON_FILES := \
									arm/ac3dsp_neon.S \
									arm/dcadsp_neon.S \
									arm/dsputil_init_neon.c \
									arm/dsputil_neon.S \
									arm/fft_fixed_neon.S \
									arm/fft_neon.S \
									arm/fmtconvert_neon.S \
									arm/h264dsp_neon.S \
									arm/h264idct_neon.S \
									arm/h264pred_neon.S \
									arm/int_neon.S \
									arm/mdct_fixed_neon.S \
									arm/mdct_neon.S \
									arm/mpegvideo_neon.S \
									arm/rdft_neon.S \
									arm/simple_idct_neon.S \
									arm/synth_filter_neon.S \
									arm/vp3dsp_neon.S \
									arm/vp56dsp_neon.S \
									arm/vp8dsp_neon.S

#######################################################
#BUILD avcodec_armv5

include $(CLEAR_VARS)

LOCAL_MODULE    := avcodec_armv5

LOCAL_C_INCLUDES := \
		$(LOCAL_PATH) \
		$(LOCAL_PATH)/..
		
LOCAL_CFLAGS := $(LOCAL_C_INCLUDES:%=-I%)
LOCAL_ARM_MODE := arm

LOCAL_CFLAGS :=-O3
LOCAL_CFLAGS +=-DANDROID -DHAVE_AV_CONFIG_H -D__ARMV5__ -DINLINE=inline 
LOCAL_CFLAGS +=-mfloat-abi=softfp -ffast-math
LOCAL_LDLIBS :=-llog -lz


LOCAL_SRC_FILES :=$(AVCODEC_FILES)
LOCAL_SRC_FILES +=$(AVCODEC_ARMV5_FILES)

include $(BUILD_STATIC_LIBRARY)

#######################################################
#BUILD avcodec_armv6_vfp

include $(CLEAR_VARS)

LOCAL_MODULE    := avcodec_armv6_vfp

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


LOCAL_SRC_FILES :=$(AVCODEC_FILES)
LOCAL_SRC_FILES +=$(AVCODEC_ARMV5_FILES)
LOCAL_SRC_FILES +=$(AVCODEC_ARMV6_FILES)
LOCAL_SRC_FILES +=$(AVCODEC_VFP_FILES)

include $(BUILD_STATIC_LIBRARY)

#######################################################
#BUILD avcodec_armv7_vfpv3

#include $(CLEAR_VARS)

#LOCAL_MODULE    := avcodec_armv7_vfpv3

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

#LOCAL_SRC_FILES :=$(AVCODEC_FILES)
#LOCAL_SRC_FILES +=$(AVCODEC_ARMV5_FILES)
#LOCAL_SRC_FILES +=$(AVCODEC_ARMV6_FILES)
#LOCAL_SRC_FILES +=$(AVCODEC_VFP_FILES)

#include $(BUILD_STATIC_LIBRARY)

#######################################################
#BUILD avcodec_armv7_neon

include $(CLEAR_VARS)

LOCAL_MODULE    := avcodec_armv7_neon

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


LOCAL_SRC_FILES :=$(AVCODEC_FILES)
LOCAL_SRC_FILES +=$(AVCODEC_ARMV5_FILES)
LOCAL_SRC_FILES +=$(AVCODEC_ARMV6_FILES)
LOCAL_SRC_FILES +=$(AVCODEC_VFP_FILES)
LOCAL_SRC_FILES +=$(AVCODEC_NEON_FILES)

include $(BUILD_STATIC_LIBRARY)
