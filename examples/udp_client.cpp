#include "sockep/SockEPFactory.h"

#include <cstdlib>
#include <iostream>
#include <memory>

int main(int argc, char *argv[])
{
	std::unique_ptr<sockep::IClientSockEP> client;
	if (argc > 2)
	{
		client = std::unique_ptr<sockep::IClientSockEP>(
		    sockep::SockEPFactory::createUdpClientSockEP(argv[1], strtol(argv[2], NULL, 10)));
	}
	else
	{
		client =
		    std::unique_ptr<sockep::IClientSockEP>(sockep::SockEPFactory::createUdpClientSockEP("127.0.0.1", 5678));
	}

	std::cout << "Client valid: " << (client->isValid() ? "true" : "false") << std::endl;

	std::string inp;
	while (true)
	{
		std::cout << "> ";
		std::getline(std::cin, inp);

		int result = client->sendMessage(inp); // inp.c_str(), inp.size());

		std::cout << "Result is: " << result << "\n";

		if (inp == "quit")
		{
			std::cout << "Quitting...\n";
			break;
		}
		if (inp == "read")
		{
			std::cout << client->getMessage() << "\n";
		}
	}

	std::cout << "Stopping\n";
	// client->sendMessage("Hello Socket!");

	// std::string serverMsg = client->getMessage();
	// std::cout << "Got message from server: " << serverMsg << std::endl;

	// client->sendMessage("I found you!");
	// serverMsg = client->getMessage();
	// std::cout << "Got message from server: " << serverMsg << std::endl;

	// client->sendMessage("quit");

	return 0;
}
