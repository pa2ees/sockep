#pragma once

#include <unistd.h>
#include <string>
#include <sys/socket.h>

namespace sockep
{
    
// Server Side Client
class ISSClientSockEP
{
public:
    ~ISSClientSockEP() {};

    virtual void sendMessage(const char* msg, size_t msgLen) = 0;
    virtual void sendMessage(const std::string &msg) = 0;
    virtual bool operator== (ISSClientSockEP const *other) = 0;
    virtual void clearSaddr() = 0;
    virtual struct sockaddr * getSaddr() const = 0;
    virtual socklen_t getSaddrLen() const = 0;
    virtual std::string to_str() const = 0;

};

}
