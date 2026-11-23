#include "name_server_core.h"

#include <network_helpers.h>
#include <peer_session_controller.h>
#include <thread>

#include "config/config.h"
#include "file_distribution_manager.h"

#include <tablog_registry.h>

namespace tdfs {
  NameServer::NameServer() {
    tablog::TablogRegistry* registry = &tablog::TablogRegistry::getInstance();
    this->logger = registry->get("TDFS");

    this->logger->log(tablog::DEBUG, "Init nameServer");

    std::promise<void> udpDiscoveryInitDone;
    std::future<void> udpDiscoveryInitFuture = udpDiscoveryInitDone.get_future();
    std::thread discoveryThread([this, udpInitDone = std::move(udpDiscoveryInitDone)]() mutable {
      udpDiscoverySession(std::move(udpInitDone));
    });

    std::promise<void> peerInitDone;
    std::future<void> peerInitFuture = peerInitDone.get_future();
    std::thread peerControlletThread([this, peerInitDone = std::move(peerInitDone)]() mutable {
      peerControllerSession(std::move(peerInitDone));
    });

    // INFO: Wait for Udp to finish setup before passing the instance
    udpDiscoveryInitFuture.wait();
    peerInitFuture.wait();
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

  void NameServer::peerControllerSession(std::promise<void> peerInitDone) {
    tdfs::Config& config = tdfs::Config::getInstance();

    const std::string interface = config.getUdpInterface();
    const int port = config.getUpdPort();
    const std::string ipV4 = ttp2::helpers::getLocalIpAddress(interface);
    this->peerSessionController = std::make_shared<ttp2::PeerSessionController>(ipV4.c_str(), port);
    auto peerSessionController = this->peerSessionController;
    std::thread networkingSession([peerSessionController]() {
      peerSessionController->networkingSession();
    });
    peerInitDone.set_value();

    if (networkingSession.joinable()) {
      networkingSession.join();
    }
  }
}
