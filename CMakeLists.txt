cmake_minimum_required(VERSION 3.21)

project(sockep CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(SOCKEP sockep)
add_library(${SOCKEP})
add_library(sockep::sockep ALIAS ${SOCKEP})

add_subdirectory(lib)

target_link_libraries(${SOCKEP} PRIVATE pthread)

target_include_directories(${SOCKEP} PUBLIC include)
target_include_directories(${SOCKEP} PRIVATE include/sockep)

add_subdirectory(src)

if (PROJECT_IS_TOP_LEVEL) # New in CMAKE 3.21
  add_subdirectory(examples)
  add_subdirectory(test)
  
  add_subdirectory(python)
endif()

