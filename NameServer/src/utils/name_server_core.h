#ifndef NAME_SERVER_CORE_H
#define NAME_SERVER_CORE_H

#include <memory>
#include <future>

#include <server_discovery.h>
#include <tablog.h>

namespace tdfs {
  class NameServer {
    public:
      NameServer();

    private:
      std::shared_ptr<tablog::Tablog> logger;
      std::shared_ptr<tud::ServerDiscovery> serverDiscovery;

      void udpDiscoverySession(std::promise<void> udpInitDone);
  };
}

#endif
