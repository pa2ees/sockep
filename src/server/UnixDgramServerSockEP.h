#pragma once

#include "ServerSockEP.h"

#include <sys/un.h>
#include <functional>

namespace sockep
{
    
class UnixDgramServerSockEP : public ServerSockEP
{
public:
    // UnixDgramServerSockEP(std::string bindPath, void (*callback)(int, uint8_t*, size_t) = nullptr);
    UnixDgramServerSockEP(std::string bindPath, std::function<void(int, const char*, size_t)> callback = nullptr);
    ~UnixDgramServerSockEP();

    void sendMessageToClient(int clientId, const char* msg, size_t msgLen) override;
    void sendMessageToClient(int clientId, const std::string &msg) override;

private:

    void runServer() override; // meant to be the function for the receive thread

    ISSClientSockEP* createNewClient() override;

    struct sockaddr_un saddr_;
    socklen_t slen_;

};
}
