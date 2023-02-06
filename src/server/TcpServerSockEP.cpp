#include "TcpServerSockEP.h"
#include "client/TcpClientSockEP.h" // so server can create new server side clients
#include <iostream>
#include <sys/poll.h>
#include <sys/socket.h>

#include "simpleLogger/SimpleLogger.h"

using namespace sockep;

TcpServerSockEP::TcpServerSockEP(std::string ipaddr, int port, std::function<void(int, const char *, size_t)> callback)
    : ServerSockEP(callback), slen_{sizeof(saddr_)}
{
	simpleLogger.debug << "Constructing TCP Server Socket...\n";

	memset(&saddr_, 0, sizeof(struct sockaddr_in));
	saddr_.sin_family = AF_INET;
	saddr_.sin_addr.s_addr = INADDR_ANY;
	saddr_.sin_port = htons(port);
	memset(&(saddr_.sin_zero), '\0', 8);

	sock_ = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_ == -1)
	{
		simpleLogger.error << "Failed to create socket!\n";
		return;
	}

	int sockOptValue = 1;
	int setsockopt_retval = setsockopt(sock_, SOL_SOCKET, SO_REUSEADDR, &sockOptValue, sizeof(sockOptValue));
	if (setsockopt_retval == -1)
	{
		simpleLogger.error << "Failed to set sock options!\n";
		return;
	}

	// unlink(saddr_.sun_path);
	int bind_retval = bind(sock_, (struct sockaddr *)&saddr_, sizeof(saddr_));
	if (bind_retval == -1)
	{
		simpleLogger.error << "Failed to bind socket to: " << saddr_.sin_addr.s_addr << " on port " << saddr_.sin_port
		                   << "\n";
		close(sock_);
		return;
	}

	int backlog = 10;
	int listen_retval = listen(sock_, backlog);
	if (listen_retval == -1)
	{
		simpleLogger.error << "Failed to listen on socket at: " << saddr_.sin_addr.s_addr << " on port "
		                   << saddr_.sin_port << "\n";
		close(sock_);
		return;
	}
	isValid_ = true;
}

TcpServerSockEP::~TcpServerSockEP()
{
	// close the socket
	closeSocket();
}

void TcpServerSockEP::handlePfdUpdates(const std::vector<struct pollfd> &pfds, std::vector<struct pollfd> &newPfds,
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
				std::cerr << "Could not create new client\n";
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

				std::cout << "Client " << pfd.fd << " disconnected.\n";
			}
			else if (pfd.revents & POLLIN)
			{ // data to read
				simpleLogger.debug << "Got message from socket " << pfd.fd << "\n";

				bool clientDisconnect = false;

				clientsMutex_.lock();
				int bytesReceived = clients_[pfd.fd]->getMessage(msg_, sizeof(msg_));

				if (bytesReceived == 0)
				{ // client disconnected
					clientDisconnect = true;
					removePfds.push_back(pfd);
					clients_.erase(pfd.fd);
				}
				clientsMutex_.unlock();

				if (clientDisconnect)
				{ // cout is slow, use it outside the clientsMutex_ lock
					simpleLogger.info << "Client " << pfd.fd << " disconnected.\n";
				}
				else if (callback_)
				{
					callback_(pfd.fd, msg_, bytesReceived);
				}
			}
		}
	}
}

std::unique_ptr<ISSClientSockEP> TcpServerSockEP::createNewClient()
{
	std::unique_ptr<TcpClientSockEP> newClient = std::unique_ptr<TcpClientSockEP>(new TcpClientSockEP());

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

int TcpServerSockEP::sendMessageToClient(int clientId, const char *msg, size_t msgLen)
{
	if (!isValid())
	{
		simpleLogger.error << "Cannot send message to client, server is not valid\n";
		return -1;
	}
	// maybe if clientId == -1 then send message to all clients?
	clientsMutex_.lock();
	auto clientIt = clients_.find(clientId);
	clientsMutex_.unlock();

	if (clientIt == clients_.end())
	{
		simpleLogger.debug << "Could not find client with id " << clientId << "\n";
		return -1;
	}
	// MSG_NOSIGNAL prevents SIGPIPE from killing the program if the client goes away
	return send(clientIt->second->getSock(), msg, msgLen, MSG_NOSIGNAL);
}

int TcpServerSockEP::sendMessageToClient(int clientId, const std::string &msg)
{
	return sendMessageToClient(clientId, msg.c_str(), msg.size());
}
