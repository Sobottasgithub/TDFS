#ifndef FILE_LOCATION_MANAGER
#define FILE_LOCATION_MANAGER

#include <memory>
#include <tablog.h>

namespace tdfs {
  class FileLocationManager {
    public:
      FileLocationManager();
    private:
      std::shared_ptr<tablog::Tablog> logger;
  };
}

#endif
