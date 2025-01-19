pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = ats.pc
EXTRA_DIST = $(pkgconfig_DATA)

ACLOCAL_AMFLAGS = -I m4
AM_CFLAGS = -Wall \
            -Wundef \
            -O2
AM_CFLAGS += @GLIB_CFLAGS@ -include glib.h

AM_CFLAGS += -I$(top_srcdir)/ar_osal/api \
             -I$(top_srcdir)/acdb/api \
             -I$(top_srcdir)/acdb/inc \
             -I$(top_srcdir)/gsl/api \
             -I$(top_srcdir)/gsl/rtc_api


AM_CFLAGS += -I ${srcdir}/inc \
             -I ${srcdir}/api \
             -I ${srcdir}/transports/diag/linux/actp/inc \
             -I ${srcdir}/transports/diag/linux/audtp/inc \
             -I ${srcdir}/online/common/inc \
             -I ${srcdir}/rtc/common/inc \
             -I ${srcdir}/fts/common/inc \
             -I ${srcdir}/transports/tcpip/linux/inc \
             -I ${srcdir}/mcs/common/inc \
             -I ${srcdir}/mcs/common/api \
             -I ${srcdir}/adie/common/inc \
             -I ${srcdir}/adie/common/api

AM_CPPFLAGS = $(AM_CFLAGS)

ats_headers = ./api/ats.h

ats_c_sources = ./src/ats.c \
                ./src/ats_command.c \
                ./adie/common/src/ats_adie_rtc.c \
                ./adie/linux/adie_rtc.c \
                ./transports/diag/linux/actp/src/actp.c \
                ./transports/diag/linux/audtp/src/audtp.c \
                ./fts/common/src/ats_fts.c \
                ./mcs/common/src/ats_mcs.c \
                ./mcs/linux/mcs.c \
                ./online/common/src/ats_online.c \
                ./rtc/common/src/ats_rtc.c \
                ./transports/tcpip/linux/src/ats_server.cpp

#Includes
lib_includedir = $(includedir)
lib_include_HEADERS = $(ats_headers)
requiredlibs = -lrt

lib_LTLIBRARIES = libats.la
libats_la_CC = @CC@
libats_la_LIBADD = -lar_osal -L$(top_builddir)/ar_osal
libats_la_LIBADD += -lacdb -L$(top_builddir)/acdb
if CUTILS_SUPPORTED
libats_la_LIBADD += -lcutils
AM_CFLAGS += -DAR_ATS_USE_CUTILS
endif

if USE_DUMMY_DIAG
AM_CFLAGS += -DATS_USES_DUMMY_DIAG \
             -I ${srcdir}/transports/diag/linux/inc
else
AM_CFLAGS += @DIAG_CFLAGS@
AM_CPPFLAGS += -DFEATURE_LE_DIAG
requiredlibs += @DIAG_LIBS@
libats_la_LIBADD += @DIAG_LIBS@
endif

libats_la_SOURCES = $(ats_c_sources)
libats_la_CFLAGS = $(AM_CFLAGS)
libats_la_CPPFLAGS = $(AM_CPPFLAGS)
libats_la_LDFLAGS = -shared -version-number @LT_VERSION_NUMBER@
