cmake_minimum_required(VERSION 3.0.0)
project(qltime_tests VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Catch2 CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(cpp_rutils CONFIG REQUIRED)
find_package(blpapi)

if (${blpapi_FOUND})
set(blpapi blpapi)
message ("using blpapi  ${blpapi}")
add_definitions(-DUSING_BLPAPI)
endif()

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

target_link_libraries(qltime_tests Catch2::Catch2 fmt::fmt cpp_rutils::cpp_rutils ${blpapi})
set_property(TARGET qltime_tests APPEND PROPERTY LINK_FLAGS /DEBUG)



target_precompile_headers(qltime_tests  PUBLIC stdafx.h)
add_definitions(-DUSING_PCH -D_CRT_SECURE_NO_WARNINGS)

enable_testing()
add_test(NAME main COMMAND qltime_tests)
