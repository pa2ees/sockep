#pragma once

#include "ServerSockEP.h"

#include <sys/un.h>

namespace sockep
{
    
class UnixDgramServerSockEP : public ServerSockEP
{
public:
    UnixDgramServerSockEP(std::string bindPath, void (*callback)(int,std::string) = nullptr);
    ~UnixDgramServerSockEP();

    void sendMessageToClient(int clientId, std::string msg) override;

private:

    void runServer() override; // meant to be the function for the receive thread

    IClientSockEP* createNewClient() override {return nullptr;};

    struct sockaddr_un saddr_;
    socklen_t slen_;

    // std::atomic<bool> serverRunning_;
    
    // SockEPType sockType_;
    // int sock_ = -1;
    // bool isValid_ = false;
    // char msg_[1000];

    // need to create an actual client class to store messages in
    // std::map<int, struct sockaddr_un> clients_;
    // std::mutex clientsMutex_;
    // std::thread serverThread_;
    // void (*callback_)(int, std::string);
    // int pipeFd_[2];
};
}
