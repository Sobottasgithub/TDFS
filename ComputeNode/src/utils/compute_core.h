#ifndef COMPUTE_CORE_H
#define COMPUTE_CORE_H

#include "file_manager.h"

#include <client_discovery.h>
#include <memory>
#include <packet_types.h>

#include <tablog.h>

namespace tdfs {
  class ComputeCore {
    public:
      ComputeCore();
      void computeCoreCycle();
      void pushApiRequest(ttp2::Packet::Packet packet);
      
    private:
      std::shared_ptr<tablog::Tablog> logger;
      std::shared_ptr<FileManager> fileManager;
      std::shared_ptr<tud::ClientDiscovery> clientDiscovery;

      std::mutex apiRequestMutex;
      std::vector<ttp2::Packet::Packet> apiRequests = {}; 

      void udpDiscoverySession();
      void peerControllerSession();
  };
}

#endif
