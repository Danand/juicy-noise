#include <iostream>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

#include "SensorsServer.h"

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

void SensorsServer::listen(uint16_t port) {
  std::puts("Begin listening");
  this->socket = openSocket(port);
}

void SensorsServer::readSensors(Sensors &sensors) {
  int status = read(
    this->socket,
    &this->buffer,
    sizeof(Sensors));

  if (status > 0) {
    memcpy(
      &sensors,
      &this->buffer,
      sizeof(Sensors));

    std::cout << "sensors.latitude = " << sensors.latitude << std::endl;
    std::cout << "sensors.longitude = " << sensors.longitude << std::endl;
  }
}
