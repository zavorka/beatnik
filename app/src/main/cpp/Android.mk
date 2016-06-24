LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := beatnik

LOCAL_C_INCLUDES += $(NDK_ROOT)/sources/boost/1.59.0/include

LOCAL_SRC_FILES := df-processor.cpp
LOCAL_SRC_FILES += external/kiss_fft/kiss_fft.c
LOCAL_SRC_FILES += external/kiss_fft/kiss_fftr.c
LOCAL_SRC_FILES += external/RealTime/RealTime.cpp

LOCAL_SRC_FILES += beatnik/FFT_rolling.cpp
LOCAL_SRC_FILES += tracker/Beat_counter.cpp
LOCAL_SRC_FILES += tracker/CSD_detection_function.cpp
LOCAL_SRC_FILES += tracker/math_utilities.cpp
LOCAL_SRC_FILES += tracker/Tracker.cpp

LOCAL_LDLIBS := -llog -latomic


include $(BUILD_SHARED_LIBRARY)