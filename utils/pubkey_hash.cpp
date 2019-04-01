#include <vector>
#include <iostream>
#include <hex_conversion.hpp>
#include <sstream>
#include <iomanip>

std::string vector_to_hex_string(std::vector<uint8_t> incoming)
{
   std::stringstream ss;
   for(auto i : incoming)
   {
      ss << std::hex << std::setw(2) << std::setfill('0') << (int)i;
   }
   return ss.str();
}

/*****
 * convert a public key to a hash160
 */
std::vector<uint8_t> pubkey_hash(std::vector<uint8_t> incoming)
{
   std::vector<uint8_t> sh256 = bc_toolbox::sha256(incoming);
   return bc_toolbox::ripemd160(sh256);
}

int main(int argc, char**argv)
{
   if (argc < 2)
      std::cerr << "Syntax: " << argv[0] << " public_key_to_be_hashed\n";

   std::vector<uint8_t> results = bc_toolbox::sha256(argv[1]);
   std::cout << vector_to_hex_string(results) << "\n";
   return 0;
}