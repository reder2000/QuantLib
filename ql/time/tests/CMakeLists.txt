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

find_vcpkg_install_missing(Catch2)
find_vcpkg_install_missing(fmt)

if (WIN32)
    FetchContent_Declare(
      blpapi
      URL "https://bcms.bloomberg.com/BLPAPI-Generic/blpapi_cpp_3.16.1.1-windows.zip"
    )
else () # assuming linux
    FetchContent_Declare(
      blpapi
      URL "https://bcms.bloomberg.com/BLPAPI-Generic/blpapi_cpp_3.16.1.1-linux.tar.gz"
    )
endif()
FetchContent_MakeAvailable(blpapi)
find_package(blpapi PATHS "${blpapi_SOURCE_DIR}/cmake" REQUIRED)
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
