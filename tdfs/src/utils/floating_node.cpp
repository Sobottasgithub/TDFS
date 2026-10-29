#include "floating_node.h"

#include <thread>

#include <tablog_registry.h>

namespace tdfs {
  FloatingNode::FloatingNode() {
    tablog::TablogRegistry* registry = &tablog::TablogRegistry::getInstance();
    this->logger = registry->get("TDFS");

    this->logger->log(tablog::DEBUG, "Init floating node");

    std::thread discoveryThread([this]() {
        udpDiscoverySession();
    });

    if (discoveryThread.joinable()) {
      discoveryThread.join();
    }
  }
}
