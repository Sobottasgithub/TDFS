#ifndef API_SERVER_H
#define API_SERVER_H

#include <string>
#include <memory>

#include <tablog.h>

namespace tdfs {
  class ApiServer {
    public:
      ApiServer(std::string interface, int port);

      void acceptClientSession();
      
    private:
      std::shared_ptr<tablog::Tablog> logger;
      
      int port;
      const char *ipV4;

      void clientManager(int serverSocket, int clientSocket);
  };
}

#endif
