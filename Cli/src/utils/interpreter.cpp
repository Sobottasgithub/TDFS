#include "interpreter.h"

#include "tokenizer.h"
#include "helpers/file_helpers.h"

#include <filesystem>
#include <packet_types.h>
#include <stdexcept>
#include <tdfs_packet_types.h>

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

  ttp2::Packet::tdfs::CopyFromLocal interpretCopyFromLocal(tdfs::cli::tokenizer::GenericCommand genericCommand) {
    ttp2::Packet::tdfs::CopyFromLocal copyFromLocal;

    if (genericCommand.flags.size() > 0)
      throw std::invalid_argument("CopyFromLocal unsupported flag(s)");

    if (genericCommand.values.size() > 1 || genericCommand.values.size() < 1)
      throw std::invalid_argument("CopyFromLocal supports only one value: path");

    std::filesystem::path filePath = std::filesystem::path(genericCommand.values.at(0));
    copyFromLocal.filePath = filePath.filename();

    std::shared_ptr<arrow::Table> file = tdfs::cli::helpers::openFile(filePath);
    copyFromLocal.start = 0;
    copyFromLocal.end = file->num_rows();
    copyFromLocal.payload = file;

    return copyFromLocal;
  }
}

namespace tdfs::cli::interpreter {
  ttp2::Packet::Packet interpret(tokenizer::GenericCommand genericCommand) {
    ttp2::Packet::Packet packet;
    if (!genericCommand.command.compare("ls")) {
      packet.payload = interpretLs(genericCommand);
    } else if (!genericCommand.command.compare("copyFromLocal")) {
      packet.payload = interpretCopyFromLocal(genericCommand);
    } else {
      throw std::invalid_argument("Unknown command. Type 'h' for help.");
    }
    return packet;
  }
}
