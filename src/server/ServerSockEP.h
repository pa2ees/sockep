#pragma once

#include "server/IServerSockEP.h"
#include "client/ISSClientSockEP.h"

#include <atomic>
#include <map>
#include <thread>
#include <mutex>
#include <cstring>
#include <functional>

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
    ServerSockEP(std::function<void(int, const char*, size_t)> callback);
    ~ServerSockEP();

    bool isValid() override {return isValid_;};

    virtual void startServer() override;
    virtual void stopServer() override;
    bool serverRunning() override;

    virtual void sendMessageToClient(int clientId, const char* msg, size_t msgLen) override = 0;
    virtual void sendMessageToClient(int clientId, const std::string &msg) override = 0;
    virtual std::vector<int> getClientIds() override;
    virtual std::string to_str() override {std::string s = "howdy"; return s;};

protected:
    virtual int addClient(ISSClientSockEP * newClient);
    virtual void runServer() = 0;
    virtual void closeSocket();

    // allow concrete class to create the proper type of client
    virtual ISSClientSockEP* createNewClient() = 0;

    ServerSockEPType sockType_;
    std::atomic<bool> serverRunning_;
    int sock_ = -1;
    bool isValid_ = false;
    char msg_[1000];

    // this should probably hold a unique pointer
    std::map<int, ISSClientSockEP *> clients_;
    std::mutex clientsMutex_;
    std::thread serverThread_;
    std::function<void(int, const char*, size_t)> callback_;
    int pipeFd_[2];
};
}
