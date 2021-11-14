#pragma once

#include <vector>
#include <string>

namespace sockep
{    

class IServerSockEP
{
public:
    ~IServerSockEP() {};
    virtual bool isValid() = 0;
    virtual void startServer() = 0;
    virtual void stopServer() = 0;
    virtual bool serverRunning() = 0;
    virtual void sendMessageToClient(int clientId, std::string msg) = 0;
    virtual std::vector<int> getClientIds() = 0;
    virtual std::string to_str() = 0;
    
};
}
