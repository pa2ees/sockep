#include "UnixDgramServerSockEP.h"
#include "client/UnixDgramClientSockEP.h" // so server can create new server side clients
#include <iostream>
#include <sys/socket.h>
#include <sys/select.h>

using namespace sockep;

// UnixDgramServerSockEP::UnixDgramServerSockEP(std::string bindPath, void (*callback)(int, uint8_t*, size_t)) : ServerSockEP(callback), slen_{sizeof(saddr_)}
UnixDgramServerSockEP::UnixDgramServerSockEP(std::string bindPath, std::function<void(int, const char*, size_t)> callback) : ServerSockEP(callback), slen_{sizeof(saddr_)}
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

UnixDgramServerSockEP::~UnixDgramServerSockEP()
{
    std::cout << "Destructor" << std::endl;
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

    std::cout << "Successfully started server thread" << std::endl;
    fd_set rfds;

    
    while (serverRunning_)
    {
        std::cout << "server tick" << std::endl;
        FD_ZERO(&rfds);

        // add our socket fd, and the pipe fd
        FD_SET(sock_, &rfds);
        FD_SET(pipeFd_[0], &rfds);

        std::cout << "calling select..." << std::endl;
        int selectStatus = select(FD_SETSIZE, &rfds, NULL, NULL, NULL);
        if (selectStatus == -1)
        {
            perror("problem with select");
            serverRunning_ = false;
        }
        else
        { // one of the file handles is ready to read
            
            if (FD_ISSET(sock_, &rfds))
            { // external message - read from socket and add client
                
                // create a new client
                ISSClientSockEP *newClient = createNewClient();
                newClient->clearSaddr();

                auto len = newClient->getSaddrLen();
                int bytesReceived = recvfrom(sock_, msg_, sizeof(msg_), 0, newClient->getSaddr(), &len);
                msg_[bytesReceived] = '\0';
                std::cout << "Received " << bytesReceived << " bytes from " << newClient->to_str() << std::endl;

                // this will always return the client id, whether it's already exists or not
                int clientId = addClient(newClient);
                
                if (callback_)
                {
                    callback_(clientId, msg_, bytesReceived);
                }
            }
            else if (FD_ISSET(pipeFd_[0], &rfds))
            { // internal message
                serverRunning_ = false;
                std::cout << "stopping server" << std::endl;
            }
            else
            {
                std::cout << "No idea what happened here" << std::endl;
                serverRunning_ = false;
            }
        }
        
    }


}

ISSClientSockEP *UnixDgramServerSockEP::createNewClient()
{
    return new UnixDgramClientSockEP();
}

void UnixDgramServerSockEP::sendMessageToClient(int clientId, const char* msg, size_t msgLen)
{
    if (!isValid())
    {
        std::cout << "Server is not valid" << std::endl;
        return;
    }
    // maybe if clientId == -1 then send message to all clients?
    clientsMutex_.lock();
    auto clientIt = clients_.find(clientId);
    clientsMutex_.unlock();

    if (clientIt == clients_.end())
    {
        std::cout << "Could not find client with id " << clientId << std::endl;
        // not found
        return;
    }
    std::cout << "sending to " << clientIt->second << std::endl;
    sendto(sock_, msg, msgLen, 0, clientIt->second->getSaddr(), clientIt->second->getSaddrLen());
    
}

void UnixDgramServerSockEP::sendMessageToClient(int clientId, const std::string &msg)
{
    sendMessageToClient(clientId, msg.c_str(), msg.size());
}
