cmake_minimum_required(VERSION 3.13)

add_executable(unixdgram_server EXCLUDE_FROM_ALL)
add_executable(unixdgram_client EXCLUDE_FROM_ALL)
add_executable(unixstream_server EXCLUDE_FROM_ALL)
add_executable(unixstream_client EXCLUDE_FROM_ALL)

add_custom_target(examples
  DEPENDS unixdgram_server unixdgram_client unixstream_server unixstream_client
  )


target_link_libraries(unixdgram_server PRIVATE sockep)
target_link_libraries(unixdgram_client PRIVATE sockep)
target_link_libraries(unixstream_server PRIVATE sockep)
target_link_libraries(unixstream_client PRIVATE sockep)

target_sources(unixdgram_server PRIVATE
unixdgram_server.cpp
)

target_sources(unixdgram_client PRIVATE
unixdgram_client.cpp
)

target_sources(unixstream_server PRIVATE
unixstream_server.cpp
)

target_sources(unixstream_client PRIVATE
unixstream_client.cpp
)
