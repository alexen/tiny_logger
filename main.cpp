/// @file main.cpp
/// @brief
/// @copyright Copyright 2023 InfoTeCS Internet Trust

#include <stdexcept>
#include <iostream>

#include <boost/core/ignore_unused.hpp>
#include <boost/exception/diagnostic_information.hpp>

#include <logger/logger.h>


int main( int argc, char** argv )
{
     boost::ignore_unused( argc, argv );
     try
     {
          alexen::tiny_logger::Logger logger;

          auto c = 10'000u;
          while( c-- )
          {
               logger.info() << "This is my very first entry!" << " It must takes one line!";
               logger.debug() << "And this is the second line of text!";
          }
     }
     catch( const std::exception& e )
     {
          std::cerr << "exception: " << boost::diagnostic_information( e ) << '\n';
          return 1;
     }
     return 0;
}
