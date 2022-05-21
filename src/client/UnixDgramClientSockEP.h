#pragma once

#include "ClientSockEP.h"
#include "ISSClientSockEP.h"
#include <iostream>
#include <sys/un.h>

namespace sockep
{

class UnixDgramClientSockEP : public ClientSockEP, public ISSClientSockEP
{
public:
	UnixDgramClientSockEP(std::string bindPath, std::string serverPath);
	UnixDgramClientSockEP(); // for server side client creation

	~UnixDgramClientSockEP();

	// for both interfaces (Client and Server Side Client)
	int sendMessage(const char *msg, size_t msgLen) override;
	int sendMessage(const std::string &msg) override;
	std::string to_str() const override;

	// for Client interface
	std::string getMessage() override;
	int getMessage(char *msg, const int msgMaxLen) override;

	// for Server Side Client interface
	bool operator==(ISSClientSockEP const *other) override;
	bool operator==(ISSClientSockEP const &other) override;
	void clearSaddr() override;
	struct sockaddr *getSaddr() const override;
	socklen_t getSaddrLen() const override;
	void setSock(int sock) override{};
	int getSock() const override { return -1; }

private:
	void handleIncomingMessage() override;
	struct sockaddr_un saddr_;
	struct sockaddr_un serverSaddr_;
};

} // namespace sockep
