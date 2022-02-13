#include "sockep/SockEPFactory.h"

#include <cstring>
#include <cstdlib>
#include <iostream>
#include <chrono>

int g_clientId;
const int buffSize = 10000000;
const int sendPacketSize = 2000;
char bigRxBuffer[buffSize];
char bigTxBuffer[buffSize];

char *rxBuffPtr;
char *txBuffPtr;
int rxCount = 0;


void printHex(const char *buf, const size_t len)
{
    for (int i = 0; i < len; i++)
    {
        std::cout << static_cast<unsigned int>(static_cast<uint8_t>(buf[i])) << " ";
    }
}

void messageHandler(int clientId, const char* msg, size_t msgLen)
{
    memcpy(rxBuffPtr, msg, msgLen);
    rxBuffPtr += msgLen;
    rxCount += msgLen;
}

int main()
{
    rxBuffPtr = bigRxBuffer;
    txBuffPtr = bigTxBuffer;

    srand(time(0));
    
    std::cout << "Fill tx buffer with random junk..\n";
    for (int i = 0; i < buffSize; i++)
    {
        bigTxBuffer[i] = (rand() % 60) + 65;
    }
    std::cout << "Clear Rx buffer..\n";
    memset(bigRxBuffer, 0, buffSize);

    auto srvr = sockep::SockEPFactory::createUnixStreamServerSockEP("/tmp/fartserver", messageHandler);
    srvr->startServer();
    auto client = sockep::SockEPFactory::createUnixStreamClientSockEP("/tmp/fartclient", "/tmp/fartserver");

    std::cout << "Send Tx buffer in " << sendPacketSize << " byte chunks..\n";
    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < buffSize / sendPacketSize; i++)
    {
        client->sendMessage(txBuffPtr, sendPacketSize);
        txBuffPtr += sendPacketSize;
    }
    auto sendDone = std::chrono::high_resolution_clock::now();
    int waitCount = 0;
    while (rxCount < buffSize)
    {
        waitCount++;
    }

    auto finish = std::chrono::high_resolution_clock::now();

    auto time_to_send = std::chrono::duration_cast<std::chrono::microseconds>(sendDone - start);
    auto time_to_finish =std::chrono::duration_cast<std::chrono::microseconds>(finish - start);
    
    std::cout << "Compare Tx and Rx buffer..\n";
    rxBuffPtr = bigRxBuffer;
    txBuffPtr = bigTxBuffer;
    if (memcmp(bigRxBuffer, bigTxBuffer, buffSize) == 0)
    {
        std::cout << "They match\n";
    }
    else
    {
        std::cerr << "They don't match\n";
        return -1;        
    }

    std::cout << buffSize / 1000000 << "MiB transfer time: " << time_to_finish.count() << " microseconds\n";
    std::cout << "Speed: " << buffSize / time_to_finish.count() << " MiB / sec\n";
    srvr->stopServer();

    return 0;
}
