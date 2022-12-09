cmake_minimum_required(VERSION 3.21)

project(simpleLogger CXX)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_library(simpleLogger INTERFACE)
add_library(simpleLogger::simpleLogger ALIAS simpleLogger)

if (DEFINED SIMPLE_LOGGER_LEVEL)
  target_compile_definitions(simpleLogger INTERFACE SIMPLE_LOGGER_LEVEL=${SIMPLE_LOGGER_LEVEL})
endif()

target_include_directories(simpleLogger INTERFACE
  include
  )


