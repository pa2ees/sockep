#pragma once

#include "server/IServerSockEP.h"
#include "client/IClientSockEP.h"

#include <atomic>
#include <map>
#include <thread>
#include <mutex>

namespace sockep
{

enum class ServerSockEPType
{
    unixDgram,
    unixStream,
    tcp,
    udp,
};
    
class ServerSockEP : public IServerSockEP
{
public:
    ServerSockEP(void (*callback)(int, std::string));
    ~ServerSockEP();

    bool isValid() override {return isValid_;};

    virtual void startServer() override;
    virtual void stopServer() override;
    bool serverRunning() override;

    virtual void sendMessageToClient(int clientId, std::string msg) override = 0;
    virtual std::vector<int> getClientIds() override;
    virtual std::string to_str() override {std::string s = "howdy"; return s;};

protected:
    virtual int addClient(IClientSockEP * newClient);
    virtual void runServer() = 0;
    virtual void closeSocket();

    // do something with a brand-new client
    // virtual void handleNewClient(IClientSockEP *newClient) = 0;

    // allow concrete class to create the proper type of client
    virtual IClientSockEP* createNewClient() = 0;

    ServerSockEPType sockType_;
    std::atomic<bool> serverRunning_;
    int sock_ = -1;
    bool isValid_ = false;
    char msg_[1000];

    std::map<int, IClientSockEP *> clients_;
    std::mutex clientsMutex_;
    std::thread serverThread_;
    void (*callback_)(int, std::string);
    int pipeFd_[2];
};
}
