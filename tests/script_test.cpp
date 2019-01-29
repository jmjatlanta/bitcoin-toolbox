#define BOOST_TEST_MODULE tests 
#include <boost/test/unit_test.hpp>

#include <vector> 
#include <stdexcept>

#include <script.hpp> 

BOOST_AUTO_TEST_SUITE( script_test )

/**
 * Helper to check vectors for equality
 */
bool test_vector(std::vector<uint8_t> incoming, std::vector<uint8_t> expected)
{
   BOOST_CHECK_EQUAL( incoming.size(), expected.size() );

   for( uint64_t i = 0; i < incoming.size(); ++i )
   {
      BOOST_CHECK_EQUAL( incoming[i], expected[i] );
   }
   return true;
}

/**
 * Test converting integer the bitcoin way
 */
BOOST_AUTO_TEST_CASE( int_conv )
{
   {
      int64_t little_int = -2147483648;
      BOOST_CHECK_THROW( bc_toolbox::pack(little_int), std::out_of_range );
   }
   {
      int64_t big_int = 2147483649;
      BOOST_CHECK_THROW( bc_toolbox::pack(big_int), std::out_of_range );
   }
   {
      int64_t test_int = 1;
      auto rslt = bc_toolbox::pack(test_int);
      std::vector<uint8_t> expected{0x01};
      BOOST_CHECK_EQUAL( 2, rslt.first );
      test_vector( rslt.second, expected );
   }
   {
      int64_t test_int = 63;
      auto rslt = bc_toolbox::pack(test_int);
      std::vector<uint8_t> expected{63};
      BOOST_CHECK_EQUAL( 2, rslt.first );
      test_vector( rslt.second, expected );
   }
   {
      int64_t test_int = 1546288031;
      auto rslt = bc_toolbox::pack(test_int);
      std::vector<uint8_t> expected{0x9f, 0x7b, 0x2a, 0x5c};
      BOOST_CHECK_EQUAL( 8, rslt.first );
      test_vector( rslt.second, expected );
   }
   {
      int64_t test_int = -1546288031;
      auto rslt = bc_toolbox::pack(test_int);
      std::vector<uint8_t> expected{0x9f, 0x7b, 0x2a, 0xdc};
      BOOST_CHECK_EQUAL( 8, rslt.first );
      test_vector( rslt.second, expected );
   }
}

/**
 * A simple script
 */
BOOST_AUTO_TEST_CASE( simple_add )
{
   bc_toolbox::script s;
   // OP_ADD 99 OP_EQUAL
   s.add_opcode(bc_toolbox::OP_ADD);
   s.add_int(99);
   s.add_opcode(bc_toolbox::OP_EQUAL);
   BOOST_TEST_MESSAGE("Convert script to binary array");
   {
      std::vector<uint8_t> expected{0x93, 0x01, 0x63, 0x87};
      test_vector( s.get_bytes_as_vector(), expected );
   }

   BOOST_TEST_MESSAGE("Hashing");
   {
      std::vector<uint8_t> expected{ 0x3f, 0x58, 0xb4, 0xf7, 0xb1, 
            0x48, 0x47, 0xa9, 0x08, 0x36, 0x94, 0xb9, 
            0xb3, 0xb5, 0x2a, 0x4c, 0xea, 0x25, 0x69, 0xed};
      test_vector( s.hash(), expected );
   }
   BOOST_TEST_MESSAGE("Generating P2SH locking script");
   {
      std::vector<uint8_t> expected { 0xa9, 0x14,
            0x3f, 0x58, 0xb4, 0xf7, 0xb1, 0x48, 0x47, 0xa9, 
            0x08, 0x36, 0x94, 0xb9, 0xb3, 0xb5, 0x2a, 0x4c, 
            0xea, 0x25, 0x69, 0xed, 0x87 };
      test_vector( s.p2sh_script(), expected );
   }
}

/**
 * Test a multisig script
 */
