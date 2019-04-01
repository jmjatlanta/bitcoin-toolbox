#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>

#include <transaction.hpp>
#include <hex_conversion.hpp>
#include <script.hpp>

void print_syntax_and_exit(int argc, char**argv)
{
   std::cerr << "Syntax: " << argv[0] << " signed_tx_json_file preimage_as_string public_key_as_hex_string\n";
   exit(1);
}

std::string read_json_file(std::string filename)
{ 
   std::ifstream f(filename);
   std::string str((std::istreambuf_iterator<char>(f)),std::istreambuf_iterator<char>());
   return str;
}

int main(int argc, char** argv)
{
   if (argc < 4)
      print_syntax_and_exit(argc, argv);
   
   // first parameter is the signed transaction as a string
   std::string tx_as_json = read_json_file(argv[1]);
   rapidjson::Document doc;
   doc.Parse(tx_as_json.c_str());
   rapidjson::Value& tx_as_value = doc["hex"];
   std::string tx_as_string = tx_as_value.GetString();

   // second parameter is the preimage
   std::string preimage_string(argv[2]);
   std::vector<uint8_t> preimage(preimage_string.begin(), preimage_string.end());
   // third parameter is the public key
   std::vector<uint8_t> public_key = bc_toolbox::hex_string_to_vector(std::string(argv[3]));

   // build the script
   bc_toolbox::script new_script;
   new_script.add_bytes_with_size(public_key);
   new_script.add_bytes_with_size(preimage);

   // convert tx_as_string back to a transaction
   bc_toolbox::transaction tx( bc_toolbox::hex_string_to_vector(tx_as_string) );
   bc_toolbox::input& in = tx.inputs.at(0);

   // merge the two scripts
   std::vector<uint8_t> merged_script = new_script.get_bytes_as_vector();
   merged_script.insert(merged_script.end(), in.sig_script.begin(), in.sig_script.end() );

   in.sig_script = merged_script;

   std::cout << bc_toolbox::vector_to_hex_string( tx.to_bytes() );

}