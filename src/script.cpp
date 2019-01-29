
#include <openssl/sha.h>
#include <openssl/ripemd.h>
#include <script.hpp>

namespace bc_toolbox {

void script::add_opcode(unsigned char opcode)
{
   bytes[byte_len] = opcode;
   ++byte_len;
}

void script::add_int(int64_t val) 
{
   auto retVal = pack(val);
   bytes[byte_len] = retVal.first / 2;
   ++byte_len;
   for(auto b : retVal.second )
   {
      bytes[byte_len] = b;
      ++byte_len;
   }
}

void script::add_bytes(std::vector<uint8_t> val)
{
   for(auto b : val)
   {
      bytes[byte_len] = b;
      ++byte_len;
   }
}

void script::add_bytes_with_size(std::vector<uint8_t> val)
{
   auto retVal = pack(val.size());
   for(uint8_t i = 0; i < retVal.first / 2; ++i) 
   {
      bytes[byte_len] = retVal.second[i];
      ++byte_len;
   }
   add_bytes(val);
}

std::vector<uint8_t> script::get_bytes_as_vector()
{
   std::vector<uint8_t> retVal(bytes, bytes + byte_len);
   return retVal;
}

/***
 * Run bytes through SHA256 and RIPEMD-160
 */
std::vector<uint8_t> script::hash()
{
   std::vector<uint8_t> temp_bytes(bytes, bytes + byte_len);
   return ripemd160(sha256(temp_bytes));
}

std::vector<uint8_t> script::p2sh_script()
{
   std::vector<uint8_t> retVal = hash();
   // add prefix OP_HASH160 OP_PUSHDATA 20
   retVal.insert(retVal.begin(), 0x14);
   retVal.insert(retVal.begin(), bc_toolbox::OP_HASH160 );
   // add suffix OPEQUAL
   retVal.push_back(bc_toolbox::OP_EQUAL);
   return retVal;
}

}