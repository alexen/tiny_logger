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

#include <logger/logger.h>


void worker( alexen::tiny_logger::Logger& logger, std::size_t iterations )
{
     logger.info() << "Worker started!";
     try
     {
          while( iterations-- )
          {
               logger.debug() << "Thread is working: time is " << boost::posix_time::microsec_clock::local_time();
          }
          BOOST_THROW_EXCEPTION( std::runtime_error{ "Iterations completed" } );
     }
     catch( const std::exception& e )
     {
          logger.error() << "Exception: " << boost::diagnostic_information( e );
     }
     logger.info() << "Worker finished!";
}


int main( int argc, char** argv )
{
     boost::ignore_unused( argc, argv );
     try
     {
          alexen::tiny_logger::Logger logger{ "appname", "./logs", nullptr };

          const auto start = std::chrono::steady_clock::now();

          const auto fn = boost::bind( worker, boost::ref( logger ), 500'000 );

          boost::thread_group tg;
          tg.create_thread( fn );
          tg.create_thread( fn );
          tg.create_thread( fn );
          tg.create_thread( fn );
          tg.join_all();

          const auto duration = std::chrono::steady_clock::now() - start;
          std::cout << "Logger: total records made: " << logger.totalRecords()
               << " for " << std::chrono::duration_cast< std::chrono::duration< double > >( duration ).count() << " s ("
               << std::chrono::duration_cast< std::chrono::milliseconds >( duration ).count() << " ms, "
               << std::chrono::duration_cast< std::chrono::microseconds >( duration ).count() << " us)\n";

     }
     catch( const std::exception& e )
     {
          std::cerr << "exception: " << boost::diagnostic_information( e ) << '\n';
          return 1;
     }
     return 0;
}
