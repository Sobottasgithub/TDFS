#include "file_manager.h"

#include "config/config.h"

#include <tablog.h>
#include <tablog_registry.h>

namespace tdfs {
  FileManager::FileManager() {
    tablog::TablogRegistry* registry = &tablog::TablogRegistry::getInstance();
    this->logger = registry->get("TDFS");
    tdfs::Config& config = tdfs::Config::getInstance();
    this->rootFilePath = config.getDataDir();
  }

  void FileManager::storeFile(ttp2::Packet::File file) {
    
  }
}
