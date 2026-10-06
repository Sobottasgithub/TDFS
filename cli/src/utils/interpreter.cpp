#include "interpreter.h"
#include <packet_types.h>
#include <stdexcept>
#include <tdfs_packet_types.h>
#include "tokenizer.h"

#include <iostream>

namespace {
  ttp2::Packet::tdfs::Ls interpretLs(tdfs::cli::tokenizer::GenericCommand genericCommand) {
    ttp2::Packet::tdfs::Ls ls;

    // TODO: Ls will support -C, -d, -h, -q, -R, -t, -S, -r, -u, -e someday
    if (genericCommand.flags.size() > 0)
      throw std::invalid_argument("Ls unsupported flag(s)");

    if (genericCommand.values.size() > 1 || genericCommand.values.size() < 1)
      throw std::invalid_argument("Ls supports only one value: path");

    ls.directory = genericCommand.values.at(0);
    return ls;
  }  
}

namespace tdfs::cli::interpreter {
  ttp2::Packet::Packet interpret(tokenizer::GenericCommand genericCommand) {
    ttp2::Packet::Packet packet;
    if (!genericCommand.command.compare("ls")) {
      packet.payload = interpretLs(genericCommand);
    } else {
      throw std::invalid_argument("Unknown command. Type 'h' for help.");
    }
    return packet;
  }
}
