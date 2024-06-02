LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

#----------------------------------------------------------------------------
#                 Common definitons
#----------------------------------------------------------------------------

acdb-def += -D_ANDROID_

#----------------------------------------------------------------------------
#             Make the Shared library (libar-acdb)
#----------------------------------------------------------------------------

#LOCAL_C_INCLUDES := $(LOCAL_PATH)/inc

LOCAL_CFLAGS := $(acdb-def)

LOCAL_C_INCLUDES := $(LOCAL_PATH)/api\
    $(LOCAL_PATH)/inc

LOCAL_SRC_FILES := \
    src/acdb.c \
    src/acdb_command.c \
    src/acdb_delta_file_mgr.c \
    src/acdb_delta_parser.c \
    src/acdb_file_mgr.c \
    src/acdb_init.c \
    src/acdb_init_utility.c \
    src/acdb_parser.c \
    src/acdb_utility.c\
    src/acdb_data_proc.c\
    src/acdb_heap.c\
    src/acdb_context_mgr.c

LOCAL_MODULE := libar-acdb
LOCAL_MODULE_OWNER := qti
LOCAL_MODULE_TAGS := optional
LOCAL_VENDOR_MODULE := true

LOCAL_HEADER_LIBRARIES := \
    libcutils_headers \
    libutils_headers \
    vendor_common_inc

LOCAL_SHARED_LIBRARIES := \
    liblx-osal

LOCAL_EXPORT_C_INCLUDE_DIRS := $(LOCAL_PATH)/api
LOCAL_EXPORT_C_INCLUDE_DIRS += $(LOCAL_PATH)/inc

include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)

#----------------------------------------------------------------------------
#                 Common definitons
#----------------------------------------------------------------------------

ats-def += -D_ANDROID_ -DAR_ATS_USE_CUTILS

#----------------------------------------------------------------------------
#             Make the Shared library (libar-acdb)
#----------------------------------------------------------------------------

#LOCAL_C_INCLUDES := $(MYLOCAL_PATH)/inc

LOCAL_CFLAGS := $(ats-def)

ifeq ($(strip $(AUDIO_FEATURE_ENABLED_MCS)),true)
	LOCAL_CFLAGS += -DMCS_ENABLED
endif

LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/ats/inc \
    $(LOCAL_PATH)/ats/rtc/common/inc \
    $(LOCAL_PATH)/ats/online/common/inc/ \
    $(LOCAL_PATH)/ats/mcs/common/inc/ \
    $(LOCAL_PATH)/ats/mcs/common/api/ \
    $(LOCAL_PATH)/ats/fts/common/inc/ \
    $(LOCAL_PATH)/ats/transports/tcpip/linux/inc/ \
    $(LOCAL_PATH)/ats/transports/diag/linux/actp/inc/ \
    $(LOCAL_PATH)/ats/transports/diag/linux/audtp/inc/ \
    $(LOCAL_PATH)/ats/adie/common/inc \
    $(LOCAL_PATH)/ats/adie/common/api \
    $(LOCAL_PATH)/ats/api

LOCAL_SRC_FILES := \
    ats/transports/tcpip/linux/src/ats_server.cpp \
    ats/src/ats.c \
    ats/src/ats_command.c \
    ats/rtc/common/src/ats_rtc.c \
    ats/online/common/src/ats_online.c \
    ats/mcs/common/src/ats_mcs.c \
    ats/mcs/linux/mcs.c \
    ats/fts/common/src/ats_fts.c \
    ats/transports/diag/linux/actp/src/actp.c \
    ats/transports/diag/linux/audtp/src/audtp.c \
    ats/adie/common/src/ats_adie_rtc.c \
    ats/adie/linux/adie_rtc.c

LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/diag/include

LOCAL_MODULE := libats
LOCAL_MODULE_OWNER := qti
LOCAL_MODULE_TAGS := optional
LOCAL_VENDOR_MODULE := true

LOCAL_HEADER_LIBRARIES := \
    libcutils_headers \
    libutils_headers \
    libdiag_headers \
    vendor_common_inc

LOCAL_SHARED_LIBRARIES := \
    liblx-osal\
    libutils\
    libcutils \
    libdiag \
    libar-gsl\
    libar-acdb

LOCAL_EXPORT_C_INCLUDE_DIRS := $(LOCAL_PATH)/ats/api
LOCAL_EXPORT_C_INCLUDE_DIRS += $(LOCAL_PATH)/ats/inc
LOCAL_EXPORT_C_INCLUDE_DIRS += $(LOCAL_PATH)/ats/adie/common/api
LOCAL_EXPORT_C_INCLUDE_DIRS += $(LOCAL_PATH)/ats/mcs/common/api

include $(BUILD_SHARED_LIBRARY)
