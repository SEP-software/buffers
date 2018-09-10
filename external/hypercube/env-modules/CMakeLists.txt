cmake_minimum_required(VERSION 2.8)
project(hypercube-python)



  set(min  "${CMAKE_CURRENT_SOURCE_DIR}/hypercube.in")
  set(mout "${CMAKE_CURRENT_BINARY_DIR}/hypercube")
  configure_file (${min} ${mout})


install(FILES  ${mout}  DESTINATION share/modules)

