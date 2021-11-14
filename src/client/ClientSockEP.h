#pragma once

#include "client/IClientSockEP.h"
#include <atomic>

namespace sockep
{

enum class ClientSockEPType
{
    unixDgram,
        unixStream,
        tcp,
        udp,
};

class ClientSockEP : public IClientSockEP
{
public:
    ClientSockEP() {};
    ~ClientSockEP() {};

    bool isValid() override {return isValid_;};
    bool operator== (IClientSockEP const *other) override {return true;};

    virtual void sendMessage(std::string msg) = 0;
    virtual std::string getMessage() = 0;
    virtual void clearSaddr() override = 0;
    virtual struct sockaddr * getSaddr() override = 0;
    virtual socklen_t getSaddrLen() override = 0;
    virtual std::string to_str() override {std::string s = "hello"; return s;};

protected:

    ClientSockEPType sockType_;
    bool isValid_ {false};
    int sock_ = -1;
    char msg_[1000];
};

}

        
