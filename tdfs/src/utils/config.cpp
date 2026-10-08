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

    std::filesystem::path configPath = getTdfsConfigDir();
    if (configFilePath.size() == 0 && std::filesystem::exists(configPath)) {
      // TODO: just load config.xml
    } else if (configFilePath.size() == 0) {
      // TODO: create new config.xml and copy values from standard_config.xml
    } else {
      // TODO: load new config file and compare to default config to look for mistakes
    }
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
