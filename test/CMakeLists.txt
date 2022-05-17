cmake_minimum_required(VERSION 3.13)

add_executable(unixdgram_test EXCLUDE_FROM_ALL)
add_executable(unixstream_test EXCLUDE_FROM_ALL)

add_custom_target(tests
  DEPENDS unixdgram_test unixstream_test
  )


target_link_libraries(unixdgram_test PRIVATE sockep)
target_link_libraries(unixstream_test PRIVATE sockep)

target_sources(unixdgram_test PRIVATE
unixdgram_test.cpp
)

target_sources(unixstream_test PRIVATE
unixstream_test.cpp
)
