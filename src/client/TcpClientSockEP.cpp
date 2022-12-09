#include "TcpClientSockEP.h"

#include <cstring> // memset
#include <iostream>

#include "simpleLogger/SimpleLogger.h"
SETUP_SIMPLE_LOGGER(simpleLogger);

using namespace sockep;

TcpClientSockEP::TcpClientSockEP(std::string serverIpaddr, int port)
{
	simpleLogger.debug << "Constructing Unix Stream Client Socket...\n";

	memset(&serverSaddr_, 0, sizeof(struct sockaddr_in));
	serverSaddr_.sin_family = AF_INET;
	serverSaddr_.sin_port = htons(port);
	serverSaddr_.sin_addr.s_addr = inet_addr(serverIpaddr.c_str());
	if (serverSaddr_.sin_addr.s_addr == (unsigned long)INADDR_NONE)
	{
		std::cerr << "Bad server ip address: " << serverIpaddr << "\n";
		return;
	}

	sock_ = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_ == -1)
	{
		simpleLogger.error << "Failed to create socket!\n";
		return;
	}

	int connect_retval = connect(sock_, (struct sockaddr *)&serverSaddr_, sizeof(serverSaddr_));
	if (connect_retval == -1)
	{
		simpleLogger.error << "Failed to connect client to server\n";
		simpleLogger.error << "Errno: " << errno << "\n";
		close(sock_);
		return;
	}

	isValid_ = true;
}

// for server side client creation
TcpClientSockEP::TcpClientSockEP() {}

TcpClientSockEP::~TcpClientSockEP()
{
	simpleLogger.debug << "Destructing TcpClientSockEP\n";
}


/******* BOTH INTERFACES **********/
int TcpClientSockEP::sendMessage(const char *msg, size_t msgLen)
{
	// MSG_NOSIGNAL prevents SIGPIPE from killing the program if there server goes away
	return send(sock_, msg, msgLen, MSG_NOSIGNAL);
}

int TcpClientSockEP::sendMessage(const std::string &msg)
{
	return sendMessage(msg.c_str(), msg.size());
}

std::string TcpClientSockEP::to_str() const
{
	return "TcpClientSock";
}

std::string TcpClientSockEP::getMessage()
{
	int bytesReceived = getMessage(msg_, sizeof(msg_));
	if (bytesReceived == -1)
	{ // an error has occurred
		isValid_ = false;
		return "";
	}
	std::string receiveStr(msg_, bytesReceived);
	return receiveStr;
}

/******* CLIENT INTERFACE **********/
int TcpClientSockEP::getMessage(char *msg, const int msgMaxLen)
{
	if (threadRunning_)
	{
		return -1;
	}
	return recv(sock_, msg, msgMaxLen, MSG_NOSIGNAL);
}

/******* SERVER SIDE CLIENT INTERFACE *********/

bool TcpClientSockEP::operator==(ISSClientSockEP const *other)
{
	simpleLogger.debug << "Comparing " << to_str() << " and " << other->to_str() << " with length "
	                   << other->getSaddrLen() << "\n";
	if (memcmp(&saddr_, other->getSaddr(), other->getSaddrLen()) == 0)
	{
		return true;
	}
	return false;
};

bool TcpClientSockEP::operator==(ISSClientSockEP const &other)
{
	return *this == &other;
};

void TcpClientSockEP::clearSaddr()
{
	memset(&saddr_, 0, sizeof(struct sockaddr_in));
}

struct sockaddr *TcpClientSockEP::getSaddr() const
{
	return (struct sockaddr *)&saddr_;
}

socklen_t TcpClientSockEP::getSaddrLen() const
{
	return sizeof(saddr_);
}

void TcpClientSockEP::setSock(int sock)
{
	sock_ = sock;
}

int TcpClientSockEP::getSock() const
{
	return sock_;
}

void TcpClientSockEP::handleIncomingMessage()
{
	int msgLen = recv(sock_, msg_, MESSAGE_MAX_LEN, MSG_NOSIGNAL);
	if (callback_)
	{
		callback_(msg_, msgLen);
	}
}
