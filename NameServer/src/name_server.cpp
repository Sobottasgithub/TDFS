#include "utils/name_server_core.h"
#include "utils/config/config.h"

#include <thread>

#include <tablog.h>
#include <tablog_registry.h>

int main() {
  tablog::TablogRegistry* registry = &tablog::TablogRegistry::getInstance();
  std::shared_ptr<tablog::Tablog> logger = std::make_shared<tablog::Tablog>();
  logger->configure("TDFS", true);
  registry->registerLogger("TDFS", logger);

  tdfs::Config& config = tdfs::Config::getInstance();
  config.configure();

  std::thread nameServerThread([]() {
    tdfs::NameServer nameServer;
  });

  if (nameServerThread.joinable()) {
    nameServerThread.join();
  }

  return 0;
}
