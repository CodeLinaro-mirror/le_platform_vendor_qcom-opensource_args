/**
 * \file ar_osal_log.c
 *
 * \brief
 *      This file has implementation of logging related helper functions.
 *
 * \copyright
 *  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 *  SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#ifdef AR_OSAL_USE_CUTILS
#include <cutils/properties.h>
#endif

/* Substitute a printable placeholder for NULL string arguments. */
#define SAFE(s) ((s) ? (s) : "(null)")

#ifdef AR_OSAL_USE_SYSLOG
#include <syslog.h>

#ifndef ANDROID_LOG_VERBOSE
#define ANDROID_LOG_VERBOSE 2
#endif

#ifndef ANDROID_LOG_DEBUG
#define ANDROID_LOG_DEBUG 3
#endif

#ifndef ANDROID_LOG_INFO
#define ANDROID_LOG_INFO 4
#endif

#ifndef ANDROID_LOG_ERROR
#define ANDROID_LOG_ERROR 6
#endif

#ifndef ANDROID_LOG_FATAL
#define ANDROID_LOG_FATAL 7
#endif

static inline void __android_log_write(int prio, const char *tag, const char *msg)
{
    syslog(prio, "%s - %s", SAFE(tag), SAFE(msg));
}
#else
#include <log/log.h>
#endif

#include "ar_osal_log.h"
#ifdef AR_OSAL_AUDIO_LOG_UTIL_BRIDGE
#include "audio_log_util.h"
#endif
#define LOG_BUF_SIZE 1024

static inline const char_t *ar_log_file_short(const char_t *file)
{
    if (file == NULL)
        return "";

    const char_t *p = strrchr(file, '/');

    return p ? p + 1 : file;
}

#ifdef AR_OSAL_AUDIO_LOG_UTIL_BRIDGE
uint32_t ar_log_lvl = (AR_CRITICAL|AR_ERROR|AR_INFO|AR_DEBUG|AR_VERBOSE);
#else
uint32_t ar_log_lvl = (AR_CRITICAL|AR_ERROR|AR_INFO);
#endif

#ifdef AR_OSAL_AUDIO_LOG_UTIL_BRIDGE
static uint32_t ar_log_to_audio_log_level(uint32_t level)
{
    switch (level) {
    case AR_CRITICAL:
        return BASE_LOG_CRITICAL;
    case AR_ERROR:
        return BASE_LOG_ERROR;
    case AR_INFO:
        return BASE_LOG_INFO;
    case AR_DEBUG:
        return BASE_LOG_DEBUG;
    case AR_VERBOSE:
        return BASE_LOG_VERBOSE;
    default:
        return 0;
    }
}

static module_id_t ar_log_to_audio_log_module(const char_t *log_tag)
{
    if (log_tag == NULL)
        return MODULE_ARGS_OSAL;

    if (strncmp(log_tag, "GPR", 3) == 0 ||
        strncmp(log_tag, "gpr_", 4) == 0)
        return MODULE_ARGS_GPR;
    if (strcmp(log_tag, "gsl") == 0)
        return MODULE_ARGS_GSL;
    if (strcmp(log_tag, "ACDB") == 0)
        return MODULE_ARGS_ACDB;
    if (strncmp(log_tag, "ATS", 3) == 0 ||
        strncmp(log_tag, "AR SOCKET", 9) == 0 ||
        strcmp(log_tag, "ADIE_RTC") == 0)
        return MODULE_ARGS_ATS;
    if (strcmp(log_tag, "AR Error Detection Logger") == 0 ||
        strcmp(log_tag, "AR Data Logger") == 0)
        return MODULE_ARGS_UTIL;
    if (strcmp(log_tag, "spf") == 0 || strcmp(log_tag, "SPF") == 0)
        return MODULE_ARGS_SPF;

    return MODULE_ARGS_OSAL;
}
#endif


_IRQL_requires_max_(DISPATCH_LEVEL)
void ar_log_init(void)
{
#ifdef AR_OSAL_AUDIO_LOG_UTIL_BRIDGE
    /* audio_log_util manages its own initialization; log level is set
     * at compile time via the ar_log_lvl initializer above. */
#else
#ifdef AR_OSAL_USE_CUTILS
    //set this property to change the args debug logging enabled.
    if(property_get_bool("vendor.audio.args.enable.debug.logs", 0)) {
        ar_log_lvl = (AR_CRITICAL|AR_ERROR|AR_INFO|AR_DEBUG);
    }
    //set this property to change the args verbose logging enabled.
    if(property_get_bool("vendor.audio.args.enable.verbose.logs", 0)) {
        ar_log_lvl = (AR_CRITICAL|AR_ERROR|AR_INFO|AR_DEBUG|AR_VERBOSE);
    }
#endif
#endif
}

_IRQL_requires_max_(DISPATCH_LEVEL)
void ar_log(uint32_t level, const char_t* log_tag, const char_t* file,
        const char_t* fn, int32_t ln, const char_t* format, ...)
{
    va_list ap;
    char buf[LOG_BUF_SIZE];
    int prefix_len;
    int message_size;
#ifdef AR_OSAL_AUDIO_LOG_UTIL_BRIDGE
    uint32_t audio_log_level;
#endif

    prefix_len = snprintf(buf, sizeof(buf), "%s:%s:%d ",
                          ar_log_file_short(file), SAFE(fn), (int)ln);
    if (prefix_len < 0)
        return;
    if ((size_t)prefix_len >= sizeof(buf)) {
        /* No room remains for the message; emit the truncated prefix. */
        buf[sizeof(buf) - 1] = '\0';
        goto emit;
    }

    va_start(ap, format);
    message_size = vsnprintf(buf + prefix_len, sizeof(buf) - prefix_len,
                             format, ap);
    va_end(ap);
    if (message_size < 0)
        return;
    buf[sizeof(buf) - 1] = '\0';

emit:
#ifdef AR_OSAL_AUDIO_LOG_UTIL_BRIDGE
    audio_log_level = ar_log_to_audio_log_level(level);

    if (audio_log_level != 0)
        (void)audio_log_logger(audio_log_level,
                               ar_log_to_audio_log_module(log_tag), "%s - %s",
                               log_tag, buf);
    return;
#else
    switch (level) {
    case AR_DEBUG:
        __android_log_write(ANDROID_LOG_DEBUG, log_tag, buf);
        break;
    case AR_INFO:
        __android_log_write(ANDROID_LOG_INFO, log_tag, buf);
        break;
    case AR_ERROR:
        __android_log_write(ANDROID_LOG_ERROR, log_tag, buf);
        break;
    case AR_VERBOSE:
        __android_log_write(ANDROID_LOG_VERBOSE, log_tag, buf);
        break;
    case AR_CRITICAL:
        __android_log_write(ANDROID_LOG_FATAL, log_tag, buf);
        break;
    default:
        break;
    }
#endif
}

_IRQL_requires_max_(DISPATCH_LEVEL)
void ar_log_deinit(void)
{
}
