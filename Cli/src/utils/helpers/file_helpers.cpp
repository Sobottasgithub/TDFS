#include "file_helpers.h"

#include <arrow/csv/options.h>
#include <arrow/io/api.h>
#include <arrow/api.h>
#include <arrow/csv/api.h>

namespace tdfs::cli::helpers {
  std::shared_ptr<arrow::Table> openFile(const std::filesystem::path filePath) {    
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
     throw std::runtime_error("Error while instantiating TableReader!");
    }
    std::shared_ptr<arrow::csv::TableReader> reader = *maybeReader;

    arrow::Result<std::shared_ptr<arrow::Table>> maybeTable = reader->Read();
    if (!maybeTable.ok()) {
      throw std::runtime_error("Error while read table from CSV file!");
    }
    std::shared_ptr<arrow::Table> table = *maybeTable;
    return table;
  }
}
