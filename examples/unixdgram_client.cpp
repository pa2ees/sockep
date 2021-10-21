#include "sockep/SockEPFactory.h"

#include <iostream>

int main()
{

    IClientSockEP *client = SockEPFactory::createUnixDgramClientSockEP("/tmp/fartclient", "/tmp/fartserver");

    std::cout << "Client valid: " << (client->isValid() ? "true" : "false") << std::endl;

    client->sendMessage("Hello Socket!");

    std::string serverMsg = client->getMessage();
    std::cout << "Got message from server: " << serverMsg << std::endl;

    client->sendMessage("I found you!");
    serverMsg = client->getMessage();
    std::cout << "Got message from server: " << serverMsg << std::endl;

    client->sendMessage("quit");
    client->closeSocket();

    return 0;
}

