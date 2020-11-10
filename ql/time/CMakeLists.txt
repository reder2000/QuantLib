cmake_minimum_required(VERSION 3.0.0)
project("ql_time" VERSION 1.0.1
             DESCRIPTION "A header only c++ library that interface to QuantLib  "
             HOMEPAGE_URL "https://github.com/reder2000/QuantLib")

include(GNUInstallDirs)

#set(CMAKE_CXX_STANDARD 20)
#set(CMAKE_CXX_STANDARD_REQUIRED ON)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

include_directories(c:/dev/src/blpapi_cpp_3.15.0.1/include)

#find_package(boost REQUIRED preprocessor)
#include_directories(${Boost_INCLUDE_DIRS})
#find_package(fmt CONFIG REQUIRED)
#find_package(blpapi REQUIRED)
#include_directories(${BLPAPI_CPP_INCLUDE_DIR})
find_package(cpp_rutils REQUIRED)

#add_definitions(-DQL_USE_STD_SHARED_PTR)

#FILE(GLOB sources CONFIGURE_DEPENDS *.cpp *.hpp *.h calendars/*.cpp calendars/*.hpp daycounters/*.cpp daycounters/*.hpp)
#FILE(GLOB sources CONFIGURE_DEPENDS *.hpp *.h calendars/*.hpp daycounters/*.hpp daycounters/actualactual.cpp  calendars/argentina.cpp)
FILE(GLOB ql_time_sources CONFIGURE_DEPENDS *.hpp *.h)
FILE(GLOB ql_time_calendars_sources CONFIGURE_DEPENDS calendars/*.hpp calendars/*.h)
FILE(GLOB ql_time_daycounters_sources CONFIGURE_DEPENDS daycounters/*.hpp daycounters/*.h)


add_library(ql_time INTERFACE)
add_library(ql_time::ql_time ALIAS ql_time)

#add_library(ql_time SHARED ${sources})
#target_link_libraries(ql_time PRIVATE fmt::fmt-header-only blpapi)


#target_precompile_headers(ql_time PUBLIC    stdafx.h)
#target_precompile_headers(ql_time INTERFACE    stdafx.h)


target_link_libraries(ql_time INTERFACE cpp_rutils::cpp_rutils)

target_include_directories(
  ${PROJECT_NAME}
  INTERFACE $<BUILD_INTERFACE:${${PROJECT_NAME}_SOURCE_DIR}/ql/time/>
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_20)

install(TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}_Targets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

include(CMakePackageConfigHelpers)
write_basic_package_version_file("${PROJECT_NAME}ConfigVersion.cmake"
                                 VERSION ${PROJECT_VERSION}
                                 COMPATIBILITY SameMajorVersion)

configure_package_config_file(
  "${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in"
  "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
  INSTALL_DESTINATION
  ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake)

install(EXPORT ${PROJECT_NAME}_Targets
        FILE ${PROJECT_NAME}Targets.cmake
        NAMESPACE ${PROJECT_NAME}::
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake)


install(FILES "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
              "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake)

install(FILES ${ql_time_sources} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/ql_time)
install(FILES ${ql_time_calendars_sources} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/ql_time/calendars)
install(FILES ${ql_time_daycounters_sources} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/ql_time/daycounters)

#install(FILES ${sources} DESTINATION include)