#include "UnixStreamClientSockEP.h"

#include <iostream>

using namespace sockep;

UnixStreamClientSockEP::UnixStreamClientSockEP(std::string bindPath, std::string serverPath)
{
    // std::cout << "Constructing Unix Stream Client Socket..." << std::endl;

    memset(&saddr_, 0, sizeof(struct sockaddr_un));
    strncpy(saddr_.sun_path, bindPath.c_str(), sizeof(saddr_.sun_path) - 1);
    saddr_.sun_family = AF_UNIX;

    memset(&serverSaddr_, 0, sizeof(struct sockaddr_un));
    strncpy(serverSaddr_.sun_path, serverPath.c_str(), sizeof(serverSaddr_.sun_path) - 1);
    serverSaddr_.sun_family = AF_UNIX;
    
    sock_ = socket(AF_UNIX, SOCK_STREAM, 0);
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

    int connect_retval = connect(sock_, (struct sockaddr*) &serverSaddr_, sizeof(serverSaddr_));
    if (connect_retval == -1)
    {
        std::cerr << "Failed to connect client to server: " << serverSaddr_.sun_path << "\n";
        close(sock_);
        return;
    }

    isValid_ = true;
}

// for server side client creation
UnixStreamClientSockEP::UnixStreamClientSockEP() {}

/******* BOTH INTERFACES **********/
void UnixStreamClientSockEP::sendMessage(const char* msg, size_t msgLen)
{    
    send(sock_, msg, msgLen, 0);
}

void UnixStreamClientSockEP::sendMessage(const std::string &msg)
{
    sendMessage(msg.c_str(), msg.size());
}

std::string UnixStreamClientSockEP::to_str() const
{
    return saddr_.sun_path;
}

std::string UnixStreamClientSockEP::getMessage()
{
    int bytesReceived = getMessage(msg_, sizeof(msg_));
    std::string receiveStr(msg_, bytesReceived);
    return receiveStr;
}

/******* CLIENT INTERFACE **********/
int UnixStreamClientSockEP::getMessage(char* msg, const int msgMaxLen)
{
    return recv(sock_, msg, msgMaxLen, 0);
}

/******* SERVER SIDE CLIENT INTERFACE *********/

bool UnixStreamClientSockEP::operator== (ISSClientSockEP const *other)
{
    // std::cout << "Comparing " << to_str() << " and " << other->to_str() << " with length " << other->getSaddrLen() << "\n";
    if (memcmp(&saddr_, other->getSaddr(), other->getSaddrLen()) == 0)
    {
        return true;
    }
    return false;
};

bool UnixStreamClientSockEP::operator== (ISSClientSockEP const &other)
{
    return *this == &other;
};

void UnixStreamClientSockEP::clearSaddr()
{
    memset(&saddr_, 0, sizeof(struct sockaddr_un));
}

struct sockaddr * UnixStreamClientSockEP::getSaddr() const
{
    return (struct sockaddr *) &saddr_;
}

socklen_t UnixStreamClientSockEP::getSaddrLen() const
{
    return sizeof(saddr_);
}

void UnixStreamClientSockEP::setSock(int sock)
{
    sock_ = sock;
}

int UnixStreamClientSockEP::getSock() const
{
    return sock_;
}
