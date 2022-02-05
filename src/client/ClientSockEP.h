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
    virtual void sendMessage(const char* msg, size_t msgLen) override = 0;
    virtual void sendMessage(const std::string &msg) override = 0;
    virtual std::string getMessage() override = 0;
    virtual void getMessage(char* msg, const int msgMaxLen) override = 0;
    virtual std::string to_str() const override = 0;

protected:

    ClientSockEPType sockType_;
    bool isValid_ {false};
    int sock_ = -1;
    char msg_[1000];
};

}

        
