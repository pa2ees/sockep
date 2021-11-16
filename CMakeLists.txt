cmake_minimum_required(VERSION 3.13)

project(sockep CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_library(sockep)

add_subdirectory(lib)

target_link_libraries(sockep PRIVATE pthread)

target_include_directories(sockep PUBLIC include)
target_include_directories(sockep PRIVATE include/sockep)

add_subdirectory(src)

add_subdirectory(examples)

add_subdirectory(python)

