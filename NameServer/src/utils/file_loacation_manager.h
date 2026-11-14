#ifndef FILE_LOCATION_MANAGER
#define FILE_LOCATION_MANAGER

#include <memory>
#include <filesystem>
#include <pugixml.hpp>

#include <tablog.h>

namespace tdfs {
  class FileLocationManager {
    public:
      FileLocationManager();
    private:
      std::shared_ptr<tablog::Tablog> logger;
      pugi::xml_document informationFile;

      void initInformationFile(const std::filesystem::path fileInformationDir);
  };
}

#endif
