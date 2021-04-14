cmake_minimum_required(VERSION 3.0.0)
project(qltime_tests VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#find_package(cpp_rutils CONFIG REQUIRED)
#get_target_property(cpp_rutils_include cpp_rutils::cpp_rutils INTERFACE_INCLUDE_DIRECTORIES)
#list(APPEND CMAKE_MODULE_PATH "${cpp_rutils_include}/cpp_rutils/cmake")
list(APPEND CMAKE_MODULE_PATH "${cpp_rutils_SOURCE_DIR}/cmake")
include(FindVcpkInstall)
include_directories("${cpp_rutils_SOURCE_DIR}")

message("include_directories ${CMAKE_CURRENT_BINARY_DIR}")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")

find_vcpkg_install_missing(Catch2)
find_vcpkg_install_missing(fmt)

IF (DEFINED ENV{TMP})
    set (tmp_path "$ENV{TMP}/cmake")
ELSE ()
    IF (DEFINED ENV{TEMP})
        set (tmp_path "$ENV{TEMP}/cmake")
    ELSE ()
        set (tmp_path "/tmp/cmake")
    ENDIF()
ENDIF()
cmake_path(CONVERT ${tmp_path} TO_CMAKE_PATH_LIST tmp_path NORMALIZE)
IF(NOT EXISTS "${tmp_path}")
    message("creating ${tmp_path}")
    file(MAKE_DIRECTORY "${tmp_path}")
ELSE()
    message("NOT creating ${tmp_path}")
ENDIF()

if (WIN32)
    set(blpapi_archive "blpapi_cpp_3.16.1.1-windows.zip")
else ()
    set(blpapi_archive "blpapi_cpp_3.16.1.1-linux.tar.gz")
endif()

IF (NOT EXISTS "${tmp_path}/${blpapi_archive}")
    message("downloading blpapi")
    set(url_blpapi "https://bcms.bloomberg.com/BLPAPI-Generic/${blpapi_archive}")
     file(DOWNLOAD ${url_blpapi} "${tmp_path}/${blpapi_archive}")
ENDIF()

include(extract_archive)

SET(blpapi_srcdir "${CMAKE_BINARY_DIR}/_deps/blpapi-src")

IF (NOT EXISTS "${blpapi_srcdir}")

    extract_archive_remove_level_one("${blpapi_srcdir}" "${tmp_path}/${blpapi_archive}")


#    message("blpapi_srcdir ${blpapi_srcdir}")#$

#    include(ExternalProject)
#    ExternalProject_Add(blpapi_dowloaded
#    CONFIGURE_COMMAND ""
#    BUILD_COMMAND ""
#    SOURCE_DIR ${blpapi_srcdir}
#    EXCLUDE_FROM_ALL TRUE
#    )

ENDIF()

find_package(blpapi PATHS "${blpapi_srcdir}/cmake" REQUIRED)
add_definitions(-DUSING_BLPAPI)

file(GLOB headers CONFIGURE_DEPENDS ../date_like.hpp *.h)
file (GLOB srcs CONFIGURE_DEPENDS  
date_like.cpp
main.cpp
test_suite_calendars.cpp
test_suite_dates.cpp
test_suite_daycounters.cpp
test_suite_schedule.cpp
)

include_directories(../../..)

set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zi")

#include(CTest)
#enable_testing()

add_executable(qltime_tests ${srcs} ${headers})

target_link_libraries(qltime_tests Catch2::Catch2 fmt::fmt cpp_rutils::cpp_rutils blpapi)

target_precompile_headers(qltime_tests  PUBLIC stdafx.h)
add_definitions(-DUSING_PCH -D_CRT_SECURE_NO_WARNINGS)

enable_testing()
add_test(NAME main COMMAND qltime_tests)
