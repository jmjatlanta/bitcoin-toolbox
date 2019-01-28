
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
   // SHA256
   unsigned char hash[SHA256_DIGEST_LENGTH];
   SHA256_CTX sha256;
   SHA256_Init(&sha256);
   SHA256_Update(&sha256, bytes, byte_len);
   SHA256_Final(hash, &sha256);
   // now for RIPEMD160
   unsigned char md[RIPEMD160_DIGEST_LENGTH];
   RIPEMD160_CTX md160;
   RIPEMD160_Init(&md160);
   RIPEMD160_Update(&md160, hash, SHA256_DIGEST_LENGTH);
   RIPEMD160_Final(md, &md160);
   return std::vector<uint8_t>(md, md + RIPEMD160_DIGEST_LENGTH);
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