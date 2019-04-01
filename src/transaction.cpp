#include <hex_conversion.hpp>
#include "transaction.hpp"

#include <cstring>
#include <cstdlib>
#include <algorithm>
#include <iostream>

namespace bc_toolbox {

void transaction::add( std::vector<uint8_t>& vec, const std::vector<uint8_t>& bytes)
{
   vec.insert( vec.end(), bytes.begin(), bytes.end() );
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
   add( vec, to_varint( out.script.size() ) ); 
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
   add( ret_val, to_varint( outputs.size() ) );
   for(auto i : outputs)
      add_output( ret_val, i );
   if (flag != 0)
   {
   add( ret_val, to_varint( witnesses.size() ) );
   for( auto i : witnesses )
      add_witness( ret_val, i );
   }
   add( ret_val, little_endian( locktime, 4) );
   return ret_val;
}

void swap_bytes(uint8_t* in, uint8_t length, unsigned char* out)
{
   memset(out, 0, length+1);
   uint8_t pos = 0;
   for(uint8_t i = length; i > 0; --i)
   {
      out[pos++] = in[i-1];
   }
}

void transaction::parse_raw_transaction(std::vector<uint8_t> tx)
{
   parsed = true;
   // convert raw_transaction to bytes
   uint8_t* bytes = tx.data();
   uint16_t bytes_read = 0;
   // version (4 bytes)
   unsigned char temp[9];
   char* ptr;
   swap_bytes(bytes, 4, temp);
   version = ( (temp[0]) << 24 | (temp[1]) << 16 | (temp[2]) << 8 | temp[3]) ;
   bytes += 4;
   // flag (optional 1 bytes)
   flag = 0;
   if (bytes[0] == 0)
   {
      flag = bc_toolbox::from_varint(bytes, bytes_read);
      bytes += bytes_read;
   }
   // inputs (number of inputs as varint)
   uint64_t num_inputs = bc_toolbox::from_varint(bytes, bytes_read);
   bytes += bytes_read;
   inputs = std::vector<input>();
   // loop through inputs
   for(uint64_t i = 0; i < num_inputs; ++i)
   { 
      uint64_t bytes_read = 0;
      input new_input(bytes, bytes_read);
      inputs.push_back(new_input);
      bytes += bytes_read;
   }
   // outputs (number of outputs as varint)
   uint64_t num_outputs = bc_toolbox::from_varint(bytes, bytes_read);
   bytes += bytes_read;
   // loop through outputs
   for(uint64_t i = 0; i < num_outputs; ++i)
   {
      output new_output(bytes, bytes_read);
      outputs.push_back(new_output);
      bytes += bytes_read;
   }
   // witnesses (omitted if flag above is not there)
   if (flag == 1)
   {

   }
   // locktime (4 bytes)
   locktime = bytes[0];
   return;
}

}