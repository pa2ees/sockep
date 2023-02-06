cmake_minimum_required(VERSION 3.21)

project(simpleLogger CXX)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_library(simpleLogger)
add_library(simpleLogger::simpleLogger ALIAS simpleLogger)

if (DEFINED SIMPLE_LOGGER_LEVEL)
  target_compile_definitions(simpleLogger PRIVATE SIMPLE_LOGGER_LEVEL=${SIMPLE_LOGGER_LEVEL})
endif()

if (DEFINED SIMPLE_LOGGER_OSTREAM)
  target_compile_definitions(simpleLogger PRIVATE SIMPLE_LOGGER_OSTREAM=${SIMPLE_LOGGER_OSTREAM})
endif()

target_include_directories(simpleLogger
  PUBLIC include
  PRIVATE include/simpleLogger
)

add_subdirectory(src)


