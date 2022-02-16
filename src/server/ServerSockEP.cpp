#include "ServerSockEP.h"

#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/poll.h>
#include <algorithm>

using namespace sockep;

// ServerSockEP::ServerSockEP(void (*callback)(int, uint8_t*, size_t)) : callback_{callback}
ServerSockEP::ServerSockEP(std::function<void(int, const char*, size_t)> callback) : callback_{callback}
{}

ServerSockEP::~ServerSockEP()
{
    closeSocket();
}

void ServerSockEP::closeSocket()
{
    // std::cout << "Closing socket" << std::endl;
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
    // std::cout << "Looking for client " << newClient->to_str() << "\n";
    std::lock_guard<std::mutex> lock(clientsMutex_);
        
    for (auto &client : clients_)
    {
        // std::cout << "Found client " << client.second->to_str() << "\n";
        if (client.second != nullptr && *client.second == *newClient) //strcmp(client.second.sun_path, clientSaddr.sun_path) == 0)
        {
            // client already in list, return id.

            // need to clear Saddr or the destructor of newClient will
            // unlink the socket when it goes out of scope
            newClient->clearSaddr();

            return client.first;
        }
    }

    int clientId = 0;
    if (!clients_.empty())
    {
        // get the id of the last client and add 1
        clientId = std::prev(clients_.end())->first + 1;
    }
    // std::cout << "Inserting client with ID " << clientId << " and address " << newClient->to_str() << std::endl; 
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
    // std::cout << "Starting thread..." << std::endl;
    serverThread_ = std::thread([=]() { this->runServer(); });
}

void ServerSockEP::runServer()
{
    if (!isValid())
    {
        std::cerr << "Socket is not valid, cannot run a server" << std::endl;
        serverRunning_ = false;
        return;
    }

    // std::cout << "Successfully started server thread" << std::endl;
    fd_set rfds;

    // create the pollfds
    std::vector<struct pollfd> pfds;
    std::vector<struct pollfd> newPfds;
    std::vector<struct pollfd> removePfds;

    // create temporary pfd to add to vector
    struct pollfd pfd;

    // create listen socket
    pfd.fd = sock_;
    pfd.events = POLLIN;
    pfds.push_back(pfd);

    // create pipe socket
    pfd.fd = pipeFd_[0];
    pfd.events = 0; // only listen for POLLHUP (other end of pipe closed)
    pfds.push_back(pfd);
    
    while (serverRunning_)
    {
        // std::cout << "server tick" << std::endl;

        // -1 == no timeout
        int pollStatus = poll(pfds.data(), pfds.size(), -1);
        if (pollStatus == -1)
        {
            perror("problem with poll");
            serverRunning_ = false;
            break;
        }

        handlePfdUpdates(pfds, newPfds, removePfds);

        // add all of the new client pfds to our pfds list
        pfds.insert(pfds.end(), newPfds.begin(), newPfds.end());
        newPfds.clear();
        // remove all of the clients that have hung up
        auto removePfd = [&pfds](const struct pollfd &pfdToRemove)
            {
                auto matchPfd = [&pfdToRemove] (struct pollfd &pfd) -> bool
                    {
                        if (memcmp(&pfdToRemove, &pfd, sizeof(struct pollfd)) == 0)
                        { // found a match
                            return true;
                        }
                        return false;
                    };
                auto pfdToRemoveLoc = find_if(pfds.begin(), pfds.end(), matchPfd);
                if (pfdToRemoveLoc != pfds.end())
                { // found something to delete
                    // std::cout << "Deleting " << pfdToRemoveLoc->fd << "\n";
                    pfds.erase(pfdToRemoveLoc);
                }
            };
        std::for_each(removePfds.begin(), removePfds.end(), removePfd);
        removePfds.clear();
    }

}

void ServerSockEP::stopServer()
{
    if (!serverRunning_)
    {
        // std::cout << "Must first start the thread before stopping it" << std::endl;
        return;
    }
    close(pipeFd_[1]);
    if (serverThread_.joinable())
    {
        // std::cout << "Server thread is joinable, joining..." << std::endl;
        serverThread_.join();
        // std::cout << "Successfully joined thread." << std::endl;
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
