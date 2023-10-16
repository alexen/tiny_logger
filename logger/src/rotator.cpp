/// @file rotator.cpp
/// @brief
/// @copyright Copyright 2023 InfoTeCS Internet Trust

#include <logger/rotator.h>

#include <time.h>
#include <stdio.h>

#include <sstream>

#include <boost/utility/string_view.hpp>


namespace alexen {
namespace tiny_logger {


Rotator::Rotator( const std::string& appName, const boost::filesystem::path& logDir )
     : appName_{ appName }
     , logDir_{ logDir }
{}


boost::filesystem::path Rotator::generateNextLogName() const
{
     static constexpr char sep = '_';
     static constexpr boost::string_view suffix = ".log";
     /// ISO C `broken-down time' structure
     static tm bdt = {};
     static timespec tmspec = {};
     static constexpr auto bufferLen = sizeof( "YYYY-MM-DD" ) - 1;
     static char buffer[ bufferLen ] = {};
     thread_local static std::ostringstream oss;

     timespec_get( &tmspec, TIME_UTC );
     localtime_r( &tmspec.tv_sec, &bdt );

     snprintf(
          buffer
          , bufferLen + 1
          , "%04d-%02d-%02d"
          , bdt.tm_year + 1900
          , bdt.tm_mon + 1
          , bdt.tm_mday
          );

     oss.str( {} );
     oss << boost::string_view{ buffer, bufferLen }
          << sep
          << appName_
          << sep
          << tmspec.tv_nsec
          << suffix;

     return logDir_ / oss.str();
}


} // namespace tiny_logger
} // namespace alexen
