cmake_minimum_required( VERSION 2.6 )
set( CMAKE_CXX_STANDARD 11 )
set( Boost_USE_STATIC_LIBS OFF )
set( Boost_USE_MULTITHREADED ON )
set( Boost_USE_STATIC_RUNTIME OFF )
set( Bitcoin_ROOT "/home/jmjatlanta/Development/cpp/bitcoin" CACHE STRING "Root directory of bitcoin (contains the src directory)" )
set( Bitcoin_INCLUDE_DIRS "${Bitcoin_ROOT}/src")
set( Bitcoin_LIBRARY_DIRS "${Bitcoin_ROOT}/src" "${Bitcoin_ROOT}/src/crypto" "${Bitcoin_ROOT}/src/secp256k1/.libs")
set( Bitcoin_LIBRARIES 
   "bitcoin_wallet"
   "bitcoin_cli"
   "bitcoin_server"
   "bitcoin_zmq"
   "bitcoin_util"
   "bitcoin_common" 
   "bitcoin_crypto_base" 
   "bitcoin_consensus"
   "bitcoin_util"
   "secp256k1"
   "bitcoin_crypto_shani" 
   "bitcoin_crypto_sse41" 
   "bitcoin_crypto_avx2" 
   )
ADD_DEFINITIONS( -DBOOST_TEST_DYN_LINK )

project ( null_func )

add_library( null_func src/translation_func_null.cpp )

project (test)

find_package( Boost COMPONENTS unit_test_framework filesystem thread REQUIRED )
find_package( OpenSSL REQUIRED )

link_directories( ${Boost_LIBRARY_DIRS} ${Bitcoin_LIBRARY_DIRS} )

include_directories( ${Boost_INCLUDE_DIRS} ${Bitcoin_LIBRARY_DIRS} )
include_directories( src ${Bitcoin_INCLUDE_DIRS} )
add_executable( test 
      tests/script_test.cpp
      # tests/key_test.cpp 
      src/hex_conversion.cpp 
      src/script.cpp
      src/transaction.cpp
   )
target_link_libraries( test 
   ${Bitcoin_LIBRARIES} 
   # null_func 
   ${Boost_LIBRARIES} 
   OpenSSL::SSL 
   -lpthread 
)

#project (test2)
#add_executable ( test2 tests/test2.cpp )
#target_link_libraries( test2 null_func )

project (hash_256)
add_executable( hash_256 utils/hash_256.cpp
   src/hex_conversion.cpp
 )
 target_link_libraries( hash_256 
   ${Bitcoin_LIBRARIES}
   ${Boost_Libraries}
   OpenSSL::SSL
 )

project (calc_timeout)
add_executable (calc_timeout utils/calc_timeout.cpp)

project (calc_script_address )
add_executable (calc_script_address 
   utils/calc_script_address.cpp 
   src/script.cpp
   src/hex_conversion.cpp
)
target_link_libraries( calc_script_address
   ${Bitcoin_LIBRARIES}
   ${Boost_Libraries}
   OpenSSL::SSL
 )

project (calc_redeem_script )
add_executable (calc_redeem_script
   utils/calc_redeem_script.cpp 
   src/script.cpp
   src/hex_conversion.cpp
)
target_link_libraries( calc_redeem_script
   ${Bitcoin_LIBRARIES}
   ${Boost_Libraries}
   OpenSSL::SSL
 )

