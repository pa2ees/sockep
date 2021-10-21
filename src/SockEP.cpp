#include "SockEP.h"
#include <iostream>
#include <sys/socket.h>
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

void UnixDgramServerSockEP::sendMessage(std::string msg)
{
    std::cerr << "Cannot send message using passive server socket!. Use a client of this server instead." << std::endl;
}

std::string UnixDgramServerSockEP::getMessage()
{
    memset(&msg_, 0, sizeof(msg_));

    if (isValid())
    {
        struct sockaddr_un clientSaddr;
        socklen_t clientSaddrLen = sizeof(struct sockaddr_un);
        memset(&clientSaddr, 0, sizeof(struct sockaddr_un));

        int bytesReceived = recvfrom(sock_, msg_, sizeof(msg_), 0, (struct sockaddr *) &clientSaddr, &clientSaddrLen);
        std::cout << "Received " << bytesReceived << " bytes from " << clientSaddr.sun_path << std::endl;

        addClient(clientSaddr);
        //sendto(sock_, "I farted", 9, 0, (struct sockaddr *) &clientSaddr, sizeof(clientSaddr));
    }
    return msg_;
}

void UnixDgramServerSockEP::addClient(struct sockaddr_un clientSaddr)
{
    // find if client already exists
    std::lock_guard<std::mutex> lock(clientsMutex_);
        
    for (auto client : clients_)
    {
        if (strcmp(client.second.sun_path, clientSaddr.sun_path) == 0)
        {
            // client already in list
            return;
        }
    }

    int clientId = 0;
    if (!clients_.empty())
    {
        // get the id of the last client and add 1
        clientId = std::prev(clients_.end())->first + 1;
    }
    std::cout << "Inserting client with ID " << clientId << " and address " << clientSaddr.sun_path << std::endl; 
    clients_.emplace(clientId, clientSaddr);
}

void UnixDgramServerSockEP::startServer()
{
    // this should start the server listening (in its own thread)
    // and populate the clientIds_ with any client that connects
    serverRunning_ = true;
}

void UnixDgramServerSockEP::stopServer()
{
    // stop
    // server
}

bool UnixDgramServerSockEP::serverRunning()
{
    return serverRunning_;
}

void UnixDgramServerSockEP::sendMessageToClient(int clientId, std::string msg)
{
    if (!isValid())
    {
        return;
    }
    // maybe if clientId == -1 then send message to all clients?
    clientsMutex_.lock();
    auto clientIt = clients_.find(clientId);
    clientsMutex_.unlock();

    if (clientIt == clients_.end())
    {
        // not found
        return;
    }
    sendto(sock_, msg.c_str(), msg.length(), 0, (struct sockaddr *) &clientIt->second, sizeof(clientIt->second));
    
}

std::vector<int> UnixDgramServerSockEP::getClientIds()
{
    std::vector<int> clientIds;

    const std::lock_guard<std::mutex> lock(clientsMutex_);
    for (auto client : clients_)
    {
        clientIds.push_back(client.first);
    }
    
    return clientIds;    
}

UnixDgramServerSockEP::UnixDgramServerSockEP(std::string bindPath) : SockEP(unixDgramServer)
{

    std::cout << "Constructing Unix Datagram Server Socket..." << std::endl;

    memset(&saddr_, 0, sizeof(struct sockaddr_un));
    strncpy(saddr_.sun_path, bindPath.c_str(), sizeof(saddr_.sun_path) - 1);
    saddr_.sun_family = AF_UNIX;
    
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

void UnixDgramClientSockEP::sendMessage(std::string msg)
{
    
    sendto(sock_, msg.c_str(), msg.size(), 0, (struct sockaddr *) &serverSaddr_, sizeof(serverSaddr_));
}

std::string UnixDgramClientSockEP::getMessage()
{
    socklen_t serverSaddrLen = sizeof(struct sockaddr_un);
    
    recvfrom(sock_, msg_, sizeof(msg_), 0, (struct sockaddr *) &serverSaddr_, &serverSaddrLen);
    return msg_;
}

UnixDgramClientSockEP::UnixDgramClientSockEP(std::string bindPath, std::string serverPath) : SockEP(unixDgramClient)
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

UnixStreamServerSockEP::UnixStreamServerSockEP(std::string bindPath) : SockEP(unixStreamServer), bindPath_{bindPath}
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
