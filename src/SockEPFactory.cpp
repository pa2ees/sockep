#include "SockEPFactory.h"

#include "server/IServerSockEP.h"
#include "client/IClientSockEP.h"
// #include "SockEP.h"

#include "server/UnixDgramServerSockEP.h"
#include "server/UnixStreamServerSockEP.h"
#include "client/UnixDgramClientSockEP.h"
#include "client/UnixStreamClientSockEP.h"
#include "server/TcpServerSockEP.h"
#include "client/TcpClientSockEP.h"
#include "server/UdpServerSockEP.h"
#include "client/UdpClientSockEP.h"


using namespace sockep;

std::unique_ptr<IServerSockEP> SockEPFactory::createUnixDgramServerSockEP(std::string bindPath, std::function<void(int, const char*, size_t)> callback)
{
    return std::unique_ptr<UnixDgramServerSockEP> (new UnixDgramServerSockEP(bindPath, callback));
}

std::unique_ptr<IClientSockEP> SockEPFactory::createUnixDgramClientSockEP(std::string bindPath, std::string serverPath)
{
    return std::unique_ptr<UnixDgramClientSockEP> (new UnixDgramClientSockEP(bindPath, serverPath));
}

std::unique_ptr<IServerSockEP> SockEPFactory::createUnixStreamServerSockEP(std::string bindPath, std::function<void(int, const char*, size_t)> callback)
{
    return std::unique_ptr<UnixStreamServerSockEP> (new UnixStreamServerSockEP(bindPath, callback));
}

std::unique_ptr<IClientSockEP> SockEPFactory::createUnixStreamClientSockEP(std::string bindPath, std::string serverPath)
{
    return std::unique_ptr<UnixStreamClientSockEP> (new UnixStreamClientSockEP(bindPath, serverPath));
}

std::unique_ptr<IServerSockEP> SockEPFactory::createTcpServerSockEP(std::string ipaddr, int port, std::function<void(int, const char*, size_t)> callback)
{
    return std::unique_ptr<TcpServerSockEP> (new TcpServerSockEP(ipaddr, port, callback));
}

std::unique_ptr<IClientSockEP> SockEPFactory::createTcpClientSockEP(std::string serverIpaddr, int port)
{
    return std::unique_ptr<TcpClientSockEP> (new TcpClientSockEP(serverIpaddr, port));
}

std::unique_ptr<IServerSockEP> SockEPFactory::createUdpServerSockEP(std::string ipaddr, int port, std::function<void(int, const char*, size_t)> callback)
{
    return std::unique_ptr<UdpServerSockEP> (new UdpServerSockEP(ipaddr, port, callback));
}

std::unique_ptr<IClientSockEP> SockEPFactory::createUdpClientSockEP(std::string serverIpaddr, int port)
{
    return std::unique_ptr<UdpClientSockEP> (new UdpClientSockEP(serverIpaddr, port));
}


