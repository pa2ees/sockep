#pragma once

#ifndef DGRAM_MAX_LEN
#define DGRAM_MAX_LEN 5000
#endif

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
    virtual int sendMessage(const char* msg, size_t msgLen) override = 0;
    virtual int sendMessage(const std::string &msg) override = 0;
    virtual std::string getMessage() override = 0;
    virtual int getMessage(char* msg, const int msgMaxLen) override = 0;
    virtual std::string to_str() const override = 0;

protected:

    ClientSockEPType sockType_;
    bool isValid_ {false};
    int sock_ = -1;
    char msg_[DGRAM_MAX_LEN];
};

}

        
