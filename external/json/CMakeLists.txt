project(json)
cmake_minimum_required(VERSION 2.8)
set(CMAKE_INCLUDE_CURRENT_DIR TRUE)


set(CMAKE_POSITION_INDEPENDENT_CODE True)
# configure a header file to pass some of the CMake settings
find_package( PythonInterp 3 REQUIRED )



set(json_MAJOR_VERSION 0)
set(json_MINOR_VERSION 2)
set(json_VERSION_STRING    ${json_MAJOR_VERSION}.${json_MINOR_VERSION})



 
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

 SET(json_DIRNAME_cmake     "cmake")

 SET(json_DIRNAME_lib     "lib")
 SET(json_DIRNAME_include "include")
INCLUDE(CMakePackageConfigHelpers)
  CONFIGURE_PACKAGE_CONFIG_FILE(
  "${CMAKE_CURRENT_SOURCE_DIR}/jsonConfig.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/jsonConfig.cmake"
  INSTALL_DESTINATION
    ${json_DIRNAME_cmake}
  PATH_VARS
    json_DIRNAME_include
    json_DIRNAME_lib
    json_DIRNAME_cmake
  )


  
  INSTALL(
  FILES
    ${CMAKE_CURRENT_BINARY_DIR}/jsonConfig.cmake
  DESTINATION
    cmake
  )


  INSTALL(
  EXPORT
   jsonExports
  DESTINATION
    ${json_DIRNAME_cmake}
  )
  

set(SOURCES jsoncpp.cpp

)
set(INCLUDES json.h
)

add_library(jsonCpp ${SOURCES} ${INCLUDES})
install(FILES ${INCLUDES} DESTINATION include)
install(TARGETS jsonCpp DESTINATION lib EXPORT jsonExports)

include(InstallRequiredSystemLibraries)
set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE")
set(CPACK_PACKAGE_VERSION_MAJOR "${json_MAJOR_VERSION}")
set(CPACK_PACKAGE_VERSION_MINOR "${json_MINOR_VERSION}")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_PACKAGE_NAME "json")
set(CPACK_PACKAGE_CONTACT "Robert Clapp")
include(CPack)
