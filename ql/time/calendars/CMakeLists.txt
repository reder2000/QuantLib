cmake_minimum_required(VERSION 3.0.0)
project(calendar VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

FILE(GLOB sources *.cpp *.hpp)
add_library(calendar SHARED ${sources})

target_precompile_headers(calendar  PUBLIC    stdafx.h)
