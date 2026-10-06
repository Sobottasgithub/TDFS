#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <string>
#include <vector>

namespace tdfs::cli::tokenizer {
  struct GenericCommand{
    std::string command = "";
    std::vector<std::string> flags = {};
    std::vector<std::string> values = {};
  };

  GenericCommand tokenize(const std::string command);
}

#endif
