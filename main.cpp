/// @file main.cpp
/// @brief
/// @copyright Copyright 2023 InfoTeCS Internet Trust

#include <set>
#include <list>
#include <chrono>
#include <stdexcept>
#include <iostream>
#include <algorithm>

#include <boost/regex.hpp>
#include <boost/thread.hpp>
#include <boost/bind/bind.hpp>
#include <boost/core/ignore_unused.hpp>
#include <boost/utility/string_view.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/directory.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include <boost/filesystem/operations.hpp>

#include <logger/macro.h>
#include <logger/rotator.h>


using FloatArrN7 = std::array< float, 7 >;


std::ostream& operator<<( std::ostream& os, const FloatArrN7& arr )
{
     std::copy(
          arr.cbegin(),
          arr.cend(),
          std::ostream_iterator< FloatArrN7::value_type >{ os, ", " }
          );
     return os;
}


void testLogger( char** argv )
{
     alexen::tiny_logger::Logger logger{ alexen::tiny_logger::makeAppName( argv ), "./logs", nullptr };

     static_assert( alexen::tiny_logger::inner::filename( __FILE__ )
          == std::string_view{ "main.cpp" } );

     const auto currtime = boost::posix_time::second_clock::local_time();
     const FloatArrN7 arr = {
          1.273645273,
          43.18736141,
          -13.83456382745,
          -991.32654121234,
          9.238173517234,
          -3.142612341634,
          34532.2387462817465
     };

     auto iterations = 150'000u;

     const auto start = std::chrono::steady_clock::now();
     while( iterations-- )
     {
          logger.info() << "This is my very first entry! Remains iterations: " << iterations;
          logger.debug() << "And this is the second line of text! Current time is " << currtime;

          LOG_INFO( logger ) << "This entry made with new macro! We are here: " << boost::filesystem::path{ argv[ 0 ] };
          LOG_ERROR( logger ) << "Error entry! Array: " << arr;
          LOG_DEBUG( logger ) << "Debug debug debug";
          LOG_WARN( logger ) << "WARNING! This is warning message!";
     }
     const auto duration = std::chrono::steady_clock::now() - start;
     std::cout << "Logger: total records made: " << logger.totalRecords()
          << " for " << std::chrono::duration_cast< std::chrono::duration< double > >( duration ).count() << " s ("
          << std::chrono::duration_cast< std::chrono::milliseconds >( duration ).count() << " ms, "
          << std::chrono::duration_cast< std::chrono::microseconds >( duration ).count() << " us)\n";
}


int main( int argc, char** argv )
{
     boost::ignore_unused( argc, argv );
     try
     {
          testLogger( argv );
     }
     catch( const std::exception& e )
     {
          std::cerr << "exception: " << boost::diagnostic_information( e ) << '\n';
          return 1;
     }
     return 0;
}
