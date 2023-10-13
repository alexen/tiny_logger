/// @file logger.h
/// @brief
/// @copyright Copyright 2023 InfoTeCS Internet Trust

#pragma once

#include <cstdint>
#include <iosfwd>

#include <boost/filesystem/path.hpp>
#include <boost/iostreams/filtering_stream.hpp>


namespace alexen {
namespace tiny_logger {


/// Не используем enum class чтобы меньше было писать:
/// вместо Level::Info - просто Info.
///
/// @note Не переопределяйте индексы значений,
/// они используются в качестве индексов массива!
enum Level {
     Debug,
     Info,
     Warn,
     Error
};


/// Единичная запись в лог.
///
/// @note Пишет сразу в целевой поток без лишнего копирования
/// и без создания промежуточных потоков и буферов!
///
class LoggerRecord {
public:
     explicit LoggerRecord( std::ostream& os, const Level level );
     ~LoggerRecord();

     /// Используем шаблон чтобы по полной использовать
     /// все перегрузки потокового оператора вывода
     template< typename T >
     LoggerRecord& operator<<( const T& value )
     {
          os_ << value;
          return *this;
     }
private:
     std::ostream& os_;
};


using OstreamPtr = boost::shared_ptr< std::ostream >;


class Logger {
public:
     explicit Logger( const boost::filesystem::path& logDir, OstreamPtr console = nullptr );

     /// Основной метод вывода в лог с указанием уровня логгирования
     LoggerRecord operator()( const Level );

     /// Вспомогательные методы вывода в лог для упрощения кода
     LoggerRecord debug() { return operator()( Debug ); }
     LoggerRecord info()  { return operator()( Info ); }
     LoggerRecord warn()  { return operator()( Warn ); }
     LoggerRecord error() { return operator()( Error ); }

     std::uintmax_t totalRecords() const noexcept { return totalRecords_ ; }

private:
     const boost::filesystem::path logDir_;
     std::uintmax_t totalRecords_ = 0;

     OstreamPtr file_;
     OstreamPtr console_;

     boost::iostreams::filtering_ostream olog_;
};


} // namespace tiny_logger
} // namespace alexen
