#include "sockep/SockEPFactory.h"

#include <cstring>
#include <cstdlib>
#include <iostream>
#include <chrono>
#include <thread>

int g_clientId;
const int buffSize = 10000000;
const int sendPacketSize = 2000;
const int numPackets = buffSize / sendPacketSize;
char bigRxBuffer[buffSize];
char bigTxBuffer[buffSize];
char *rxBuffPtr;
char *txBuffPtr;
char rxPacketIndicator[numPackets];
int rxCount = 0;
int txCount = 0;
bool firstMessage = true;
bool clientListenerRunning = true;
std::unique_ptr<sockep::IClientSockEP> client;

auto rxStart = std::chrono::high_resolution_clock::now();
auto rxFinish = std::chrono::high_resolution_clock::now();


void clientListener()
{
    rxBuffPtr = bigRxBuffer;
    rxCount = 0;
    while (clientListenerRunning)
    {
        int bytesReceived = client->getMessage(rxBuffPtr, sendPacketSize);
        rxBuffPtr += bytesReceived;
        rxCount += bytesReceived;
        if (rxCount >= buffSize)
        {
            std::cout << "Finally got enough bytes\n";
            clientListenerRunning = false;
        }
    }
}

void printHex(const char *buf, const size_t len)
{
    for (int i = 0; i < len; i++)
    {
        std::cout << static_cast<unsigned int>(static_cast<uint8_t>(buf[i])) << " ";
    }
}

void messageHandler(const char *msg, size_t msgLen)
{
    if (firstMessage)
    {
        rxStart = std::chrono::high_resolution_clock::now();
        std::cout << "Handling message of size " << msgLen << "\n";
        firstMessage = false;
    }
    int id;
    memcpy(&id, msg, sizeof(int));
    // memcpy(rxBuffPtr, msg, msgLen);
    // rxBuffPtr += msgLen;
    // rxCount += msgLen;
    if (id >= numPackets)
    {
        std::cout << "Invalid packet ID " << id << "\n";
        return;
    }
    if (rxPacketIndicator[id] == 1)
    {
        std::cout << "Duplicate id " << id << "\n";
    }
    rxPacketIndicator[id] = 1;
    rxFinish = std::chrono::high_resolution_clock::now();
    // std::cout << "Handled id " << id << "\n";
}

void serverMessageHandler(int clientId, const char* msg, size_t msgLen)
{
    messageHandler(msg, msgLen);
}


int main()
{
    rxBuffPtr = bigRxBuffer;
    txBuffPtr = bigTxBuffer;

    srand(time(0));
    
    std::cout << "Fill tx buffer with random junk..\n";
    for (int i = 0; i < buffSize; i++)
    {
        // bigTxBuffer[i] = (rand() % 60) + 65;
        bigTxBuffer[i] = (rand() % 256);

    }
    for (int i = 0; i < numPackets; i++)
    {// set an index for each packet
        memcpy(&bigTxBuffer[i*sendPacketSize], &i, sizeof(int));
        // bigTxBuffer[i*sendPacketSize] = i;
    }

    std::cout << "Clear Rx buffer..\n";
    memset(bigRxBuffer, 0, buffSize);
    memset(rxPacketIndicator, 0, numPackets);

    auto srvr = sockep::SockEPFactory::createUdpServerSockEP("127.0.0.1", 5678, serverMessageHandler);
    srvr->startServer();
    client = sockep::SockEPFactory::createUdpClientSockEP("127.0.0.1", 5678);

    std::cout << "Send Tx buffer in " << sendPacketSize << " byte chunks..\n";
    auto start = std::chrono::high_resolution_clock::now();

    int sendRes = 0;
    bool success = true;
    // send all packets from client to server
    for (int i = 0; i < buffSize / sendPacketSize; i++)
    { 
        sendRes = client->sendMessage(txBuffPtr, sendPacketSize);
        if (sendRes == -1)
        {
            std::cerr << "Error sending message\n";
            success = false;
            break;
        }
        txBuffPtr += sendPacketSize;
    }
    std::cout << "Done sending messages to the client\n";
    
    auto sendDone = std::chrono::high_resolution_clock::now();
    int waitCount = 0;

    usleep(1000 * 200);
    int packetsReceived = 0;
    for (int i = 0; i < numPackets; i++)
    {
        packetsReceived += rxPacketIndicator[i];
    }

    auto time_to_send = std::chrono::duration_cast<std::chrono::microseconds>(sendDone - start);
    auto rxTime = std::chrono::duration_cast<std::chrono::microseconds>(rxFinish - rxStart);

    std::cout << "Received " << packetsReceived << " packets out of " << numPackets << " in " << rxTime.count() << " microseconds\n";
    std::cout << "Speed: " << (packetsReceived * sendPacketSize) / rxTime.count() << " MiB / sec\n";
    std::cout << "Loss : " << (100 - (packetsReceived * 100) / numPackets) << "%\n";

    memset(bigRxBuffer, 0, buffSize);
    memset(rxPacketIndicator, 0, numPackets);

    std::vector<int> ssClients = srvr->getClientIds();
    std::cout << "There are " << ssClients.size() << " clients\n";
    if (ssClients.size() < 1)
    {
        std::cerr << "No clients, aborting\n";
        srvr->stopServer();
        return -1;
    }

    int ssClient = ssClients.front();
    client->startRecvThread(messageHandler);
    usleep(100 * 1000);    
    

    start = std::chrono::high_resolution_clock::now();
    firstMessage = true;

    txBuffPtr = bigTxBuffer;
    txCount = 0;
    // send all packets from server to client
    for (int i = 0; i < buffSize / sendPacketSize; i++)
    {
        sendRes = srvr->sendMessageToClient(ssClient, txBuffPtr, sendPacketSize);

        if (sendRes == -1)
        {
            std::cerr << "Error sending message\n";
            success = false;
            break;
        }
        txBuffPtr += sendPacketSize;
    }
    std::cout << "Done sending from server to client\n";

    usleep(1000 * 200);
    packetsReceived = 0;
    for (int i = 0; i < numPackets; i++)
    {
        packetsReceived += rxPacketIndicator[i];
    }

    time_to_send = std::chrono::duration_cast<std::chrono::microseconds>(sendDone - start);
    rxTime = std::chrono::duration_cast<std::chrono::microseconds>(rxFinish - rxStart);

    std::cout << "Received " << packetsReceived << " packets out of " << numPackets << " in " << rxTime.count() << " microseconds\n";
    std::cout << "Speed: " << (packetsReceived * sendPacketSize) / rxTime.count() << " MiB / sec\n";
    std::cout << "Loss : " << (100 - (packetsReceived * 100) / numPackets) << "%\n";

    srvr->stopServer();

    return 0;
}
