cmake_minimum_required(VERSION 2.8)
project(hypercube)

set(SOURCES buffer.cpp compress.cpp buffers.cpp blocking.cpp
 
)
set(INCLUDES store.h buffer.h compress.h
)


add_library(buffers ${SOURCES} ${INCLUDES})
install(FILES ${INCLUDES} DESTINATION include)
install(TARGETS buffers DESTINATION lib EXPORT bufferExports)
