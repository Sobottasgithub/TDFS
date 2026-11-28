#include "cli_engine.h"

#include <client_session_controller.h>
#include <iostream>
#include <packet_types.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <string>
#include <tdfs_packet_types.h>

#include "tokenizer.h"
#include "interpreter.h"

namespace tdfs::cli {
  CliEngine::CliEngine() {
    displayWelcomeMessage();

    char* query;
    bool queryCycle = true;
    while(queryCycle) {
      // TODO: get responses

      query = ::readline("\033[45;97m TDFS \033[0m> ");
      if (query == nullptr) {
        continue;
      }
      std::string stringQuery = query;
      if (stringQuery.length() == 0) {
        continue;
      }
      
      if (!stringQuery.compare("h")) {
        displayHelp();
      } else if (!stringQuery.compare("q")) {
        // TODO: disconnect from tdfs server
        queryCycle = false;
        std::cout << "terminated" << std::endl;
      } else if (!stringQuery.compare("c")) {
        connect();
      } else {
        executeCommand(stringQuery);
      }
    }
  }

  void CliEngine::executeCommand(const std::string stringQuery) {
    try {
      tokenizer::GenericCommand genericCommand = tokenizer::tokenize(stringQuery);
      ttp2::Packet::Packet packet = interpreter::interpret(genericCommand);
      this->apiClient.sendPacket(packet);

      if (std::holds_alternative<ttp2::Packet::tdfs::Ls>(packet.payload)) {
        while (!this->apiClient.hasResponse()) {}
        handleResponse(this->apiClient.popResonse());
      }
      
    } catch (const std::invalid_argument& invalidArgument) {
      std::cout << "\033[45;101m ERROR: \033[0m " << invalidArgument.what() << std::endl;
    }
  }

  void CliEngine::handleResponse(ttp2::Packet::Packet packet) {
    if (std::holds_alternative<ttp2::Packet::tdfs::LsSolution>(packet.payload)) {
      ttp2::Packet::tdfs::LsSolution lsSolution = std::get<ttp2::Packet::tdfs::LsSolution>(packet.payload);
      for (int index = 0; index < lsSolution.content.size(); index++) {
        std::cout << lsSolution.content.at(index);

        if (index+1 < lsSolution.content.size()) {
          std::cout << " ";
        }
      }
      std::cout << std::endl;
    }
  }

  void CliEngine::connect() {
    if (this->apiClient.isConnected()) {
      std::cout << "You are connected to: " << this->apiClient.getIp() << std::endl;
      std::string option = ::readline("(1) cancel\n(2) connect\n(3) disconnect\n choose: ");

      if (!option.compare("1")) {
        return;
      } else if (!option.compare("2")) {
        this->apiClient.disconnect();
        connectToTdfs();
      } else if (!option.compare("3")) {
        this->apiClient.disconnect();
      } else {
        std::cout << "Unknown action!" << std::endl;
      }
      return;
    }
    connectToTdfs();
  }

  void CliEngine::connectToTdfs() {
    char* ipV4 = ::readline(" Ip: ");
    char* charPort = ::readline(" Port: ");
    try {
      int port = std::stoi(charPort);
      this->apiClient.connect(ipV4, port);
    } catch (const std::invalid_argument&) {
      std::cout << "\033[45;101m ERROR: \033[0m Invalid Port" << std::endl;
    } catch (const std::out_of_range&) {
      std::cout << "\033[45;101m ERROR: \033[0m Port does not exist" << std::endl;
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
    std::cout << "Connect: c" << std::endl;
  }
}
