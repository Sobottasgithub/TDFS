#ifndef API_SERVER_H
#define API_SERVER_H

#include <string>

namespace tdfs {
  class ApiServer {
    public:
      ApiServer(std::string interface, int port);

      void acceptClientSession();
      static void clientManager(int serverSocket, int clientSocket);
      
    private:
      int port;
      const char *ipV4;
  };
}

#endif
