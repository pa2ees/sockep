#pragma once

//#include "sockep/ISockEP.h"
#include "sockep/client/IClientSockEP.h"
#include "sockep/server/IServerSockEP.h"

#include <functional>
#include <memory>

namespace sockep
{
class SockEPFactory
{
private:
	SockEPFactory(){};
	~SockEPFactory(){};

public:
	static std::unique_ptr<IServerSockEP>
	createUnixDgramServerSockEP(std::string bindPath, std::function<void(int, const char *, size_t)> callback);
	static std::unique_ptr<IClientSockEP> createUnixDgramClientSockEP(std::string bindPath, std::string serverPath);
	static std::unique_ptr<IServerSockEP>
	createUnixStreamServerSockEP(std::string bindPath, std::function<void(int, const char *, size_t)> callback);
	static std::unique_ptr<IClientSockEP> createUnixStreamClientSockEP(std::string bindPath, std::string serverPath);
	static std::unique_ptr<IServerSockEP>
	createTcpServerSockEP(std::string ipaddr, int port, std::function<void(int, const char *, size_t)> callback);
	static std::unique_ptr<IClientSockEP> createTcpClientSockEP(std::string serverIpaddr, int port);
	static std::unique_ptr<IServerSockEP>
	createUdpServerSockEP(std::string ipaddr, int port, std::function<void(int, const char *, size_t)> callback);
	static std::unique_ptr<IClientSockEP> createUdpClientSockEP(std::string serverIpaddr, int port);
};
} // namespace sockep
