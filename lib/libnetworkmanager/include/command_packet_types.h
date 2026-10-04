#ifndef COMMAND_PACKET_TYPES_H
#define COMMAND_PACKET_TYPES_H

#include <string>
#include <vector>

namespace tdfs {
  namespace network_manager {
    struct Ls {
      std::string directory;
    };

    struct LsSolution {
      std::vector<std::string> content;
    };
  }
}

#endif
