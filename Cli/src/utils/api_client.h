#ifndef API_CLIENT_H
#define API_CLIENT_H

#include <client_session_controller.h>
#include <memory>
#include <thread>

namespace tdfs::cli {
  class ApiClient {
    public:
      ApiClient();
      bool connect(std::string ipV4, int port);
      bool isConnected();
      void disconnect();

      std::string getIp();
      int getPort();

      void sendPacket(ttp2::Packet::Packet packet);

      bool hasResponse();
      ttp2::Packet::Packet popResonse();
      
    private:
      std::shared_ptr<ttp2::ClientSessionController> clientSessionController;
      std::thread clientNetworkThread;

      std::string ipV4 = "";
      int port = 0;
  };
}

#endif
