#ifndef NAME_SERVER_CORE_H
#define NAME_SERVER_CORE_H

#include <memory>

#include <tablog.h>

namespace tdfs {
  class NameServer {
    public:
      NameServer();

    private:
      std::shared_ptr<tablog::Tablog> logger;

      void udpDiscoverySession();
  };
}

#endif
