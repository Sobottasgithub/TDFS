#ifndef COMPUTE_CORE_H
#define COMPUTE_CORE_H

#include "file_manager.h"

#include <client_discovery.h>
#include <memory>
#include <map>
#include <packet_types.h>

#include <tablog.h>

namespace tdfs {
  class ComputeCore {
    public:
      ComputeCore();
      void computeCoreCycle();
      void pushApiRequest(ttp2::Packet::Packet packet);


      void registerApiSocket(const int socket);
      void pushSolutionToApiSocket(const int socket, ttp2::Packet::Packet packet);
      ttp2::Packet::Packet getSolutionFromApiSocket(const int socket);
      int getSolutionApiSocketSize(const int socket);
      bool hasApiSocketSolution(const int socket);
      void removeRegisteredApiSocket(const int socket);

    private:
      std::shared_ptr<tablog::Tablog> logger;
      std::shared_ptr<FileManager> fileManager;
      std::shared_ptr<tud::ClientDiscovery> clientDiscovery;

      std::mutex apiRequestMutex;
      std::vector<ttp2::Packet::Packet> apiRequests = {};

      std::mutex apiSolutionMutex;
      std::map<int, std::vector<ttp2::Packet::Packet>> apiSolutionMap;

      void udpDiscoverySession();
      void peerControllerSession();
  };
}

#endif
