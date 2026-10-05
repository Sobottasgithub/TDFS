#include "../include/packet_wrapper.h"
#include "../include/packet_identifier.h"
#include "../include/command_packet_types.h"
#include <stdexcept>

namespace tdfs::network_manager {
  ttp2::Packet::Universal PacketWrapper::toUniversal(packet_variant packet) {
    
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
