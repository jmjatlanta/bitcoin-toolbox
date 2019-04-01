#include <iostream>
#include <vector>
#include <cstdint>

#include <transaction.hpp>
#include <hex_conversion.hpp>

void print_syntax_and_exit(int argc, char**argv)
{
   std::cerr << "Syntax: " << argv[0] << " signed_tx_as_hex preimage_as_string\n";
   exit(1);
}

class signed_tx
{
   public:
   signed_tx(char* bytes) : original_bytes(bytes)
   { 

   }
   void add_secret();

   private:
      char* original_bytes;
      



};

int main(int argc, char** argv)
{
   if (argc < 3)
      print_syntax_and_exit(argc, argv);
   
   // first parameter is the signed transaction as a string
   // convert to binary array
   bc_toolbox::transaction tx( bc_toolbox::hex_string_to_vector(std::string(argv[1]))
   // second parameter is the preimage
}