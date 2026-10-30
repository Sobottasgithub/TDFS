#include "config.h"

#include <bits/fs_path.h>
#include <stdexcept>

namespace tdfs {
  Config& Config::getInstance() {
    static Config instance;
    return instance;
  }

  void Config::configure(std::string configFilePath) {
    std::filesystem::path defaultConfigPath = std::filesystem::current_path() / "NameServer/src/utils/config/default_config.xml";
    pugi::xml_parse_result result = defaultConfigDocument.load_file(defaultConfigPath.c_str());
    if (!result) {
      throw std::invalid_argument("Error while loading default config");
    }

    std::filesystem::path userConfigFilePath = getTdfsConfigDir() / "config.xml";
    if (configFilePath.size() == 0 && std::filesystem::exists(userConfigFilePath)) { // Use config
      loadFile(userConfigFilePath);
    } else if (configFilePath.size() == 0) { // Create and use new config
      defaultConfigDocument.save_file(userConfigFilePath.c_str());
      configDocument.reset(defaultConfigDocument);
    } else { // Use provided config
      loadFile(std::filesystem::path(configFilePath));
    }
  }

  int Config::getReplications() {
    return getIntValueFromConfig("/Configuration/Replications");
  }

  int Config::getBlocksize() {
    return getIntValueFromConfig("/Configuration/Blocksize");
  }
  
  std::filesystem::path Config::getNameDir() {
    return getPathValueFromConfig("/Configuration/NameDirectory");
  }

  std::string Config::getUdpDiscoveryInterface() {
    return getStringValueFromConfig("/Configuration/UdpDiscovery/Interface");
  }

  int Config::getUdpDiscoveryPort() {
    return getIntValueFromConfig("/Configuration/UdpDiscovery/Port");
  }
 
  void Config::loadFile(std::filesystem::path path) {
    pugi::xml_parse_result result = configDocument.load_file(path.c_str());
    if (!result) {
      throw std::invalid_argument("Error while loading config");
    }
    configDocument.save_file(path.c_str()); // store potential corrections
  }

  int Config::getIntValueFromConfig(std::string key) {
    std::lock_guard<std::mutex> guard(this->configMutex);
    pugi::xpath_node value = configDocument.select_node(key.c_str());
    if (value) {
      return value.node().text().as_int();
    } else {
      throw std::invalid_argument(key + " has no value");
    }
  }

  std::string Config::getStringValueFromConfig(std::string key) {
    std::lock_guard<std::mutex> guard(this->configMutex);
    pugi::xpath_node value = configDocument.select_node(key.c_str());
    if (value) {
      return value.node().child_value();
    } else {
      throw std::invalid_argument(key + " has no value");
    }
  }

  bool Config::getBoolValueFromConfig(std::string key) {
    std::lock_guard<std::mutex> guard(this->configMutex);
    pugi::xpath_node value = configDocument.select_node(key.c_str());
    if (value) {
      return value.node().text().as_bool();
    } else {
      throw std::invalid_argument(key + " has no value");
    }
  }

  std::filesystem::path Config::getPathValueFromConfig(std::string key) {
    std::lock_guard<std::mutex> guard(this->configMutex);
    pugi::xpath_node value = configDocument.select_node(key.c_str());
    if (value) {
      return std::filesystem::path(value.node().child_value());
    } else {
      throw std::invalid_argument(key + " has no value");
    }
  }

  std::vector<std::filesystem::path> Config::getPathValuesFromConfig(std::string key) {
    std::lock_guard<std::mutex> guard(this->configMutex);
    std::vector<std::filesystem::path> paths;
    pugi::xpath_node_set dataDirNodes = configDocument.select_nodes(key.c_str());
    for (pugi::xpath_node dataDirNode : dataDirNodes) {
      paths.push_back(std::filesystem::path(dataDirNode.node().child_value()));
    }
    return paths;
  }

  std::filesystem::path Config::getTdfsConfigDir() {
    std::lock_guard<std::mutex> guard(this->configMutex);
    std::filesystem::path tdfsConfigDir = getConfigDir() / "tdfs/name";

    if (!std::filesystem::exists(tdfsConfigDir)) {
      std::filesystem::create_directories(tdfsConfigDir);
    }

    return tdfsConfigDir;
  }

  std::filesystem::path Config::getConfigDir() {
    const char* xdg_config = std::getenv("XDG_CONFIG_HOME");
    if (xdg_config && xdg_config[0] != '\0') {
      return std::filesystem::path(xdg_config);
    }

    const char* home = std::getenv("HOME");
    if (home && home[0] != '\0') {
      return std::filesystem::path(home) / ".config";
    }

    throw std::runtime_error("Unable to find config dir");
  }
}
