#include "api_client.h"

namespace tdfs::cli {
  ApiClient::ApiClient() {
  }

  bool ApiClient::connect(std::string ipV4, int port) {
    this->clientSessionController = std::make_shared<ttp2::ClientSessionController>(ipV4, port);

    auto clientSessionController = this->clientSessionController;
    std::thread networkThread([clientSessionController]() {
        clientSessionController->networkingSession();
    });
    networkThread.detach();

    this->ipV4 = ipV4;
    this->port = port;
    
    return true;
  }

  bool ApiClient::isConnected() {
    if (this->clientSessionController != nullptr && this->clientSessionController->isConnected()) {
      return true;
    }
    return false;
  }

  void ApiClient::disconnect() {
    if (isConnected()) {
      this->clientSessionController->disconnect();

      this->ipV4 = "";
      this->port = 0;

      this->clientSessionController = nullptr;
    }
  }

  std::string ApiClient::getIp() {
    return this->ipV4;
  }

  int ApiClient::getPort() {
    return this->port;
  }
}
