#include "tokenizer.h"

namespace tdfs {
  namespace cli {
    Tokenizer::GenericCommand Tokenizer::tokenize(const std::string command) {
      std::vector<std::string> tokens = toTokens(command);

      GenericCommand genericCommand;
      genericCommand.command = tokens.at(0);

      // INFO: Starts with index 1 because first token is the command
      int index = 1;
      for (; index < tokens.size(); index++) {
        std::string currentToken = tokens.at(index);

        std::string firstChar(1, currentToken.at(0));
        if (!firstChar.compare("-")) {
          genericCommand.flags.push_back(currentToken);
        } else {
          break;
        }
      }
      
      for(; index < tokens.size(); index++) {
        genericCommand.values.push_back(tokens.at(index));
      }
      
      return std::move(genericCommand);
    }

    std::vector<std::string> Tokenizer::toTokens(const std::string command) {
      std::vector<std::string> tokens;

      std::string currentToken = "";
      for (int index = 0; index < command.size(); ++index) {
        std::string currentChar(1, command[index]);
        
        // Create tokens
        if (!currentChar.compare(" ")) {
          tokens.push_back(currentToken);
          currentToken = "";
        } else if (index == command.size()-1) {
          currentToken += currentChar;
          tokens.push_back(currentToken);
          break;
        } else {
          currentToken += currentChar;
        }
      }
      
      return tokens;
    }
  }
}
