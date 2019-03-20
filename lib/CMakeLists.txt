cmake_minimum_required(VERSION 2.8)
project(buffers-lib)
include_directories(${ZFP_INCLUDE_DIR})
include_directories(${TBB_INCLUDE_DIRS})
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
endif(USE_ZFP)

message("XXXX " ${USE_GCP} ${BUILD_GCP})
if(USE_GCP)
  set(SOURCES ${SOURCES} gcpBuffer.cpp gcpBuffers.cpp)
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
target_link_libraries(buffers jsonCpp hypercube ${ZFP_LIBRARIES})

if(NOT SUPPORT_LIBRARIES_BUILD)
  install(FILES ${INCLUDES} DESTINATION include)
  install(TARGETS buffers DESTINATION lib EXPORT bufferExports)
endif(NOT SUPPORT_LIBRARIES_BUILD)
