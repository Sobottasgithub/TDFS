#ifndef FILE_DISTRIBUTION_MANAGER
#define FILE_DISTRIBUTION_MANAGER

#include "file_loacation_manager.h"
#include "config/config.h"

#include <memory>
#include <arrow/csv/api.h>
#include <filesystem>

#include <tablog.h>

namespace tdfs {
  class FileDistributionManager {
    public:
      FileDistributionManager();

      std::shared_ptr<arrow::Table> simulateFileUpload(const std::filesystem::path filePath);
      bool addFile(std::shared_ptr<arrow::Table> inputFile, const std::filesystem::path fileName);

    private:
      std::shared_ptr<tablog::Tablog> logger;
      FileLocationManager fileLocationManager;
      tdfs::Config& config = tdfs::Config::getInstance();

      std::filesystem::path getUniqueFileName(std::filesystem::path fileName);
  };
}

#endif
