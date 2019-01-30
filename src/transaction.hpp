#pragma once

#include <vector>
#include <cstdint>

namespace bc_toolbox {

class input
{
   public:
      std::vector<uint8_t> hash;
      uint16_t index;
      std::vector<uint8_t> sig_script; // includes length (in bytes) + script
      uint32_t sequence; // tx version as defined by sender
};

class output
{
   public:
      uint64_t value; // value in satoshis
      std::vector<uint8_t> script;
};

class witness
{
   public:
      std::vector<uint8_t> data;
};

class transaction
{
   public:
      /***
       * @brief Retrieve the transaction as a vector of bytes
       * @returns the binary representation of the transaction
       */
      std::vector<uint8_t> to_bytes();
   public:
      uint32_t version;
      uint16_t flag; // segwit flag, if present, will always be 0001
      std::vector<input> inputs;
      std::vector<output> outputs;
      std::vector<witness> witnesses;
      uint32_t locktime; // block height or timestamp when tx finalizes
   private:
      static void add( std::vector<uint8_t>& vec, const std::vector<uint8_t>& bytes );
      static void add_input( std::vector<uint8_t>& vec, const input& in );
      static void add_output( std::vector<uint8_t>& vec, const output& out );
      static void add_witness( std::vector<uint8_t>& vec, const witness& wit );
};

}