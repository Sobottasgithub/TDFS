#include "name_server.h"

#include <tablog_registry.h>

namespace tdfs {
  NameServer::NameServer() {
    tablog::TablogRegistry* registry = &tablog::TablogRegistry::getInstance();
    this->logger = registry->get("TDFS");

    this->logger->log(tablog::DEBUG, "Init nameServer");
  }
}
