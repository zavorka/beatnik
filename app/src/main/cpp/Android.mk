LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := beatnik

LOCAL_SRC_FILES := native-lib.cpp

LOCAL_LDLIBS := -llog -latomic


include $(BUILD_SHARED_LIBRARY)