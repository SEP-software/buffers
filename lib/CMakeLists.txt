cmake_minimum_required(VERSION 2.8)
project(buffers-lib)
INCLUDE_DIRECTORIES(${ZFP_INCLUDE_DIR})
set(SOURCES store.cpp buffer.cpp compress.cpp  blocking.cpp nocompress.cpp compressTypes.cpp
 memoryAll.cpp
 zfpCompress.cpp
 buffers.cpp

)
set(INCLUDES store.h buffer.h compress.h compressTypes.h nocompress.h  memoryUsage.h  memoryAll.h
zfpCompress.h
)


add_library(buffers ${SOURCES} ${INCLUDES})
TARGET_LINK_LIBRARIES(buffers  hypercube ${ZFP_LIBRARIES}  )

install(FILES ${INCLUDES} DESTINATION include)
install(TARGETS buffers DESTINATION lib EXPORT bufferExports)
