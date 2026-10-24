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
    this->clientNetworkThread = std::move(networkThread);

    return true;
  }
}
