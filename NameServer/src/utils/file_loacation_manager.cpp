#include "file_loacation_manager.h"

#include "config/config.h"

#include <filesystem>
#include <fstream>
#include <stdexcept>

#include <string>
#include <tablog.h>
#include <tablog_registry.h>

namespace tdfs {
  FileLocationManager::FileLocationManager() {
    tablog::TablogRegistry* registry = &tablog::TablogRegistry::getInstance();
    this->logger = registry->get("TDFS");

    this->logger->log(tablog::DEBUG, "Init file location manager");

    tdfs::Config& config = tdfs::Config::getInstance();
    const std::filesystem::path fileInformationDir = config.getFileInformationDir();
    initXmlFile(fileInformationDir);
    informationFile.reset();
  }

  void FileLocationManager::initXmlFile(const std::filesystem::path fileInformationDir) {
    if (!fileInformationDir.filename().empty() && fileInformationDir.has_extension()) {
      throw std::invalid_argument("FileInformationDir expected path. A File was provided");
    }

    if (!std::filesystem::exists(fileInformationDir)) {
      this->logger->log(tablog::DEBUG, "Create file information parent dirs: " + fileInformationDir.string());
      std::filesystem::create_directories(fileInformationDir);
    }

    const std::filesystem::path fileInformationFile = fileInformationDir / "file_information.xml";
    if (!std::filesystem::exists(fileInformationFile)) {
      this->logger->log(tablog::DEBUG, "Create information file: " + fileInformationFile.filename().string());
      std::ofstream InformationFile(fileInformationFile);
      InformationFile.close();
    }

    pugi::xml_parse_result loadResult = this->informationFile.load_file(fileInformationFile.c_str());
    if (loadResult.status == pugi::status_no_document_element) {
      pugi::xml_node filesRoot = this->informationFile.append_child("Files");
      if (!this->informationFile.save_file(fileInformationFile.c_str())) {
        throw std::runtime_error("Failed to store Files in information file");
      }
    } else if (!loadResult) {
      throw std::invalid_argument("Error while loading file Information");
    }
  }

  bool FileLocationManager::createFile(const std::string fileName) {
    if (!informationFile.empty()) {
      const std::string fileXmlPath = "/Files/File[@name='" + fileName + "']";
      pugi::xpath_node match = this->informationFile.select_node(fileXmlPath.c_str());

      // INFO: Only create File if it does not exist already
      if (!match) {
        // TODO: Create Files... <File name="">
        return true;        
      }
    }
    return false;
  }
}
