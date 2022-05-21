#pragma once

#include <string>
#include <vector>
// #include <array>

// #define SOCKEP_SERVER_TRANSMIT_MSG_MAX_LEN 1000

namespace sockep
{

class IServerSockEP
{
public:
	virtual ~IServerSockEP(){};
	virtual bool isValid() = 0;
	virtual void startServer() = 0;
	virtual void stopServer() = 0;
	virtual bool serverRunning() = 0;
	// virtual int sendMessageToClient(int clientId, const std::array<char, SOCKEP_SERVER_TRANSMIT_MSG_MAX_LEN>, size_t
	// msgLen) = 0;
	virtual int sendMessageToClient(int clientId, const char *msg, size_t msgLen) = 0;
	virtual int sendMessageToClient(int clientId, const std::string &msg) = 0;
	virtual std::vector<int> getClientIds() = 0;
	virtual std::string to_str() = 0;
};
} // namespace sockep
