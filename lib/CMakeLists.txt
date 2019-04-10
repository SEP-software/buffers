cmake_minimum_required(VERSION 3.7)
project(buffers-lib)
include_directories(${ZFP_INCLUDE_DIR})
include_directories(${TBB_INCLUDE_DIRS})
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(DEP_LIBS )
set(SOURCES
    store.cpp
    buffer.cpp
    compress.cpp
    blocking.cpp
    nocompress.cpp
    compressTypes.cpp
    memoryAll.cpp
    simpleMemoryLimit.cpp
    fileBuffer.cpp
    fileBuffers.cpp
    buffers.cpp)
if(USE_ZFP)
  set(SOURCES ${SOURCES} ZfpCompress.cpp)
  set(DEP_LIBS zfp)
endif(USE_ZFP)

if(USE_GCP)
  set(SOURCES ${SOURCES} 
	  gcpBuffer.cpp 
	  gcpBuffers.cpp
	  )
  set(DEP_LIBS  ${DEP_LIBS} storage_client)
endif(USE_GCP)

set(INCLUDES
    store.h
    buffer.h
    compress.h
    compressTypes.h
    nocompress.h
    memoryUsage.h
    memoryAll.h
    ZfpCompress.h
    simpleMemoryLimit.h
    fileBuffer.h
    gcpBuffer.h
    gcpBuffers.h
    fileBuffers.h)

add_library(buffers ${SOURCES} ${INCLUDES})

target_include_directories(
  buffers
  PUBLIC $<INSTALL_INTERFACE:include>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/../>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

 target_link_libraries(buffers hypercube jsonCpp ${DEP_LIBS} ${TBB_LIBRARIES})
if(NOT SUPPORT_LIBRARIES_BUILD)
  install(FILES ${INCLUDES} DESTINATION include)
  install(TARGETS buffers DESTINATION lib EXPORT buffersTargets)
endif(NOT SUPPORT_LIBRARIES_BUILD)


