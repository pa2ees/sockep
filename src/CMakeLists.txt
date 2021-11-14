cmake_minimum_required(VERSION 3.13)

target_sources(sockep PRIVATE

SockEPFactory.cpp
server/ServerSockEP.cpp
server/UnixDgramServerSockEP.cpp
# client/ClientSockEP.cpp
client/UnixDgramClientSockEP.cpp
)
