#include "sockep/SockEPFactory.h"

#include <iostream>

int main()
{

    IServerSockEP *srvr = SockEPFactory::createUnixDgramServerSockEP("/tmp/fartserver");

    std::cout << "Server valid: " << (srvr->isValid() ? "true" : "false") << std::endl;

    std::string recvStr = srvr->getMessage();
    
    while (recvStr != "quit")
    {
        std::cout << "Message is: " << recvStr << std::endl;
        
        std::vector<int> clients = srvr->getClientIds();
        std::cout << "There are " << clients.size() << " clients" << std::endl;
        if (!clients.empty())
        {
            int firstClientId = clients.front();
            std::cout << "Sending message to the first client (ID " << firstClientId << ")" << std::endl;
            srvr->sendMessageToClient(firstClientId, "FartFace");
        }
        recvStr = srvr->getMessage();
    }
    
    srvr->closeSocket();
    
    return 0;
}
