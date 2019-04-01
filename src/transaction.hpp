#pragma once

#include <vector>
#include <cstdint>

namespace bc_toolbox {

class input
{
   public:
      input() {};
      input(uint8_t* bytes, uint64_t& bytes_read)
      {
         uint8_t* pos = bytes;
         for(uint8_t i = 0; i < 32; i++)
         {
            hash.push_back(pos[0]);
            pos++;
         }
         index = pos[0];
         pos += 4;
         // read length of signature script
         uint16_t read = 0;
         uint64_t script_length = from_varint(pos, read);
         pos += read;
         for(uint16_t i = 0; i < script_length; ++i)
         {
            sig_script.push_back(pos[0]);
            pos++;
         }
         sequence = ( (pos[3]) << 24 | (pos[2]) << 16 | (pos[1]) << 8 | pos[0]);
         bytes_read = pos + 4 - bytes;
      }
      std::vector<uint8_t> hash;
      uint32_t index;
      std::vector<uint8_t> sig_script; // includes length (in bytes) + script
      uint32_t sequence; // tx version as defined by sender
};

class output
{
   public:
      output() {};
      output(uint8_t* bytes, uint16_t& bytes_read)
      {
         //8 bytes for value
         uint8_t* pos = bytes;
         unsigned char temp[9];
         value = ( (bytes[7]) << 56 | (bytes[6]) << 48 | (bytes[5]) << 40 | (bytes[4]) << 32 
               | (bytes[3]) << 24 | (bytes[2]) << 16 | (bytes[1]) << 8 | bytes[0]) ;
         pos += 8;
         // read length of signature script
         uint16_t read = 0;
         uint64_t script_length = from_varint(pos, read);
         pos += read;
         for(uint16_t i = 0; i < script_length; ++i)
         {
            script.push_back(pos[0]);
            pos++;
         }
         bytes_read = pos - bytes;
      }
      uint64_t value; // value in satoshis
      std::vector<uint8_t> script;
      std::vector<uint8_t> data;
      bool use_data = false;
};

class witness
{
   public:
      std::vector<uint8_t> data;
};

class transaction
{
   public:
      transaction() {};
      transaction(std::vector<uint8_t> raw_transaction)
      {
         parse_raw_transaction(raw_transaction);
      }
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
      void parse_raw_transaction(std::vector<uint8_t> raw_transaction);
      bool parsed = false;
};

}