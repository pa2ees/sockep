#include "sockep/SockEPFactory.h"

#include <cstring>
#include <cstdlib>
#include <iostream>

int g_clientId;
const int buffSize = 1000000;
char bigRxBuffer[buffSize];
char bigTxBuffer[buffSize];
char *rxBuffPtr;
char *txBuffPtr;
bool reported = false;

void messageHandler(int clientId, const char* msg, size_t msgLen)
{
    if (!reported)
    {
        std::cout << "Handling messages\n";
        reported = true;
    }
    memcpy(rxBuffPtr, msg, msgLen);
    rxBuffPtr += msgLen;
}

int main()
{
    rxBuffPtr = bigRxBuffer;
    txBuffPtr = bigTxBuffer;

    srand(time(0));
    
    std::cout << "Fill tx buffer with random junk..\n";
    for (int i = 0; i < buffSize; i++)
    {
        bigTxBuffer[i] = rand();
    }
    std::cout << "Clear Rx buffer..\n";
    memset(bigRxBuffer, 0, buffSize);

    auto srvr = sockep::SockEPFactory::createUnixDgramServerSockEP("/tmp/fartserver", messageHandler);
    srvr->startServer();
    auto client = sockep::SockEPFactory::createUnixDgramClientSockEP("/tmp/fartclient", "/tmp/fartserver");

    std::cout << "Send Tx buffer in 1000 byte chunks..\n";
    for (int i = 0; i < buffSize / 1000; i++)
    {
        txBuffPtr += i * 1000;
        client->sendMessage(txBuffPtr, 1000);
    }

    std::cout << "Compare Tx and Rx buffer..\n";
    if (memcmp(bigRxBuffer, bigTxBuffer, buffSize) == 0)
    {
        std::cout << "They match!\n";
    }
    else
    {
        std::cout << "They don't match\n";
    }

    srvr->stopServer();

    return 0;
}
