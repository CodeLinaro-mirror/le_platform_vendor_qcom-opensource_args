cmake_minimum_required(VERSION 3.5)

project(ar-gpr)

set(gpr_includes
    api/
    api/private/
    core/inc/
    core/inc/ar_utils/generic/
    core/src/
    datalinks/gpr_ly/inc/
    ext/dynamic_allocation/inc/
    ext/logging/inc/)

include_directories(${gpr_includes})

set(gpr_SOURCE
    core/src/gpr_drv.c
    core/src/gpr_drv_island.c
    core/src/gpr_list.c
    core/src/gpr_list_island.c
    core/src/gpr_main.c
    core/src/gpr_memq.c
    core/src/gpr_memq_island.c
    core/src/hash_based/gpr_session.c
    core/src/hash_based/gpr_session_island.c
    ext/dynamic_allocation/src/gpr_dynamic_allocation.c
    ext/logging/src/gpr_log_generic.c
    ext/logging/stub_src/gpr_log_diag_stub.c)

if(DEFINED PLATFORM_LY)
    list(APPEND gpr_SOURCE
        platform/ly/gpr_init_ly_wrapper.c
        datalinks/gpr_ly/src/gpr_glink.c
        datalinks/gpr_ly/src/target/cmn/glink_port_info_cmn.c
    )
endif()

add_definitions(-DSESSION_ARRAY_SIZE=200)
add_definitions(-Dstrlcat=g_strlcat)

add_library(ar-gpr SHARED ${gpr_SOURCE})
target_link_libraries(ar-gpr PUBLIC ar-osal glink_client)

install(TARGETS ar-gpr LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}")
install(DIRECTORY ${gpr_includes} DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")


