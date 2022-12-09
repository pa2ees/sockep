#include "ClientSockEP.h"

#include <iostream>
#include <sys/socket.h>
#include <unistd.h>

#include "simpleLogger/SimpleLogger.h"
SETUP_SIMPLE_LOGGER(simpleLogger);

using namespace sockep;

ClientSockEP::~ClientSockEP()
{
	stopRecvThread();
}

int ClientSockEP::startRecvThread(std::function<void(const char *, size_t)> callback)
{
	if (threadRunning_ == true)
	{
		simpleLogger.error << "Error: Recv Thread already running.\n";
		return -1;
	}
	threadRunning_ = true;
	if (pipe(pipeFd_) != 0)
	{
		simpleLogger.error << "Error: Failed to create pipe. errno: " << errno << "\n";
		threadRunning_ = false;
		return -1;
	}
	callback_ = callback;
	recvThread_ = std::thread([this]() { this->runThread(); });
	return 0;
}

void ClientSockEP::runThread()
{
	if (!isValid())
	{
		simpleLogger.error << "Error: Socket is not valid, cannot run client thread\n";
		threadRunning_ = false;
		return;
	}

	fd_set rfds;

	// create the 2 pollfds needed (socket and pipe)
	std::array<struct pollfd, 2> pfds;

	// create aliases to the 2 pollfds for ease of use
	struct pollfd &sockPfd = pfds[0];
	struct pollfd &pipePfd = pfds[1];

	// create listen socket
	sockPfd.fd = sock_;
	sockPfd.events = POLLIN;

	// create pipe socket
	pipePfd.fd = pipeFd_[0];
	pipePfd.events = 0; // only listen for POLLHUP (other end of pipe closed)

	while (threadRunning_)
	{
		// -1 == no timeout
		int pollStatus = poll(pfds.data(), pfds.size(), -1);
		if (pollStatus == -1)
		{
			simpleLogger.error << "Error: problem with poll. errno: " << errno << "\n";
			threadRunning_ = false;
			break;
		}
		if (pipePfd.revents & POLLHUP)
		{ // pipe closed, need to terminate
			simpleLogger.info << "Pipe POLLHUP\n";
			threadRunning_ = false;
			break;
		}
		else if (sockPfd.revents & POLLHUP)
		{ // server closed connection, need to terminate
			simpleLogger.info << "Sock POLLHUP\n";
			threadRunning_ = false;
			isValid_ = false;
			break;
		}
		else if (sockPfd.revents & POLLIN)
		{ // message to read, give to concrete instantiations to handle
			simpleLogger.debug << "Handling incoming message\n";
			handleIncomingMessage();
		}
		else if (sockPfd.revents & POLLERR)
		{
			bool socketOk = handleError(POLLERR);
			// std::cout << "Unable to send message to server. Server could be unavailable.\n";
			// bool reconnected = reconnectSocket();
			if (!socketOk)
			{
				simpleLogger.error << "Problem handling error " << POLLERR << "\n"
				                   << "Shutting down receive thread\n";
				isValid_ = false;
				threadRunning_ = false;
			}
		}
		else
		{ // no idea what happened here
			simpleLogger.error << "Error handling poll, status: " << pollStatus << "\n";
			simpleLogger.error << "pipePfd.revents: " << pipePfd.revents << "\n";
			simpleLogger.error << "sockPfd.revents: " << sockPfd.revents << "\n";
			isValid_ = false;
			threadRunning_ = false;
		}
	}
}

int ClientSockEP::stopRecvThread()
{
	simpleLogger.debug << "Stopping recv thread\n";
	if (!threadRunning_)
	{
		simpleLogger.debug << "Thread not running\n";
		return 0;
	}
	close(pipeFd_[1]);
	if (recvThread_.joinable())
	{
		simpleLogger.debug << "Joining...\n";
		recvThread_.join();
	}
	return 0;
}

bool ClientSockEP::recvThreadRunning()
{
	return threadRunning_;
}
