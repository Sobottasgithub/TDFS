#ifndef COMPUTE_CORE_H
#define COMPUTE_CORE_H

#include <memory>

#include <tablog.h>

namespace tdfs {
  class ComputeCore {
    protected:
      std::shared_ptr<tablog::Tablog> logger;

      void udpDiscoverySession(); 
  };
}

#endif
