cmake_minimum_required(VERSION 3.0.0)
project(qltime_tests VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Catch2 CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(cpp_rutils CONFIG REQUIRED)

file(GLOB headers ../date_like.hpp )
file (GLOB srcs CONFIGURE_DEPENDS *.cpp)

include_directories(../../..)

#include(CTest)
#enable_testing()

add_executable(qltime_tests ${srcs} ${headers})

target_link_libraries(qltime_tests Catch2::Catch2 fmt::fmt cpp_rutils::cpp_rutils)

target_precompile_headers(qltime_tests  PUBLIC stdafx.h)
add_definitions(-DUSING_PCH)

enable_testing()
add_test(NAME main COMMAND qltime_tests)
