cmake_minimum_required(VERSION 3.0.0)
project(ql_time VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

include_directories(c:/dev/src/blpapi_cpp_3.15.0.1/include)

find_package(boost REQUIRED preprocessor)
include_directories(${Boost_INCLUDE_DIRS})
find_package(fmt CONFIG REQUIRED)
find_package(blpapi REQUIRED)
include_directories(${BLPAPI_CPP_INCLUDE_DIR})

add_definitions(-DQL_USE_STD_SHARED_PTR)

#FILE(GLOB sources CONFIGURE_DEPENDS *.cpp *.hpp *.h calendars/*.cpp calendars/*.hpp daycounters/*.cpp daycounters/*.hpp)
FILE(GLOB sources CONFIGURE_DEPENDS *.hpp *.h calendars/*.hpp daycounters/actualactual.cpp daycounters/*.hpp)
add_library(ql_time SHARED ${sources})
target_link_libraries(ql_time PRIVATE fmt::fmt-header-only blpapi)

#target_precompile_headers(ql_time PUBLIC    stdafx.h)
