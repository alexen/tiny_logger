/// @file rotator.cpp
/// @brief
/// @copyright Copyright 2023 InfoTeCS Internet Trust

#include <logger/rotator.h>

#include <time.h>
#include <stdio.h>

#include <set>
#include <iomanip>
#include <sstream>

#include <boost/regex.hpp>
#include <boost/utility/string_view.hpp>
#include <boost/filesystem/directory.hpp>
#include <boost/filesystem/operations.hpp>


namespace alexen {
namespace tiny_logger {


namespace {
namespace impl {


struct LastWriteTimeGreater
{
     bool operator()( const boost::filesystem::path& lhs, const boost::filesystem::path& rhs ) const
     {
          return boost::filesystem::last_write_time( lhs ) > boost::filesystem::last_write_time( rhs );
     }
};
using FilePathSet = std::multiset< boost::filesystem::path, LastWriteTimeGreater >;


} // namespace impl
} // namespace {unnamed}



Rotator::Rotator(
     const std::string& appName
     , const boost::filesystem::path& logDir
     , const std::size_t maxLogSize
)
     : appName_{ appName }
     , logDir_{ logDir }
     , maxLogSize_{ maxLogSize }
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
          << std::setw( 9 ) << std::setfill( '0' ) << tmspec.tv_nsec
          << suffix;

     return logDir_ / oss.str();
}


boost::filesystem::path Rotator::getCurrentLogFile() const
{
     boost::regex re{ alexen::tiny_logger::Rotator::logNamePattern };

     impl::FilePathSet logFiles;
     std::copy_if( boost::filesystem::directory_iterator{ logDir_ }, {}
          , std::inserter( logFiles, logFiles.end() )
          , [ &re ]( const boost::filesystem::directory_entry& entry ){
               return boost::regex_match( entry.path().filename().string(), re )
                    && boost::filesystem::is_regular_file( entry )
                    && !boost::filesystem::is_symlink( entry );
          });

     const auto latest = logFiles.begin();
     if( latest != logFiles.end() && boost::filesystem::file_size( *latest ) < maxLogSize_ )
     {
          return *latest;
     }
     return generateNextLogName();
}


} // namespace tiny_logger
} // namespace alexen
