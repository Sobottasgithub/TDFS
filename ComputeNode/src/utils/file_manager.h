#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include <filesystem>
#include <packet_types.h>

#include <tablog.h>

namespace tdfs {
  class FileManager {
    public:
      FileManager();

      void storeFile(ttp2::Packet::File file);
      
    private:
      std::shared_ptr<tablog::Tablog> logger;
      std::filesystem::path rootFilePath;
  };
}

#endif
