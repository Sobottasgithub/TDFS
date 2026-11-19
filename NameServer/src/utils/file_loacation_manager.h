#ifndef FILE_LOCATION_MANAGER
#define FILE_LOCATION_MANAGER

#include <memory>
#include <filesystem>
#include <pugixml.hpp>
#include <vector>

#include <tablog.h>

namespace tdfs {
  class FileLocationManager {
    public:
      FileLocationManager();

      bool createFile(const std::string fileName);
      bool createSnippet(const std::string fileName, const unsigned int size,
                         const unsigned int start, const unsigned int end);
      bool createReplication(const std::string fileName, const unsigned int id,
                             const std::filesystem::path snippetStoragePath,
                             const std::string storageNodeIpv4, const unsigned int port);
      std::vector<std::filesystem::path> getFileNames();
      
    private:
      std::shared_ptr<tablog::Tablog> logger;
      std::filesystem::path xmlFilePath;
      pugi::xml_document xmlFile;

      void initXmlFile(const std::filesystem::path fileInformationDir);
      bool storeXmlFile();
  };
}

#endif
