#include <stdexcept>
#include <algorithm>

#include <hex_conversion.hpp>

#include <openssl/sha.h>
#include <openssl/ripemd.h>

#include <base58.h>

namespace bc_toolbox {

   std::pair<uint8_t, std::vector<uint8_t> > pack(int64_t incoming) 
   {
      // verify it is within range
      if (incoming < -2147483647 || incoming > 2147483647)
         throw std::out_of_range( "Integer out of range" );

      bool isNegative = (incoming < 0 ? true : false);
      incoming = abs(incoming);
      std::pair<uint8_t, std::vector<uint8_t> > retVal;
      retVal.second.resize(4);
      int64_t current_val = incoming;
      for(uint8_t i = 0; i < 4; ++i)
      {
         // grab the lowest 2 bytes
         retVal.second[i] = current_val & 255;
         // shift the bytes to the right by 2
         current_val = current_val >> 8;
      }
      // get rid of trailing zeros
      while( retVal.second[retVal.second.size() -1] == 0 )
         retVal.second.erase(retVal.second.end()-1);
      // if the first digit is 8 or greater, add a zero to the beginning
      if (retVal.second[retVal.second.size() - 1] > 127 )
         retVal.second.insert(retVal.second.begin(), 0);
      // if it is negative, must bitwise OR 0x80
      if (isNegative)
      {
         uint8_t last_pos = retVal.second.size()-1;
         retVal.second[last_pos] = retVal.second[last_pos] | 0x80;
      }
      retVal.first = retVal.second.size() * 2;
      return retVal;
   }

