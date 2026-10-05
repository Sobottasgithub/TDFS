#ifndef PACKET_WRAPPER_H
#define PACKET_WRAPPER_H

#include "command_packet_types.h"
#include "packet_types.h"

namespace tdfs {
  namespace network_manager {
    class PacketWrapper {
      public:
        static ttp2::Packet::Universal toUniversal(packet_variant packet);
        static packet_variant fromUniversal(ttp2::Packet::Universal universal);
      private:
        template <typename T>
        static std::vector<uint8_t> structToBytes(const T& data);
        template <typename T>
        static T bytesToStruct(const std::vector<uint8_t>& bytes);
    };
  }
}

#endif
