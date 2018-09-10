project(hypercube)
cmake_minimum_required(VERSION 2.8)
set(CMAKE_INCLUDE_CURRENT_DIR TRUE)


set(CMAKE_POSITION_INDEPENDENT_CODE True)
# configure a header file to pass some of the CMake settings
find_package( PythonInterp 3 REQUIRED )



set(hypercube_MAJOR_VERSION 0)
set(hypercube_MINOR_VERSION 2)
set(hypercube_VERSION_STRING    ${hypercube_MAJOR_VERSION}.${hypercube_MINOR_VERSION})



 
# add the binary tree to the search path for include files
# so that we will find TutorialConfig.h
include_directories("${PROJECT_BINARY_DIR}")


include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpic")
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
else()
        message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

enable_language(CXX)

 SET(hypercube_DIRNAME_cmake     "cmake")

 SET(hypercube_DIRNAME_lib     "lib")
 SET(hypercube_DIRNAME_include "include")
INCLUDE(CMakePackageConfigHelpers)
  CONFIGURE_PACKAGE_CONFIG_FILE(
  "${CMAKE_CURRENT_SOURCE_DIR}/hypercubeConfig.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/hypercubeConfig.cmake"
  INSTALL_DESTINATION
    ${hypercube_DIRNAME_cmake}
  PATH_VARS
    hypercube_DIRNAME_include
    hypercube_DIRNAME_lib
    hypercube_DIRNAME_cmake
  )


  
  INSTALL(
  FILES
    ${CMAKE_CURRENT_BINARY_DIR}/hypercubeConfig.cmake
  DESTINATION
    cmake
  )


  INSTALL(
  EXPORT
   hypercubeExports
  DESTINATION
    ${hypercube_DIRNAME_cmake}
  )
  
add_subdirectory(lib)
add_subdirectory(python)

add_subdirectory(env-modules)
message("CHECK 2HYPER "${HYPERCUBE_INTERNAL_INCLUDE_DIR})


include(InstallRequiredSystemLibraries)
set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE")
set(CPACK_PACKAGE_VERSION_MAJOR "${hypercube_MAJOR_VERSION}")
set(CPACK_PACKAGE_VERSION_MINOR "${hypercube_MINOR_VERSION}")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_PACKAGE_NAME "hypercube")
set(CPACK_PACKAGE_CONTACT "Robert Clapp")
include(CPack)
