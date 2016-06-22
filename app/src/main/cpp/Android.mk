LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := beatnik

LOCAL_SRC_FILES := native-lib.cpp
LOCAL_SRC_FILES += external/kiss_fft/kiss_fft.c
LOCAL_SRC_FILES += external/kiss_fft/kiss_fftr.c

LOCAL_LDLIBS := -llog -latomic


include $(BUILD_SHARED_LIBRARY)