   std::string opcode_to_string(short opcode)
   {
      switch(opcode)
      {
         case OP_0: return "OP_0";
         case OP_PUSHDATA1: return "OP_PUSHDATA1";
         case OP_PUSHDATA2: return "OP_PUSHDATA2";
         case OP_PUSHDATA4: return "OP_PUSHDATA4";
         case OP_1: return "OP_1";
         case OP_2: return "OP_2";
         case OP_3: return "OP_3";
         case OP_4: return "OP_4";
         case OP_5: return "OP_5";
         case OP_6: return "OP_6";
         case OP_7: return "OP_7";
         case OP_8: return "OP_8";
         case OP_9: return "OP_9";
         case OP_10: return "OP_10";
         case OP_11: return "OP_11";
         case OP_12: return "OP_12";
         case OP_13: return "OP_13";
         case OP_14: return "OP_14";
         case OP_15: return "OP_15";
         case OP_16: return "OP_16";
         case OP_1NEGATE: return "OP_1NEGATE";
         // flow control
         case OP_NOP: return "OP_NOP";
         case OP_IF: return "OP_IF";
         case OP_NOTIF: return "OP_NOTIF";
         case OP_ELSE: return "OP_ELSE";
         case OP_ENDIF: return "OP_ENDIF";
         case OP_VERIFY: return "OP_VERIFY";
         case OP_RETURN: return "OP_RETURN";
         // stack
         case OP_TOTALSTACK: return "OP_TOTALSTACK";
         case OP_FROMALTSTACK: return "OP_FROMALTSTACK";
         case OP_IFDUP: return "OP_IFDUP";
         case OP_DEPTH: return "OP_DEPTH";
         case OP_DROP: return "OP_DROP";
         case OP_DUP: return "OP_DUP";
         case OP_NIP: return "OP_NIP";
         case OP_OVER: return "OP_OVER";
         case OP_PICK: return "OP_PICK";
         case OP_ROLL: return "OP_ROLL";
         case OP_ROT: return "OP_ROT";
         case OP_SWAP: return "OP_SWAP";
         case OP_TUCK: return "OP_TUCK";
         case OP_2DROP: return "OP_2DROP";
         case OP_2DUP: return "OP_2DUP";
         case OP_3DUP: return "OP_3DUP";
         case OP_2OVER: return "OP_2OVER";
         case OP_2ROT: return "OP_2ROT";
         case OP_2SWAP: return "OP_2SWAP";
         // splice
         case OP_CAT: return "OP_CAT";
         case OP_SUBSTR: return "OP_SUBSTR";
         case OP_LEFT: return "OP_LEFT";
         case OP_RIGHT: return "OP_RIGHT";
         case OP_SIZE: return "OP_SIZE";
         // bitwise logic
         case OP_INVERT: return "OP_INVERT";
         case OP_AND: return "OP_AND";
         case OP_OR: return "OP_OR";
         case OP_XOR: return "OP_XOR";
         case OP_EQUAL: return "OP_EQUAL";
         case OP_EQUALVERIFY: return "OP_EQUALVERIFY";
         // arithmetic
         case OP_1ADD: return "OP_1ADD";
         case OP_1SUB: return "OP_1SUB";
         case OP_2MUL: return "OP_2MUL";
         case OP_2DIV: return "OP_2DIV";
         case OP_NEGATE: return "OP_NEGATE";
         case OP_ABS: return "OP_ABS";
         case OP_NOT: return "OP_NOT";
         case OP_0NOTEQUAL: return "OP_0NOTEQUAL";
         case OP_ADD: return "OP_ADD";
         case OP_SUB: return "OP_SUB";
         case OP_MUL: return "OP_MUL";
         case OP_DIV: return "OP_DIV";
         case OP_MOD: return "OP_MOD";
         case OP_LSHIFT: return "OP_LSHIFT";
         case OP_RSHIFT: return "OP_RSHIFT";
         case OP_BOOLAND: return "OP_BOOLAND";
         case OP_BOOLOR: return "OP_BOOLOR";
         case OP_NUMEQUAL: return "OP_NUMEQUAL";
         case OP_NUMEQUALVERIFY: return "OP_NUMEQUALVERIFY";
         case OP_NUMNOTEQUAL: return "OP_NUMNOTEQUAL";
         case OP_LESSTHAN: return "OP_LESSTHAN";
         case OP_GREATERTHAN: return "OP_GREATERTHAN";
         case OP_LESSTHANOREQUAL: return "OP_LESSTHANOREQUAL";
         case OP_GREATERTHANOREQUAL: return "OP_GREATERTHANOREQUAL";
         case OP_MIN: return "OP_MIN";
         case OP_MAX: return "OP_MAX";
         case OP_WITHIN: return "OP_WITHIN";
         // crypto
         case OP_RIPEMD160: return "OP_RIPEMD160";
         case OP_SHA1: return "OP_SHA1";
         case OP_SHA256: return "OP_SHA256";
         case OP_HASH160: return "OP_HASH160";
         case OP_HASH256: return "OP_HASH256";
         case OP_CODESEPARATOR: return "OP_CODESEPARATOR";
         case OP_CHECKSIG: return "OP_CHECKSIG";
         case OP_CHECKSIGVERIFY: return "OP_CHECKSIGVERIFY";
         case OP_CHECKMULTISIG: return "OP_CHECKMULTISIG";
         case OP_CHECKMULTISIGVERIFY: return "OP_CHECKMULTISIGVERIFY";
         // locktime
         case OP_CHECKLOCKTIMEVERIFY: return "OP_CHECKLOCKTIMEVERIFY";
         case OP_CHECKSEQUENCEVERIFY: return "OP_CHECKSEQUENCEVERIFY";
         // pseudo-words
         case OP_PUBKEYHASH: return "OP_PUBKEYHASH";
         case OP_PUBKEY: return "OP_PUBKEY";
         case OP_INVALIDOPCODE: return "OP_INVALIDOPCODE";
         // reserved words
         case OP_RESERVED: return "OP_RESERVED";
         case OP_VER: return "OP_VER";
         case OP_VERIF: return "OP_VERIF";
         case OP_VERNOTIF: return "OP_VERNOTIF";
         case OP_RESERVED1: return "OP_RESERVED1";
         case OP_RESERVED2: return "OP_RESERVED2";
         case OP_NOP1: return "OP_NOP1";
         case OP_NOP4: return "OP_NOP2";
         case OP_NOP5: return "OP_NOP5";
         case OP_NOP6: return "OP_NOP6";
         case OP_NOP7: return "OP_NOP7";
         case OP_NOP8: return "OP_NOP8";
         case OP_NOP9: return "OP_NOP9";
         case OP_NOP10: return "OP_NOP10";
         default:
            throw std::invalid_argument("Invalid OPCODE");
      }
   }

