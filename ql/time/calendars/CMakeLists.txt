cmake_minimum_required(VERSION 3.0.0)
project(calendar VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

include_directories(../../..)

include_directories(c:/dev/src/blpapi_cpp_3.15.0.1/include)

find_package(boost REQUIRED preprocessor)
include_directories(${Boost_INCLUDE_DIRS})
find_package(fmt CONFIG REQUIRED)

add_definitions(-DQL_USE_STD_SHARED_PTR)

FILE(GLOB sources CONFIGURE_DEPENDS *.cpp *.hpp)
add_library(calendar SHARED ${sources})
target_link_libraries(calendar PRIVATE fmt::fmt-header-only)

target_precompile_headers(calendar PUBLIC    stdafx.h)
