#include "compute_core.h"

#include "config/config.h"

#include <network_helpers.h>
#include <peer_session_controller.h>
#include <client_discovery.h>
#include <tablog.h>
#include <thread>
#include <algorithm>

#include <tablog_registry.h>

namespace tdfs {
  ComputeCore::ComputeCore() {
    tablog::TablogRegistry* registry = &tablog::TablogRegistry::getInstance();
    this->logger = registry->get("TDFS");

    this->logger->log(tablog::DEBUG, "Init floating node");

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
    auto clientDiscovery = std::make_shared<tud::ClientDiscovery>(interface, port, port, "TDFS");
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

    while (true) {
      if (peerSessionController->hasPacket()) {
        peerSessionController->popReceivedPacket();
        logger->log(tablog::DEBUG, "receivedPacket");
      }
    }

    if (networkingSession.joinable()) {
      networkingSession.join();
    }
  }
}
