cmake_minimum_required(VERSION 3.19.0)
project("ql_time" VERSION 1.0.1
             DESCRIPTION "A header only c++ library that interface to QuantLib  "
             HOMEPAGE_URL "https://github.com/reder2000/QuantLib")


add_subdirectory(tests)
enable_testing()

include(GNUInstallDirs)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

find_package(cpp_rutils REQUIRED)

FILE(GLOB ql_time_sources CONFIGURE_DEPENDS *.hpp *.h *.cpp)
FILE(GLOB ql_time_calendars_sources CONFIGURE_DEPENDS calendars/*.hpp calendars/*.h calendars/*.cpp)
FILE(GLOB ql_time_daycounters_sources CONFIGURE_DEPENDS daycounters/*.hpp daycounters/*.h daycounters/*.cpp)


add_library(ql_time INTERFACE ${ql_time_sources} ${ql_time_calendars_sources} ${ql_time_daycounters_sources})
add_library(ql_time::ql_time ALIAS ql_time)

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

install(FILES ${ql_time_sources} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/ql/time)
install(FILES ${ql_time_calendars_sources} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/ql/time/calendars)
install(FILES ${ql_time_daycounters_sources} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/ql/time/daycounters)

