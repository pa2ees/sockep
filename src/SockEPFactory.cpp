#include "SockEPFactory.h"

#include "server/IServerSockEP.h"
#include "client/IClientSockEP.h"
// #include "SockEP.h"

#include "server/UnixDgramServerSockEP.h"
#include "server/UnixStreamServerSockEP.h"
#include "client/UnixDgramClientSockEP.h"
#include "client/UnixStreamClientSockEP.h"

#include <functional>

using namespace sockep;

IServerSockEP *SockEPFactory::createUnixDgramServerSockEP(std::string bindPath, std::function<void(int, const char*, size_t)> callback)
{
    return new UnixDgramServerSockEP(bindPath, callback);
}

IClientSockEP *SockEPFactory::createUnixDgramClientSockEP(std::string bindPath, std::string serverPath)
{
    return new UnixDgramClientSockEP(bindPath, serverPath);
}

IServerSockEP *SockEPFactory::createUnixStreamServerSockEP(std::string bindPath, std::function<void(int, const char*, size_t)> callback)
{
    return new UnixStreamServerSockEP(bindPath, callback);
}

IClientSockEP *SockEPFactory::createUnixStreamClientSockEP(std::string bindPath, std::string serverPath)
{
    return new UnixStreamClientSockEP(bindPath, serverPath);
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

