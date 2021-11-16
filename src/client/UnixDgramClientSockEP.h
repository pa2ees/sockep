#pragma once

#include "ClientSockEP.h"
#include "ISSClientSockEP.h"
#include <sys/un.h>

namespace sockep
{
    
class UnixDgramClientSockEP : public ClientSockEP, public ISSClientSockEP
{
public:
    UnixDgramClientSockEP(std::string bindPath, std::string serverPath);
    UnixDgramClientSockEP(); // for server side client creation

    // for both interfaces (Client and Server Side Client)
    void sendMessage(std::string msg) override;
    std::string to_str() override;

    // for Client interface
    std::string getMessage() override;

    // for Server Side Client interface
    bool operator== (ISSClientSockEP const *other) override {return true;};
    void clearSaddr() override;
    struct sockaddr * getSaddr() override;
    socklen_t getSaddrLen() override;

private:
    struct sockaddr_un saddr_;
    struct sockaddr_un serverSaddr_;

};

}
