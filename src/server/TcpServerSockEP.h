#pragma once

#include "ServerSockEP.h"

#include <arpa/inet.h>
#include <functional>

namespace sockep
{

class TcpServerSockEP : public ServerSockEP
{
public:
	TcpServerSockEP(std::string ipaddr, int port, std::function<void(int, const char *, size_t)> callback = nullptr);
	~TcpServerSockEP();

	int sendMessageToClient(int clientId, const char *msg, size_t msgLen) override;
	int sendMessageToClient(int clientId, const std::string &msg) override;

private:
	// void runServer() override; // meant to be the function for the receive thread
	void handlePfdUpdates(const std::vector<struct pollfd> &pfds, std::vector<struct pollfd> &newPfds,
	                      std::vector<struct pollfd> &removePfds) override;

	std::unique_ptr<ISSClientSockEP> createNewClient() override;

	struct sockaddr_in saddr_;
	socklen_t slen_;
};
} // namespace sockep
