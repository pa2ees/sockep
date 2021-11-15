#include "ServerSockEP.h"

#include <iostream>
#include <unistd.h>
#include <sys/socket.h>

using namespace sockep;

ServerSockEP::ServerSockEP(void (*callback)(int, std::string)) : callback_{callback}
{}

ServerSockEP::~ServerSockEP()
{
    std::cout << "ServerSockEP Destructor" << std::endl;
    // close the socket
    closeSocket();
}

void ServerSockEP::closeSocket()
{
    std::cout << "Closing socket" << std::endl;
    if (serverRunning())
    {
        stopServer();
    }
    close(sock_);
    isValid_ = false;
}

int ServerSockEP::addClient(ISSClientSockEP *newClient)
{
    // find if client already exists
    std::lock_guard<std::mutex> lock(clientsMutex_);
        
    for (auto client : clients_)
    {
        if (client.second == newClient) //strcmp(client.second.sun_path, clientSaddr.sun_path) == 0)
        {
            // client already in list, return id.
            return client.first;
        }
    }

    int clientId = 0;
    if (!clients_.empty())
    {
        // get the id of the last client and add 1
        clientId = std::prev(clients_.end())->first + 1;
    }
    std::cout << "Inserting client with ID " << clientId << " and address " << newClient->to_str() << std::endl; 
    clients_.emplace(clientId, newClient);
    // handleNewClient(newClient);
    return clientId;
}

void ServerSockEP::startServer()
{
    serverRunning_ = true;
    if (pipe(pipeFd_) != 0)
    {
        perror("Failed to create pipe");
        serverRunning_ = false;
        return;
    }
    std::cout << "Starting thread..." << std::endl;
    serverThread_ = std::thread([=]() { this->runServer(); });
}

void ServerSockEP::stopServer()
{
    if (!serverRunning_)
    {
        std::cout << "Must first start the thread before stopping it" << std::endl;
        return;
    }
    close(pipeFd_[1]);
    if (serverThread_.joinable())
    {
        std::cout << "Server thread is joinable, joining..." << std::endl;
        serverThread_.join();
        std::cout << "Successfully joined thread." << std::endl;
    }
}

bool ServerSockEP::serverRunning()
{
    return serverRunning_;
}

// void ServerSockEP::sendMessageToClient(int clientId, std::string msg)
// {
//     if (!isValid())
//     {
//         return;
//     }
//     // maybe if clientId == -1 then send message to all clients?
//     clientsMutex_.lock();
//     auto clientIt = clients_.find(clientId);
//     clientsMutex_.unlock();

//     if (clientIt == clients_.end())
//     {
//         // not found
//         return;
//     }
//     // here's where it gets hard? dgram servers need to send using their own socket
//     // but stream servers send using the "client" socket
//     // maybe sendMessageToClient(IClientSockEP *client, std::string msg)
//     // what about different threads trying to send using the same client?
//     // client should have a "send mutex"?
//     sendto(sock_, msg.c_str(), msg.length(), 0, (struct sockaddr *) &clientIt->second, sizeof(clientIt->second));
    
// }

std::vector<int> ServerSockEP::getClientIds()
{
    std::vector<int> clientIds;

    const std::lock_guard<std::mutex> lock(clientsMutex_);
    for (auto client : clients_)
    {
        clientIds.push_back(client.first);
    }
    
    return clientIds;    
}
