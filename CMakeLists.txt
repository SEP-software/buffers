cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

option(BUILD_ZFP "Build with ZFP support" ON)
option(BUILD_GCP "Build with GCP support" OFF)
if(NOT SUBPACKAGE)
  option(BUILD_TEST "Build with test support" OFF)
  option(INSTALL_CMAKE_ENV "Install cmake build environment" ON)
  option(BUILD_DOC "Build documentation" OFF)
  option(BUILD_DEBUG_LIBS "Build gperftool libs" OFF)
  option(BUILD_PYTHON "Build python interfaces" ON)
endif(NOT SUBPACKAGE)

if(BUILD_ZFP)
  set(USE_ZFP true)
endif(BUILD_ZFP)

project(buffer LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_INCLUDE_CURRENT_DIR TRUE)

if(BUILD_GCP)
  set(USE_GCP true)
endif(BUILD_GCP)

if(BUILD_GCP)
	find_package(storage_client REQUIRED)
endif(BUILD_GCP)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

# configure a header file to pass some of the CMake settings
find_package(TBB REQUIRED)

set(buffer_MAJOR_VERSION 0)
set(buffer_MINOR_VERSION 2)
set(buffer_VERSION_STRING ${buffer_MAJOR_VERSION}.${buffer_MINOR_VERSION})

include_directories("${PROJECT_BINARY_DIR}")
include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)

configure_file("${PROJECT_SOURCE_DIR}/buffersConfig.h.in"
               "${PROJECT_BINARY_DIR}/buffersConfig.h")

if(NOT SUBPACKAGE)
  if(NOT TARGET hypercube)
    execute_process(COMMAND git submodule update --init --recursive --
                            external/hypercube
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
  endif(NOT TARGET hypercube)

  if(NOT TARGET jsonCpp)
    execute_process(COMMAND git submodule update --init --recursive --
                            external/json
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
  endif(NOT TARGET jsonCpp)

  if(BUILD_ZFP)
    execute_process(COMMAND git submodule update --init --recursive --
                            external/zfp
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
  endif(BUILD_ZFP)
endif(NOT SUBPACKAGE)
set(SUBPACKAGE true)

if(NOT TARGET hypercube)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/hypercube)
endif(NOT TARGET hypercube)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/json)

#if(BUILD_GCP AND NOT TARGET storage_client)
#	set(GOOGLE_CLOUD_CPP_ENABLE_FIRESTONE no)
#	set(GOOGLE_CLOUD_CPP_ENABLE_BIGTABLE no)
#  add_subdirectory(external/cloud)
#endif(BUILD_GCP AND NOT TARGET storage_client)

if(BUILD_ZFP AND NOT TARGET zfp)
  add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/zfp)
endif(BUILD_ZFP AND NOT TARGET zfp)

include(ExternalProject)

if(BUILD_DEBUG_LIBS)
  externalproject_add(
    gperftools_src
    PREFIX
    gperftools_src
    INSTALL_DIR
    ${CMAKE_INSTALL_PREFIX}
    URL
    https://github.com/gperftools/gperftools/releases/download/gperftools-2.7/gperftools-2.7.tar.gz
    CONFIGURE_COMMAND
    ./configure
    --enable-frame-pointers
    --prefix=<INSTALL_DIR>
    BUILD_IN_SOURCE
    1)

  externalproject_get_property(gperftools_src INSTALL_DIR)
  set(GPERF_LIBDIR ${INSTALL_DIR}/lib)
  set(PROFILER_LIB profiler)
  set(TCMALLOC_LIB tcmalloc)
  add_library(${PROFILER_LIB} UNKNOWN IMPORTED)
  add_library(${TCMALLOC_LIB} UNKNOWN IMPORTED)
  set_property(
    TARGET ${PROFILER_LIB}
    PROPERTY
      IMPORTED_LOCATION
      ${GPERF_LIBDIR}/${CMAKE_SHARED_LIBRARY_PREFIX}profiler${CMAKE_SHARED_LIBRARY_SUFFIX}
    )
  set_property(
    TARGET ${TCMALLOC_LIB}
    PROPERTY
      IMPORTED_LOCATION
      ${GPERF_LIBDIR}/${CMAKE_SHARED_LIBRARY_PREFIX}tcmalloc${CMAKE_SHARED_LIBRARY_SUFFIX}
    )
  add_dependencies(${PROFILER_LIB} gperftools_src)
  add_dependencies(${TCMALLOC_LIB} gperftools_src)
endif(BUILD_DEBUG_LIBS)

# set(buffer_DIRNAME_cmake "lib/cmake") set(buffer_DIRNAME_lib "lib")
# set(buffer_DIRNAME_include "include")
include(CMakePackageConfigHelpers)
if(NOT SUPPORT_LIBRARIES_BUILD)

  # configure_package_config_file(
  # "${CMAKE_CURRENT_SOURCE_DIR}/bufferConfig.cmake.in"
  # "${CMAKE_CURRENT_BINARY_DIR}/bufferConfig.cmake" INSTALL_DESTINATION
  # ${buffer_DIRNAME_cmake} PATH_VARS buffer_DIRNAME_include buffer_DIRNAME_lib
  # buffer_DIRNAME_cmake)

  # install(FILES ${CMAKE_CURRENT_BINARY_DIR}/bufferConfig.cmake DESTINATION
  # lib/cmake) if(NOT INSTALL_CMAKE_ENV) install(EXPORT bufferExports
  # DESTINATION ${buffer_DIRNAME_cmake}) endif(NOT INSTALL_CMAKE_ENV)
endif(NOT SUPPORT_LIBRARIES_BUILD)

add_subdirectory(lib)

# first we can indicate the documentation build as an option and set it to ON by
# default

# check if Doxygen is installed
if(BUILD_DOC)
  find_package(Doxygen)
  if(DOXYGEN_FOUND)
    # set input and output files
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/docs/Doxyfile)

    # request to configure the file
    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
    message("Doxygen build started")

    # note the option ALL which allows to build the docs together with the
    # application
    add_custom_target(doc_doxygen_buffers ALL
                      COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
                      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                      COMMENT "Generating API documentation with Doxygen"
                      VERBATIM)
  else(DOXYGEN_FOUND)
    message(
      "Doxygen need to be installed to generate the doxygen documentation")
  endif(DOXYGEN_FOUND)
endif(BUILD_DOC)

include(CMakePackageConfigHelpers)
configure_package_config_file(buffersConfig.cmake.in
                              "${CMAKE_CURRENT_BINARY_DIR}/buffersConfig.cmake"
                              INSTALL_DESTINATION
                              cmake/SEP/IO)

# install(EXPORT buffersTargets FILE buffersTargets.cmake NAMESPACE SEP::IO
# DESTINATION cmake/sep/io)

include(InstallRequiredSystemLibraries)
set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE")
set(CPACK_PACKAGE_VERSION_MAJOR "${buffer_MAJOR_VERSION}")
set(CPACK_PACKAGE_VERSION_MINOR "${buffer_MINOR_VERSION}")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_PACKAGE_NAME "buffer")
set(CPACK_PACKAGE_CONTACT "Robert Clapp")
include(CPack)
