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


int main( int argc, char** argv )
{
     boost::ignore_unused( argc, argv );
     try
     {
     }
     catch( const std::exception& e )
     {
          std::cerr << "exception: " << boost::diagnostic_information( e ) << '\n';
          return 1;
     }
     return 0;
}
