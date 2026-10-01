cmake_minimum_required(VERSION 3.5)

project(ar-osal)

option(AR_OSAL_AUDIO_LOG_UTIL_BRIDGE
    "Route ARGS logs through audio-log-util" OFF)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -I/${CMAKE_INSTALL_INCLUDEDIR}/diag_lsm/include")

set(ar_osal_includes
    api/)

include_directories(${ar_osal_includes})
include_directories(./platform/ly/inc/)

set(ar_osal_SOURCE
    ./src/linux/ar_osal_heap.c
    ./src/linux/ar_osal_log.c
    ./src/linux/ar_osal_mem_op.c
    ./src/linux/ar_osal_mutex.c
    ./src/linux/ar_osal_signal.c
    ./src/linux/ar_osal_sleep.c
    ./src/linux/ar_osal_string.c
    ./src/linux/ar_osal_thread.c
    ./src/linux/ar_osal_timer.c
    ./src/linux/ar_osal_stub_log_pkt_op.c
    ./src/linux/ar_osal_file_io.c
    ./src/linux/qcom/dyn_pd/ar_osal_dyn_pd.c)

if(DEFINED PLATFORM_LY)
    list(APPEND ar_osal_SOURCE
        ./platform/ly/src/ar_osal_shmem.c
        ./platform/ly/src/ar_osal_servreg.c
        ./platform/ly/src/ar_osal_boot.c)
endif()

add_definitions(-DLINUX_ENABLED)
add_definitions(-Dstrlcpy=g_strlcpy)
add_definitions(-Dstrlcat=g_strlcat)
add_definitions(-D__unused=__attribute__\(\(__unused__\)\))
add_definitions(-D__LINUX__)
add_definitions(-DAR_OSAL_USE_SYSLOG)

if(DEFINED PLATFORM_LY)
    add_definitions(-DPLATFORM_LY)
    add_definitions(-DAR_OSAL_HAS_BOOT_STATE_PROVIDER)
endif()

add_library(ar-osal SHARED ${ar_osal_SOURCE})
target_link_libraries(ar-osal PUBLIC audio_utils pthread glib-2.0 cutils kiumd diag-lsm ssr-client)
if(AR_OSAL_AUDIO_LOG_UTIL_BRIDGE)
    target_compile_definitions(ar-osal PRIVATE AR_OSAL_AUDIO_LOG_UTIL_BRIDGE)
    target_link_libraries(ar-osal PUBLIC audio_log_util)
endif()

install(TARGETS ar-osal LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}")
install(DIRECTORY ${ar_osal_includes} DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")
