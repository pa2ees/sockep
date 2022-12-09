cmake_minimum_required(VERSION 3.21)

add_subdirectory(simpleLogger)

if (${PROJECT_IS_TOP_LEVEL})
  add_subdirectory(pybind11)
endif()

