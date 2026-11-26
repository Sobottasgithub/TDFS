#include "file_distribution_manager.h"

#include <arrow/csv/options.h>
#include <arrow/io/api.h>
#include <arrow/api.h>

#include <arrow/table.h>
#include <filesystem>
#include <memory>
#include <packet_types.h>
#include <peer_session_controller.h>
#include <string>
#include <tablog.h>
#include <tablog_registry.h>

namespace tdfs {
  FileDistributionManager::FileDistributionManager(std::shared_ptr<tud::ServerDiscovery> serverDiscovery, std::shared_ptr<ttp2::PeerSessionController> peerSessionController) {
    tablog::TablogRegistry* registry = &tablog::TablogRegistry::getInstance();
    this->logger = registry->get("TDFS");

    this->serverDiscovery = serverDiscovery;
    this->peerSessionController = peerSessionController;

    this->logger->log(tablog::DEBUG, "Done!");
  }

  bool FileDistributionManager::addFile(std::shared_ptr<arrow::Table> inputFile, const std::filesystem::path fileName) {
    std::filesystem::path newFileName = getUniqueFileName(fileName);
    this->fileLocationManager.createFile(newFileName);

    std::vector<std::string> computeNodes = this->serverDiscovery->getDiscoveredAddresses();
    logger->log(tablog::DEBUG, "ComputeNodes: " + std::to_string(computeNodes.size()));

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
    const int nodePort = config.getNodePort();
    int64_t offset = 0;
    int snippetIndex = 0;
    int nodeReplicationDistributionIndex = 0;
    while (offset < totalRows) {
        int64_t currentBatchSize = std::min(blockSize, totalRows - offset);
        this->fileLocationManager.createSnippet(newFileName, currentBatchSize, offset, (offset + currentBatchSize - 1));
        std::shared_ptr<arrow::Table> slicedTableBlock = inputFile->Slice(offset, currentBatchSize);

        logger->log(tablog::DEBUG, "Processing rows " + std::to_string(offset) + " to " + std::to_string(offset + currentBatchSize - 1));
        std::filesystem::path snippetFilePath = newFileName.parent_path().string() + "/" + newFileName.stem().string() +
                                                "-snippet-" + std::to_string(snippetIndex) + newFileName.extension().string();
        logger->log(tablog::DEBUG, "fileName: " + snippetFilePath.string());
        for (int replicationIndex = 0; replicationIndex < currentReplicationCount; replicationIndex++) {
          const std::string nodeIpV4 = computeNodes.at(nodeReplicationDistributionIndex);

          // INFO: Create Node Packet
          ttp2::Packet::File file;
          file.filePath = snippetFilePath;
          file.start = offset;
          file.end = (offset + currentBatchSize - 1);
          file.payload = slicedTableBlock;
          ttp2::Packet::Packet packet;
          packet.payload = file;
          ttp2::PeerSessionController::AddressedPacket addressedPacket;
          addressedPacket.ip = nodeIpV4.c_str();
          addressedPacket.port = nodePort;
          addressedPacket.packet = packet;
          this->peerSessionController->sendPacket(addressedPacket);
          
          const std::string realFilePath = newFileName;
          this->fileLocationManager.createReplication(newFileName, snippetIndex, snippetFilePath, nodeIpV4);

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
