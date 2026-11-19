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
      pugi::xml_node fileRoot = this->xmlFile.child("Files");
      const std::string fileXmlPath = "File[@name='" + fileName + "']";
      pugi::xpath_node match = fileRoot.select_node(fileXmlPath.c_str());

      // INFO: Only create File if it does not exist already
      if (!match) {
        pugi::xml_node newFile = fileRoot.append_child("File");
        newFile.append_attribute("name") = fileName;

        if (storeXmlFile()) {
          return true;
        }
      }
    }
    return false;
  }

  bool FileLocationManager::createSnippet(const std::string fileName, const unsigned int size,
                                          const unsigned int start, const unsigned int end) {
    if (!this->xmlFile.empty()) {
      pugi::xml_node fileRoot = this->xmlFile.child("Files");
      const std::string fileXmlPath = "File[@name='" + fileName + "']";
      pugi::xpath_node selectedFilePath = fileRoot.select_node(fileXmlPath.c_str());

      if (selectedFilePath) {
        pugi::xml_node file = fileRoot.select_node(fileXmlPath.c_str()).node();
        pugi::xpath_node_set snippets = file.select_nodes("Snippet");
        int lastSnippetId = -1; // -1 to determin if snippet 0 already exists or not
        // INFO: Check if snippet with xStart and xEnd already exists
        for (pugi::xpath_node snippetPath : snippets) {
          pugi::xml_node snippet = snippetPath.node();

          pugi::xpath_node xStart = snippet.select_node("XStart");
          if (xStart) {
            const unsigned int snippetXStart = xStart.node().text().as_int();
            if (snippetXStart == start) {
              return false;
            } 
          }
          pugi::xpath_node xEnd = snippet.select_node("XEnd");
          if (xEnd) {
            const unsigned int snippetXEnd = xEnd.node().text().as_int();
            if (snippetXEnd == end) {
              return false;
            } 
          }
          // INFO: Get id
          pugi::xml_attribute idAttribute = snippet.attribute("id");
          if (idAttribute) {
            const int currentId = idAttribute.as_int();
            if (currentId > lastSnippetId) {
              lastSnippetId = currentId;
            } 
          }
        }
        // INFO: Create snippet
        pugi::xml_node newSnippet = file.append_child("Snippet");
        newSnippet.append_attribute("id") = lastSnippetId+1;

        pugi::xml_node xStartNode = newSnippet.append_child("XStart");
        xStartNode.text().set(start);
        pugi::xml_node xEndNode = newSnippet.append_child("XEnd");
        xEndNode.text().set(end);
        pugi::xml_node sizeNode = newSnippet.append_child("Size");
        sizeNode.text().set(size);

        if (storeXmlFile()) {
          return true;
        }
      }
    }
    return false;
  }

  bool FileLocationManager::createReplication(const std::string fileName,
                                              const unsigned int id,
                                              const std::filesystem::path snippetStoragePath,
                                              const std::string storageNodeIpv4,
                                              const unsigned int port) {
    if (!this->xmlFile.empty()) {
      pugi::xml_node fileRoot = this->xmlFile.child("Files");
      const std::string fileXmlPath = "File[@name='" + fileName + "']";
      pugi::xpath_node selectedFilePath = fileRoot.select_node(fileXmlPath.c_str());

      if (selectedFilePath) {
        pugi::xml_node file = fileRoot.select_node(fileXmlPath.c_str()).node();
        const std::string snippetXmlPath = "Snippet[@id='" + std::to_string(id) + "']";
        pugi::xpath_node selectedSnippetPath = file.select_node(snippetXmlPath.c_str());

        if (selectedSnippetPath) {
          pugi::xml_node snippet = file.select_node(snippetXmlPath.c_str()).node();

          // INFO: Get last replication id
          int lastReplicationId = -1;
          for (pugi::xpath_node replicationPath : snippet) {
            pugi::xml_node replication = replicationPath.node();
            std::string nodeName = replication.name();
            if (!nodeName.compare("Replication")) {
              pugi::xml_attribute idAttribute = replication.attribute("id");
              if (idAttribute) {
                const int currentId = idAttribute.as_int();
                if (currentId > lastReplicationId) {
                  lastReplicationId = currentId;
                } 
              }
            }
          }

          pugi::xml_node newReplication = snippet.append_child("Replication");
          newReplication.append_attribute("id") = lastReplicationId+1;

          pugi::xml_node newStorageNode = newReplication.append_child("StorageNode");
          pugi::xml_node storageNodeIp = newStorageNode.append_child("Ip");
          storageNodeIp.text().set(storageNodeIpv4);
          pugi::xml_node storageNodePort = newStorageNode.append_child("Port");
          storageNodePort.text().set(storageNodePort);

          pugi::xml_node pathNode = newReplication.append_child("StoragePath");
          pathNode.text().set(snippetStoragePath.string());

          if (storeXmlFile()) {
            return true;
          }
        }
      }
    }
    return false;
  }

  std::vector<std::filesystem::path> FileLocationManager::getFileNames() {
    std::vector<std::filesystem::path> fileNames;

    if (!this->xmlFile.empty()) {
      pugi::xml_node fileRoot = this->xmlFile.child("Files");
      for (pugi::xpath_node filePath : fileRoot) {
        pugi::xml_node file = filePath.node();
        std::string fileXmlName = file.name();
        if (!fileXmlName.compare("File")) {
          pugi::xml_attribute nameAttribute = file.attribute("name");
          if (nameAttribute) {
            fileNames.emplace_back(nameAttribute.as_string());
          }
        }
      }
    }
    return fileNames;
  }
}
