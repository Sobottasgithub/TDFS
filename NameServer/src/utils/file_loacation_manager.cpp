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
    this->xmlFile.reset();
  }

  void FileLocationManager::initXmlFile(const std::filesystem::path fileInformationDir) {
    if (!fileInformationDir.filename().empty() && fileInformationDir.has_extension()) {
      throw std::invalid_argument("FileInformationDir expected path. A File was provided");
    }

    if (!std::filesystem::exists(fileInformationDir)) {
      this->logger->log(tablog::DEBUG, "Create file information parent dirs: " + fileInformationDir.string());
      std::filesystem::create_directories(fileInformationDir);
    }

    this->xmlFilePath = fileInformationDir / "file_information.xml";
    if (!std::filesystem::exists(this->xmlFilePath)) {
      this->logger->log(tablog::DEBUG, "Create information file: " + this->xmlFilePath.filename().string());
      std::ofstream InformationFile(this->xmlFilePath);
      InformationFile.close();
    }

    pugi::xml_parse_result loadResult = this->xmlFile.load_file(this->xmlFilePath.c_str());
    if (loadResult.status == pugi::status_no_document_element) {
      pugi::xml_node filesRoot = this->xmlFile.append_child("Files");
      if (!this->xmlFile.save_file(this->xmlFilePath.c_str())) {
        throw std::runtime_error("Failed to store Files in information file");
      }
    } else if (!loadResult) {
      throw std::invalid_argument("Error while loading file Information");
    }
  }

  bool FileLocationManager::storeXmlFile() {
    return this->xmlFile.save_file(this->xmlFilePath.c_str());
  }

  bool FileLocationManager::createFile(const std::string fileName) {
    if (!this->xmlFile.empty()) {
      const std::string fileXmlPath = "/Files/File[@name='" + fileName + "']";
      pugi::xpath_node match = this->xmlFile.select_node(fileXmlPath.c_str());

      // INFO: Only create File if it does not exist already
      if (!match) {
        // TODO: Create Files... <File name="">
        return true;        
      }
    }
    return false;
  }
}
