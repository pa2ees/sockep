#include "ServerSockEP.h"

#include <iostream>
#include <unistd.h>
#include <sys/socket.h>

using namespace sockep;

// ServerSockEP::ServerSockEP(void (*callback)(int, uint8_t*, size_t)) : callback_{callback}
ServerSockEP::ServerSockEP(std::function<void(int, const char*, size_t)> callback) : callback_{callback}
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

int ServerSockEP::addClient(std::unique_ptr<ISSClientSockEP> newClient)
{
    // find if client already exists
    std::cout << "Looking for client " << newClient->to_str() << "\n";
    std::lock_guard<std::mutex> lock(clientsMutex_);
        
    for (auto &client : clients_)
    {
        std::cout << "Found client " << client.second->to_str() << "\n";
        if (client.second != nullptr && *client.second == *newClient) //strcmp(client.second.sun_path, clientSaddr.sun_path) == 0)
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
    clients_.emplace(clientId, std::move(newClient));
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

std::vector<int> ServerSockEP::getClientIds()
{
    std::vector<int> clientIds;

    const std::lock_guard<std::mutex> lock(clientsMutex_);
    for (auto &client : clients_)
    {
        clientIds.push_back(client.first);
    }
    
    return clientIds;    
}
