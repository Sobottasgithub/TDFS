#ifndef FILE_HELPERS_H
#define FILE_HELPERS_H

#include <memory>
#include <arrow/table.h>
#include <filesystem>

namespace tdfs::cli::helpers {
  std::shared_ptr<arrow::Table> simulateFileUpload(const std::filesystem::path filePath);
}

#endif
