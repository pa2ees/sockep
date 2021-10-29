cmake_minimum_required(VERSION 3.13)

project(sockep CXX)

add_library(sockep)

target_link_libraries(sockep PRIVATE pthread)

target_include_directories(sockep PUBLIC include)
target_include_directories(sockep PRIVATE include/sockep)

add_subdirectory(src)

add_subdirectory(examples)