BOOST_AUTO_TEST_CASE( multisig )
{
   bc_toolbox::script s;
   s.add_opcode(bc_toolbox::OP_2);
   std::vector<uint8_t> address1{
      0x21, 0x03, 0x07, 0xfd, 0x37, 0x5e, 0xd7, 0xcc, 
      0xed, 0x0f, 0x50, 0x72, 0x3e, 0x3e, 0x1a, 0x97, 
      0xbb, 0xe7, 0xcc, 0xff, 0x73, 0x18, 0xc8, 0x15, 
      0xdf, 0x4e, 0x99, 0xa5, 0x9b, 0xc9, 0x4d, 0xbc, 
      0xd8, 0x19 };
   s.add_bytes(address1);
   std::vector<uint8_t> address2{
      0x21, 0x03, 0x67, 0xc4, 0xf6, 0x66, 0xf1, 0x82, 
      0x79, 0x00, 0x9c, 0x94, 0x1e, 0x57, 0xfa, 0xb3, 
      0xe4, 0x26, 0x53, 0xc6, 0x55, 0x3e, 0x5c, 0xa0, 
      0x92, 0xc1, 0x04, 0xd1, 0xdb, 0x27, 0x9e, 0x32, 
      0x8a, 0x28 };
   s.add_bytes(address2);
   s.add_opcode(bc_toolbox::OP_2);
   s.add_opcode(bc_toolbox::OP_CHECKMULTISIG);
   {
      std::vector<uint8_t> expected{
         0x52, 0x21, 0x03, 0x07, 0xfd, 0x37, 0x5e, 0xd7, 0xcc, 
         0xed, 0x0f, 0x50, 0x72, 0x3e, 0x3e, 0x1a, 0x97, 
         0xbb, 0xe7, 0xcc, 0xff, 0x73, 0x18, 0xc8, 0x15, 
         0xdf, 0x4e, 0x99, 0xa5, 0x9b, 0xc9, 0x4d, 0xbc, 
         0xd8, 0x19, 
         0x21, 0x03, 0x67, 0xc4, 0xf6, 0x66, 0xf1, 0x82, 
         0x79, 0x00, 0x9c, 0x94, 0x1e, 0x57, 0xfa, 0xb3, 
         0xe4, 0x26, 0x53, 0xc6, 0x55, 0x3e, 0x5c, 0xa0, 
         0x92, 0xc1, 0x04, 0xd1, 0xdb, 0x27, 0x9e, 0x32, 
         0x8a, 0x28, 0x52, 0xae
      };
      test_vector( s.get_bytes_as_vector(), expected );
   }
   {
      std::vector<uint8_t> expected {
         0xba, 0xbf, 0x90, 0x63, 0xce, 0xe8, 0xab, 0x6e,
         0x93, 0x34, 0xf9, 0x5f, 0x6d, 0x4e, 0x91, 0x48,
         0xd0, 0xe5, 0x51, 0xc2 };
      test_vector( s.hash(), expected );
   }
   {
      std::vector<uint8_t> expected {
         0xa9, 0x14, 0xba, 0xbf, 0x90, 0x63, 0xce, 0xe8,
         0xab, 0x6e, 0x93, 0x34, 0xf9, 0x5f, 0x6d, 0x4e,
         0x91, 0x48, 0xd0, 0xe5, 0x51, 0xc2, 0x87 };
      test_vector( s.p2sh_script(), expected );
   }
}

/**
 * Test a timelock script
 */
BOOST_AUTO_TEST_CASE( timelock_script )
{
   bc_toolbox::script s;
   s.add_int(1546288031);
   s.add_opcode(bc_toolbox::OP_CHECKLOCKTIMEVERIFY);
   s.add_opcode(bc_toolbox::OP_DROP);
   s.add_opcode(bc_toolbox::OP_DUP);
   s.add_opcode(bc_toolbox::OP_HASH160);
   std::vector<uint8_t> signature{
         0x37, 0x1c, 0x20, 0xfb, 0x2e, 0x98, 0x99, 0x33, 
         0x8c, 0xe5, 0xe9, 0x99, 0x08, 0xe6, 0x4f, 0xd3, 
         0x0b, 0x78, 0x93, 0x13 };
   s.add_bytes_with_size(signature);
   s.add_opcode(bc_toolbox::OP_EQUALVERIFY);
   s.add_opcode(bc_toolbox::OP_CHECKSIG);
   {
      std::vector<uint8_t> expected{
         0x04, 0x9f, 0x7b, 0x2a, 0x5c, 0xb1, 0x75, 0x76, 
         0xa9, 0x14, 0x37, 0x1c, 0x20, 0xfb, 0x2e, 0x98, 
         0x99, 0x33, 0x8c, 0xe5, 0xe9, 0x99, 0x08, 0xe6, 
         0x4f, 0xd3, 0x0b, 0x78, 0x93, 0x13, 0x88, 0xac};
      test_vector( s.get_bytes_as_vector(), expected );
   }
}

