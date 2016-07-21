LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := beatnik

LOCAL_DISABLE_FATAL_LINKER_WARNINGS := true

ifeq ($(TARGET_ARCH_ABI),$(filter $(TARGET_ARCH_ABI), armeabi-v7a))
LOCAL_ARM_NEON  := true
endif # TARGET_ARCH_ABI == armeabi-v7a


LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../external/boost/include

LOCAL_SRC_FILES += df-processor.cpp
LOCAL_SRC_FILES += btrack.cpp

LOCAL_SRC_FILES += external/kiss_fft/kiss_fft.c
LOCAL_SRC_FILES += external/kiss_fft/kiss_fftr.c

LOCAL_SRC_FILES += tracker/BTrack.cpp

#LOCAL_STATIC_LIBRARIES := cpufeatures

LOCAL_LDLIBS += -llog -latomic

include $(BUILD_SHARED_LIBRARY)
#$(call import-module,android/cpufeatures)