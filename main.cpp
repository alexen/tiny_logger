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


#define YES_NO( f ) ((f) ? "yes" : "no")


void worker( alexen::tiny_logger::Logger& logger, std::size_t iterations )
{
     while( iterations-- )
     {
          /// Эта запись в лог длиной ровно 100 байт
          logger.debug() << "Thread is working: time is ~"
               << boost::posix_time::microsec_clock::local_time();
          boost::this_thread::sleep( boost::posix_time::microseconds{ 15 } );
     }
}


int main( int argc, char** argv )
{
     boost::ignore_unused( argc, argv );
     try
     {
          alexen::tiny_logger::Logger logger{ "appname", "./logs", nullptr };

          const auto start = std::chrono::steady_clock::now();

          const auto iterations = 1'000u;
          const auto threads = 5u;
          const auto fn = boost::bind( worker, boost::ref( logger ), 1'000 );

          boost::thread_group tg;
          for( auto i = 0u; i < threads; ++i )
          {
               tg.create_thread( fn );
          }
          tg.join_all();

          const auto duration = std::chrono::steady_clock::now() - start;

          const auto secs = std::chrono::duration_cast< std::chrono::duration< double > >( duration ).count();
          const auto msec = std::chrono::duration_cast< std::chrono::milliseconds >( duration ).count();
          const auto usec = std::chrono::duration_cast< std::chrono::microseconds >( duration ).count();

          logger.updateStat();

          std::cout
               << "Logger stat for " << secs << " s (" << msec << " ms, " << usec << " us)\n"
               << " - records : " << logger.totalRecords() << '\n'
               << " - bytes   : " << logger.totalChars() << " (correct? " << YES_NO(logger.totalChars() == (threads * iterations * 100)) << ")\n";
     }
     catch( const std::exception& e )
     {
          std::cerr << "exception: " << boost::diagnostic_information( e ) << '\n';
          return 1;
     }
     return 0;
}
