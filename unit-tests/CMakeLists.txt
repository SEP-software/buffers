cmake_minimum_required(VERSION 3.1)
project(buffers-utest)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../lib")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../external/googletest/googletest/include")
if(BUILD_ZFP)
INCLUDE_DIRECTORIES(${ZFP_INCLUDE_DIR})
endif(BUILD_ZFP)

#cmake_policy(SET CMP0083 NEW)
  if (APPLE)
    add_definitions(-DGTEST_USE_OWN_TR1_TUPLE)
    add_definitions(-D__GLIBCXX__)
  endif (APPLE)

 set(GTEST ../external/googletest/googlemock/gtest/libgtest.a  ../external/googletest/googlemock/gtest/libgtest_main.a  ../external/googletest/googlemock/gtest/libgtest.a )

add_executable( storeTest storeTest.cc)
target_link_libraries(storeTest    buffers gtest gtest_main gtest )
add_test(NAME store COMMAND  storeTest)


add_executable( blockTest blockTest.cc)
target_link_libraries(blockTest    buffers gtest gtest_main gtest jsonCpp)
add_test(NAME blocktest COMMAND  blockTest)

add_executable( fileBufferTest fileBufferTest.cc)
target_link_libraries(fileBufferTest    buffers gtest gtest_main gtest jsonCpp hypercube)
add_test(NAME filebuffer COMMAND fileBufferTest)


if(BUILD_ZFP)
add_executable( ZfpCompressTest ZfpCompressTest.cc)
target_link_libraries(ZfpCompressTest    buffers gtest gtest_main gtest jsonCpp)
add_test(NAME zfp COMMAND ZfpCompressTest)
ENDIF(BUILD_ZFP)

if(BUILD_GCP)
add_executable( GCPtest GCPtest.cc)
target_link_libraries(GCPtest    buffers gtest gtest_main gtest jsonCpp storage_client tbb)
add_test(NAME gcp COMMAND   GCPtest)
else(BUILD_GCP)
endif(BUILD_GCP)



add_executable(fileBuffersTest fileBuffersTest.cc)
target_link_libraries(fileBuffersTest    buffers gtest gtest_main gtest  tbb jsonCpp hypercube)
add_test(AllStoreTests fileBuffersTest)

