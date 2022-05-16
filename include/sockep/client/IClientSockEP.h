#pragma once

#include <unistd.h>
#include <string>
// #include <array>
#include <sys/socket.h>

// #define SOCKEP_CLIENT_RECV_MSG_MAX_LEN 1000
// #define SOCKEP_CLIENT_TRANSMIT_MSG_MAX_LEN 1000

namespace sockep
{
    
class IClientSockEP
{
public:
    ~IClientSockEP() {};

    virtual bool isValid() = 0;

    // returns the number of bytes sent, or -1 indicates an error
    // virtual int sendMessage(const std::array<char, SOCKEP_CLIENT_TRANSMIT_MSG_MAX_LEN> &msg, size_t msgLen);
    virtual int sendMessage(const char* msg, size_t msgLen) = 0;
    virtual int sendMessage(const std::string &msg) = 0;

    // When using getMessageStr, check isValid() after each call to see if an error occurred
    virtual std::string getMessage() = 0;

    // virtual int getMessage(std::array<char, SOCKEP_CLIENT_RECV_MSG_MAX_LEN> &msg) = 0;
    virtual int getMessage(char* msg, const int msgMaxLen) = 0;
    virtual std::string to_str() const = 0;

};

}
