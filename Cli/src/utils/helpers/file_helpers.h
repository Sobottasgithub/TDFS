#ifndef FILE_HELPERS_H
#define FILE_HELPERS_H

#include <memory>
#include <arrow/table.h>
#include <filesystem>

namespace tdfs::cli {
  namespace helpers {
    std::shared_ptr<arrow::Table> openFile(const std::filesystem::path filePath);
  }
}

#endif
