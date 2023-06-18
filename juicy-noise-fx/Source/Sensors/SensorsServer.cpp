#include <iostream>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <thread>
#include <chrono>

#include "SensorsServer.h"

SensorsServer::SensorsServer(std::queue<Sensors> &sensorsQueue) {
  this->sensorsQueue = &sensorsQueue;
}

int openSocket(uint16_t port) {
  int server_fd = socket(AF_INET, SOCK_STREAM, 0);

  std::cout << "Socket descriptor created: " << server_fd << std::endl;

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

  std::cout << "sockopt = " << sockopt << std::endl;

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

  std::cout << "bindResult = " << bindResult << std::endl;

  if (bindResult < 0) {
    perror("bind failed");
    exit(EXIT_FAILURE);
  }

  int listenResult = listen(server_fd, 3);

  std::cout << "listenResult = " << listenResult << std::endl;

  if (listenResult < 0) {
    perror("listen");
    exit(EXIT_FAILURE);
  }

  int addrlen = sizeof(address);

  std::cout << "addrlen = " << addrlen << std::endl;

  int socket = accept(
    server_fd,
    (struct sockaddr*)&address,
    (socklen_t*)&addrlen);

  std::cout << "socket = " << socket << std::endl;

  if (socket < 0) {
    perror("accept");
    exit(EXIT_FAILURE);
  }

  std::puts("Got socket");

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

void listenThreaded(uint16_t port, std::queue<Sensors> &sensorsQueue) {
  std::puts("Listen in thread");

  int socket = openSocket(port);

  float buffer[sizeof(Sensors) / sizeof(float)];

  std::puts("Start reading sensors");

  while (1) {
    Sensors sensors;

    if (readSensors(socket, buffer, sensors)) {
      sensorsQueue.push(sensors);
      std::cout << "Enqueued Sensors:" << std::endl;
      std::cout << "sensors.latitude = " << sensors.latitude << std::endl;
      std::cout << "sensors.longitude = " << sensors.longitude << std::endl;
    }
  }
}

void SensorsServer::listen(uint16_t port) {
  std::puts("Begin listening");

  std::thread listening(
    &listenThreaded,
    port,
    std::ref(*this->sensorsQueue));

    listening.detach();
}
