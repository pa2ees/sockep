#include "UnixDgramClientSockEP.h"

#include <iostream>

using namespace sockep;

UnixDgramClientSockEP::UnixDgramClientSockEP(std::string bindPath, std::string serverPath)
{
    std::cout << "Constructing Unix Datagram Client Socket..." << std::endl;

    memset(&saddr_, 0, sizeof(struct sockaddr_un));
    strncpy(saddr_.sun_path, bindPath.c_str(), sizeof(saddr_.sun_path) - 1);
    saddr_.sun_family = AF_UNIX;

    memset(&serverSaddr_, 0, sizeof(struct sockaddr_un));
    strncpy(serverSaddr_.sun_path, serverPath.c_str(), sizeof(serverSaddr_.sun_path) - 1);
    serverSaddr_.sun_family = AF_UNIX;
    
    sock_ = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sock_ == -1)
    {
        std::cerr << "Failed to create socket!" << std::endl;
        return;
    }

    unlink(saddr_.sun_path);
    int bind_retval = bind(sock_, (struct sockaddr*)&saddr_, sizeof(saddr_));
    if (bind_retval == -1)
    {
        std::cerr << "Failed to bind socket to: " << saddr_.sun_path << std::endl;
        return;
    }

    isValid_ = true;
}

// for server side client creation
UnixDgramClientSockEP::UnixDgramClientSockEP() {}

/******* BOTH INTERFACES **********/
void UnixDgramClientSockEP::sendMessage(std::string msg)
{    
    sendto(sock_, msg.c_str(), msg.size(), 0, (struct sockaddr *) &serverSaddr_, sizeof(serverSaddr_));
}

/******* CLIENT INTERFACE **********/
std::string UnixDgramClientSockEP::getMessage()
{
    socklen_t serverSaddrLen = sizeof(struct sockaddr_un);
    
    recvfrom(sock_, msg_, sizeof(msg_), 0, (struct sockaddr *) &serverSaddr_, &serverSaddrLen);
    return msg_;
}

/******* SERVER SIDE CLIENT INTERFACE *********/

void UnixDgramClientSockEP::clearSaddr()
{
    memset(&saddr_, 0, sizeof(struct sockaddr_un));
}

struct sockaddr * UnixDgramClientSockEP::getSaddr()
{
    return (struct sockaddr *) &saddr_;
}

socklen_t UnixDgramClientSockEP::getSaddrLen()
{
    return sizeof(saddr_);
}

