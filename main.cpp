/// @file main.cpp
/// @brief
/// @copyright Copyright 2023 InfoTeCS Internet Trust

#include <set>
#include <list>
#include <chrono>
#include <stdexcept>
#include <iostream>
#include <algorithm>
#include <random>

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


struct RandomChars {
     explicit RandomChars( std::size_t n ) : n{ n } {}
     std::ostream& fill( std::ostream& os ) const
     {
          static constexpr boost::string_view chars = "qwertyuiopfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM";
          static std::default_random_engine gen{ static_cast< std::default_random_engine::result_type >( clock() ) };
          static std::uniform_int_distribution< std::size_t > distr{ 0, chars.size() - 1u };

          std::generate_n( std::ostreambuf_iterator< char >{ os }, n,
               []{ return chars[ distr( gen ) ]; } );

          return os;
     }
     const std::size_t n = 0u;
};


std::ostream& operator<<( std::ostream& os, const RandomChars& rc )
{
     return rc.fill( os );
}


void worker( alexen::tiny_logger::Logger& logger, std::size_t iterations )
{
     while( iterations-- )
     {
          /// Эта запись в лог длиной ровно 100 байт:
          /// префикс (44 символа), 55 случайных символов и перенос строки
          logger.debug() << RandomChars{ 55 };
     }
}


int main( const int argc, char** argv )
{
     boost::ignore_unused( argc, argv );
     try
     {
          alexen::tiny_logger::Logger logger{ "appname", "./logs", nullptr };

          std::size_t iterations = 1'000u;
          std::size_t threads = boost::thread::hardware_concurrency();

          if( argc > 2 )
          {
               threads = std::stoi( argv[ 2 ] );
          }
          if( argc > 1 )
          {
               iterations = std::stoll( argv[ 1 ] );
          }

          const auto fn = boost::bind( worker, boost::ref( logger ), iterations );

          const auto start = std::chrono::steady_clock::now();

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
               << " - iterations: " << iterations << '\n'
               << " - threads   : " << threads << '\n'
               << " - records   : " << logger.totalRecords() << " (" << (secs ? logger.totalRecords() / secs : 0.) << " records/s)" << '\n'
               << " - bytes     : " << logger.totalChars()
               << " [" << (logger.totalChars() == (threads * iterations * 100) ? "correct" : "error") << "] ("
               << (secs ? logger.totalChars() / secs : 0.) << " bytes/s)\n";
     }
     catch( const std::exception& e )
     {
          std::cerr << "exception: " << boost::diagnostic_information( e ) << '\n';
          return 1;
     }
     return 0;
}
