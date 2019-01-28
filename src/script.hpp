#pragma once

#include <hex_conversion.hpp>

namespace bc_toolbox {

class script
{
   public:
      script() : byte_len(0) {}
      ~script() {}
      void add_opcode(unsigned char opcode);
      void add_int(int64_t val);
      void add_bytes(std::vector<uint8_t> val);
      unsigned char* get_bytes() { return bytes; }
      std::vector<uint8_t> get_bytes_as_vector();
      uint16_t get_byte_len() { return byte_len; }
      std::vector<uint8_t> hash();
      std::vector<uint8_t> p2sh_script();
   private:
      uint8_t bytes[520];
      uint16_t byte_len;
};

}