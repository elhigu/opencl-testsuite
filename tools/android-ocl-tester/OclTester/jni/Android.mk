#
# To build this set up automatic builder in eclipse: http://mobilepearls.com/labs/ndk-builder-in-eclipse/
# Or just run "ndk-build" command in project directory
#

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

# Locate OpenCL SDK root folder
INTELOCLSDKROOT="/Users/mikaelle/Projects/Vincit/AndroidSDK/adt-bundle-mac-x86_64-20140624/intel-opencl-for-android-1.2-4.4.0.117"

ifeq "$(TARGET_ARCH_ABI)" "x86"
LOCAL_CFLAGS += -I$(INTELOCLSDKROOT)/include
LOCAL_LDFLAGS += -llog -ljnigraphics -L$(INTELOCLSDKROOT)/lib64/android32 -lOpenCL

LOCAL_MODULE    := ocl-tester
LOCAL_SRC_FILES := jni-wrapper.cpp ocl-tester.cpp
endif


include $(BUILD_SHARED_LIBRARY)
