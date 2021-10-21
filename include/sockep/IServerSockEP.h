#pragma once

#include "ISockEP.h"
#include <vector>

class IServerSockEP : virtual public ISockEP
{
public:
    ~IServerSockEP() {};
    virtual void startServer() = 0;
    virtual void stopServer() = 0;
    virtual bool serverRunning() = 0;
    virtual void sendMessageToClient(int clientId, std::string msg) = 0;
    virtual std::vector<int> getClientIds() = 0;
};
