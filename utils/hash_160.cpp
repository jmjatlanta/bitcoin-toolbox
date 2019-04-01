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

void print_error_message(int argc, char** argv)
{
   std::cerr << "Syntax: " << argv[0] << " [hex | text] text_or_hex_to_be_hashed\n";
   exit(1);
}

int main(int argc, char**argv)
{
   if (argc < 3)
      print_error_message(argc, argv);

   std::string text_to_be_hashed(argv[2]);
   bool is_hex = false;
   if (std::string(argv[1]) == "hex")
      is_hex = true;
   if (!is_hex && std::string(argv[1]) != "text")
      print_error_message(argc, argv);

   std::vector<uint8_t> incoming(text_to_be_hashed.begin(), text_to_be_hashed.end());

   if (is_hex)
      incoming = bc_toolbox::hex_string_to_vector(text_to_be_hashed);
   
   std::vector<uint8_t> results = bc_toolbox::ripemd160(bc_toolbox::sha256(incoming));
   std::cout << vector_to_hex_string(results) << "\n";
   return 0;
}