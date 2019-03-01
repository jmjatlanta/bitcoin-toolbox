#include <boost/test/unit_test.hpp>

#include <vector> 
#include <stdexcept>
#include <iostream>

#include <script.hpp>
#include <transaction.hpp>
#include <key_io.h>

const std::function<std::string(const char*)> G_TRANSLATION_FUN = [](const char* str) { return str; };

BOOST_AUTO_TEST_SUITE( key_test )

BOOST_AUTO_TEST_CASE( translate )
{
   const char* input = "Hello, World";
   std::string output = G_TRANSLATION_FUN(input);
   BOOST_CHECK(std::string(input) == output);
}


BOOST_AUTO_TEST_CASE( key_conv )
{
   std::string secret = "";

   CKey key = DecodeSecret(secret);
   BOOST_CHECK( key.IsValid() );
}

BOOST_AUTO_TEST_SUITE_END()
