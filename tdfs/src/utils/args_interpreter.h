#ifndef ARGS_INTERPRETER_H
#define ARGS_INTERPRETER_H

#include <vector>
#include <string>

namespace tdfs {
  class ArgsInterpreter {
    public:
      ArgsInterpreter(int argc, char *argv[]);

      std::vector<std::string> getArg(std::string flag, int trailingArgumentCount);

    private:
      int argc;
      std::vector<std::string> argv;
  };
}

#endif
