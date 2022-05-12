#pragma once

#ifndef MESSAGE_MAX_LEN
#define MESSAGE_MAX_LEN 5000
#endif

#include "server/IServerSockEP.h"
#include "client/ISSClientSockEP.h"

#include <atomic>
#include <map>
#include <thread>
#include <mutex>
#include <cstring>
#include <functional>
#include <memory>

#include <sys/poll.h>

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
    virtual ~ServerSockEP();

    bool isValid() override {return isValid_;};

    virtual void startServer() override;
    virtual void stopServer() override;
    bool serverRunning() override;

    virtual int sendMessageToClient(int clientId, const char* msg, size_t msgLen) override = 0;
    virtual int sendMessageToClient(int clientId, const std::string &msg) override = 0;
    virtual std::vector<int> getClientIds() override;
    virtual std::string to_str() override {std::string s = "howdy"; return s;};

protected:
    virtual int addClient(std::unique_ptr<ISSClientSockEP> newClient);
    void runServer();
    virtual void handlePfdUpdates(const std::vector<struct pollfd> &pfds, std::vector<struct pollfd> &newPfds, std::vector<struct pollfd> &removePfds) = 0;
    virtual void closeSocket();

    // allow concrete class to create the proper type of client
    virtual std::unique_ptr<ISSClientSockEP> createNewClient() = 0;

    ServerSockEPType sockType_;
    std::atomic<bool> serverRunning_ {false};
    int sock_ = -1;
    bool isValid_ = false;
    char msg_[MESSAGE_MAX_LEN];

    // this should probably hold a unique pointer
    std::map<int, std::unique_ptr<ISSClientSockEP>> clients_;
    std::mutex clientsMutex_;
    std::thread serverThread_;
    std::function<void(int, const char*, size_t)> callback_;
    int pipeFd_[2];
};
}
