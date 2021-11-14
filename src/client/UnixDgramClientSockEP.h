#pragma once

#include "ClientSockEP.h"
#include <sys/un.h>

namespace sockep
{

    
class UnixDgramClientSockEP : public ClientSockEP
{
public:
    UnixDgramClientSockEP(std::string bindPath, std::string serverPath);
    void sendMessage(std::string msg) override;
    std::string getMessage() override;
    bool operator== (IClientSockEP const *other) override {return true;};
    void clearSaddr() override {};
    struct sockaddr * getSaddr() override {return nullptr;};
    socklen_t getSaddrLen() override {socklen_t val; return val;};
    // std::string to_str() override {std::string s = "hello"; return s;};

private:
    struct sockaddr_un saddr_;
    struct sockaddr_un serverSaddr_;

};

}
