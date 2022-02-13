#include "UnixStreamServerSockEP.h"
#include "client/UnixStreamClientSockEP.h" // so server can create new server side clients
#include <iostream>
#include <sys/socket.h>
#include <sys/poll.h>
#include <algorithm>

using namespace sockep;

// UnixStreamServerSockEP::UnixStreamServerSockEP(std::string bindPath, void (*callback)(int, uint8_t*, size_t)) : ServerSockEP(callback), slen_{sizeof(saddr_)}
UnixStreamServerSockEP::UnixStreamServerSockEP(std::string bindPath, std::function<void(int, const char*, size_t)> callback) : ServerSockEP(callback), slen_{sizeof(saddr_)}
{
    // std::cout << "Constructing Unix Stream Server Socket..." << std::endl;

    memset(&saddr_, 0, sizeof(struct sockaddr_un));
    strncpy(saddr_.sun_path, bindPath.c_str(), sizeof(saddr_.sun_path) - 1);
    saddr_.sun_family = AF_UNIX;
    
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

    int backlog = 10;
    int listen_retval = listen(sock_, backlog);
    if (listen_retval == -1)
    {
        std::cerr << "Failed to listen on socket at: " << saddr_.sun_path << "\n";
        return;
    }
    isValid_ = true;
}

UnixStreamServerSockEP::~UnixStreamServerSockEP()
{
    // std::cout << "Destructor" << std::endl;
    // close the socket
    closeSocket();
    unlink(saddr_.sun_path);
}

// run as a thread when IServerSockEP::startServer() is called
void UnixStreamServerSockEP::runServer()
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

        for (auto pfd : pfds)
        {
            // std::cout << "Fd: " << pfd.fd << " | events: " << pfd.events << " | revents : " << pfd.revents << "\n";
            // handle receive socket
            if (pfd.fd == sock_ && pfd.revents & POLLIN)
            { // new client connection
                std::unique_ptr<ISSClientSockEP> newClient = createNewClient();
                if (newClient == nullptr)
                { // something went wrong with the creation of the client
                    continue;
                }

                // add the new client to the list of new fds to add to the list
                // of fds after the for loop is done, to not invalidate iterators
                pfd.fd = newClient->getSock();
                pfd.events = POLLIN;
                newPfds.push_back(pfd);

                clientsMutex_.lock();
                clients_[pfd.fd] = std::move(newClient);
                clientsMutex_.unlock();

            }
            else if (pfd.fd == pipeFd_[0] && pfd.revents & POLLHUP)
            { // need to terminate
                serverRunning_ = false;
                // std::cout << "stopping server" << std::endl;
                break;
            }
            else if (pfd.fd != sock_ && pfd.fd != pipeFd_[0])
            {
                if (pfd.revents & POLLHUP)
                { // must be before POLLIN because a hup sets POLLIN bit also
                    clientsMutex_.lock();
                    removePfds.push_back(pfd);
                    clients_.erase(pfd.fd);

                    clientsMutex_.unlock();

                }
                else if (pfd.revents & POLLIN)
                { // data to read
                    // std::cout << "Got message from socket " << pfd.fd << "\n";

                    clientsMutex_.lock();
                    int bytesReceived = clients_[pfd.fd]->getMessage(msg_, sizeof(msg_));

                    clientsMutex_.unlock();
                    
                    if (callback_)
                    {
                        callback_(pfd.fd, msg_, bytesReceived);
                        
                    }
                }
            }
        }
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

std::unique_ptr<ISSClientSockEP> UnixStreamServerSockEP::createNewClient()
{
    std::unique_ptr<UnixStreamClientSockEP> newClient = std::unique_ptr<UnixStreamClientSockEP> (new UnixStreamClientSockEP());
    
    newClient->clearSaddr();

    auto len = newClient->getSaddrLen();

    int newClientSock = accept(sock_, newClient->getSaddr(), &len);
    if (newClientSock == -1)
    { // failed to create socket for new client
        std::cerr << "Failed to create socket for new client\n";
        return nullptr;
    }
    newClient->setSock(newClientSock);

    return newClient;
}

int UnixStreamServerSockEP::sendMessageToClient(int clientId, const char* msg, size_t msgLen)
{
    if (!isValid())
    {
        std::cerr << "Server is not valid" << std::endl;
        return -1;
    }
    // maybe if clientId == -1 then send message to all clients?
    clientsMutex_.lock();
    auto clientIt = clients_.find(clientId);
    clientsMutex_.unlock();

    if (clientIt == clients_.end())
    {
        std::cerr << "Could not find client with id " << clientId << std::endl;
        return -1;
    }
    return send(clientIt->second->getSock(), msg, msgLen, 0);//, clientIt->second->getSaddr(), clientIt->second->getSaddrLen());
}

int UnixStreamServerSockEP::sendMessageToClient(int clientId, const std::string &msg)
{
    return sendMessageToClient(clientId, msg.c_str(), msg.size());
}
