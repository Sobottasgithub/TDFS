#include "compute_core.h"

#include "config/config.h"

#include <client_discovery.h>
#include <thread>
#include <algorithm>

namespace tdfs {
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
    }

    if (clientDiscoveryThread.joinable()) {
      clientDiscoveryThread.join();
    }
  }
}
