#include "../include/packet_wrapper.h"
#include "../include/packet_identifier.h"
#include "../include/command_packet_types.h"
#include <packet_types.h>
#include <stdexcept>
#include <variant>

namespace tdfs::network_manager {
  ttp2::Packet::Universal PacketWrapper::toUniversal(packet_variant packet) {
    ttp2::Packet::Universal universal;

    if (std::holds_alternative<struct tdfs::network_manager::Ls>(packet)) {
      universal.type = tdfs::network_manager::PacketIdentifiers::Ls;
    } else if (std::holds_alternative<struct tdfs::network_manager::LsSolution>(packet)) {
      universal.type = tdfs::network_manager::PacketIdentifiers::LsSolution;
    } else {
      throw std::invalid_argument("Error encoding payload: Unknown type!");
    }

    universal.bytes = structToBytes(packet);
    return universal;
  }
  
  packet_variant PacketWrapper::fromUniversal(ttp2::Packet::Universal universal) {
    switch (universal.type) {
      case tdfs::network_manager::PacketIdentifiers::Ls:
        return bytesToStruct<struct tdfs::network_manager::Ls>(universal.bytes);
      case tdfs::network_manager::PacketIdentifiers::LsSolution:
        return bytesToStruct<struct tdfs::network_manager::LsSolution>(universal.bytes);
      default:
        throw std::invalid_argument("Unknown packet type!");
    };
  }

  template <typename T>
  std::vector<uint8_t> PacketWrapper::structToBytes(const T& data) {
    std::vector<uint8_t> bytes(sizeof(T));
    std::memcpy(bytes.data(), &data, sizeof(T));
    return bytes;
  }

  template <typename T>
  T PacketWrapper::bytesToStruct(const std::vector<uint8_t>& bytes) {
    T data;
    std::memcpy(&data, bytes.data(), sizeof(T));
    return data;
  }
}
