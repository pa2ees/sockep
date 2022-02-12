#include "UnixDgramServerSockEP.h"
#include "client/UnixDgramClientSockEP.h" // so server can create new server side clients
#include <iostream>
#include <sys/socket.h>
#include <sys/poll.h>

using namespace sockep;

UnixDgramServerSockEP::UnixDgramServerSockEP(std::string bindPath, std::function<void(int, const char*, size_t)> callback) : ServerSockEP(callback), slen_{sizeof(saddr_)}
{
    // std::cout << "Constructing Unix Datagram Server Socket..." << std::endl;

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

UnixDgramServerSockEP::~UnixDgramServerSockEP()
{
    // std::cout << "Destructor" << std::endl;
    // close the socket
    closeSocket();
}


void UnixDgramServerSockEP::runServer()
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
                newClient->clearSaddr();

                auto len = newClient->getSaddrLen();
                int bytesReceived = recvfrom(sock_, msg_, sizeof(msg_), 0, newClient->getSaddr(), &len);
                msg_[bytesReceived] = '\0';
                // std::cout << "Received " << bytesReceived << " bytes from " << newClient->to_str() << std::endl;

                // this will always return the client id, whether it's already exists or not
                int clientId = addClient(std::move(newClient));
                
                if (callback_)
                {
                    callback_(clientId, msg_, bytesReceived);
                }
            }
            else if (pfd.fd == pipeFd_[0] && pfd.revents & POLLHUP)
            { // need to terminate
                serverRunning_ = false;
                // std::cout << "stopping server" << std::endl;
                break;
            }
            else if (pfd.fd != sock_ && pfd.fd != pipeFd_[0])
            {
                std::cerr << "No idea what happened here" << std::endl;
                serverRunning_ = false;
                break;
            }
        }
    }
}

std::unique_ptr<ISSClientSockEP> UnixDgramServerSockEP::createNewClient()
{
    return std::unique_ptr<UnixDgramClientSockEP> (new UnixDgramClientSockEP());
}

void UnixDgramServerSockEP::sendMessageToClient(int clientId, const char* msg, size_t msgLen)
{
    if (!isValid())
    {
        std::cerr << "Server is not valid" << std::endl;
        return;
    }
    // maybe if clientId == -1 then send message to all clients?
    clientsMutex_.lock();
    auto clientIt = clients_.find(clientId);
    clientsMutex_.unlock();

    if (clientIt == clients_.end())
    {
        std::cerr << "Could not find client with id " << clientId << std::endl;
        // not found
        return;
    }
    // std::cout << "sending to " << clientIt->second->to_str() << std::endl;
    sendto(sock_, msg, msgLen, 0, clientIt->second->getSaddr(), clientIt->second->getSaddrLen());
    
}

void UnixDgramServerSockEP::sendMessageToClient(int clientId, const std::string &msg)
{
    sendMessageToClient(clientId, msg.c_str(), msg.size());
}
