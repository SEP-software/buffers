cmake_minimum_required(VERSION 2.8)
project(hypercube)

set(SOURCES axis.cpp hypercube.cpp ioTypes.cc
 
)
set(INCLUDES axis.h hypercube.h ioTypes.h
)


add_library(hypercube ${SOURCES} ${INCLUDES})
install(FILES ${INCLUDES} DESTINATION include)
install(TARGETS hypercube DESTINATION lib EXPORT hypercubeExports)
