cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

option(BUILD_ZFP "Build with ZFP support" ON)
option(BUILD_GCP "Build with GCP support" OFF)
if(NOT SUBPACKAGE)
  option(BUILD_TESTS "Build with test support" ON)
  option(INSTALL_CMAKE_ENV "Install cmake build environment" ON)
else(NOT SUBPACKAGE)
  option(BUILD_TESTS "Build with test support" OFF)
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



#if(BUILD_GCP)
#  externalproject_add(STORAGE_INSTALL
#                      CMAKE_ARGS
#                      -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
#                      -DCMAKE_INSTALL_LIBDIR=lib
#                      -DBUILD_TESTING=false
#                      -Bstorage_client
#                      GIT_REPOSITORY
#                      "https://github.com/GoogleCloudPlatform/google-cloud-cpp.git"
#                      PATCH_COMMAND git submodule update --init
#  BUILD_COMMAND ${CMAKE_COMMAND} -E echo "XXXStarting $<CONFIG> build"
#  COMMAND       ${CMAKE_COMMAND} --build <BINARY_DIR> --config $<CONFIG> --target storage_client -j 8
#  COMMAND       ${CMAKE_COMMAND} -E echo "$<CONFIG> build complete"
#                      INSTALL_DIR
#                      ${CMAKE_INSTALL_PREFIX})
#  set(GCP_INCLUDE_DIR ${CMAKE_INSTALL_PREFIX}/include)
#
#  set(
#    GCP_LIBRARIES
#    ${CMAKE_INSTALL_PREFIX}/lib/${CMAKE_SHARED_LIBRARY_PREFIX}zfp${CMAKE_SHARED_LIBRARY_SUFFIX}
#    )
#endif(BUILD_GCP)




if(BUILD_GCP)
  set(USE_GCP true)
  find_package(storage_client 0.3.0 REQUIRED )

 if(GOOGLE_STORAGE_ROOT)
    include_directories(${GOOGLE_STORAGE_ROOT}/include)
 else(GOOGLE_STORAGE_ROOT)
    MESSAGE(FATAL_ERROR "Must specitfy GOOGLE_STORAGE_ROOT")
 endif(GOOGLE_STORAGE_ROOT)
endif(BUILD_GCP)


set(buffer_DIRNAME_cmake "lib/cmake")
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

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/bufferConfig.cmake DESTINATION lib/cmake)
if(NOT INSTALL_CMAKE_ENV)
install(EXPORT bufferExports DESTINATION ${buffer_DIRNAME_cmake})
endif(NOT INSTALL_CMAKE_ENV)
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
