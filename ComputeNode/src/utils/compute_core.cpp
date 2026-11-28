#include "compute_core.h"

#include "config/config.h"

#include <network_helpers.h>
#include <packet_types.h>
#include <peer_session_controller.h>
#include <client_discovery.h>
#include <string>
#include <tablog.h>
#include <tdfs_packet_types.h>
#include <thread>
#include <algorithm>

#include <tablog_registry.h>

namespace tdfs {
  ComputeCore::ComputeCore() {
    tablog::TablogRegistry* registry = &tablog::TablogRegistry::getInstance();
    this->logger = registry->get("TDFS");
    this->fileManager = std::make_shared<FileManager>();
  }

  void ComputeCore::computeCoreCycle() {
    std::thread discoveryThread([this]() {
      udpDiscoverySession();
    });

    std::thread peerSessionThread([this]() {
      peerControllerSession();
    });

    if (discoveryThread.joinable()) {
      discoveryThread.join();
    }

    if (peerSessionThread.joinable()) {
      peerSessionThread.join();
    }
  }

  void ComputeCore::udpDiscoverySession() {
    tdfs::Config& config = tdfs::Config::getInstance();

    std::string interface = config.getUdpDiscoveryInterface();
    int port = config.getUdpDiscoveryPort();
    this->clientDiscovery = std::make_shared<tud::ClientDiscovery>(interface, port, port, "TDFS");
    auto clientDiscovery = this->clientDiscovery;
    std::thread clientDiscoveryThread([clientDiscovery]() {
      clientDiscovery->discoveryCycle();
    });

    // DEBUG ONLY
    // WARNING: REMOVE LATER
    logger->log(tablog::INFO, "~~ Discovered Addresses ~~");
    std::vector<std::string> discoveredAddresses;
    while(true) {
      std::vector<std::string> newDiscoveries = clientDiscovery->getDiscoveredAddresses();
      for (int index = 0; index < newDiscoveries.size(); index++) {
        if(std::find(discoveredAddresses.begin(), discoveredAddresses.end(), newDiscoveries[index]) == discoveredAddresses.end()) {
          logger->log(tablog::INFO, "--> " + newDiscoveries[index]);
          discoveredAddresses.push_back(newDiscoveries[index]);
        }
      }
      
      std::this_thread::yield();
    }

    if (clientDiscoveryThread.joinable()) {
      clientDiscoveryThread.join();
    }
  }

  void ComputeCore::peerControllerSession() {
    tdfs::Config& config = tdfs::Config::getInstance();

    const std::string interface = config.getUdpInterface();
    const int port = config.getUpdPort();
    const std::string ipV4 = ttp2::helpers::getLocalIpAddress(interface);
    std::shared_ptr<ttp2::PeerSessionController> peerSessionController = std::make_shared<ttp2::PeerSessionController>(ipV4.c_str(), port);
    std::thread networkingSession([peerSessionController]() {
      peerSessionController->networkingSession();
    });

    while (peerSessionController->isConnected()) {
      // TODO: split apiRequests and handleNameServerRequest in their own thread
      // TODO: apiRequests that are meant to get a response:
      //       1. own struct with apiClient socket
      //       2. store in vector (inProgress)
      //       3. When receiving a NameServer response assign by same id
      {
        std::vector<std::string> discoveredNameServers = this->clientDiscovery->getDiscoveredAddresses();
        const int nameServerPort = config.getNameServerPort();
        if (discoveredNameServers.size() > 0) {
          const std::string nameServerIp = discoveredNameServers.at(0);
          {
            std::lock_guard<std::mutex> lock(apiRequestMutex);
            while (!apiRequests.empty()) {
              ttp2::Packet::Packet currentPacket = apiRequests.at(0);
              apiRequests.erase(apiRequests.begin());

              // TODO: Only for testing purposes
              //       Push back to correct api connection
              if (std::holds_alternative<ttp2::Packet::tdfs::Ls>(currentPacket.payload)) {
                ttp2::Packet::tdfs::Ls ls = std::get<ttp2::Packet::tdfs::Ls>(currentPacket.payload);
                ttp2::Packet::tdfs::LsSolution lsSolution;
                lsSolution.requestId = ls.requestId;
                lsSolution.content.emplace_back("Test");
                currentPacket.payload = lsSolution;

                pushSolutionToApiSocket(ls.requestId, currentPacket);
              
                continue;
              }
            
              ttp2::PeerSessionController::AddressedPacket addressedPacket;
              addressedPacket.ip = nameServerIp;
              addressedPacket.port = nameServerPort;
              addressedPacket.packet = currentPacket;
              peerSessionController->sendPacket(addressedPacket);
            }
          }
        }
      }
      
      if (peerSessionController->hasPacket()) {
        ttp2::PeerSessionController::AddressedPacket addressedPacket = peerSessionController->popReceivedPacket();
        ttp2::Packet::Packet packet = addressedPacket.packet;
        if (std::holds_alternative<ttp2::Packet::tdfs::CopyFromLocal>(packet.payload)) {
            ttp2::Packet::tdfs::CopyFromLocal copyFromLocal = std::get<ttp2::Packet::tdfs::CopyFromLocal>(packet.payload);
            this->fileManager->storeFile(copyFromLocal);
        }
        logger->log(tablog::DEBUG, "receivedPacket");
      }
    }

    if (networkingSession.joinable()) {
      networkingSession.join();
    }
  }

  void ComputeCore::pushApiRequest(ttp2::Packet::Packet packet) {
    std::lock_guard<std::mutex> lock(apiRequestMutex);
    apiRequests.emplace_back(packet);
  }

  void ComputeCore::registerApiSocket(const int socket) {
    std::lock_guard<std::mutex> lock(this->apiSolutionMutex);
    this->apiSolutionMap.try_emplace(socket, std::vector<ttp2::Packet::Packet>{});
  }

  void ComputeCore::pushSolutionToApiSocket(const int socket, ttp2::Packet::Packet packet) {
    std::lock_guard<std::mutex> lock(this->apiSolutionMutex);
    this->apiSolutionMap[socket].emplace_back(packet);
  }

  ttp2::Packet::Packet ComputeCore::getSolutionFromApiSocket(const int socket) {
    std::lock_guard<std::mutex> lock(this->apiSolutionMutex);
    if (this->apiSolutionMap[socket].size() > 0) {
      ttp2::Packet::Packet packet = this->apiSolutionMap[socket].at(0);
      this->apiSolutionMap[socket].erase(this->apiSolutionMap[socket].begin());
      return packet;
    } else {
      ttp2::Packet::Packet emptyPacket;
      return emptyPacket;
    }
  }

  int ComputeCore::getSolutionApiSocketSize(const int socket) {
    std::lock_guard<std::mutex> lock(this->apiSolutionMutex);
    return this->apiSolutionMap[socket].size();
  }

  bool ComputeCore::hasApiSocketSolution(const int socket) {
    std::lock_guard<std::mutex> lock(this->apiSolutionMutex);
    if (this->apiSolutionMap[socket].size() > 0) {
      return true;
    }
    return false;
  }

  void ComputeCore::removeRegisteredApiSocket(const int socket) {
    std::lock_guard<std::mutex> lock(this->apiSolutionMutex);
    this->apiSolutionMap.erase(socket);
  }
}
