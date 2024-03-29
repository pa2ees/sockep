cmake_minimum_required(VERSION 3.13)

add_executable(unixdgram_server EXCLUDE_FROM_ALL)
add_executable(unixdgram_client EXCLUDE_FROM_ALL)
add_executable(unixstream_server EXCLUDE_FROM_ALL)
add_executable(unixstream_client EXCLUDE_FROM_ALL)
add_executable(tcp_server EXCLUDE_FROM_ALL)
add_executable(tcp_client EXCLUDE_FROM_ALL)
add_executable(udp_server EXCLUDE_FROM_ALL)
add_executable(udp_client EXCLUDE_FROM_ALL)

add_custom_target(examples
  DEPENDS unixdgram_server unixdgram_client unixstream_server unixstream_client tcp_server tcp_client udp_server udp_client
  )


target_link_libraries(unixdgram_server PRIVATE ${SOCKEP})
target_link_libraries(unixdgram_client PRIVATE ${SOCKEP})
target_link_libraries(unixstream_server PRIVATE ${SOCKEP})
target_link_libraries(unixstream_client PRIVATE ${SOCKEP})
target_link_libraries(tcp_server PRIVATE ${SOCKEP})
target_link_libraries(tcp_client PRIVATE ${SOCKEP})
target_link_libraries(udp_server PRIVATE ${SOCKEP})
target_link_libraries(udp_client PRIVATE ${SOCKEP})


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

target_sources(tcp_server PRIVATE
  tcp_server.cpp
  )

target_sources(tcp_client PRIVATE
  tcp_client.cpp
  )

target_sources(udp_server PRIVATE
  udp_server.cpp
  )

target_sources(udp_client PRIVATE
  udp_client.cpp
  )
