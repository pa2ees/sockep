#include "sockep/SockEPFactory.h"

#include <iostream>

int main()
{
    sockep::IClientSockEP *client = sockep::SockEPFactory::createUnixDgramClientSockEP("/tmp/fartclient", "/tmp/fartserver");

    std::cout << "Client valid: " << (client->isValid() ? "true" : "false") << std::endl;

    std::string inp;
    while (true)
    {
        std::cout << "> ";
        std::getline(std::cin, inp);

        client->sendMessage(inp);

        if (inp == "quit")
        {
            break;
        }
    }
    // client->sendMessage("Hello Socket!");

    // std::string serverMsg = client->getMessage();
    // std::cout << "Got message from server: " << serverMsg << std::endl;

    // client->sendMessage("I found you!");
    // serverMsg = client->getMessage();
    // std::cout << "Got message from server: " << serverMsg << std::endl;

    // client->sendMessage("quit");

    return 0;
}

