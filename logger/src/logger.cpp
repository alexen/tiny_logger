/// @file logger.cpp
/// @brief
/// @copyright Copyright 2023 InfoTeCS Internet Trust

#include <logger/logger.h>

#include <array>
#include <iostream>

#include <boost/utility/string_view.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>


namespace alexen {
namespace tiny_logger {


static std::ostream& operator<<( std::ostream& os, const Level level )
{
     static constexpr std::array< boost::string_view, Error + 1 > asText = {
          "debug", "info", "warn", "error"
     };
     BOOST_ASSERT_MSG( static_cast< std::size_t >( level ) < asText.size(), "Invalid log level" );
     return os << '<' << asText[ level ] << '>';
}


/// Даже **не** сохраняем значение @a level для экономии памяти!
LoggerRecord::LoggerRecord( std::ostream& os, const Level level )
     : os_{ os }
{
     os_ << boost::posix_time::microsec_clock::local_time()
          << ' ' << '<' << level << ':' << ' ';
}


/// Деструктор просто сбрасывает буфер (если таковой имеется)
/// с переносом строки
LoggerRecord::~LoggerRecord()
{
     os_ << std::endl;
}


LoggerRecord Logger::operator()( const Level level )
{
     return LoggerRecord{ std::cerr, level };
}


} // namespace tiny_logger
} // namespace alexen
