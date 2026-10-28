#include "api_server.h"

#include <string>
#include <tablog.h>
#include <thread>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/epoll.h>

#include <server_session_controller.h>
#include <packet_types.h>

#include <tablog_registry.h>

namespace tdfs {
  ApiServer::ApiServer(std::string interface, int port) {
    tablog::TablogRegistry* registry = &tablog::TablogRegistry::getInstance();
    this->logger = registry->get("TDFS");

    if (!ttp2::ServerSessionController::isValidInterface(interface)) {
        this->logger->log(tablog::ERROR, "Please provide a correct interface");
        return;
    }

    std::string ip = ttp2::ServerSessionController::getLocalIpAddress(interface);
    this->ipV4 = ip.c_str();
    this->port = port;
  }

  void ApiServer::clientManager(int serverSocket, int clientSocket) {
      auto serverSessionController = std::make_shared<ttp2::ServerSessionController>(serverSocket, clientSocket);

      std::thread networkingSession([serverSessionController]() {
          serverSessionController->networkingSession();
      });

      while (serverSessionController->isConnected()) {
          if (serverSessionController->hasRequest()) {
              ttp2::Packet::Packet packet = serverSessionController->popRequest();
              this->logger->log(tablog::DEBUG, "Received packet id: " + std::to_string(packet.id));
              serverSessionController->pushResponse(packet);
          }
      }

      this->logger->log(tablog::INFO, "Terminated!");
      networkingSession.detach();    
  }


  void ApiServer::acceptClientSession() {
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(this->port);
    serverAddress.sin_addr.s_addr = inet_addr(this->ipV4);

    int serverSocket = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if(bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) {
        this->logger->log(tablog::ERROR, "Bind failed!");
        return;
    }

    // Create epoll
    int epollFd = epoll_create1(0);
    if (epollFd == -1) {
        this->logger->log(tablog::ERROR, "Failed to create epoll!");
    }
    // Set epoll action for server
    struct epoll_event serverEvents;
    serverEvents.events = EPOLLIN | EPOLLOUT;
    serverEvents.data.fd = serverSocket;
    if (epoll_ctl(epollFd, EPOLL_CTL_ADD, serverSocket, &serverEvents) == -1) {
        this->logger->log(tablog::ERROR, "Failed to set epoll_ctl!");
        return;
    }

    listen(serverSocket, 5);
    std::vector<std::thread> clientConnections;
    while (true) {
        const int MAX_EVENTS = 10;
        struct epoll_event events[MAX_EVENTS];
        int epollRequestCount = epoll_wait(epollFd, events, MAX_EVENTS, -1);
        
        for (int index = 0; index < epollRequestCount; ++index) {
            if (events[index].data.fd == serverSocket) {
                int clientSocket = accept4(serverSocket, nullptr, nullptr, SOCK_NONBLOCK);
                this->logger->log(tablog::INFO, "New clientSocket: " + std::to_string(clientSocket));

                clientConnections.push_back(std::thread([this, serverSocket, clientSocket]() {
                    this->clientManager(serverSocket, clientSocket);
                }));
            }
        }
    }
  }
}
