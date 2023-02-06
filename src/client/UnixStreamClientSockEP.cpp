#include "UnixStreamClientSockEP.h"

#include <iostream>

#include "simpleLogger/SimpleLogger.h"

using namespace sockep;

UnixStreamClientSockEP::UnixStreamClientSockEP(std::string bindPath, std::string serverPath)
{
	simpleLogger.debug << "Constructing Unix Stream Client Socket...\n";

	memset(&saddr_, 0, sizeof(struct sockaddr_un));
	strncpy(saddr_.sun_path, bindPath.c_str(), sizeof(saddr_.sun_path) - 1);
	saddr_.sun_family = AF_UNIX;

	memset(&serverSaddr_, 0, sizeof(struct sockaddr_un));
	strncpy(serverSaddr_.sun_path, serverPath.c_str(), sizeof(serverSaddr_.sun_path) - 1);
	serverSaddr_.sun_family = AF_UNIX;

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

	int connect_retval = connect(sock_, (struct sockaddr *)&serverSaddr_, sizeof(serverSaddr_));
	if (connect_retval == -1)
	{
		simpleLogger.error << "Failed to connect client to server: " << serverSaddr_.sun_path << "\n";
		close(sock_);
		return;
	}

	isValid_ = true;
}

// for server side client creation
UnixStreamClientSockEP::UnixStreamClientSockEP() : ownsSocketFile_{false} {}

UnixStreamClientSockEP::~UnixStreamClientSockEP()
{
	if (ownsSocketFile_)
	{
		unlink(saddr_.sun_path);
	}
}


/******* BOTH INTERFACES **********/
int UnixStreamClientSockEP::sendMessage(const char *msg, size_t msgLen)
{
	// MSG_NOSIGNAL prevents SIGPIPE from killing the program if the server goes away
	return send(sock_, msg, msgLen, MSG_NOSIGNAL);
}

int UnixStreamClientSockEP::sendMessage(const std::string &msg)
{
	return sendMessage(msg.c_str(), msg.size());
}

std::string UnixStreamClientSockEP::to_str() const
{
	return saddr_.sun_path;
}

std::string UnixStreamClientSockEP::getMessage()
{
	int bytesReceived = getMessage(msg_, sizeof(msg_));
	std::string receiveStr(msg_, bytesReceived);
	return receiveStr;
}

/******* CLIENT INTERFACE **********/
int UnixStreamClientSockEP::getMessage(char *msg, const int msgMaxLen)
{
	if (threadRunning_)
	{
		return -1;
	}
	return recv(sock_, msg, msgMaxLen, MSG_NOSIGNAL);
}

/******* SERVER SIDE CLIENT INTERFACE *********/

bool UnixStreamClientSockEP::operator==(ISSClientSockEP const *other)
{
	simpleLogger.debug << "Comparing " << to_str() << " and " << other->to_str() << " with length "
	                   << other->getSaddrLen() << "\n";
	if (memcmp(&saddr_, other->getSaddr(), other->getSaddrLen()) == 0)
	{
		return true;
	}
	return false;
};

bool UnixStreamClientSockEP::operator==(ISSClientSockEP const &other)
{
	return *this == &other;
};

void UnixStreamClientSockEP::clearSaddr()
{
	memset(&saddr_, 0, sizeof(struct sockaddr_un));
}

struct sockaddr *UnixStreamClientSockEP::getSaddr() const
{
	return (struct sockaddr *)&saddr_;
}

socklen_t UnixStreamClientSockEP::getSaddrLen() const
{
	return sizeof(saddr_);
}

void UnixStreamClientSockEP::setSock(int sock)
{
	sock_ = sock;
}

int UnixStreamClientSockEP::getSock() const
{
	return sock_;
}

void UnixStreamClientSockEP::handleIncomingMessage()
{
	int msgLen = recv(sock_, msg_, MESSAGE_MAX_LEN, MSG_NOSIGNAL);
	if (callback_)
	{
		callback_(msg_, msgLen);
	}
}
