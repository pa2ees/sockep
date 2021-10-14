#include "sockep/SockEPFactory.h"

#include <iostream>

int main()
{

    IServerSockEP *srvr = SockEPFactory::createUnixDgramServerSockEP("/tmp/fartserver");

    std::cout << "Server valid: " << (srvr->isValid() ? "true" : "false") << std::endl;

    std::string recvStr = srvr->getMessage();
    std::cout << "Message is: " << recvStr << std::endl;

    srvr->closeSocket();
    
    return 0;
}
