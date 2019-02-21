LOCAL_PATH := $(call my-dir)
SRC_DIR := ../../

include $(CLEAR_VARS)

LOCAL_SRC_FILES  := $(SRC_DIR)/src/gameCommon.cpp \
                    $(SRC_DIR)/src/cameraCelib.cpp \
                    $(SRC_DIR)/src/motionDetection.cpp \
                    $(SRC_DIR)/src/detect.cpp

LOCAL_SRC_FILES += 	com_ysten_camera_MotionDetect.cpp

# Open tbb option
LOCAL_CFLAGS := -DHAVE_OPENMP

LOCAL_C_INCLUDES += $(SRC_DIR) $(LOCAL_PATH) $(LOCAL_PATH)/include $(LOCAL_PATH)/events $(LOCAL_PATH)/sdk/native/jni/include 
#$(SRC_DIR)/include 



LOCAL_LDLIBS     += -llog -L$(LOCAL_PATH)/sdk/native/libs/armeabi-v7a -lopencv_java


LOCAL_ARM_MODE := arm
LOCAL_CFLAGS += -O3	-std=c++11
#LOCAL_ARM_NEON := true 

LOCAL_MODULE     := motionDetect

include $(BUILD_SHARED_LIBRARY)
#include $(BUILD_EXECUTABLE)
