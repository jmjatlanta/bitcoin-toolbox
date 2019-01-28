#define BOOST_TEST_MODULE tests 
#include <boost/test/unit_test.hpp>

#include <vector> 
#include <hex_conversion.hpp> 

BOOST_AUTO_TEST_SUITE( script_test )

BOOST_AUTO_TEST_CASE( convert_script )
{
   std::vector<unsigned char> script = { bc_toolbox::OP_DUP, bc_toolbox::OP_HASH160, 14, 0 };
   std::vector<unsigned char> expected = { 0x76, 0xa9, 14, 0 };
   BOOST_CHECK_EQUAL(script[0], 0x76);
}

BOOST_AUTO_TEST_SUITE_END()
