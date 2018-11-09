cmake_minimum_required(VERSION 3.1)
project(buffers-utest)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../lib")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../external/googletest/googletest/include")
ifdef(BUILD_ZFP)
INCLUDE_DIRECTORIES(${ZFP_INCLUDE_DIR})
endif(BUILD_ZFP)

  if (APPLE)
    add_definitions(-DGTEST_USE_OWN_TR1_TUPLE)
    add_definitions(-D__GLIBCXX__)
  endif (APPLE)

 set(GTEST ../external/googletest/googlemock/gtest/libgtest.a  ../external/googletest/googlemock/gtest/libgtest_main.a  ../external/googletest/googlemock/gtest/libgtest.a )

add_executable( storeTest storeTest.cc)
target_link_libraries(storeTest    buffers gtest gtest_main gtest )
add_test(AllStoreTests storeTest)


add_executable( blockTest blockTest.cc)
target_link_libraries(blockTest    buffers gtest gtest_main gtest jsonCpp)
add_test(AllStoreTests blockTest)

add_executable( bufferTest bufferTest.cc)
target_link_libraries(bufferTest    buffers gtest gtest_main gtest jsonCpp hypercube)
add_test(AllStoreTests bufferTest)


if(BUILD_ZFP)
add_executable( ZfpCompressTest ZfpCompressTest.cc)
target_link_libraries(ZfpCompressTest    buffers gtest gtest_main gtest jsonCpp)
add_test(AllStoreTests ZfpCompressTest)
ENDIF(BUILD_ZFP)



add_executable(buffersTest buffersTest.cc)
target_link_libraries(buffersTest    buffers gtest gtest_main gtest  tbb jsonCpp hypercube)
add_test(AllStoreTests buffersTest)

