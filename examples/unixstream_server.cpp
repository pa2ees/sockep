#include "sockep/SockEPFactory.h"

#include <iostream>
#include <string>
#include <unistd.h>
#include <memory>

bool running = true;
std::unique_ptr<sockep::IServerSockEP> srvr;

void messageHandler(int clientId, const char* msg, size_t msgLen)
{
    std::string message(msg, msgLen);
    std::cout << "Got message from client " << clientId << ": " << message << std::endl;
    srvr->sendMessageToClient(clientId, "Got your message, Hello!!");
    if (message == "quit")
    {
        running = false;
    }
    
}

int main()
{
    running = true;

    srvr = std::unique_ptr<sockep::IServerSockEP>(sockep::SockEPFactory::createUnixStreamServerSockEP("/tmp/fartserver", messageHandler));

    std::cout << "Server valid: " << (srvr->isValid() ? "true" : "false") << std::endl;
    srvr->startServer();
    std::cout << "Server started\n";
    
    while (running)
    {
        usleep(1000 * 10); // 10 ms

    }
    
    srvr->stopServer();
    //srvr->closeSocket();
    
    return 0;
}
