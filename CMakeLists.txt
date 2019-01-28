cmake_minimum_required( VERSION 2.6 )
set( Boost_USE_STATIC_LIBS OFF )
set( Boost_USE_MULTITHREADED ON )
set( Boost_USE_STATIC_RUNTIME OFF )
ADD_DEFINITIONS( -DBOOST_TEST_DYN_LINK )

project (test)

find_package( Boost COMPONENTS unit_test_framework REQUIRED )

link_directories( ${Boost_LIBRARY_DIRS} )
include_directories( ${Boost_INCLUDE_DIRS} )
include_directories( src )
add_executable( test tests/script_test.cpp )
target_link_libraries( test ${Boost_LIBRARIES} )
