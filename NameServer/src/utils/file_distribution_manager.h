#ifndef FILE_DISTRIBUTION_MANAGER
#define FILE_DISTRIBUTION_MANAGER

#include <memory>
#include <arrow/csv/api.h>

namespace tdfs {
  class FileDistributionManager {
    public:
      FileDistributionManager();

      std::shared_ptr<arrow::Table> simulateFileUpload();
            
      bool addFile();
  };
}

#endif
