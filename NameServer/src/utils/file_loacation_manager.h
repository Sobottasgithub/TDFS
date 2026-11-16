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

      bool createFile(const std::string fileName);
      
    private:
      std::shared_ptr<tablog::Tablog> logger;
      std::filesystem::path xmlFilePath;
      pugi::xml_document xmlFile;

      void initXmlFile(const std::filesystem::path fileInformationDir);
      bool storeXmlFile();
  };
}

#endif
