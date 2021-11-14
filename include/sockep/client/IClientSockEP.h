#pragma once

//#include "ISockEP.h"

#include <unistd.h>
#include <string>
#include <sys/socket.h>

namespace sockep
{
    
class IClientSockEP
{
public:
    ~IClientSockEP() {};

    virtual bool isValid() = 0;
    virtual bool operator== (IClientSockEP const *other) = 0; 
    virtual void sendMessage(std::string msg) = 0;
    virtual std::string getMessage() = 0;
    virtual void clearSaddr() = 0;
    virtual struct sockaddr * getSaddr() = 0;
    virtual socklen_t getSaddrLen() = 0;
    virtual std::string to_str() = 0;

};

}
