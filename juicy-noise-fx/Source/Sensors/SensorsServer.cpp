#include <iostream>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <queue>
#include <thread>
#include <chrono>

#include "SensorsServer.h"

SocketDescriptor openServerSocketDescriptor(SocketPort port, sockaddr_in &address) {
  SocketDescriptor server_socket_descriptor = socket(
    AF_INET,
    SOCK_STREAM,
    0);

  std::cout << "Server socket descriptor created: "
            << server_socket_descriptor
            << std::endl;

  if (server_socket_descriptor == 0) {
    std::cerr << "Error: Failed to create socket descriptor" << std::endl;
    return EMPTY_SOCKET_DESCRIPTOR;
  }

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(port);

  int addrlen = sizeof(address);

  int bind_result = bind(
    server_socket_descriptor,
    (struct sockaddr*)&address,
    sizeof(address));

  if (bind_result < 0) {
    std::cerr << "Error: Failed to bind any address with port "
              << port
              << std::endl;

    return EMPTY_SOCKET_DESCRIPTOR;
  }

  int listen_result = listen(server_socket_descriptor, SOMAXCONN);

  if (listen_result < 0) {
    std::cerr << "Error: Failed to begin listening on port "
              << port
              << std::endl;

    return EMPTY_SOCKET_DESCRIPTOR;
  }

  return server_socket_descriptor;
}

int openClientSocketDescriptor(SocketDescriptor server_socket_descriptor, sockaddr_in &address) {

  int addrlen = sizeof(address);

  int client_socket_descriptor = accept(
    server_socket_descriptor,
    (struct sockaddr*)&address,
    (socklen_t*)&addrlen);

  if (client_socket_descriptor < 0) {
    std::cerr << "Error: Failed to accept client connection"
              << std::endl;

    return EMPTY_SOCKET_DESCRIPTOR;
  }

  std::cout << "Client socket descriptor created: "
            << client_socket_descriptor
            << std::endl;

  return client_socket_descriptor;
}

void SensorsServer::run() {
    while (!threadShouldExit()) {
      sockaddr_in address;

      if (this->server_socket_descriptor < 0) {
        std::cerr << "Warning: Server socket did not opened yet" << std::endl;
        this->server_socket_descriptor = openServerSocketDescriptor(this->port, address);
      }

      if (this->client_socket_descriptor < 0) {
        std::cerr << "Warning: Client socket did not opened yet" << std::endl;
        this->client_socket_descriptor = openClientSocketDescriptor(this->server_socket_descriptor, address);
      }

      if (this->client_socket_descriptor < 0) {
        std::cerr << "Warning: Client socket was not opened somehow, skipping iteration" << std::endl;
        wait(THREAD_DELAY);
        continue;
      }

      int bufferSize = sizeof(this->sensorsBuffer);

      int received_bytes_amount_total = 0;

      do {
        int received_bytes_amount = recv(
          this->client_socket_descriptor,
          this->sensorsBuffer + received_bytes_amount_total,
          bufferSize - received_bytes_amount_total,
          0);

        if (received_bytes_amount < 0) {
          std::cerr << "Error: Connection closed with error "
                    << received_bytes_amount
                    << "by a client "
                    << this->client_socket_descriptor
                    << std::endl;

          stop();

          break;
        }

        if (received_bytes_amount == 0) {
          std::cerr << "Error: Connection closed by a client "
                    << this->client_socket_descriptor
                    << std::endl;

          stop();

          break;
        }

        received_bytes_amount_total += received_bytes_amount;

      } while (received_bytes_amount_total < bufferSize);

      std::cout << "Debug: Received "
                << received_bytes_amount_total
                << " bytes by client "
                << this->client_socket_descriptor
                << std::endl;

      if (received_bytes_amount_total != bufferSize) {
        std::cerr << "Warning: Received incorrect amount of bytes, expected: "
                  << bufferSize
                  << ", actual: "
                  << received_bytes_amount_total
                  << std::endl;

        continue;
      }

      memcpy(
        &sensors,
        this->sensorsBuffer,
        SIZEOF_SENSORS);

      std::cout << sensors << std::endl;

      std::lock_guard<std::mutex> lock(mutex);

      this->sensorsQueue.push(sensors);

      wait(THREAD_DELAY);
    }
}

void SensorsServer::stop() {
  close(this->client_socket_descriptor);
  close(this->server_socket_descriptor);

  this->client_socket_descriptor = EMPTY_SOCKET_DESCRIPTOR;
  this->server_socket_descriptor = EMPTY_SOCKET_DESCRIPTOR;
}
