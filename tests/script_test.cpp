#define BOOST_TEST_MODULE tests 
#include <boost/test/unit_test.hpp>

#include <vector> 
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <fstream>

#include <script.hpp>
#include <transaction.hpp>

BOOST_AUTO_TEST_SUITE( script_test )

/**
 * Helper to check vectors for equality
 */
bool test_vector(std::vector<uint8_t> incoming, std::vector<uint8_t> expected)
{
   BOOST_CHECK_EQUAL( incoming.size(), expected.size() );

   for( uint64_t i = 0; i < incoming.size(); ++i )
   {
      if ( incoming[i] != expected[i] )
      {
         std::stringstream ss;
         ss << std::hex << "At position " << std::to_string(i);
         ss << " found 0x" << +incoming[i];
         ss << " but expected 0x" << +expected[i];
         BOOST_TEST_MESSAGE( ss.str() );
         BOOST_CHECK_EQUAL( incoming[i] ,expected[i] );
      }
   }
   return true;
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

std::vector<uint8_t> string_to_vector(std::string incoming)
{
   std::vector<uint8_t> ret_val(incoming.begin(), incoming.end());
   return ret_val;
}

/*****
 * convert a public key to a hash160
 */
std::vector<uint8_t> pubkey_hash(std::vector<uint8_t> incoming)
{
   std::vector<uint8_t> sh256 = bc_toolbox::sha256(incoming);
   return bc_toolbox::ripemd160(sh256);
}

std::string read_pk_from_file(std::string filename)
{
   std::ifstream infile(filename);
   std::string retval;
   infile >> retval;
   return retval;
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

BOOST_AUTO_TEST_CASE( hash_test )
{
   std::vector<uint8_t> sha256 = bc_toolbox::sha256("Bitcoin_rules!");
   std::vector<uint8_t> hash_lock = bc_toolbox::ripemd160(sha256);

   std::vector<uint8_t> expected_hash = {
      0x81, 0x03, 0xb0, 0xdf, 0x9a, 0xd7, 0x5e, 0x2b, 0x77, 0x4f,
      0x43, 0xd6, 0xe7, 0xe7, 0x1e, 0xea, 0xa2, 0xc7, 0x3e, 0xfb
   };
   std::cout << "The hash of Bitcoin_rules! is :" << vector_to_hex_string(expected_hash) << std::endl;
   test_vector( hash_lock, expected_hash); 
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

/*****
 * Testing my interpretation of an HTLC script
 */
BOOST_AUTO_TEST_CASE( timelock_script_jmjatlanta )
{
   // key stuff
   std::string priv_key_string = read_pk_from_file("john_privkey.txt");
   std::vector<uint8_t> priv_key = bc_toolbox::hex_string_to_vector(priv_key_string);
   std::vector<uint8_t> jmjatlanta_pubkey_hash = pubkey_hash(priv_key);
   std::vector<uint8_t> recipient_pubkey_hash = pubkey_hash(priv_key);
   // lock parameters
   std::vector<uint8_t> sha256 = bc_toolbox::sha256("jmjatlanta_rules!");
   std::vector<uint8_t> hash_lock = bc_toolbox::ripemd160(sha256);
   int timeout = 1551447083; // 2019-3-2 13:31:23

   // following bip199
   bc_toolbox::script s;
   s.add_opcode(bc_toolbox::OP_IF);
   s.add_opcode(bc_toolbox::OP_HASH160);
   s.add_bytes_with_size(hash_lock);
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
   s.add_bytes_with_size(jmjatlanta_pubkey_hash);
   s.add_opcode(bc_toolbox::OP_ENDIF);
   s.add_opcode(bc_toolbox::OP_EQUALVERIFY);
   s.add_opcode(bc_toolbox::OP_CHECKSIG);

   std::cout << "Script as a byte string: " << vector_to_hex_string(s.get_bytes_as_vector()) << std::endl;
   std::vector<uint8_t> redeem_script = s.hash();
   // append 0x05 for mainnet, or 0xc4 for testnet
   redeem_script.insert(redeem_script.begin(), 0xc4);
   // then base58check
   std::string address1 = bc_toolbox::base58check(redeem_script);
   BOOST_CHECK_EQUAL(address1, "2MwLbxZcHjovnhKMhQHa87TBpJKT1cxCJ8x");
   std::string results = vector_to_hex_string(redeem_script);
   std::cout << "Results of timelock_script_jmjatlanta: " << results << std::endl;

   // now we need to fund the address with a UTXO that contains the script
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
      std::cout << "StackExchange script: " << vector_to_hex_string(expected_redeem_script) << std::endl;
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
   // now send funds to the address
   // now start the redeem by creating a basic raw transaction
   // now sign the basic raw transaction
   // now bring in the secret, as the wallet does not support custom scripts
   // now send the transaction
}

BOOST_AUTO_TEST_CASE( transaction_tests )
{
   bc_toolbox::transaction trx;
   trx.version = 2;
   trx.flag = 1;
   bc_toolbox::input in;
   in.hash = {
      0x11, 0xb6, 0xe0, 0x46, 0x0b, 0xb8, 0x10, 0xb0, 0x57, 0x44, 0xf8, 
      0xd3, 0x82, 0x62, 0xf9, 0x5f, 0xba, 0xb0, 0x2b, 0x16, 0x8b, 0x07, 
      0x05, 0x98, 0xa6, 0xf3, 0x1f, 0xad, 0x43, 0x8f, 0xce, 0xd4 };
   in.index = 0;
   in.sig_script = {
      0x16, 0x00, 0x14, 0x27, 0xc1, 0x06, 0x01, 0x3c, 0x00, 0x42, 0xda, 
      0x16, 0x5c, 0x08, 0x2b, 0x38, 0x70, 0xc3, 0x1f, 0xb3, 0xab, 0x46, 0x83
   };
   in.sequence = 0xfeffffff;
   trx.inputs.push_back(in);
   bc_toolbox::output out1;
   out1.value = 1000000000;
   out1.script = {
      0xa9, 0x14, 0xd8, 0xb6, 0xfc, 0xc8, 0x5a, 0x38, 0x32, 0x61, 0xdf, 
      0x05, 0x42, 0x3d, 0xdf, 0x06, 0x8a, 0x89, 0x87, 0xbf, 0x02, 0x87, 0x87
   };
   trx.outputs.push_back(out1);
   bc_toolbox::output out2;
   out2.value = 8499980080;
   out2.script = {
      0xa9, 0x14, 0xd5, 0xdf, 0x0b, 0x9c, 0xa6, 0xc0, 0xe1, 0xba, 0x60, 
      0xa9, 0xff, 0x29, 0x35, 0x9d, 0x26, 0x00, 0xd9, 0xc6, 0x65, 0x9d, 0x87
   };
   trx.outputs.push_back(out2);
   bc_toolbox::witness wit1;
   wit1.data = {
      0x30, 0x44, 0x02, 0x20, 0x3b, 0x85, 0xcb, 0x05, 0xb4, 0x3c, 0xc6, 
      0x8d, 0xf7, 0x2e, 0x2e, 0x54, 0xc6, 0xcb, 0x50, 0x8a, 0xa3, 0x24, 
      0xa5, 0xde, 0x0c, 0x53, 0xf1, 0xbb, 0xfe, 0x99, 0x7c, 0xbd, 0x75, 
      0x09, 0x77, 0x4d, 0x02, 0x20, 0x41, 0xe1, 0xb1, 0x82, 0x3b, 0xda, 
      0xdd, 0xcd, 0x65, 0x81, 0xd7, 0xcd, 0xe6, 0xe6, 0xa4, 0xc4, 0xdb, 
      0xef, 0x48, 0x3e, 0x42, 0xe5, 0x9e, 0x04, 0xdb, 0xac, 0xba, 0xf5, 
      0x37, 0xc3, 0xe3, 0xe8, 0x01
   };
   trx.witnesses.push_back(wit1);
   bc_toolbox::witness wit2;
   wit2.data = {
      0x03, 0xfb, 0xbd, 0xb3, 0xb3, 0xfc, 0x3a, 0xbb, 0xbd, 0x98, 0x3b, 
      0x20, 0xa5, 0x57, 0x44, 0x5f, 0xb0, 0x41, 0xd6, 0xf2, 0x1c, 0xc5, 
      0x97, 0x7d, 0x21, 0x21, 0x97, 0x1c, 0xb1, 0xce, 0x52, 0x98, 0x97
   };
   trx.witnesses.push_back(wit2);
   trx.locktime = 140; // block 140 (8c000000)
   std::vector<uint8_t> expected = {
      0x02, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x11, 0xb6, 0xe0, 0x46, 
      0x0b, 0xb8, 0x10, 0xb0, 0x57, 0x44, 0xf8, 0xd3, 0x82, 0x62, 0xf9, 
      0x5f, 0xba, 0xb0, 0x2b, 0x16, 0x8b, 0x07, 0x05, 0x98, 0xa6, 0xf3, 
      0x1f, 0xad, 0x43, 0x8f, 0xce, 0xd4, 0x00, 0x00, 0x00, 0x00, 0x17, 
      0x16, 0x00, 0x14, 0x27, 0xc1, 0x06, 0x01, 0x3c, 0x00, 0x42, 0xda, 
      0x16, 0x5c, 0x08, 0x2b, 0x38, 0x70, 0xc3, 0x1f, 0xb3, 0xab, 0x46, 
      0x83, 0xfe, 0xff, 0xff, 0xff, 0x02, 0x00, 0xca, 0x9a, 0x3b, 0x00, 
      0x00, 0x00, 0x00, 0x17, 0xa9, 0x14, 0xd8, 0xb6, 0xfc, 0xc8, 0x5a, 
      0x38, 0x32, 0x61, 0xdf, 0x05, 0x42, 0x3d, 0xdf, 0x06, 0x8a, 0x89, 
      0x87, 0xbf, 0x02, 0x87, 0x87, 0x30, 0x67, 0xa3, 0xfa, 0x01, 0x00, 
      0x00, 0x00, 0x17, 0xa9, 0x14, 0xd5, 0xdf, 0x0b, 0x9c, 0xa6, 0xc0, 
      0xe1, 0xba, 0x60, 0xa9, 0xff, 0x29, 0x35, 0x9d, 0x26, 0x00, 0xd9, 
      0xc6, 0x65, 0x9d, 0x87, 0x02, 0x47, 0x30, 0x44, 0x02, 0x20, 0x3b, 
      0x85, 0xcb, 0x05, 0xb4, 0x3c, 0xc6, 0x8d, 0xf7, 0x2e, 0x2e, 0x54, 
      0xc6, 0xcb, 0x50, 0x8a, 0xa3, 0x24, 0xa5, 0xde, 0x0c, 0x53, 0xf1, 
      0xbb, 0xfe, 0x99, 0x7c, 0xbd, 0x75, 0x09, 0x77, 0x4d, 0x02, 0x20, 
      0x41, 0xe1, 0xb1, 0x82, 0x3b, 0xda, 0xdd, 0xcd, 0x65, 0x81, 0xd7, 
      0xcd, 0xe6, 0xe6, 0xa4, 0xc4, 0xdb, 0xef, 0x48, 0x3e, 0x42, 0xe5, 
      0x9e, 0x04, 0xdb, 0xac, 0xba, 0xf5, 0x37, 0xc3, 0xe3, 0xe8, 0x01, 
      0x21, 0x03, 0xfb, 0xbd, 0xb3, 0xb3, 0xfc, 0x3a, 0xbb, 0xbd, 0x98, 
      0x3b, 0x20, 0xa5, 0x57, 0x44, 0x5f, 0xb0, 0x41, 0xd6, 0xf2, 0x1c, 
      0xc5, 0x97, 0x7d, 0x21, 0x21, 0x97, 0x1c, 0xb1, 0xce, 0x52, 0x98, 
      0x97, 0x8c, 0x00, 0x00, 0x00 
   };
   test_vector(trx.to_bytes(), expected );
}

BOOST_AUTO_TEST_CASE( varint_test )
{
   {
      uint8_t test = 0;
      BOOST_TEST_MESSAGE( "Testing 0");
      std::vector<uint8_t> result = bc_toolbox::to_varint(test);
      std::vector<uint8_t> expected = { 0x00 };
      test_vector( result, expected );
      test = 252;
      BOOST_TEST_MESSAGE( "Testing 252");
      result = bc_toolbox::to_varint(test);
      expected = { 0xfc };
      test_vector( result, expected );
      test = 106;
      BOOST_TEST_MESSAGE( "Testing 106");
      result = bc_toolbox::to_varint(test);
      expected = { 0x6a };
      test_vector( result, expected );
   }
   {
      BOOST_TEST_MESSAGE( "Testing 253");
      uint16_t test = 253;
      std::vector<uint8_t> result = bc_toolbox::to_varint( test );
      std::vector<uint8_t> expected = { 0xfd, 0xfd, 0x00 };
      test_vector( result, expected );
      BOOST_TEST_MESSAGE( "Testing 550");
      test = 550;
      result = bc_toolbox::to_varint( test );
      expected = { 0xfd, 0x26, 0x02 };
      test_vector( result, expected );
   }
   {
      BOOST_TEST_MESSAGE( "Testing 998000");
      uint64_t test = 998000;
      std::vector<uint8_t> result = bc_toolbox::to_varint( test );
      std::vector<uint8_t> expected = { 0xfe, 0x70, 0x3a, 0x0f, 0x00 };
      test_vector( result, expected );
   }
}

BOOST_AUTO_TEST_CASE( raw_transaction_parse )
{
   std::string raw_tx_string = "0200000001284f2c75c4ff937f83f48b16f56b2f9049fe101a2341e219e7996cd1f28eb54d01000000af4cad63a914d31466ed1232e9e156c859e74911489cc7d430df8876a9423032613637623661306262336532373234353832633333313666313337393832623066643163643766613737386334396431343238646134626234376438333935376704bc7aa55cb17576a9423032613637623661306262336532373234353832633333313666313337393832623066643163643766613737386334396431343238646134626234376438333935376888acffffffff01a0bb0d000000000017a9141911177214bca4efb78eaf27f3cbc5d3ded12a5a8700000000";
   std::vector<uint8_t> bytes = bc_toolbox::hex_string_to_vector(raw_tx_string);
   bc_toolbox::transaction tx(bytes);
   std::vector<uint8_t> result_bytes = tx.to_bytes();
   test_vector(result_bytes, bytes);
}

BOOST_AUTO_TEST_SUITE_END()