/**
 * Test a timelock script mentioned at 
 * https://bitcoin.stackexchange.com/questions/74753/htlc-hash-time-lock-contract-using-bitcoin-qt
 */
BOOST_AUTO_TEST_CASE( timelock_script_stackexchange )
{
   std::vector<uint8_t> sha256 = bc_toolbox::sha256("Bitcoin_rules!");
   std::vector<uint8_t> hash_lock = bc_toolbox::ripemd160(sha256);

   bc_toolbox::script s;
   s.add_opcode(bc_toolbox::OP_IF);
   s.add_opcode(bc_toolbox::OP_HASH160);
   s.add_bytes_with_size(hash_lock);
   s.add_opcode(bc_toolbox::OP_EQUAL);
   s.add_opcode(bc_toolbox::OP_ELSE);
   s.add_opcode(bc_toolbox::OP_DUP);
   s.add_opcode(bc_toolbox::OP_HASH160);
   std::vector<uint8_t> pubkey_hash {
      0x6a, 0x81, 0xe5, 0x87, 0x58, 0x5e, 0x58, 0xb0, 
      0x7d, 0xce, 0x29, 0x3a, 0x08, 0x98, 0x94, 0xa0, 
      0xf8, 0xa6, 0x1b, 0x84 };
   s.add_bytes_with_size(pubkey_hash);
   s.add_opcode(bc_toolbox::OP_EQUALVERIFY);
   s.add_opcode(bc_toolbox::OP_CHECKSIG);
   s.add_opcode(bc_toolbox::OP_ENDIF);
   {
      std::vector<uint8_t> expected_redeem_script {
         0x63, 0xa9, 0x14, 0x81, 0x03, 0xb0, 0xdf, 0x9a, 
         0xd7, 0x5e, 0x2b, 0x77, 0x4f, 0x43, 0xd6, 0xe7, 
         0xe7, 0x1e, 0xea, 0xa2, 0xc7, 0x3e, 0xfb, 0x87, 
         0x67, 0x76, 0xa9, 0x14, 0x6a, 0x81, 0xe5, 0x87, 
         0x58, 0x5e, 0x58, 0xb0, 0x7d, 0xce, 0x29, 0x3a, 
         0x08, 0x98, 0x94, 0xa0, 0xf8, 0xa6, 0x1b, 0x84, 
         0x88, 0xac, 0x68
      };
      test_vector( s.get_bytes_as_vector(), expected_redeem_script ); 
   }
   {
      std::vector<uint8_t> expected_redeem_script_hash {
            0x36, 0x18, 0x01, 0x28, 0x6b, 0x4b, 0xe1, 
            0x37, 0xf6, 0xa4, 0x68, 0xff, 0x8a, 0x34, 
            0xcf, 0xce, 0xb0, 0x71, 0x76, 0x3c };
      test_vector( s.hash(), expected_redeem_script_hash );
   }
   {
      std::vector<uint8_t> expected_p2sh_locking_script {
         0xa9, 0x14, 0x36, 0x18, 0x01, 0x28, 0x6b, 0x4b, 
         0xe1, 0x37, 0xf6, 0xa4, 0x68, 0xff, 0x8a, 0x34, 
         0xcf, 0xce, 0xb0, 0x71, 0x76, 0x3c, 0x87 };
      test_vector( s.p2sh_script(), expected_p2sh_locking_script);
   }
   {
      // attempt to get an address. Start with the redeem script...
      std::vector<uint8_t> script_hash = s.hash();
      // then append 0x05 for mainnet, or 0xc4 for testnet
      script_hash.insert(script_hash.begin(), 0xc4);
      // then base58check
      std::string address1 = bc_toolbox::base58check(script_hash);
      BOOST_CHECK_EQUAL(address1, "2MxBFEWKRPBy96BCxmuZuXkz5CfivDg8e1a");
   }
}

BOOST_AUTO_TEST_SUITE_END()
