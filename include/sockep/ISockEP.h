#pragma once

#include <string>

class ISockEP
{
public:
    ~ISockEP() {};

    virtual bool isValid() = 0;
    virtual int getSock() = 0;
    virtual std::string getMessage() = 0;
    virtual void sendMessage(std::string msg) = 0;
    virtual int closeSocket() = 0;
};
