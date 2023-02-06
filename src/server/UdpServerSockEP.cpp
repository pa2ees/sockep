#include "UdpServerSockEP.h"
#include "client/UdpClientSockEP.h" // so server can create new server side clients
#include <iostream>
#include <sys/poll.h>
#include <sys/socket.h>

#include "simpleLogger/SimpleLogger.h"

using namespace sockep;

UdpServerSockEP::UdpServerSockEP(std::string ipaddr, int port, std::function<void(int, const char *, size_t)> callback)
    : ServerSockEP(callback), slen_{sizeof(saddr_)}
{
	simpleLogger.debug << "Constructing UDP Server Socket...\n";

	memset(&saddr_, 0, sizeof(struct sockaddr_in));
	saddr_.sin_family = AF_INET;
	saddr_.sin_addr.s_addr = INADDR_ANY;
	saddr_.sin_port = htons(port);
	memset(&(saddr_.sin_zero), '\0', 8);

	sock_ = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock_ == -1)
	{
		simpleLogger.error << "Failed to create Udp Server socket!\n";
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

	isValid_ = true;
}

UdpServerSockEP::~UdpServerSockEP()
{
	// close the socket
	closeSocket();
}

void UdpServerSockEP::handlePfdUpdates(const std::vector<struct pollfd> &pfds, std::vector<struct pollfd> &newPfds,
                                       std::vector<struct pollfd> &removePfds)
{
	for (auto &pfd : pfds)
	{
		simpleLogger.debug << "Fd: " << pfd.fd << " | events: " << pfd.events << " | revents : " << pfd.revents << "\n";
		// handle receive socket
		if (pfd.fd == sock_ && pfd.revents & POLLIN)
		{ // new client connection
			std::unique_ptr<ISSClientSockEP> newClient = createNewClient();
			newClient->clearSaddr();

			auto len = newClient->getSaddrLen();

			int bytesReceived = recvfrom(sock_, msg_, sizeof(msg_), 0, newClient->getSaddr(), &len);
			msg_[bytesReceived] = '\0';
			simpleLogger.debug << "Received " << bytesReceived << " bytes from " << newClient->to_str() << "\n";

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
			simpleLogger.debug << "stopping server\n";
			break;
		}
		else if (pfd.fd != sock_ && pfd.fd != pipeFd_[0])
		{
			simpleLogger.error << "No idea what happened here\n";
			serverRunning_ = false;
			break;
		}
	}
}

std::unique_ptr<ISSClientSockEP> UdpServerSockEP::createNewClient()
{
	return std::unique_ptr<UdpClientSockEP>(new UdpClientSockEP());
}

int UdpServerSockEP::sendMessageToClient(int clientId, const char *msg, size_t msgLen)
{
	if (msgLen > MESSAGE_MAX_LEN)
	{
		simpleLogger.error << "Datagram message too long! Max Datagram length: " << MESSAGE_MAX_LEN << "\n";
		return -1;
	}

	if (!isValid())
	{
		simpleLogger.error << "Cannot send message to client, UDP server is not valid" << std::endl;
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
	int retval = sendto(sock_, msg, msgLen, 0, clientIt->second->getSaddr(), clientIt->second->getSaddrLen());
	if (retval == -1)
	{
		simpleLogger.error << "Error sending message, errno: " << errno << "\n";
	}
	return retval;
}

int UdpServerSockEP::sendMessageToClient(int clientId, const std::string &msg)
{
	return sendMessageToClient(clientId, msg.c_str(), msg.size());
}
