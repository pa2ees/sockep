#include "SockEP.h"
#include <iostream>
#include <sys/socket.h>
#include <sys/select.h>
#include <unistd.h>
#include <vector>

SockEP::SockEP(SockEPType sockType) : sockType_{sockType}
{
    std::cout << "Constructing SockEP... " << std::endl;
}

SockEP::~SockEP()
{
    closeSocket();
}

int SockEP::closeSocket()
{
    close(sock_);
    isValid_ = false;
    return 0;
}

std::string SockEP::getMessage()
{
    memset(msg_, 0, sizeof(msg_));

    if (isValid())
    {
        int read_res = read(sock_, msg_, sizeof(msg_));
        if (read_res != 0)
        {
            std::cerr << "Could not read from socket!" << read_res << std::endl;
        }
    }
    return msg_;
}

void SockEP::sendMessage(std::string msg)
{
    if (isValid())
    {
        int write_res = write(sock_, msg.c_str(), msg.size());
        if (write_res != 0)
        {
            std::cerr << "Could not write to socket!" << write_res << std::endl;
        }
    }
}



UnixStreamServerSockEP::UnixStreamServerSockEP(std::string bindPath) : bindPath_{bindPath}
{
    std::cout << "Constructing Unix Stream Server Socket..." << std::endl;

    sock_ = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock_ == -1)
    {
        std::cerr << "Failed to create socket!" << std::endl;
    }
    else
    {
        isValid_ = true;

    }
}

UnixStreamClientSockEP::UnixStreamClientSockEP(std::string serverPath) : SockEP(unixStreamClient), serverPath_{serverPath}
{
    std::cout << "Constructing Unix Stream Client Socket..." << std::endl;

    sock_ = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock_ == -1)
    {
        std::cerr << "Failed to create socket!" << std::endl;
    }
    else
    {
        isValid_ = true;

    }
}

// TCPSockEP::TCPSockEP() : SockEP(TCP)
// {
//     std::cout << "Constructing TCP Socket..." << std::endl;
    
// }
// UDPSockEP::UDPSockEP() : SockEP(UDP)
// {
//     std::cout << "Constructing UDP Socket..." << std::endl;
    
// }
