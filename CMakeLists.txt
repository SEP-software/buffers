project(buffer)
cmake_minimum_required(VERSION 3.3)
set(CMAKE_INCLUDE_CURRENT_DIR TRUE)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

enable_testing()

# configure a header file to pass some of the CMake settings
find_package( PythonInterp 3 REQUIRED )
FIND_PACKAGE(TBB REQUIRED)


set(buffer_MAJOR_VERSION 0)
set(buffer_MINOR_VERSION 2)
set(buffer_VERSION_STRING    ${buffer_MAJOR_VERSION}.${buffer_MINOR_VERSION})



 
# add the binary tree to the search path for include files
# so that we will find TutorialConfig.h
include_directories("${PROJECT_BINARY_DIR}")
include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)

#FIND_PACKAGE(hypercube)
#if(hypercube_FOUND)
 # message("Found hypercube")
  #INCLUDE_DIRECTORIES(${hypercube_DIRNAME_include})
  #link_directories(hypercube_DIRNAME_lib)
#else(hypercube_FOUND)

    # we have a submodule setup for foo, assume it is under external/foo
    # now we need to clone this submodule
  if(NOT TARGET hypercube)
    execute_process(COMMAND git submodule update --init --recursive -- external/hypercube
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    message("before add subdirectory")
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/hypercube)
    INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/external/hypercube/lib)
  endif(NOT TARGET hypercube)


execute_process(COMMAND git submodule update --init --recursive -- external/googletest
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
add_subdirectory(external/googletest)


if(NOT TARGET jsonCpp)
    execute_process(COMMAND git submodule update --init --recursive -- external/json
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    message("before add subdirectory")
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/json)
    INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/external/json)
endif(NOT TARGET jsonCpp)



ExternalProject_Add (
       ZFP_INSTALL
       CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX} -DCMAKE_INSTALL_LIBDIR=lib
      GIT_REPOSITORY "https://github.com/LLNL/zfp"
            INSTALL_DIR  ${CMAKE_INSTALL_PREFIX})

   set(ZFP_INCLUDE_DIR ${CMAKE_INSTALL_PREFIX}/include)

     set(ZFP_LIBRARIES ${CMAKE_INSTALL_PREFIX}/lib/${CMAKE_SHARED_LIBRARY_PREFIX}zfp${CMAKE_SHARED_LIBRARY_SUFFIX})

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpic")
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
else()
        message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()


if(APPLE)
set(WITH_SZ false)
else(APPLE) 
set(WITH_SZ true)
add_subdirectory(sz)
endif(APPLE)




configure_file (
  "${PROJECT_SOURCE_DIR}/buffersConfig.h.in"
  "${PROJECT_BINARY_DIR}/buffersConfig.h"
  )

enable_language(CXX)

 SET(buffer_DIRNAME_cmake     "cmake")

 SET(buffer_DIRNAME_lib     "lib")
 SET(buffer_DIRNAME_include "include")
INCLUDE(CMakePackageConfigHelpers)
  CONFIGURE_PACKAGE_CONFIG_FILE(
  "${CMAKE_CURRENT_SOURCE_DIR}/bufferConfig.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/bufferConfig.cmake"
  INSTALL_DESTINATION
    ${buffer_DIRNAME_cmake}
  PATH_VARS
    buffer_DIRNAME_include
    buffer_DIRNAME_lib
    buffer_DIRNAME_cmake
  )


  INSTALL(
  FILES
    ${CMAKE_CURRENT_BINARY_DIR}/bufferConfig.cmake
  DESTINATION
    cmake
  )


  INSTALL(
  EXPORT
   bufferExports
  DESTINATION
    ${buffer_DIRNAME_cmake}
  )

add_subdirectory(lib)
  add_subdirectory(unit-tests)

#add_subdirectory(python)

#add_subdirectory(env-modules)


include(InstallRequiredSystemLibraries)
set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE")
set(CPACK_PACKAGE_VERSION_MAJOR "${buffer_MAJOR_VERSION}")
set(CPACK_PACKAGE_VERSION_MINOR "${buffer_MINOR_VERSION}")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_PACKAGE_NAME "buffer")
set(CPACK_PACKAGE_CONTACT "Robert Clapp")
include(CPack)
