MY_LOCAL_PATH := $(call my-dir)

include $(MY_LOCAL_PATH)/../../../../../liblowladb/android/jni/Android.mk

LOCAL_PATH := $(MY_LOCAL_PATH)

include $(CLEAR_VARS)

LOCAL_MODULE    := lowladbjni
LOCAL_SRC_FILES := LDBClient.cpp LDBCollection.cpp LDBDatabase.cpp LDBObject.cpp \
    LDBObjectBuilder.cpp LDBObjectId.cpp LDBWriteResult.cpp LDBCursor.cpp
LOCAL_CPPFLAGS += -std=c++11 -D__STDC_LIMIT_MACROS -D__GXX_EXPERIMENTAL_CXX0X__ -DUNIX -DMONGO_USE_LONG_LONG_INT
LOCAL_LDLIBS := -llog
LOCAL_STATIC_LIBRARIES := lowladb

include $(BUILD_SHARED_LIBRARY)