   std::vector<uint8_t> sha256(std::string incoming) 
   {
      std::vector<uint8_t> conv{incoming.c_str(), incoming.c_str() + incoming.size()};
      return sha256(conv);
   }

   std::vector<uint8_t> sha256(std::vector<uint8_t> incoming)
   {
      unsigned char hash[SHA256_DIGEST_LENGTH];
      SHA256_CTX sha256;
      SHA256_Init(&sha256);
      SHA256_Update(&sha256, &incoming[0], incoming.size());
      SHA256_Final(hash, &sha256);
      return std::vector<uint8_t>(hash, hash + SHA256_DIGEST_LENGTH);
   }

   std::vector<uint8_t> ripemd160(std::vector<uint8_t> incoming)
   {
      unsigned char md[RIPEMD160_DIGEST_LENGTH];
      RIPEMD160_CTX md160;
      RIPEMD160_Init(&md160);
      RIPEMD160_Update(&md160, &incoming[0], incoming.size());
      RIPEMD160_Final(md, &md160);
      return std::vector<uint8_t>(md, md + RIPEMD160_DIGEST_LENGTH);
   }

   std::string base58check(std::vector<uint8_t> incoming)
   {
      // use bitcoin library for base58check function
      return EncodeBase58Check(incoming);
   }

   std::vector<uint8_t> little_endian(uint64_t val, uint8_t bytes)
   {
      std::vector<uint8_t> ret_val;
      uint64_t current_val = val;
      for( uint8_t pos = 0; pos < bytes; ++pos )
      {
            // grab the lowest 2 bytes
            ret_val.push_back(current_val & 255);
            // shift the bytes to the right by 2
            current_val = current_val >> 8;
      }
      return ret_val;
   }

   std::vector<uint8_t> big_endian(uint64_t val, uint8_t bytes)
   {
      std::vector<uint8_t> ret_val = little_endian(val, bytes);
      std::reverse(ret_val.begin(), ret_val.end());
      return ret_val;
   }

   std::vector<uint8_t> to_varint(uint64_t val)
   {
      std::vector<uint8_t> ret_val = little_endian(val, 8);
      // get rid of trailing zeros
      while ( ret_val.size() > 1 && ret_val[ ret_val.size() - 1] == 0)
         ret_val.erase( ret_val.end() -1 );
      // add prefix
      if ( val < 0xfd )
      { 
         // do nothing
      }
      else
      {
         if ( val <= 0xffff )
         {
            ret_val.insert( ret_val.begin(), 0xfd );
            // pad with 0s until size is 3
            while(ret_val.size() < 3)
               ret_val.push_back(0x00);
         } else
         {
            if ( val <= 0xffffffff )
            {
               ret_val.insert( ret_val.begin(), 0xfe );
               // pad with 0s until size is 5
               while(ret_val.size() < 5)
                  ret_val.push_back(0x00);
            }
            else
            {
               ret_val.insert( ret_val.begin(), 0xff );
               // pad with 0s until size is 9
               while(ret_val.size() < 9)
                  ret_val.push_back(0x00);
            }
         }
      }
      return ret_val;
   }

   uint64_t from_varint( std::vector<uint8_t> val )
   {
      if ( val.size() == 1 )
         return (uint8_t)val[0];
      else
      {
         if ( (val.size() == 3 && val[0] == 0xffff)
               || (val.size() == 5 && val[0] == 0xffffffff) 
               || (val.size() == 9 && val[0] == 0xffffffffffffffff) )
         {
            val.erase( val.begin() );
            uint64_t ret_val;
            // put values back in big-endian order
            std::reverse(val.begin(), val.end());
            if ( val.size() == 2)
            {
               uint16_t new_val = val[0];
               ret_val = new_val;
            }
            if ( val.size() == 4)
            {
               uint32_t new_val = val[0];
               ret_val = new_val;
            }
            if ( val.size() == 8 )
            {
               ret_val = val[0];
            }
            return ret_val;
         } // if size matches
      } // if size is not equal 1
      throw std::out_of_range("varint out of range");
   } // from_varint
} // namespace bc_toolbox
