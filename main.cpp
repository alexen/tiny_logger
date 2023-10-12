/// @file main.cpp
/// @brief
/// @copyright Copyright 2023 InfoTeCS Internet Trust

#include <string_view>
#include <stdexcept>
#include <iostream>

#include <boost/core/ignore_unused.hpp>
#include <boost/utility/string_view.hpp>
#include <boost/exception/diagnostic_information.hpp>

#include <logger/macro.h>

int main( int argc, char** argv )
{
     boost::ignore_unused( argc, argv );
     try
     {
          alexen::tiny_logger::Logger logger;

          static_assert( alexen::tiny_logger::inner::filename( __FILE__ )
               == std::string_view{ "main.cpp" } );

          auto c = 5'000u;
          while( c-- )
          {
               logger.info() << "This is my very first entry!" << " It must takes one line!";
               logger.debug() << "And this is the second line of text!";

               LOG_INFO( logger ) << "This entry made with new macro!";
               LOG_ERROR( logger ) << "Error entry!";
               LOG_DEBUG( logger ) << "Debug debug debug";
               LOG_WARN( logger ) << "WARNING! This is warning message!";
          }
     }
     catch( const std::exception& e )
     {
          std::cerr << "exception: " << boost::diagnostic_information( e ) << '\n';
          return 1;
     }
     return 0;
}
