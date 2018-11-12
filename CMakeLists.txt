cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

option(BUILD_ZFP "Build with ZFP support" ON)
if(NOT SUBPACKAGE)
  option(BUILD_TESTS "Build with test support" OFF)
  option(INSTALL_CMAKE_ENV "Install cmake build environment" ON)
else(NOT SUBPACKAGE)
  option(BUILD_TESTS "Build with test support" ON)
endif(NOT SUBPACKAGE)
set(SUBPACKAGE true)

if(BUILD_ZFP)
  set(USE_ZFP true)
endif(BUILD_ZFP)

project(buffer LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE True)
set(CMAKE_INCLUDE_CURRENT_DIR TRUE)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

enable_testing()

# configure a header file to pass some of the CMake settings
find_package(TBB REQUIRED)

set(buffer_MAJOR_VERSION 0)
set(buffer_MINOR_VERSION 2)
set(buffer_VERSION_STRING ${buffer_MAJOR_VERSION}.${buffer_MINOR_VERSION})

include_directories("${PROJECT_BINARY_DIR}")
include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)

configure_file("${PROJECT_SOURCE_DIR}/buffersConfig.h.in"
               "${PROJECT_BINARY_DIR}/buffersConfig.h")

if(NOT TARGET hypercube)
  execute_process(COMMAND git submodule update --init --recursive --
                          external/hypercube
                  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
  add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/hypercube)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}/external/hypercube/lib)
endif(NOT TARGET hypercube)

if(BUILD_TESTS)
execute_process(COMMAND git submodule update --init --recursive --
                        external/googletest
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
add_subdirectory(external/googletest)
endif(BUILD_TESTS)


if(NOT TARGET jsonCpp)
  execute_process(COMMAND git submodule update --init --recursive --
                          external/json
                  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
  add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/json)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}/external/json)
endif(NOT TARGET jsonCpp)

if(BUILD_ZFP)
  externalproject_add(ZFP_INSTALL
                      CMAKE_ARGS
                      -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
                      -DCMAKE_INSTALL_LIBDIR=lib
                      -DZFP_WITH_OPENMP=false
                      GIT_REPOSITORY
                      "https://github.com/LLNL/zfp"
                      INSTALL_DIR
                      ${CMAKE_INSTALL_PREFIX})

  set(ZFP_INCLUDE_DIR ${CMAKE_INSTALL_PREFIX}/include)

  set(
    ZFP_LIBRARIES
    ${CMAKE_INSTALL_PREFIX}/lib/${CMAKE_SHARED_LIBRARY_PREFIX}zfp${CMAKE_SHARED_LIBRARY_SUFFIX}
    )
endif(BUILD_ZFP)
set(buffer_DIRNAME_cmake "cmake")
set(buffer_DIRNAME_lib "lib")
set(buffer_DIRNAME_include "include")
include(CMakePackageConfigHelpers)
if(NOT SUPPORT_LIBRARIES_BUILD)
configure_package_config_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/bufferConfig.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/bufferConfig.cmake"
  INSTALL_DESTINATION
  ${buffer_DIRNAME_cmake}
  PATH_VARS
  buffer_DIRNAME_include
  buffer_DIRNAME_lib
  buffer_DIRNAME_cmake)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/bufferConfig.cmake DESTINATION cmake)

install(EXPORT bufferExports DESTINATION ${buffer_DIRNAME_cmake})
endif(NOT SUPPORT_LIBRARIES_BUILD)

add_subdirectory(lib)

if(BUILD_TESTS)
  add_subdirectory(unit-tests)
endif(BUILD_TESTS)

include(InstallRequiredSystemLibraries)
set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE")
set(CPACK_PACKAGE_VERSION_MAJOR "${buffer_MAJOR_VERSION}")
set(CPACK_PACKAGE_VERSION_MINOR "${buffer_MINOR_VERSION}")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_PACKAGE_NAME "buffer")
set(CPACK_PACKAGE_CONTACT "Robert Clapp")
include(CPack)
