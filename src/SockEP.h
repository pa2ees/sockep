#pragma once

#include "ISockEP.h"
#include "server/IServerSockEP.h"
#include "client/IClientSockEP.h"

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


class UnixDgramClientSockEP : public SockEP, public IClientSockEP
{
public:
    UnixDgramClientSockEP(std::string bindPath, std::string serverPath);
    void sendMessage(std::string msg) override;
    std::string getMessage() override;
    bool operator== (IClientSockEP const *other) override {return true;};
    void clearClientSaddr() override {};
    struct sockaddr * getClientSaddr() override {return nullptr;};
    socklen_t getClientSaddrLen() override {socklen_t val; return val;};
    std::string to_str() override {std::string s = "hello"; return s;};

private:
    struct sockaddr_un saddr_;
    struct sockaddr_un serverSaddr_;
};

class UnixStreamServerSockEP : public IServerSockEP
{
public:
    UnixStreamServerSockEP(std::string bindPath);
    bool isValid() override {return isValid_;};
    void startServer() override {};
    void stopServer() override {};
    bool serverRunning() override {return false;};
    void sendMessageToClient(int clientId, std::string msg) override {};
    std::vector<int> getClientIds() override {return {0,1};};

private:
    std::string bindPath_;
    SockEPType sockType_;
    int sock_ = -1;
    bool isValid_ = false;
    char msg_[1000];
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
