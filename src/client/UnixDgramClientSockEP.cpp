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
void UnixDgramClientSockEP::sendMessage(const char* msg, size_t msgLen)
{    
    sendto(sock_, msg, msgLen, 0, (struct sockaddr *) &serverSaddr_, sizeof(serverSaddr_));
}

void UnixDgramClientSockEP::sendMessage(const std::string &msg)
{
    sendMessage(msg.c_str(), msg.size());
}

std::string UnixDgramClientSockEP::to_str() const
{
    return saddr_.sun_path;
}

/******* CLIENT INTERFACE **********/
std::string UnixDgramClientSockEP::getMessage()
{
    socklen_t serverSaddrLen = sizeof(struct sockaddr_un);
    
    recvfrom(sock_, msg_, sizeof(msg_), 0, (struct sockaddr *) &serverSaddr_, &serverSaddrLen);
    return msg_;
}

int UnixDgramClientSockEP::getMessage(char* msg, const int msgMaxLen)
{
    socklen_t serverSaddrLen = sizeof(struct sockaddr_un);
    
    recvfrom(sock_, msg, msgMaxLen, 0, (struct sockaddr *) &serverSaddr_, &serverSaddrLen);
}

/******* SERVER SIDE CLIENT INTERFACE *********/

bool UnixDgramClientSockEP::operator== (ISSClientSockEP const *other)
{
    std::cout << "Comparing " << to_str() << " and " << other->to_str() << " with length " << other->getSaddrLen() << "\n";
    if (memcmp(&saddr_, other->getSaddr(), other->getSaddrLen()) == 0)
    {
        return true;
    }
    return false;
};

void UnixDgramClientSockEP::clearSaddr()
{
    memset(&saddr_, 0, sizeof(struct sockaddr_un));
}

struct sockaddr * UnixDgramClientSockEP::getSaddr() const
{
    return (struct sockaddr *) &saddr_;
}

socklen_t UnixDgramClientSockEP::getSaddrLen() const
{
    return sizeof(saddr_);
}

