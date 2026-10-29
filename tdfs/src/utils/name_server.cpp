#include "name_server.h"

#include <server_discovery.h>
#include <thread>

#include "config/config.h"

#include <tablog_registry.h>

namespace tdfs {
  NameServer::NameServer() {
    tablog::TablogRegistry* registry = &tablog::TablogRegistry::getInstance();
    this->logger = registry->get("TDFS");

    this->logger->log(tablog::DEBUG, "Init nameServer");

    std::thread discoveryThread([this]() {
        udpDiscoverySession();
    });

    if (discoveryThread.joinable()) {
      discoveryThread.join();
    }    
  }

  void NameServer::udpDiscoverySession() {
    tdfs::Config& config = tdfs::Config::getInstance();

    std::string interface = config.getUdpDiscoveryInterface();
    int port = config.getUdpDiscoveryPort();
    auto serverDiscovery = std::make_shared<tud::ServerDiscovery>(interface, port, port, "TDFS");
    std::thread serverDiscoveryThread([serverDiscovery]() {
      serverDiscovery->discoveryCycle();
    });

    if (serverDiscoveryThread.joinable()) {
      serverDiscoveryThread.join();
    }
  }
}
