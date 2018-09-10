project(hypercube-python)
cmake_minimum_required(VERSION 3.0)

set(CMAKE_INCLUDE_CURRENT_DIR TRUE)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../lib")
find_package(pybind11)  


if (pybind11_FOUND)
  MESSAGE("Found pybind11")
  include_directories(${PYTHON_INCLUDE_DIR})
  include_directories(${pybind11_INCLUDE_DIR})
  pybind11_add_module(pyHypercube MODULE pyHyper.cpp)
  target_link_libraries(pyHypercube PRIVATE  hypercube)

  install( TARGETS pyHypercube DESTINATION lib/python)
  install(FILES Hypercube.py DESTINATION lib/python)
else(pybind11_FOUND)
  MESSAGE("DID not find pybind11")
endif(pybind11_FOUND)
