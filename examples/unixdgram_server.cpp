#include "sockep/SockEPFactory.h"

#include <iostream>
#include <string>
#include <unistd.h>

bool running = true;
IServerSockEP *srvr;
void messageHandler(int clientId, std::string msg)
{
    std::cout << "Got message from client " << clientId << ": " << msg << std::endl;
    srvr->sendMessageToClient(clientId, "Got your message, Hello!!");
    if (msg == "quit")
    {
        running = false;
    }
    
}

int main()
{
    running = true;

    srvr = SockEPFactory::createUnixDgramServerSockEP("/tmp/fartserver", messageHandler);

    std::cout << "Server valid: " << (srvr->isValid() ? "true" : "false") << std::endl;
    srvr->startServer();
    std::cout << "Server started";
    
    // std::string recvStr = srvr->getMessage();
    
    while (running)
    {
        usleep(1000 * 10); // 10 ms
        // std::cout << "Message is: " << recvStr << std::endl;
        
        // std::vector<int> clients = srvr->getClientIds();
        // std::cout << "There are " << clients.size() << " clients" << std::endl;
        // if (!clients.empty())
        // {
        //     int firstClientId = clients.front();
        //     std::cout << "Sending message to the first client (ID " << firstClientId << ")" << std::endl;
        //     srvr->sendMessageToClient(firstClientId, "FartFace");
        // }
        // recvStr = srvr->getMessage();
    }
    
    srvr->stopServer();
    srvr->closeSocket();
    
    return 0;
}
