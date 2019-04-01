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

int main(int argc, char**argv)
{
   if (argc < 2)
      std::cerr << "Syntax: " << argv[0] << " text_to_be_hashed\n";

   std::string text_to_be_hashed(argv[1]);
   std::vector<uint8_t> incoming(text_to_be_hashed.begin(), text_to_be_hashed.end());
   std::cout << vector_to_hex_string(incoming) << "\n";
   return 0;
}