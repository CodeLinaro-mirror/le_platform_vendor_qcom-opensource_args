cmake_minimum_required(VERSION 3.5)

project(ar-util)

set(ar_util_includes
    api/
    inc/
        )

include_directories(
    ${ar_util_includes}
    ../ar_osal/api
    ../spf/api/apm
    ../spf/api/ar_utils)

set(ar_util_SOURCE
    src/ar_util_data_log.c
    src/ar_util_list.c
    src/ar_util_err_detection.c)

add_library(ar-util SHARED ${ar_util_SOURCE})

install(TARGETS ar-util LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}")
install(DIRECTORY ${ar_util_includes} DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

##add_subdirectory(test)
