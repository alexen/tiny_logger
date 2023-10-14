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


namespace alexen {
namespace tiny_logger {


namespace {
namespace impl {


const struct Timestamp_ {} timestamp;

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
LoggerRecord::LoggerRecord( std::ostream& os, const Level level )
     : os_{ os }
{
     static constexpr boost::string_view tail = ": ";
     os_ << impl::timestamp << ' ' << level << tail;
}


/// Деструктор просто сбрасывает буфер (если таковой имеется)
/// с переносом строки
LoggerRecord::~LoggerRecord()
{
     os_ << std::endl;
}


Logger::Logger( const boost::filesystem::path& logDir, OstreamPtr console )
     : logDir_{ logDir }
{
     createDirectories();

     /// Как это ни странно, порядок вызова имеет значение.
     /// Сначала нужно установить фильтр, дублирующий поток,
     /// а затем - целевой поток.
     if( console )
     {
          addDuplicatedStream( console );
     }
     addDestinationStream( makeDestinationStream( buildLogPath() ) );
     BOOST_ASSERT( !storage_.empty() );
}


void Logger::createDirectories()
{
     boost::filesystem::create_directories( logDir_ );
}


void Logger::addDuplicatedStream( OstreamPtr ostr )
{
     BOOST_ASSERT( !!ostr );
     olog_.push( boost::iostreams::tee_filter< std::ostream >{ *ostr } );
     storage_.push_back( std::move( ostr ) );
}


void Logger::addDestinationStream( OstreamPtr ostr )
{
     BOOST_ASSERT( !!ostr );
     olog_.push( *ostr );
     storage_.push_back( std::move( ostr ) );
}


boost::filesystem::path Logger::buildLogPath() const
{
     return logDir_ / "logname.log";
}


OstreamPtr Logger::makeDestinationStream( const boost::filesystem::path& log )
{
     return boost::make_shared< boost::filesystem::ofstream >( log );
}


LoggerRecord Logger::operator()( const Level level )
{
     ++totalRecords_;
     return LoggerRecord{ olog_, level };
}


} // namespace tiny_logger
} // namespace alexen
