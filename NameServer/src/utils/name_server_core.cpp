#include "name_server_core.h"

#include <thread>

#include "config/config.h"
#include "file_distribution_manager.h"

#include <tablog_registry.h>

namespace tdfs {
  NameServer::NameServer() {
    tablog::TablogRegistry* registry = &tablog::TablogRegistry::getInstance();
    this->logger = registry->get("TDFS");

    this->logger->log(tablog::DEBUG, "Init nameServer");

    std::promise<void> udpInitDone;
    std::future<void> udpInitFuture = udpInitDone.get_future();
    std::thread discoveryThread([this, udpInitDone = std::move(udpInitDone)]() mutable {
        udpDiscoverySession(std::move(udpInitDone));
    });

    // INFO: Wait for Udp to finish setup before passing the instance
    udpInitFuture.wait();
    FileDistributionManager fileDistributionManager(this->serverDiscovery);

    if (discoveryThread.joinable()) {
      discoveryThread.join();
    }    
  }

  void NameServer::udpDiscoverySession(std::promise<void> udpInitDone) {
    tdfs::Config& config = tdfs::Config::getInstance();

    std::string interface = config.getUdpDiscoveryInterface();
    int port = config.getUdpDiscoveryPort();
    this->serverDiscovery = std::make_shared<tud::ServerDiscovery>(interface, port, port, "TDFS");
    auto serverDiscovery = this->serverDiscovery;
    std::thread serverDiscoveryThread([serverDiscovery]() {
      serverDiscovery->discoveryCycle();
    });
    udpInitDone.set_value();

    if (serverDiscoveryThread.joinable()) {
      serverDiscoveryThread.join();
    }
  }
}
