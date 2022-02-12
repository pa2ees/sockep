#pragma once

//#include "sockep/ISockEP.h"
#include "sockep/server/IServerSockEP.h"
#include "sockep/client/IClientSockEP.h"

#include <functional>
#include <memory>

namespace sockep
{
class SockEPFactory
{
private:
    SockEPFactory() {};
    ~SockEPFactory() {};
public:
    static std::unique_ptr<IServerSockEP> createUnixDgramServerSockEP(std::string bindPath, std::function<void(int, const char*, size_t)> callback);
    static std::unique_ptr<IClientSockEP> createUnixDgramClientSockEP(std::string bindPath, std::string serverPath);
    static std::unique_ptr<IServerSockEP> createUnixStreamServerSockEP(std::string bindPath, std::function<void(int, const char*, size_t)> callback);
    static std::unique_ptr<IClientSockEP> createUnixStreamClientSockEP(std::string bindPath, std::string serverPath);
    // static ISockEP *createUnixDgramSocket();
    // static ISockEP *createUnixStreamSocket();
    // static ISockEP *createTCPSocket();
    // static ISockEP *createUDPSocket();
    
};
}

