cmake_minimum_required(VERSION 3.13)

add_executable(unixdgram_test EXCLUDE_FROM_ALL)
add_executable(unixstream_test EXCLUDE_FROM_ALL)
add_executable(udp_test EXCLUDE_FROM_ALL)
add_executable(tcp_test EXCLUDE_FROM_ALL)

add_custom_target(tests
  DEPENDS unixdgram_test unixstream_test udp_test tcp_test
  )


target_link_libraries(unixdgram_test PRIVATE sockep)
target_link_libraries(unixstream_test PRIVATE sockep)
target_link_libraries(udp_test PRIVATE sockep)
target_link_libraries(tcp_test PRIVATE sockep)

target_sources(unixdgram_test PRIVATE
unixdgram_test.cpp
)

target_sources(unixstream_test PRIVATE
unixstream_test.cpp
)

target_sources(udp_test PRIVATE
udp_test.cpp
)

target_sources(tcp_test PRIVATE
tcp_test.cpp
)
