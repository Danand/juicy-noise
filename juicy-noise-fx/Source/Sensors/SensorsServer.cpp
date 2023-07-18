#include <iostream>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <queue>
#include <thread>
#include <chrono>
#include <cmath>
#include <atomic>

#include "SensorsServer.h"

SocketDescriptor openServerSocketDescriptor(const SocketPort &port, sockaddr_in &address)
{
    SocketDescriptor serverSocketDescriptor = socket(
        AF_INET,
        SOCK_STREAM,
        0);

    std::cout << "Server socket descriptor created: "
              << serverSocketDescriptor
              << std::endl;

    if (serverSocketDescriptor == 0)
    {
        std::cerr << "Error: Failed to create socket descriptor" << std::endl;
        return EMPTY_SOCKET_DESCRIPTOR;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    int addressLength = sizeof(address);

    int bindResult = bind(
        serverSocketDescriptor,
        (struct sockaddr*)&address,
        sizeof(address));

    if (bindResult < 0)
    {
        std::cerr << "Error: Failed to bind any address with port "
                  << port
                  << std::endl;

        return EMPTY_SOCKET_DESCRIPTOR;
    }

    int listenResult = listen(serverSocketDescriptor, SOMAXCONN);

    if (listenResult < 0)
    {
        std::cerr << "Error: Failed to begin listening on port "
                  << port
                  << std::endl;

        return EMPTY_SOCKET_DESCRIPTOR;
    }

    std::cout << "Server socket descriptor created: "
              << serverSocketDescriptor
              << std::endl;

    return serverSocketDescriptor;
}

int openClientSocketDescriptor(SocketDescriptor serverSocketDescriptor, sockaddr_in &address)
{
    int addressLength = sizeof(address);

    int clientSocketDescriptor = accept(
        serverSocketDescriptor,
        (struct sockaddr*)&address,
        (socklen_t*)&addressLength);

    if (clientSocketDescriptor < 0)
    {
        std::cerr << "Error: Failed to accept client connection"
                  << std::endl;

        return EMPTY_SOCKET_DESCRIPTOR;
    }

    std::cout << "Client socket descriptor created: "
              << clientSocketDescriptor
              << std::endl;

    return clientSocketDescriptor;
}

void SensorsServer::run()
{
    while (!threadShouldExit())
    {
        if (this->portConnected != this->port)
        {
            std::cerr << "Warning: Receiving port changed from "
                      << this->portConnected
                      << " to "
                      << this->port
                      << std::endl;

            stop();
        }

        sockaddr_in address;

        if (this->serverSocketDescriptor < 0)
        {
            std::cerr << "Warning: Server socket did not opened yet" << std::endl;
            this->serverSocketDescriptor = openServerSocketDescriptor(this->port, address);
        }

        if (this->clientSocketDescriptor < 0)
        {
            std::cerr << "Warning: Client socket did not opened yet" << std::endl;
            this->clientSocketDescriptor = openClientSocketDescriptor(this->serverSocketDescriptor, address);

            if (this->clientSocketDescriptor > 0)
            {
                this->portConnected = this->port;
            }
        }

        if (this->clientSocketDescriptor < 0)
        {
            std::cerr << "Warning: Client socket was not opened somehow, skipping iteration" << std::endl;
            wait(this->latency.load());
            continue;
        }

        int bufferSize = sizeof(this->sensorsBuffer);

        int receivedBytesAmountTotal = 0;

        do
        {
            int receivedBytesAmount = recv(
                this->clientSocketDescriptor,
                this->sensorsBuffer + receivedBytesAmountTotal,
                bufferSize - receivedBytesAmountTotal,
                0);

            if (receivedBytesAmount < 0)
            {
                std::cerr << "Error: Connection closed with error "
                          << receivedBytesAmount
                          << " by a client "
                          << this->clientSocketDescriptor
                          << std::endl;

                stop();

                break;
            }

            if (receivedBytesAmount == 0)
            {
                std::cerr << "Error: Connection closed by a client "
                          << this->clientSocketDescriptor
                          << std::endl;

                stop();

                break;
            }

        receivedBytesAmountTotal += receivedBytesAmount;

        }
        while (receivedBytesAmountTotal < bufferSize);

        if (receivedBytesAmountTotal != bufferSize)
        {
            std::cerr << "Warning: Received incorrect amount of bytes, expected: "
                      << bufferSize
                      << ", actual: "
                      << receivedBytesAmountTotal
                      << std::endl;

            continue;
        }

        memcpy(
            &sensors,
            this->sensorsBuffer,
            SIZEOF_SENSORS);

        mutex.lock();

        this->sensorsQueue.push(sensors);

        mutex.unlock();

        wait(this->latency.load());
    }
}

void SensorsServer::stop()
{
    close(this->clientSocketDescriptor);
    close(this->serverSocketDescriptor);

    this->clientSocketDescriptor = EMPTY_SOCKET_DESCRIPTOR;
    this->serverSocketDescriptor = EMPTY_SOCKET_DESCRIPTOR;
}
