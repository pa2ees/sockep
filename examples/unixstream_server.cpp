#include "sockep/SockEPFactory.h"

#include <iostream>
#include <memory>
#include <string>
#include <unistd.h>

bool running = true;
std::unique_ptr<sockep::IServerSockEP> srvr;

void messageHandler(int clientId, const char *msg, size_t msgLen)
{
	std::string message(msg, msgLen);
	std::cout << "Got message from client " << clientId << ": " << message << std::endl;
	if (message == "quit")
	{
		running = false;
	}
	else
	{
		srvr->sendMessageToClient(clientId, "Got your message, Hello!!");
	}
}

int main()
{
	running = true;

	srvr = std::unique_ptr<sockep::IServerSockEP>(
	    sockep::SockEPFactory::createUnixStreamServerSockEP("/tmp/fartserver", messageHandler));

	std::cout << "Server valid: " << (srvr->isValid() ? "true" : "false") << std::endl;
	srvr->startServer();
	std::cout << "Server started\n";

	while (running)
	{
		usleep(1000 * 10); // 10 ms
	}

	usleep(1000 * 100); // 10 ms
	srvr->stopServer();
	// srvr->closeSocket();

	return 0;
}
