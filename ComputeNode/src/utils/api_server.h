#ifndef API_SERVER_H
#define API_SERVER_H

#include "compute_core.h"

#include <string>
#include <memory>

#include <tablog.h>

namespace tdfs {
  class ApiServer {
    public:
      ApiServer(std::string interface, int port, std::shared_ptr<tdfs::ComputeCore> computeCore);
      
    private:
      std::shared_ptr<tablog::Tablog> logger;
      std::shared_ptr<tdfs::ComputeCore> computeCore;

      void clientManager(int serverSocket, int clientSocket);
  };
}

#endif
