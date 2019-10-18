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



include_directories(
  "${CMAKE_CURRENT_SOURCE_DIR}/../external/googletest/googletest/include")
if(BUILD_ZFP)
  include_directories(${ZFP_INCLUDE_DIR})
endif(BUILD_ZFP)

# cmake_policy(SET CMP0083 NEW)
if(APPLE)
  add_definitions(-DGTEST_USE_OWN_TR1_TUPLE)
  add_definitions(-D__GLIBCXX__)
endif(APPLE)

set(GTEST
    ../external/googletest/googlemock/gtest/libgtest.a
    ../external/googletest/googlemock/gtest/libgtest_main.a
    ../external/googletest/googlemock/gtest/libgtest.a)

if(BUILD_TEST)
add_executable(storeTest storeTest.cc)
target_link_libraries(storeTest buffers gtest gtest_main gtest)
add_test(NAME store COMMAND storeTest)


add_executable(testBlock testBlock.cc)
target_link_libraries(testBlock buffers gtest gtest_main gtest)
add_test(NAME block COMMAND testBlock)


add_executable(blockTest blockTest.cc)
target_link_libraries(blockTest buffers gtest gtest_main gtest jsonCpp)
add_test(NAME blocktest COMMAND blockTest)

add_executable(fileBufferTest fileBufferTest.cc)
target_link_libraries(fileBufferTest
                      buffers
                      gtest_main
                      jsonCpp
                      hypercube)
add_test(NAME filebuffer COMMAND fileBufferTest)

if(BUILD_ZFP)
  add_executable(ZfpCompressTest ZfpCompressTest.cc)
  target_link_libraries(ZfpCompressTest buffers gtest gtest_main gtest jsonCpp)
  add_test(NAME zfp COMMAND ZfpCompressTest)
endif(BUILD_ZFP)

if(BUILD_GCP)
  add_executable(GCPtest GCPtest.cc)
  target_link_libraries(GCPtest
                        buffers
                        gtest_main
                        jsonCpp
                        storage_client
                        tbb
                        ${TCMALLOC_LIB}
                        ${PROFILER_LIB})
  add_test(NAME gcp COMMAND GCPtest)
else(BUILD_GCP)

endif(BUILD_GCP)

add_executable(fileBuffersTest fileBuffersTest.cc)
target_link_libraries(fileBuffersTest
                      buffers
                      gtest_main
                      tbb
                      jsonCpp
                      hypercube)
add_test(AllStoreTests fileBuffersTest)

endif(BUILD_TEST)
