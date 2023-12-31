/// @file logger.cpp
/// @brief
/// @copyright Copyright 2023 InfoTeCS Internet Trust

#include <logger/logger.h>

#include <time.h>
#include <stdio.h>

#include <array>
#include <iomanip>
#include <iostream>

#include <boost/make_shared.hpp>
#include <boost/utility/string_view.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/iostreams/tee.hpp>
#include <boost/thread/thread.hpp>


namespace alexen {
namespace tiny_logger {


namespace {
namespace impl {


const struct Timestamp_ {} timestamp;
const struct ThreadId_ {} threadId;

inline std::ostream& operator<<( std::ostream& os, const Timestamp_& )
{
     constexpr auto timestampFormatLen = sizeof( "YYYY-MM-DDTHH:MM:SS" ) - 1u;

     static std::time_t t = 0;
     static tm tm_ = {};
     static char buffer[ timestampFormatLen ];

     time( &t );
     localtime_r( &t, &tm_ );

     snprintf(
          buffer
          , timestampFormatLen + 1u /// Плюс один нулевой символ
          , "%4d-%02d-%02dT%02d:%02d:%02d"
          , tm_.tm_year + 1900
          , tm_.tm_mon + 1
          , tm_.tm_mday
          , tm_.tm_hour
          , tm_.tm_min
          , tm_.tm_sec
          );
     return os.write( buffer, timestampFormatLen );
}

inline std::ostream& operator<<( std::ostream& os, const ThreadId_& )
{
     return os << '{' << boost::this_thread::get_id() << '}';
}


} // namespace impl


inline std::ostream& operator<<( std::ostream& os, const Level level )
{
     static constexpr boost::string_view head = "<";
     static constexpr boost::string_view tail = ">";
     static constexpr std::array< boost::string_view, Error + 1 > asText = {
          "debug", "info", "warn", "error"
     };
     BOOST_ASSERT_MSG( static_cast< std::size_t >( level ) < asText.size(), "Invalid log level" );
     os << head << asText[ level ] << tail;
     return os;
}


} // namespace {unnamed}


/// Даже **не** сохраняем значение @a level для экономии памяти!
LoggerRecord::LoggerRecord( boost::unique_lock< boost::mutex >&& lock, std::ostream& os, const Level level )
     : lock_{ std::move( lock ) }
     , os_{ os }
{
     static constexpr boost::string_view tail = ": ";
     os_ << impl::timestamp << ' ' << impl::threadId << ' ' << level << tail;
}


/// Деструктор просто сбрасывает буфер (если таковой имеется)
/// с переносом строки
LoggerRecord::~LoggerRecord()
{
     os_ << std::endl;
}


Logger::Logger(
     const std::string& appName
     , const boost::filesystem::path& logDir
     , OstreamPtr console
)
     : rotator_{ appName, logDir }
     , console_{ console }
{
     prepareLogDirectory();
     setFilteringStreams();
     startLoggingInto( boost::unique_lock< boost::mutex >{ mutex_ }, rotator_.getCurrentLogFile() );
}


void Logger::prepareLogDirectory()
{
     boost::filesystem::create_directories( rotator_.logDir() );
}


void Logger::setFilteringStreams()
{
     if( console_ )
     {
          olog_.push( boost::iostreams::tee_filter< std::ostream >{ *console_ } );
     }
     olog_.push( boost::ref( counter_ ) );
     olog_.push( ofile_ );
}


void Logger::startLoggingInto( const boost::unique_lock< boost::mutex >&, const boost::filesystem::path& path )
{
     ofile_.close();
     rotator_.rotateLogs();
     updateStat();
     counter_.reset( boost::filesystem::exists( path ) ? boost::filesystem::file_size( path ) : 0u );
     ofile_.open( path, std::ios_base::out | std::ios_base::app );
}


LoggerRecord Logger::operator()( const Level level )
{
     boost::unique_lock< boost::mutex > lock{ mutex_ };
     if( counter_.chars() > rotator_.maxLogSize() )
     {
          startLoggingInto( lock, rotator_.generateNextLogName() );
     }
     ++totalRecords_;
     return LoggerRecord{ std::move( lock ), olog_, level };
}


void Logger::updateStat()
{
     totalChars_ += counter_.chars();
}


} // namespace tiny_logger
} // namespace alexen
