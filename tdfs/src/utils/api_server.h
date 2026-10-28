#ifndef API_SERVER_H
#define API_SERVER_H

#include <string>
#include <memory>

#include <tablog.h>

namespace tdfs {
  class ApiServer {
    public:
      ApiServer(std::string interface, int port);
      
    private:
      std::shared_ptr<tablog::Tablog> logger;

      void clientManager(int serverSocket, int clientSocket);
  };
}

#endif
