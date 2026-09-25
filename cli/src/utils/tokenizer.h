#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <string>
#include <vector>

namespace tdfs {
  namespace cli {
    class Tokenizer {
      public:
        struct GenericCommand{
          std::string command = "";
          std::vector<std::string> flags = {};
          std::vector<std::string> values = {};
        };

        static GenericCommand tokenize(const std::string command);

      private:
        static std::vector<std::string> toTokens(const std::string command);
    };
  }
}

#endif
