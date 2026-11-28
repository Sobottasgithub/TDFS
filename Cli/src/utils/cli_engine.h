#ifndef CLI_ENGINE_H
#define CLI_ENGINE_H

#include "api_client.h"

namespace tdfs {
  namespace cli {
    class CliEngine {
      public:
        CliEngine();

      private:
        ApiClient apiClient;

        void connect();
        void connectToTdfs();
        void executeCommand(const std::string stringQuery);
        void handleResponse(ttp2::Packet::Packet packet);
        
        void displayWelcomeMessage();
        void displayHelp();
    };
  }
}

#endif
