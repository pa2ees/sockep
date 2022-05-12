#include "sockep/SockEPFactory.h"

#include <iostream>
#include <memory>
#include <cstdlib>

int main(int argc, char *argv[])
{
    std::unique_ptr<sockep::IClientSockEP> client;
    if (argc > 2)
    {
        client = std::unique_ptr<sockep::IClientSockEP>(sockep::SockEPFactory::createTcpClientSockEP(argv[1], strtol(argv[2], NULL, 10)));
    }
    else
    {
        client = std::unique_ptr<sockep::IClientSockEP>(sockep::SockEPFactory::createTcpClientSockEP("127.0.0.1", 5678));
    }
    
    std::cout << "Client valid: " << (client->isValid() ? "true" : "false") << std::endl;

    std::string inp;
    while (true)
    {
        std::cout << "> ";
        std::getline(std::cin, inp);

        client->sendMessage(inp);//inp.c_str(), inp.size());

        if (inp == "quit")
        {
            break;
        }
        if (inp == "read")
        {
            std::cout << client->getMessage() << "\n";
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

