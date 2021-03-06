#include "UdpClientSockEP.h"

#include <cstring> // memset
#include <iostream>

using namespace sockep;

UdpClientSockEP::UdpClientSockEP(std::string serverIpaddr, int port)
{
	// std::cout << "Constructing Unix Stream Client Socket..." << std::endl;

	memset(&serverSaddr_, 0, sizeof(struct sockaddr_in));
	// strncpy(saddr_.sin_path, bindPath.c_str(), sizeof(saddr_.sin_path) - 1);
	serverSaddr_.sin_family = AF_INET;
	serverSaddr_.sin_port = htons(port);
	serverSaddr_.sin_addr.s_addr = inet_addr(serverIpaddr.c_str());
	if (serverSaddr_.sin_addr.s_addr == (unsigned long)INADDR_NONE)
	{
		std::cerr << "Bad server ip address: " << serverIpaddr << "\n";
		return;
	}

	sock_ = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock_ == -1)
	{
		std::cerr << "Failed to create socket!" << std::endl;
		return;
	}


	// int sockOptValue = 1;
	// int setsockopt_retval = setsockopt(sock_, SOL_SOCKET, SO_REUSEADDR, &sockOptValue, sizeof(sockOptValue));
	// if (setsockopt_retval == -1)
	// {
	//     std::cerr << "Failed to set sock options!\n";
	//     return;
	// }

	int bind_retval = bind(sock_, (struct sockaddr *)&saddr_, sizeof(saddr_));
	if (bind_retval == -1)
	{
		std::cerr << "Failed to bind socket\n";
		return;
	}

	int connect_retval = connect(sock_, (struct sockaddr *)&serverSaddr_, sizeof(serverSaddr_));
	if (connect_retval == -1)
	{
		std::cerr << "Failed to connect client to server\n";
		std::cerr << "Errno: " << errno << "\n";
		close(sock_);
		return;
	}

	isValid_ = true;
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
	// std::cout << "Comparing " << to_str() << " and " << other->to_str() << " with length " << other->getSaddrLen() <<
	// "\n";
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
