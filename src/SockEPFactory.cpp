#include "SockEPFactory.h"

#include "server/IServerSockEP.h"
#include "client/IClientSockEP.h"
// #include "SockEP.h"

#include "server/UnixDgramServerSockEP.h"
#include "server/UnixStreamServerSockEP.h"
#include "client/UnixDgramClientSockEP.h"
#include "client/UnixStreamClientSockEP.h"


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

// ISockEP *SockEPFactory::createUnixStreamSockEP()
// {
//     return new UnixStreamSockEP();
// }
// ISockEP *SockEPFactory::createTCPSockEP()
// {
//     ISockEP *val;
//     return val;
// }
// ISockEP *SockEPFactory::createUDPSockEP()
// {
//     ISockEP *val;
//     return val;
// }

