#include "cli_engine.h"

#include <iostream>

namespace tdfs::cli {
  CliEngine::CliEngine() {
    displayWelcomeMessage();
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
