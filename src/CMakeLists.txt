cmake_minimum_required(VERSION 3.13)

target_include_directories(${SOCKEP}
  PRIVATE ${CMAKE_CURRENT_LIST_DIR}
  )

target_sources(${SOCKEP} PRIVATE
  SockEPFactory.cpp
  server/ServerSockEP.cpp
  server/UnixDgramServerSockEP.cpp
  server/UnixStreamServerSockEP.cpp
  server/TcpServerSockEP.cpp
  server/UdpServerSockEP.cpp
  client/ClientSockEP.cpp
  client/UnixDgramClientSockEP.cpp
  client/UnixStreamClientSockEP.cpp
  client/TcpClientSockEP.cpp
  client/UdpClientSockEP.cpp
)
