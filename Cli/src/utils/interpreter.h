#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <packet_types.h>
#include "tokenizer.h"

namespace tdfs::cli::interpreter {
  ttp2::Packet::Packet interpret(tokenizer::GenericCommand genericCommand);
}

#endif
