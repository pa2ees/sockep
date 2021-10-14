#pragma once

#include "ISockEP.h"
#include "IServerSockEP.h"
#include "IClientSockEP.h"

#include <sys/un.h>

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
    SockEP (SockEPType sockType);

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
    UnixDgramServerSockEP(std::string bindPath);
    std::string getMessage() override;
    void sendMessage(std::string msg) override;

private:
    struct sockaddr_un saddr_;
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
