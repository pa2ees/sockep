cmake_minimum_required(VERSION 3.13)

pybind11_add_module(pysockep python_wrapper.cpp)

target_link_libraries(pysockep PRIVATE ${SOCKEP})

