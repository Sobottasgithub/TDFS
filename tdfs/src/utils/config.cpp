#include "config.h"

namespace tdfs {
  Config& Config::getInstance() {
    static Config instance;
    return instance;
  }
}
