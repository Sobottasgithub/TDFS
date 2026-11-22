#include "file_distribution_manager.h"

#include <arrow/csv/options.h>
#include <arrow/io/api.h>
#include <arrow/api.h>

#include <arrow/table.h>
#include <filesystem>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <tablog.h>
#include <tablog_registry.h>

namespace tdfs {
  FileDistributionManager::FileDistributionManager(std::shared_ptr<tud::ServerDiscovery> serverDiscovery) {
    tablog::TablogRegistry* registry = &tablog::TablogRegistry::getInstance();
    this->logger = registry->get("TDFS");

    this->serverDiscovery = serverDiscovery;

    std::cout << "Provide file path: ";
    std::filesystem::path filePath;
    std::cin >> filePath;

    addFile(simulateFileUpload(filePath), filePath);

    this->logger->log(tablog::DEBUG, "Done!");
  }

  std::shared_ptr<arrow::Table> FileDistributionManager::simulateFileUpload(const std::filesystem::path filePath) {    
    arrow::io::IOContext ioContext = arrow::io::default_io_context();

    arrow::Result<std::shared_ptr<arrow::io::ReadableFile>> maybeFile = arrow::io::ReadableFile::Open(filePath);
    std::shared_ptr<arrow::io::InputStream> fileInput = *maybeFile;

    arrow::csv::ReadOptions readOptions = arrow::csv::ReadOptions::Defaults();
    arrow::csv::ParseOptions parseOptions = arrow::csv::ParseOptions::Defaults();
    arrow::csv::ConvertOptions convertOptions = arrow::csv::ConvertOptions::Defaults();

    arrow::Result<std::shared_ptr<arrow::csv::TableReader>> maybeReader = arrow::csv::TableReader::Make(ioContext,
                                                    fileInput,
                                                    readOptions,
                                                    parseOptions,
                                                    convertOptions);
    if (!maybeReader.ok()) {
     this->logger->log(tablog::ERROR, "Error while instantiating TableReader!");
     throw std::runtime_error("Error while instantiating TableReader!");
    }
    std::shared_ptr<arrow::csv::TableReader> reader = *maybeReader;

    arrow::Result<std::shared_ptr<arrow::Table>> maybeTable = reader->Read();
    if (!maybeTable.ok()) {
      this->logger->log(tablog::ERROR, "Error while read table from CSV file!");
      throw std::runtime_error("Error while read table from CSV file!");
    }
    std::shared_ptr<arrow::Table> table = *maybeTable;
    return table;
  }

  bool FileDistributionManager::addFile(std::shared_ptr<arrow::Table> inputFile, const std::filesystem::path fileName) {
    std::filesystem::path newFileName = getUniqueFileName(fileName);
    this->fileLocationManager.createFile(newFileName);

    // WARNING: This only simulates UDP discovery
    // TODO: Replace with real UDP discovered nodes
    //       And check if it is a compute node
    std::vector<std::string> computeNodes = {"127.0.0.1", "127.0.0.2", "127.0.0.3"};
    
    int currentReplicationCount = config.getReplications();
    if (computeNodes.size() < currentReplicationCount) {
      bool displayedWarning = false;
      while (computeNodes.size() < currentReplicationCount) {
        if (!displayedWarning) {
          this->logger->log(tablog::WARNING, "Node count was not sufficient to satisfy replication count. Scaling down replication count...");
          displayedWarning = true;
        }
        currentReplicationCount--;
      }
    }

    const int64_t blockSize = config.getBlocksize();
    const int64_t totalRows = inputFile->num_rows();
    int64_t offset = 0;
    int snippetIndex = 0;
    int nodeReplicationDistributionIndex = 0;
    while (offset < totalRows) {
        int64_t currentBatchSize = std::min(blockSize, totalRows - offset);
        this->fileLocationManager.createSnippet(newFileName, currentBatchSize, offset, (offset + currentBatchSize - 1));
        std::shared_ptr<arrow::Table> slicedTableBlock = inputFile->Slice(offset, currentBatchSize);

        logger->log(tablog::DEBUG, "Processing rows " + std::to_string(offset) + " to " + std::to_string(offset + currentBatchSize - 1));
        for (int replicationIndex = 0; replicationIndex < currentReplicationCount; replicationIndex++) {
          const std::string nodeIpV4 = computeNodes.at(nodeReplicationDistributionIndex);
          // TODO: Send slicedTableBlock to compute node
          const std::string realFilePath = newFileName; // TODO: Replace with compute nodes real file location later
          this->fileLocationManager.createReplication(newFileName, snippetIndex, realFilePath, nodeIpV4);

          if (nodeReplicationDistributionIndex + 1 == computeNodes.size()) {
            nodeReplicationDistributionIndex = 0;
          } else {
            nodeReplicationDistributionIndex++;
          }
        }

        snippetIndex++;
        offset += currentBatchSize;
    }
    
    return true;
  }

  std::filesystem::path FileDistributionManager::getUniqueFileName(std::filesystem::path fileName) {
    std::vector<std::filesystem::path> existingFileNames = this->fileLocationManager.getFileNames();

    bool uniqueNameNotFound = true;
    int fileNameDuplicateExtention = 0;
    std::filesystem::path newFileName = fileName;
    while(uniqueNameNotFound) {
      bool fileNameFound = false;
      for (int index = 0; index < existingFileNames.size(); index++) {
        if (!existingFileNames.at(index).compare(newFileName))  {
          fileNameFound = true;
        }
      }

      if (fileNameFound) {
        fileNameDuplicateExtention++;
        newFileName = fileName.parent_path().string() + "/" + 
                      fileName.stem().string() +
                      "(" + std::to_string(fileNameDuplicateExtention) + ")" +
                      fileName.extension().string();
      } else {
        uniqueNameNotFound = false;
      }
    }

    return newFileName;
  }
}
