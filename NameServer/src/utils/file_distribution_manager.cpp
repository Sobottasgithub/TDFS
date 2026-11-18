#include "file_distribution_manager.h"

#include <arrow/csv/options.h>
#include <arrow/io/api.h>
#include <filesystem>

#include <iostream>

namespace tdfs {
  FileDistributionManager::FileDistributionManager() {
    simulateFileUpload();
  }

  std::shared_ptr<arrow::Table> FileDistributionManager::simulateFileUpload() {
    std::cout << "Provide file path: ";
    std::filesystem::path filePath;
    std::cin >> filePath;
    
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
}
