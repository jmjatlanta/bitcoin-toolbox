#include <hex_conversion.hpp>
#include "transaction.hpp"

#include <algorithm>
#include <iostream>

namespace bc_toolbox {

void transaction::add( std::vector<uint8_t>& vec, const std::vector<uint8_t>& bytes)
{
   vec.insert( vec.end(), bytes.begin(), bytes.end() );
}

std::vector<uint8_t> transaction::little_endian(uint64_t val, uint8_t bytes)
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

std::vector<uint8_t> transaction::big_endian(uint64_t val, uint8_t bytes)
{
   std::vector<uint8_t> ret_val = little_endian(val, bytes);
   std::reverse(ret_val.begin(), ret_val.end());
   return ret_val;
}

void transaction::add_input( std::vector<uint8_t>&vec, const input& in)
{
   add( vec, in.hash );
   add( vec, little_endian( in.index, 4 ) );
   add( vec, little_endian( in.sig_script.size(), 1 ) );
   add( vec, in.sig_script );
   add( vec, big_endian( in.sequence, 4 ) );
}

void transaction::add_output( std::vector<uint8_t>& vec, const output& out )
{ 
   add( vec, little_endian( out.value, 8 ) );
   // TODO: this is a variable length integer, convert it!
   add( vec, little_endian( out.script.size(), 1 ) ); 
   add( vec, out.script );
}

void transaction::add_witness( std::vector<uint8_t>& vec, const witness& wit )
{ 
   add( vec, little_endian( wit.data.size(), 1 ) );
   add( vec, wit.data );
}

std::vector<uint8_t> transaction::to_bytes()
{
   std::vector<uint8_t> ret_val;
   add( ret_val, little_endian(version, 4) );
   if (flag != 0)
      add( ret_val, big_endian(flag, 2) );
   add( ret_val, little_endian( inputs.size(), 1 ) );
   for(auto i : inputs)
      add_input( ret_val, i );
   // TODO: This is a variable length integer. Convert it!
   add( ret_val, little_endian( outputs.size(), 1 ) );
   for(auto i : outputs)
      add_output( ret_val, i );
   add( ret_val, little_endian( witnesses.size(), 1) );
   for( auto i : witnesses )
      add_witness( ret_val, i );
   add( ret_val, little_endian( locktime, 4) );
   return ret_val;
}

}