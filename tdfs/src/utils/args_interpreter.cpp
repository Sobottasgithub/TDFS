#include "args_interpreter.h"
#include <stdexcept>
#include <string>

namespace tdfs {
  ArgsInterpreter::ArgsInterpreter(int argc, char *argv[]) {
    this->argc = argc;

    int index = 0;
    while (index < argc) {
      this->argv.push_back(std::string(argv[index]));
      index++;
    }
  }

  std::vector<std::string> ArgsInterpreter::getArg(std::string flag, int trailingArgumentCount) {
    std::vector<std::string> trailingArguments;
    for (int index = 0; index < argc; index++) {
      if (!flag.compare(argv.at(index))) {
        if (index + trailingArgumentCount <= argc) {
          for (int trailingIndex = index+1;
               trailingIndex < index + trailingArgumentCount + 1;
               trailingIndex++) {
            trailingArguments.push_back(argv.at(trailingIndex));
          }
        } else {
          throw std::runtime_error("To few trailing arguments provided!");
        }
        break;
      }
    }
    return trailingArguments;
  }
}
