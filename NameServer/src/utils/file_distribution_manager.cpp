#include "file_distribution_manager.h"

#include <arrow/csv/options.h>
#include <arrow/io/api.h>

#include <filesystem>
#include <iostream>
#include <string>

namespace tdfs {
  FileDistributionManager::FileDistributionManager() {
    std::cout << "Provide file path: ";
    std::filesystem::path filePath;
    std::cin >> filePath;

    addFile(simulateFileUpload(filePath), filePath);
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
     std::cout << "Error while instantiating TableReader!" << std::endl;
    }
    std::shared_ptr<arrow::csv::TableReader> reader = *maybeReader;

    arrow::Result<std::shared_ptr<arrow::Table>> maybeTable = reader->Read();
    if (!maybeTable.ok()) {
      std::cout << "Error while read table from CSV file!" << std::endl;
    }
    std::shared_ptr<arrow::Table> table = *maybeTable;
    return table;
  }

  bool FileDistributionManager::addFile(std::shared_ptr<arrow::Table> inputFile, const std::filesystem::path fileName) {
    std::filesystem::path newFileName = getUniqueFileName(fileName);
    this->fileLocationManager.createFile(newFileName);
    // this->fileLocationManager.createSnippet("Test", 100, 0, 100);
    // this->fileLocationManager.createSnippet("Test", 100, 101, 200);
    // this->fileLocationManager.createReplication("Test", 0, "test/path/snippet1.csv", "127.0.0.1", 4000);

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
