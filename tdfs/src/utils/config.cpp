#include "config.h"

#include <bits/fs_path.h>
#include <stdexcept>

namespace tdfs {
  Config& Config::getInstance() {
    static Config instance;
    return instance;
  }

  void Config::configure(std::string configFilePath) {
    std::filesystem::path defaultConfigPath = std::filesystem::current_path() / "tdfs/src/utils/config/default_config.xml";
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

  int Config::getConfigReplications() {
    return getIntValueFromConfig("/configuration/property[name='replications']/value");
  }

  int Config::getConfigBlocksize() {
    return getIntValueFromConfig("/configuration/property[name='blocksize']/value");
  }
  
  std::filesystem::path Config::getConfigNameDir() {
    return getPathValueFromConfig("/configuration/property[name='name.directory']/value");
  }

  std::vector<std::filesystem::path> Config::getConfigDataDir() {
    return getPathValuesFromConfig("/configuration/property[name='data.directory']/value");
  }

  bool Config::getConfigIsFloatingNode() {
    return getBoolValueFromConfig("/configuration/property[name='floating.node']/value");
  }
  
  bool Config::getConfigIsNameNode() {
    return getBoolValueFromConfig("/configuration/property[name='name.node']/value");
  }
 
  void Config::loadFile(std::filesystem::path path) {
    pugi::xml_parse_result result = configDocument.load_file(path.c_str());
    if (!result) {
      throw std::invalid_argument("Error while loading config");
    }
    verifyConfig();
    configDocument.save_file(path.c_str()); // store potential corrections
  }

  void Config::verifyConfig() {
    pugi::xml_node defaultRoot = defaultConfigDocument.child("configuration");
    pugi::xml_node root = configDocument.child("configuration");
    if (!root) {
      throw std::runtime_error("<configuration> missing in xml");
    }

    for (pugi::xml_node defaultProperty : defaultRoot.children("property")) {
      bool containsProperty = false;
      for (pugi::xml_node property : root.children("property")) {
        std::string defaultName = defaultProperty.child_value("name");
        std::string name = property.child_value("name");
        
        if (!defaultName.compare(name)) {
          if (!property.child("value")) {
            // Replace missing value
            pugi::xml_node value = property.append_child("value");
            value.append_child(pugi::node_pcdata).set_value(defaultProperty.child_value("value"));
          }
          containsProperty = true;
          break;
        }
      }
      if (!containsProperty) {
        // Replace missing property
        pugi::xml_node property = root.append_child("property");
        pugi::xml_node name = property.append_child("name");
        name.append_child(pugi::node_pcdata).set_value(defaultProperty.child_value("name"));
        pugi::xml_node value = property.append_child("value");
        value.append_child(pugi::node_pcdata).set_value(defaultProperty.child_value("value"));
      }
    }    
  }

  int Config::getIntValueFromConfig(std::string key) {
    pugi::xpath_node value = configDocument.select_node(key.c_str());
    if (value) {
      return value.node().text().as_int();
    } else {
      throw std::invalid_argument(key + " has no value");
    }
  }

  bool Config::getBoolValueFromConfig(std::string key) {
    pugi::xpath_node value = configDocument.select_node(key.c_str());
    if (value) {
      return value.node().text().as_bool();
    } else {
      throw std::invalid_argument(key + " has no value");
    }
  }

  std::filesystem::path Config::getPathValueFromConfig(std::string key) {
    pugi::xpath_node value = configDocument.select_node(key.c_str());
    if (value) {
      return std::filesystem::path(value.node().child_value());
    } else {
      throw std::invalid_argument(key + " has no value");
    }
  }

  std::vector<std::filesystem::path> Config::getPathValuesFromConfig(std::string key) {
    std::vector<std::filesystem::path> paths;
    pugi::xpath_node_set dataDirNodes = configDocument.select_nodes(key.c_str());
    for (pugi::xpath_node dataDirNode : dataDirNodes) {
      paths.push_back(std::filesystem::path(dataDirNode.node().child_value()));
    }
    return paths;
  }

  std::filesystem::path Config::getTdfsConfigDir() {
    std::filesystem::path tdfsConfigDir = getConfigDir() / "tdfs";

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
