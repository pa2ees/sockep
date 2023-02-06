#include "UdpClientSockEP.h"

#include <cstring> // memset
#include <iostream>

#include "simpleLogger/SimpleLogger.h"

using namespace sockep;

UdpClientSockEP::UdpClientSockEP(std::string serverIpaddr, int port)
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

	bool connected = connectSocket();
	if (!connected)
	{
		return;
	}

	isValid_ = true;
}

bool UdpClientSockEP::connectSocket()
{
	if (sock_ != -1)
	{
		return true;
	}

	sock_ = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock_ == -1)
	{
		simpleLogger.error << "Failed to create socket!\n";
		return false;
	}

	simpleLogger.debug << "Connecting socket...\n";
	int connect_retval = connect(sock_, (struct sockaddr *)&serverSaddr_, sizeof(serverSaddr_));
	if (connect_retval == -1)
	{
		simpleLogger.error << "Failed to connect client to server\n";
		simpleLogger.error << "Errno: " << errno << "\n";
		close(sock_);
		sock_ = -1;
		return false;
	}
	return true;
}

bool UdpClientSockEP::disconnectSocket()
{
	simpleLogger.debug << "Closing socket...\n";
	close(sock_);
	sock_ = -1;
	return true;
}

bool UdpClientSockEP::reconnectSocket()
{
	disconnectSocket();
	return connectSocket();
}

bool UdpClientSockEP::handleError(int error)
{
	simpleLogger.warning << "Unable to send message to UDP server. Server could be unavailable.\n";
	return reconnectSocket();
}

// for server side client creation
UdpClientSockEP::UdpClientSockEP() {}

UdpClientSockEP::~UdpClientSockEP()
{
	// unlink(saddr_.sin_path);
}


/******* BOTH INTERFACES **********/
int UdpClientSockEP::sendMessage(const char *msg, size_t msgLen)
{
	// MSG_NOSIGNAL prevents SIGPIPE from killing the program if there server goes away
	return send(sock_, msg, msgLen, MSG_NOSIGNAL);
}

int UdpClientSockEP::sendMessage(const std::string &msg)
{
	return sendMessage(msg.c_str(), msg.size());
}

std::string UdpClientSockEP::to_str() const
{
	return "UdpClientSock";
}

std::string UdpClientSockEP::getMessage()
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
int UdpClientSockEP::getMessage(char *msg, const int msgMaxLen)
{
	if (threadRunning_)
	{
		return -1;
	}
	socklen_t serverSaddrLen = sizeof(struct sockaddr_in);
	return recvfrom(sock_, msg, msgMaxLen, MSG_NOSIGNAL, (struct sockaddr *)&serverSaddr_, &serverSaddrLen);
}

/******* SERVER SIDE CLIENT INTERFACE *********/

bool UdpClientSockEP::operator==(ISSClientSockEP const *other)
{
	simpleLogger.debug << "Comparing " << to_str() << " and " << other->to_str() << " with length "
	                   << other->getSaddrLen() << "\n";
	if (memcmp(&saddr_, other->getSaddr(), other->getSaddrLen()) == 0)
	{
		return true;
	}
	return false;
};

bool UdpClientSockEP::operator==(ISSClientSockEP const &other)
{
	return *this == &other;
};

void UdpClientSockEP::clearSaddr()
{
	memset(&saddr_, 0, sizeof(struct sockaddr_in));
}

struct sockaddr *UdpClientSockEP::getSaddr() const
{
	return (struct sockaddr *)&saddr_;
}

socklen_t UdpClientSockEP::getSaddrLen() const
{
	return sizeof(saddr_);
}

void UdpClientSockEP::setSock(int sock)
{
	sock_ = sock;
}

int UdpClientSockEP::getSock() const
{
	return sock_;
}

void UdpClientSockEP::handleIncomingMessage()
{
	socklen_t serverSaddrLen = sizeof(struct sockaddr_in);
	int msgLen =
	    recvfrom(sock_, msg_, MESSAGE_MAX_LEN, MSG_NOSIGNAL, (struct sockaddr *)&serverSaddr_, &serverSaddrLen);
	if (callback_)
	{
		callback_(msg_, msgLen);
	}
}
