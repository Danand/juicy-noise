#include <iostream>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

#include "SensorsServer.h"

int openSocket(uint16_t port) {
  int server_fd = socket(
    AF_INET,
    SOCK_STREAM,
    0);

  std::cout << "Server socket descriptor created: "
            << server_fd
            << std::endl;

  if (server_fd == 0) {
    std::cerr << "Error: Failed to create socket descriptor" << std::endl;
    exit(EXIT_FAILURE);
  }

  sockaddr_in address;

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(port);

  int addrlen = sizeof(address);

  int bind_result = bind(
    server_fd,
    (struct sockaddr*)&address,
    addrlen);

  if (bind_result < 0) {
    std::cerr << "Error: Failed to bind any address with port "
              << port
              << std::endl;

    exit(EXIT_FAILURE);
  }

  int listen_result = listen(server_fd, SOMAXCONN);

  if (listen_result < 0) {
    std::cerr << "Error: Failed to begin listening on port "
              << port
              << std::endl;

    exit(EXIT_FAILURE);
  }

  int client_fd = accept(
    server_fd,
    (struct sockaddr*)&address,
    (socklen_t*)&addrlen);

  if (client_fd < 0) {
    std::cerr << "Error: Failed to accept client connection on port"
              << port
              << std::endl;

    exit(EXIT_FAILURE);
  }

  std::cout << "Client socket descriptor created: "
            << client_fd
            << std::endl;

  return client_fd;
}

std::ostream& operator <<(std::ostream& stream, const Sensors& sensors) {
    stream << "{"    << "\n";
    stream << "    " << "\"longitude\": "     << sensors.longitude     << "," << "\n";
    stream << "    " << "\"latitude\": "      << sensors.latitude      << "," << "\n";
    stream << "    " << "\"accelerationX\": " << sensors.accelerationX << "," << "\n";
    stream << "    " << "\"accelerationY\": " << sensors.accelerationY << "," << "\n";
    stream << "    " << "\"accelerationZ\": " << sensors.accelerationZ << "," << "\n";
    stream << "    " << "\"rotationX\": "     << sensors.rotationX     << "," << "\n";
    stream << "    " << "\"rotationY\": "     << sensors.rotationY     << "," << "\n";
    stream << "    " << "\"rotationZ\": "     << sensors.rotationZ     << "," << "\n";
    stream << "    " << "\"gravityX\": "      << sensors.gravityX      << "," << "\n";
    stream << "    " << "\"gravityY\": "      << sensors.gravityY      << "," << "\n";
    stream << "    " << "\"gravityZ\": "      << sensors.gravityZ      << "\n";
    stream << "}";

    return stream;
}

void printSensorsContent(Sensors &sensors) {
  std::cout << sensors << std::endl;
}

void SensorsServer::listen(uint16_t port) {
  std::cout << "Begin listening" << std::endl;
  this->socket = openSocket(port);
}

void SensorsServer::readSensors(Sensors &sensors) {
  if (this->socket < 0) {
    std::cerr << "Warning: Client socket is already closed or not being opened yet" << std::endl;
    return;
  }

  int bufferSize = sizeof(this->buffer);

  int received_bytes_amount_total = 0;

  do {
    int received_bytes_amount = recv(
      this->socket,
      this->buffer + received_bytes_amount_total,
      bufferSize - received_bytes_amount_total,
      0);

    if (received_bytes_amount < 0) {
      std::cerr << "Error: Connection closed with error "
                << received_bytes_amount
                << "by a client "
                << this->socket
                << std::endl;

      closeSocket();

      return;
    }

    if (received_bytes_amount == 0) {
      std::cerr << "Error: Connection closed by a client "
                << this->socket
                << std::endl;

      closeSocket();

      return;
    }

    received_bytes_amount_total += received_bytes_amount;

  } while (received_bytes_amount_total < bufferSize);

  std::cout << "Debug: Received "
            << received_bytes_amount_total
            << " bytes by client "
            << this->socket
            << std::endl;

  if (received_bytes_amount_total != bufferSize) {

    std::cerr << "Warning: Received incorrect amount of bytes, expected: "
              << bufferSize
              << ", actual: "
              << received_bytes_amount_total
              << std::endl;

    return;
  }

  memcpy(
    &sensors,
    this->buffer,
    sizeof(Sensors));

  printSensorsContent(sensors);
}

void SensorsServer::closeSocket() {
  close(this->socket);
  this->socket = -1;
}
