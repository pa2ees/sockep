#include "UnixStreamServerSockEP.h"
#include "client/UnixStreamClientSockEP.h" // so server can create new server side clients
#include <iostream>
#include <sys/poll.h>
#include <sys/socket.h>

#include "simpleLogger/SimpleLogger.h"
SETUP_SIMPLE_LOGGER(simpleLogger);
using namespace sockep;

UnixStreamServerSockEP::UnixStreamServerSockEP(std::string bindPath,
                                               std::function<void(int, const char *, size_t)> callback)
    : ServerSockEP(callback), slen_{sizeof(saddr_)}
{
	simpleLogger.debug << "Constructing Unix Stream Server Socket...\n";

	memset(&saddr_, 0, sizeof(struct sockaddr_un));
	strncpy(saddr_.sun_path, bindPath.c_str(), sizeof(saddr_.sun_path) - 1);
	saddr_.sun_family = AF_UNIX;

	sock_ = socket(AF_UNIX, SOCK_STREAM, 0);
	if (sock_ == -1)
	{
		simpleLogger.error << "Failed to create socket!\n";
		return;
	}
	unlink(saddr_.sun_path);
	int bind_retval = bind(sock_, (struct sockaddr *)&saddr_, sizeof(saddr_));
	if (bind_retval == -1)
	{
		simpleLogger.error << "Failed to bind socket to: " << saddr_.sun_path << "\n";
		return;
	}

	int backlog = 10;
	int listen_retval = listen(sock_, backlog);
	if (listen_retval == -1)
	{
		simpleLogger.error << "Failed to listen on socket at: " << saddr_.sun_path << "\n";
		return;
	}
	isValid_ = true;
}

UnixStreamServerSockEP::~UnixStreamServerSockEP()
{
	simpleLogger.debug << "Destructor\n";
	// close the socket
	closeSocket();
	unlink(saddr_.sun_path);
}

void UnixStreamServerSockEP::handlePfdUpdates(const std::vector<struct pollfd> &pfds,
                                              std::vector<struct pollfd> &newPfds,
                                              std::vector<struct pollfd> &removePfds)
{
	for (auto &pfd : pfds)
	{
		simpleLogger.debug << "Fd: " << pfd.fd << " | events: " << pfd.events << " | revents : " << pfd.revents << "\n";
		// handle receive socket
		if (pfd.fd == sock_ && pfd.revents & POLLIN)
		{ // new client connection
			std::unique_ptr<ISSClientSockEP> newClient = createNewClient();
			if (newClient == nullptr)
			{ // something went wrong with the creation of the client
				simpleLogger.error << "Could not create new client\n";
				continue;
			}

			// add the new client to the list of new fds to add to the list
			// of fds after the for loop is done, to not invalidate iterators
			struct pollfd newPfd;
			newPfd.fd = newClient->getSock();
			newPfd.events = POLLIN;
			newPfds.push_back(newPfd);

			clientsMutex_.lock();
			clients_[newPfd.fd] = std::move(newClient);
			clientsMutex_.unlock();
		}
		else if (pfd.fd == pipeFd_[0] && pfd.revents & POLLHUP)
		{ // need to terminate
			serverRunning_ = false;
			simpleLogger.debug << "stopping server\n";
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

				simpleLogger.info << "Client " << pfd.fd << " disconnected.\n";
			}
			else if (pfd.revents & POLLIN)
			{ // data to read
				simpleLogger.debug << "Got message from socket " << pfd.fd << "\n";

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
}

std::unique_ptr<ISSClientSockEP> UnixStreamServerSockEP::createNewClient()
{
	std::unique_ptr<UnixStreamClientSockEP> newClient =
	    std::unique_ptr<UnixStreamClientSockEP>(new UnixStreamClientSockEP());

	newClient->clearSaddr();

	auto len = newClient->getSaddrLen();

	int newClientSock = accept(sock_, newClient->getSaddr(), &len);
	if (newClientSock == -1)
	{ // failed to create socket for new client
		simpleLogger.error << "Failed to create socket for new client\n";
		return nullptr;
	}
	newClient->setSock(newClientSock);

	return newClient;
}

int UnixStreamServerSockEP::sendMessageToClient(int clientId, const char *msg, size_t msgLen)
{
	if (!isValid())
	{
		simpleLogger.error << "Server is not valid\n";
		return -1;
	}
	// maybe if clientId == -1 then send message to all clients?
	clientsMutex_.lock();
	auto clientIt = clients_.find(clientId);
	clientsMutex_.unlock();

	if (clientIt == clients_.end())
	{
		simpleLogger.error << "Could not find client with id " << clientId << "\n";
		return -1;
	}
	// MSG_NOSIGNAL prevents SIGPIPE from killing the program if the client goes away
	return send(clientIt->second->getSock(), msg, msgLen, MSG_NOSIGNAL);
}

int UnixStreamServerSockEP::sendMessageToClient(int clientId, const std::string &msg)
{
	return sendMessageToClient(clientId, msg.c_str(), msg.size());
}
