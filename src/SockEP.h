#pragma once

#include "ISockEP.h"
#include "IServerSockEP.h"
#include "IClientSockEP.h"

#include <sys/un.h>
#include <thread>
#include <mutex>
#include <map>
#include <atomic>

enum SockEPType
{
    unixDgramServer,
    unixDgramClient,
    unixStreamServer,
    unixStreamClient,
    TCPServer,
    TCPClient,
    UDPServer,
    UDPClient,
};

class SockEP : virtual public ISockEP
{
public:
    SockEP(SockEPType sockType);
    ~SockEP();

    bool isValid() override {return isValid_;};
    int getSock() override {return sock_;};
    int closeSocket() override;

    // maybe we should deal with char[] instead of std::string?
    // easier to send binary data with char[]
    virtual std::string getMessage() override;
    virtual void sendMessage(std::string msg) override;

protected:
    SockEPType sockType_;
    int sock_ = -1;
    bool isValid_ = false;
    char msg_[1000];
};

class UnixDgramServerSockEP : public SockEP, public IServerSockEP
{
public:
    UnixDgramServerSockEP(std::string bindPath, void (*callback)(int,std::string) = nullptr);
    std::string getMessage() override;
    void sendMessage(std::string msg) override;
    void startServer() override;
    void stopServer() override;
    bool serverRunning() override;
    void sendMessageToClient(int clientId, std::string msg) override;
    std::vector<int> getClientIds() override;

private:

    int addClient(struct sockaddr_un clientSaddr);
    //int addMessageToClient(int clientId, char *message, int messageLen);
    void runServer(); // meant to be the function for the receive thread

    struct sockaddr_un saddr_;
    std::atomic<bool> serverRunning_;
    
    // need to create an actual client class to store messages in
    std::map<int, struct sockaddr_un> clients_;
    std::mutex clientsMutex_;
    std::thread serverThread_;
    void (*callback_)(int, std::string);
    int pipeFd_[2];
};

class UnixDgramClientSockEP : public SockEP, public IClientSockEP
{
public:
    UnixDgramClientSockEP(std::string bindPath, std::string serverPath);
    void sendMessage(std::string msg) override;
    std::string getMessage() override;

private:
    struct sockaddr_un saddr_;
    struct sockaddr_un serverSaddr_;
};

class UnixStreamServerSockEP : public SockEP, public IServerSockEP
{
public:
    UnixStreamServerSockEP(std::string bindPath);
    void sendMessage(std::string msg) override {};
    void startServer() override {};
    void stopServer() override {};
    bool serverRunning() override {return false;};
    void sendMessageToClient(int clientId, std::string msg) override {};
    std::vector<int> getClientIds() override {return {0,1};};

private:
    std::string bindPath_;
};

class UnixStreamClientSockEP : public SockEP, public IClientSockEP
{
public:
    UnixStreamClientSockEP(std::string serverPath);

private:
    std::string serverPath_;
};

// class UnixStreamSockEP : public SockEP
// {
// public:
//     UnixStreamSockEP();

// };

// class TCPSockEP : public SockEP
// {
// public:
//     TCPSockEP();

// };

// class UDPSockEP : public SockEP
// {
// public:
//     UDPSockEP();

// };
