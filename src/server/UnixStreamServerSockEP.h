#pragma once

#include "ServerSockEP.h"

#include <sys/un.h>
#include <functional>

namespace sockep
{
    
class UnixStreamServerSockEP : public ServerSockEP
{
public:
    UnixStreamServerSockEP(std::string bindPath, std::function<void(int, const char*, size_t)> callback = nullptr);
    ~UnixStreamServerSockEP();

    int sendMessageToClient(int clientId, const char* msg, size_t msgLen) override;
    int sendMessageToClient(int clientId, const std::string &msg) override;

private:

    // void runServer() override; // meant to be the function for the receive thread
    void handlePfdUpdates(const std::vector<struct pollfd> &pfds, std::vector<struct pollfd> newPfds, std::vector<struct pollfd> &removePfds) override;

    std::unique_ptr<ISSClientSockEP> createNewClient() override;

    struct sockaddr_un saddr_;
    socklen_t slen_;

};
}
