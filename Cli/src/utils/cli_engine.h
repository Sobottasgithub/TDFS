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
        
        void displayWelcomeMessage();
        void displayHelp();
    };
  }
}

#endif
