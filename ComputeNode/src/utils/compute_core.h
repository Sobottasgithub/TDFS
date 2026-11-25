#ifndef COMPUTE_CORE_H
#define COMPUTE_CORE_H

#include "file_manager.h"
#include <memory>

#include <tablog.h>

namespace tdfs {
  class ComputeCore {
    public:
      ComputeCore();
      
    private:
      std::shared_ptr<tablog::Tablog> logger;
      std::shared_ptr<FileManager> fileManager;

      void udpDiscoverySession();
      void peerControllerSession();
  };
}

#endif
