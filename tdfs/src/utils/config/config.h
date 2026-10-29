#ifndef CONFIG_H
#define CONFIG_H

#include <pugixml.hpp>
#include <filesystem>
#include <vector>
#include <mutex>

namespace tdfs {
  class Config {
    public:
      Config(const Config&) = delete;
      Config& operator=(const Config&) = delete;
      Config(Config&&) = delete;
      Config& operator=(Config&&) = delete;

      static Config& getInstance();

      void configure(std::string configFilePath = "");

      int getReplications();
      int getBlocksize();
      std::filesystem::path getNameDir();
      std::vector<std::filesystem::path> getDataDir();
      bool getIsFloatingNode();
      bool getIsNameNode();
      std::string getTcpServerInterface();
      int getTcpServerPort();
      std::string getUdpDiscoveryInterface();
      int getUdpDiscoveryPort();

      std::filesystem::path getTdfsConfigDir();
      
    private:
      std::mutex configMutex;

      pugi::xml_document defaultConfigDocument;
      pugi::xml_document configDocument;

      Config() = default;
      
      void loadFile(std::filesystem::path path);
      void verifyConfig();

      int getIntValueFromConfig(std::string key);
      std::string getStringValueFromConfig(std::string key);
      bool getBoolValueFromConfig(std::string key);
      std::filesystem::path getPathValueFromConfig(std::string key);
      std::vector<std::filesystem::path> getPathValuesFromConfig(std::string key);

      std::filesystem::path getConfigDir();
  };
}

#endif
