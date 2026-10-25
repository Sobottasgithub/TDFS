#ifndef API_SERVER_H
#define API_SERVER_H

#include <string>

namespace tdfs {
  class ApiServer {
    public:
      ApiServer(std::string interface, int port);

      void acceptClientSession();
      
    private:
      int port;
      const char *ipV4;

      static void clientManager(int serverSocket, int clientSocket);
  };
}

#endif
