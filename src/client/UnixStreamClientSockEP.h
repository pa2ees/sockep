#pragma once

#include "ClientSockEP.h"
#include "ISSClientSockEP.h"
#include <iostream>
#include <sys/un.h>

namespace sockep
{
    
class UnixStreamClientSockEP : public ClientSockEP, public ISSClientSockEP
{
public:
    UnixStreamClientSockEP(std::string bindPath, std::string serverPath);
    UnixStreamClientSockEP(); // for server side client creation
    ~UnixStreamClientSockEP();

    // for both interfaces (Client and Server Side Client)
    int sendMessage(const char* msg, size_t msgLen) override;
    int sendMessage(const std::string &msg) override;
    std::string to_str() const override;

    // for Client interface
    std::string getMessage() override;
    int getMessage(char* msg, const int msgMaxLen) override;

    // for Server Side Client interface
    bool operator== (ISSClientSockEP const *other) override;
    bool operator== (ISSClientSockEP const &other) override;
    void clearSaddr() override;
    struct sockaddr * getSaddr() const override;
    socklen_t getSaddrLen() const override;

    void setSock(int sock) override;
    int getSock() const override;

private:
    struct sockaddr_un saddr_;
    struct sockaddr_un serverSaddr_;

};

}
