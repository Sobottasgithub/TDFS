#ifndef FILE_DISTRIBUTION_MANAGER
#define FILE_DISTRIBUTION_MANAGER

#include "file_loacation_manager.h"
#include "config/config.h"

#include <memory>
#include <arrow/csv/api.h>
#include <filesystem>

#include <peer_session_controller.h>
#include <server_discovery.h>
#include <tablog.h>

namespace tdfs {
  class FileDistributionManager {
    public:
      FileDistributionManager(std::shared_ptr<tud::ServerDiscovery> serverDiscovery, std::shared_ptr<ttp2::PeerSessionController> peerSessionController);

      std::shared_ptr<arrow::Table> simulateFileUpload(const std::filesystem::path filePath);
      bool addFile(std::shared_ptr<arrow::Table> inputFile, const std::filesystem::path fileName);

    private:
      std::shared_ptr<tablog::Tablog> logger;
      FileLocationManager fileLocationManager;
      std::shared_ptr<tud::ServerDiscovery> serverDiscovery;
      std::shared_ptr<ttp2::PeerSessionController> peerSessionController;
      tdfs::Config& config = tdfs::Config::getInstance();

      std::filesystem::path getUniqueFileName(std::filesystem::path fileName);
  };
}

#endif
