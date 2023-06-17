#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <thread>

#include "SensorsServer.h"

SensorsServer::SensorsServer(std::queue<Sensors> &sensorsQueue) {
  this->sensorsQueue = &sensorsQueue;
}

void SensorsServer::listen(uint16_t port) {
  std::thread listening(
    listenThreaded,
    port,
    std::ref(this->sensorsQueue));
}

void listenThreaded(uint16_t port, std::queue<Sensors> &sensorsQueue) {
  int socket = openSocket(port);

  float buffer[sizeof(Sensors) / sizeof(float)];

  while (1) {
    Sensors sensors;

    if (readSensors(socket, buffer, sensors)) {
      sensorsQueue.push(sensors);
    }
  }
}

int openSocket(uint16_t port) {
  int server_fd = socket(AF_INET, SOCK_STREAM, 0);

  if (server_fd == 0) {
    perror("socket failed");
    exit(EXIT_FAILURE);
  }

  int opt = 1;

  int sockopt = setsockopt(
    server_fd,
    SOL_SOCKET,
    SO_REUSEADDR | SO_REUSEPORT,
    &opt,
    sizeof(opt));

  if (sockopt) {
    perror("setsockopt");
    exit(EXIT_FAILURE);
  }

  sockaddr_in address;

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(port);

  int bindResult = bind(
    server_fd,
    (struct sockaddr*)&address,
    sizeof(address));

  if (bindResult < 0) {
    perror("bind failed");
    exit(EXIT_FAILURE);
  }

  if (listen(server_fd, 3) < 0) {
    perror("listen");
    exit(EXIT_FAILURE);
  }

  int addrlen = sizeof(address);

  int socket = socket = accept(
    server_fd,
    (struct sockaddr*)&address,
    (socklen_t*)&addrlen);

  if (socket < 0) {
    perror("accept");
    exit(EXIT_FAILURE);
  }

  return socket;
}

int readSensors(
  int socket,
  float (&buffer)[sizeof(Sensors) / sizeof(float)],
  Sensors &sensors) {
  int status = read(socket, buffer, sizeof(Sensors));

  memcpy(&sensors, buffer, sizeof(Sensors));

  return status;
}

