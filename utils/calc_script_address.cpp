#include <string>
#include <iostream>
#include <vector>
#include <script.hpp>
#include <iomanip>
#include <sstream>

void print_help_and_exit(int argc, char** argv)
{
   std::cerr << "Syntax: " << argv[0] << " [ testnet | mainnet ] HASHLOCK RECEIVER_PRIMARY_KEY TIMELOCK SENDER_PRIMARY_KEY\n";
   if (argc > 1)
   {
      std::cerr << "Parameters passed: ";
      for(int i = 1; i < argc; ++i)
      {
         std::cerr << argv[i] << " ";
      }
      std::cerr << "\n";
   }
   exit(1);
}

std::vector<uint8_t> string_to_vector(std::string input)
{
   std::vector<uint8_t> v(input.begin(), input.end());
   return v;
}

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
   // parse the command line
   if(argc < 5)
      print_help_and_exit(argc, argv);

   bool testnet = false;
   if (std::string(argv[1]) == "testnet")
      testnet = true;
   if (!testnet && std::string(argv[1]) != "mainnet")
      print_help_and_exit(argc, argv);
   std::vector<uint8_t> hash160_hash_lock = bc_toolbox::hex_string_to_vector(argv[2]);
   std::vector<uint8_t> recipient_pubkey = bc_toolbox::hex_string_to_vector(argv[3]);
   std::vector<uint8_t> recipient_pubkey_hash = bc_toolbox::ripemd160(bc_toolbox::sha256(recipient_pubkey));
   uint32_t timeout = std::atoi(argv[4]);
   std::vector<uint8_t> sender_pubkey = bc_toolbox::hex_string_to_vector(argv[5]);
   std::vector<uint8_t> sender_pubkey_hash = bc_toolbox::ripemd160(bc_toolbox::sha256(sender_pubkey));

   // following bip199
   bc_toolbox::script s;
   s.add_opcode(bc_toolbox::OP_IF);
   s.add_opcode(bc_toolbox::OP_HASH160);
   s.add_bytes_with_size(hash160_hash_lock);
   s.add_opcode(bc_toolbox::OP_EQUALVERIFY);
   s.add_opcode(bc_toolbox::OP_DUP);
   s.add_opcode(bc_toolbox::OP_HASH160);
   s.add_bytes_with_size(recipient_pubkey_hash);
   s.add_opcode(bc_toolbox::OP_ELSE);
   s.add_int(timeout);
   s.add_opcode(bc_toolbox::OP_CHECKLOCKTIMEVERIFY);
   s.add_opcode(bc_toolbox::OP_DROP);
   s.add_opcode(bc_toolbox::OP_DUP);
   s.add_opcode(bc_toolbox::OP_HASH160);
   s.add_bytes_with_size(sender_pubkey_hash);
   s.add_opcode(bc_toolbox::OP_ENDIF);
   s.add_opcode(bc_toolbox::OP_EQUALVERIFY);
   s.add_opcode(bc_toolbox::OP_CHECKSIG);

   std::vector<uint8_t> redeem_script = s.hash();
   // append 0x05 for mainnet, or 0xc4 for testnet
   if (testnet)
      redeem_script.insert(redeem_script.begin(), 0xc4);
   else
      redeem_script.insert(redeem_script.begin(), 0x05);
   
   // then base58check
   std::string address1 = bc_toolbox::base58check(redeem_script);
   std::cout << address1 << "\n";

   return 0;
}
