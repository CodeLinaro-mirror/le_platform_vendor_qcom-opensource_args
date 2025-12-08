cmake_minimum_required(VERSION 3.5)

project(ar-ats)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -I/${CMAKE_INSTALL_INCLUDEDIR}/diag_lsm/include")

set(ats_includes
    adie/common/api/
    adie/common/inc/
    api/
    transports/common/api
    transports/diag/linux/actp/inc/
    transports/diag/linux/audtp/inc/
    transports/tcpip_server/api/
    fts/common/inc/
    inc/
    dls/common/inc/
    mcs/common/api/
    mcs/common/inc/
    online/common/inc/
    rtc/common/inc/
    transports/tcpip/linux/inc/)

include_directories(${ats_includes})

set(ats_SOURCE
    transports/diag/linux/actp/src/actp.c
    transports/diag/linux/audtp/src/audtp.c
    transports/common/src/ats_transport.c
    fts/common/src/ats_fts.c
    online/common/src/ats_online.c
    rtc/common/src/ats_rtc.c
    src/ats.c
    src/ats_command.c
    dls/common/src/ats_dls.c)

if(DEFINED PLATFORM_LY)
    list(APPEND ats_SOURCE
            mcs/ly/ats_mcs.c
            adie/ly/ats_adie_rtc.c
        )
    add_definitions(-DPLATFORM_LY)
endif()

add_definitions(-Dstrlcpy=g_strlcpy)
add_definitions(-Dstrlcat=g_strlcat)

add_library(ar-ats SHARED ${ats_SOURCE})

target_link_libraries(ar-ats PUBLIC dl acdb ar-osal ar-gsl diag-lsm)

install(TARGETS ar-ats LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}")
install(DIRECTORY ${ats_includes} DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")
