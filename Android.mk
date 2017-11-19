# Copyright 2013 The Android Open Source Project

LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= main.cpp

LOCAL_C_INCLUDES := \
    $(TOP)/frameworks/av/include

LOCAL_SHARED_LIBRARIES:= libcutils libmedia

LOCAL_MODULE:= memory_leak

include $(BUILD_EXECUTABLE)
