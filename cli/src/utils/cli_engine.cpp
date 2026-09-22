#include "cli_engine.h"

#include <iostream>
#include <readline/readline.h>
#include <readline/history.h>

namespace tdfs::cli {
  CliEngine::CliEngine() {
    displayWelcomeMessage();

    char* query;
    bool queryCycle = true;
    while(queryCycle) {
      query = ::readline("\033[45;97m TDFS \033[0m> ");
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
    std::cout << "type 'm' for help" << std::endl;
  }
}
