#ifndef NAME_SERVER_CORE_H
#define NAME_SERVER_CORE_H

#include <memory>
#include <future>

#include <peer_session_controller.h>
#include <server_discovery.h>
#include <tablog.h>

namespace tdfs {
  class NameServer {
    public:
      NameServer();

    private:
      std::shared_ptr<tablog::Tablog> logger;
      std::shared_ptr<tud::ServerDiscovery> serverDiscovery;
      std::shared_ptr<ttp2::PeerSessionController> peerSessionController;

      void udpDiscoverySession(std::promise<void> udpInitDone);
      void peerControllerSession(std::promise<void> peerInitDone);
  };
}

#endif
