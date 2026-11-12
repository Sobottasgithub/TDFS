#include "file_loacation_manager.h"

#include <filesystem>

#include "config/config.h"

#include <tablog.h>
#include <tablog_registry.h>

namespace tdfs {
  FileLocationManager::FileLocationManager() {
    tablog::TablogRegistry* registry = &tablog::TablogRegistry::getInstance();
    this->logger = registry->get("TDFS");

    this->logger->log(tablog::DEBUG, "Init file location manager");

    tdfs::Config& config = tdfs::Config::getInstance();
    std::filesystem::path fileInformationDir = config.getFileInformationDir();
    this->logger->log(tablog::DEBUG, "File Information Dir: " + fileInformationDir.filename().string());
  }
}
