#ifndef CONFIG_H
#define CONFIG_H

#include <pugixml.hpp>
#include <filesystem>
#include <vector>

namespace tdfs {
  class Config {
    public:
      Config(const Config&) = delete;
      Config& operator=(const Config&) = delete;
      Config(Config&&) = delete;
      Config& operator=(Config&&) = delete;

      static Config& getInstance();

      void configure(std::string configFilePath = "");

      int getConfigReplications();
      int getConfigBlocksize();
      std::filesystem::path getConfigNameDir();
      std::vector<std::filesystem::path> getConfigDataDir();
      bool getConfigIsFloatingNode();
      bool getConfigIsNameNode();
      
    private:
      pugi::xml_document defaultConfigDocument;
      pugi::xml_document configDocument;

      Config() = default;
      
      void loadFile(std::filesystem::path path);
      void verifyConfig();

      int getIntValueFromConfig(std::string key);
      bool getBoolValueFromConfig(std::string key);
      std::filesystem::path getPathValueFromConfig(std::string key);
      std::vector<std::filesystem::path> getPathValuesFromConfig(std::string key);

      std::filesystem::path getTdfsConfigDir();
      std::filesystem::path getConfigDir();
  };
}

#endif
