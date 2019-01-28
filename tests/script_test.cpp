#define BOOST_TEST_MODULE tests 
#include <boost/test/unit_test.hpp>

#include <vector> 
#include <stdexcept>

#include <script.hpp> 

BOOST_AUTO_TEST_SUITE( script_test )

BOOST_AUTO_TEST_CASE( convert_script )
{
   std::vector<unsigned char> script = { bc_toolbox::OP_DUP, bc_toolbox::OP_HASH160, 14, 0 };
   std::vector<unsigned char> expected = { 0x76, 0xa9, 14, 0 };
   BOOST_CHECK_EQUAL(script[0], 0x76);
}

bool test_vector(std::vector<uint8_t> incoming, std::vector<uint8_t> expected)
{
   BOOST_CHECK_EQUAL( incoming.size(), expected.size() );

   for( uint64_t i = 0; i < incoming.size(); ++i )
   {
      BOOST_CHECK_EQUAL( incoming[i], expected[i] );
   }
   return true;
}

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

BOOST_AUTO_TEST_CASE( simple_add )
{
   bc_toolbox::script s;
   // OP_ADD 99 OP_EQUAL
   s.add_opcode(bc_toolbox::OP_ADD);
   s.add_int(1); // push 1 byte onto stack
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

BOOST_AUTO_TEST_SUITE_END()
