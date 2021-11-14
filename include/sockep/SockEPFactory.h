#pragma once

//#include "sockep/ISockEP.h"
#include "sockep/server/IServerSockEP.h"
#include "sockep/client/IClientSockEP.h"

namespace sockep
{
class SockEPFactory
{
private:
    SockEPFactory() {};
    ~SockEPFactory() {};
public:
    static IServerSockEP *createUnixDgramServerSockEP(std::string bindPath, void (*callback)(int, std::string));
    static IClientSockEP *createUnixDgramClientSockEP(std::string bindPath, std::string serverPath);
    // static IServerSockEP *createUnixStreamServerSockEP(std::string bindPath);
    // static IClientSockEP *createUnixStreamClientSockEP(std::string serverPath);
    // static ISockEP *createUnixDgramSocket();
    // static ISockEP *createUnixStreamSocket();
    // static ISockEP *createTCPSocket();
    // static ISockEP *createUDPSocket();
    
};
}

