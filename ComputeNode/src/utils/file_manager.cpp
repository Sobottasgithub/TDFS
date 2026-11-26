#include "file_manager.h"

#include "config/config.h"

#include <filesystem>
#include <fstream>
#include <arrow/table.h>
#include <arrow/csv/writer.h>
#include <arrow/io/file.h>

#include <stdexcept>
#include <tablog.h>
#include <tablog_registry.h>
#include <tdfs_packet_types.h>

namespace tdfs {
  FileManager::FileManager() {
    tablog::TablogRegistry* registry = &tablog::TablogRegistry::getInstance();
    this->logger = registry->get("TDFS");
    tdfs::Config& config = tdfs::Config::getInstance();
    
    this->rootFilePath = config.getDataDir();
    if (!std::filesystem::exists(this->rootFilePath)) {
      logger->log(tablog::ERROR, "Data directory does not exist. Please provide a correct path.");
      throw std::runtime_error("Data directory does not exist");
    }
  }

  void FileManager::storeFile(ttp2::Packet::tdfs::CopyFromLocal copyFromLocal) {
    logger->log(tablog::INFO, "File: " + copyFromLocal.filePath +
                              " Start: " + std::to_string(copyFromLocal.start) +
                              " End: " + std::to_string(copyFromLocal.end));

    std::string snippetPath = this->rootFilePath.string() + copyFromLocal.filePath;

    // INFO: Create file
    if (!std::filesystem::exists(snippetPath)) {
      std::ofstream outfile(snippetPath.c_str());
      if (outfile.is_open()) {
        outfile.close();
      } else {
        logger->log(tablog::ERROR, "File snippet could not be created!");
        return;
      }
    } else {
      logger->log(tablog::ERROR, "File already exists!?");
      return;
    }

    // INFO: Store payload
    auto outfileResult = arrow::io::FileOutputStream::Open(snippetPath);
    if (!arrow::io::FileOutputStream::Open(snippetPath).ok()) {
      logger->log(tablog::ERROR, "Failed to open file!");
      return;
    }
    std::shared_ptr<arrow::io::FileOutputStream> outfile = *outfileResult;

    arrow::csv::WriteOptions writeOptions = arrow::csv::WriteOptions::Defaults();
    if (!arrow::csv::WriteCSV(*copyFromLocal.payload, writeOptions, outfile.get()).ok()) {
      logger->log(tablog::ERROR, "Failed to write Parquet table!");
      return;
    }

    if (!outfile->Close().ok()) {
      logger->log(tablog::ERROR, "Failed to close file!");
    }
  }
}
