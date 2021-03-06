cmake_minimum_required(VERSION 3.0.0)
project(qltime_tests VERSION 0.1.0)

message("Configuring ql_time/tests ...")

list(APPEND CMAKE_MODULE_PATH "${rr_cmake_SOURCE_DIR}") 

include(rr_cmake/FindVcpkInstall)
include(rr_cmake/cxx20)

find_vcpkg_install_missing(Catch2)
find_vcpkg_install_missing(fmt)

include(rr_cmake/blpapi)
find_blpapi()
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

include(CTest)
#enable_testing()

add_executable(qltime_tests ${srcs} ${headers})

target_link_libraries(qltime_tests Catch2::Catch2 fmt::fmt cpp_rutils::cpp_rutils blpapi)

target_precompile_headers(qltime_tests  PUBLIC stdafx.h)
add_definitions(-DUSING_PCH -D_CRT_SECURE_NO_WARNINGS)

enable_testing()
add_test(NAME main COMMAND qltime_tests)

message("... done ql_time/tests Configuring")