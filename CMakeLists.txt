cmake_minimum_required( VERSION 2.6 )
set( Boost_USE_STATIC_LIBS OFF )
set( Boost_USE_MULTITHREADED ON )
set( Boost_USE_STATIC_RUNTIME OFF )
set( Bitcoin_ROOT "/home/jmjatlanta/Development/cpp/bitcoin" CACHE STRING "Root directory of bitcoin (contains the src directory)" )
set( Bitcoin_INCLUDE_DIRS "${Bitcoin_ROOT}/src")
set( Bitcoin_LIBRARY_DIRS "${Bitcoin_ROOT}/src" "${Bitcoin_ROOT}/src/crypto" )
set( Bitcoin_LIBRARIES "bitcoin_common" "bitcoin_crypto_base" "bitcoin_crypto_shani" "bitcoin_crypto_sse41" "bitcoin_crypto_avx2" )
ADD_DEFINITIONS( -DBOOST_TEST_DYN_LINK )

project (test)

find_package( Boost COMPONENTS unit_test_framework REQUIRED )
find_package( OpenSSL REQUIRED )

link_directories( ${Boost_LIBRARY_DIRS} ${Bitcoin_LIBRARY_DIRS} )

include_directories( ${Boost_INCLUDE_DIRS} ${Bitcoin_LIBRARY_DIRS} )
include_directories( src ${Bitcoin_INCLUDE_DIRS} )
add_executable( test 
      tests/script_test.cpp 
      src/hex_conversion.cpp 
      src/script.cpp
      src/transaction.cpp
   )
target_link_libraries( test ${Boost_LIBRARIES} ${Bitcoin_LIBRARIES} OpenSSL::SSL )
