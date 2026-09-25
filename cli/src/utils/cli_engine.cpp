#include "cli_engine.h"

#include <iostream>
#include <readline/readline.h>
#include <readline/history.h>
#include <string>

namespace tdfs::cli {
  CliEngine::CliEngine() {
    displayWelcomeMessage();

    char* query;
    bool queryCycle = true;
    while(queryCycle) {
      // TODO: get responses

      query = ::readline("\033[45;97m TDFS \033[0m> ");
      std::string stringQuery = query;
      
      if (!stringQuery.compare("h")) {
        displayHelp();
      } else if (!stringQuery.compare("q")) {
        // TODO: disconnect from tdfs server
        queryCycle = false;
        std::cout << "terminated" << std::endl;
      } else {
        // TODO: parse tdfs query string
      }
    }
  }

  void CliEngine::displayWelcomeMessage() {
    std::cout << " _______ _____  ______ _____         _____ _      _____ " << std::endl;
    std::cout << "|__   __|  __ \\|  ____/ ____|       / ____| |    |_   _|" << std::endl;
    std::cout << "   | |  | |  | | |__ | (___        | |    | |      | |  " << std::endl;
    std::cout << "   | |  | |  | |  __| \\___ \\       | |    | |      | |  " << std::endl;
    std::cout << "   | |  | |__| | |    ____) |      | |____| |____ _| |_ " << std::endl;
    std::cout << "   |_|  |_____/|_|   |_____/        \\_____|______|_____|" << std::endl;
    std::cout << "Tablo Distributed File System" << std::endl;
    std::cout << "type 'h' for help" << std::endl;
  }

  void CliEngine::displayHelp() {
    // TODO WARNING: This will later be replaced by a better system
    // that can generate the commands with args etc automatically when
    // the first real commands are implemented
    std::cout << "---- TDFS commands ----" << std::endl;
    std::cout << "Quit: q" << std::endl;
  }
}
