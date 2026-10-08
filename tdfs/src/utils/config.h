#ifndef CONFIG_H
#define CONFIG_H

#include <pugixml.hpp>
#include <filesystem>

namespace tdfs {
  class Config {
    public:
      Config(const Config&) = delete;
      Config& operator=(const Config&) = delete;
      Config(Config&&) = delete;
      Config& operator=(Config&&) = delete;

      static Config& getInstance();

      void configure(std::string configFilePath = "");
      
    private:
      pugi::xml_document configDocument;
      pugi::xml_document defaultConfigDocument;

      Config() = default;
      std::filesystem::path getTdfsConfigDir();
      std::filesystem::path getConfigDir();
  };
}

#endif
