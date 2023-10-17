/// @file logger.h
/// @brief
/// @copyright Copyright 2023 InfoTeCS Internet Trust

#pragma once

#include <iosfwd>
#include <iostream>

#include <boost/shared_ptr.hpp>
#include <boost/core/addressof.hpp>
#include <boost/core/null_deleter.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/lock_guard.hpp>
#include <boost/atomic.hpp>

#include <logger/rotator.h>


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
     LoggerRecord( boost::mutex& m, std::ostream& os, const Level level );
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
     boost::lock_guard< boost::mutex > lock_;
     std::ostream& os_;
};


using OstreamPtr = boost::shared_ptr< std::ostream >;


/// Удобная ф-ция для того чтобы превращать потоки,
/// переданные по ссылке, в boost::shared_ptr
///
/// @attention Исходный поток, передаваемый по ссылке,
/// должен жить все время существования формируемого указателя!
///
inline OstreamPtr makeOstreamPtr( std::ostream& os )
{
     return boost::shared_ptr< std::ostream >{ boost::addressof( os ), boost::null_deleter{} };
}


/// Фильтр для подсчета кол-во записанных в поток байт
struct Counter : boost::iostreams::multichar_output_filter {
     explicit Counter( std::size_t init = 0u ) : chars_{ init } {}

     template< typename Sink >
     std::streamsize write( Sink& sink, const char_type* s, std::streamsize n )
     {
          const auto result = boost::iostreams::write( sink, s, n );
          chars_ += result > 0 ? result : 0;
          return result;
     }

     std::size_t chars() const noexcept { return chars_; }
     void reset( std::size_t init = 0u ) noexcept { chars_ = init; }

private:
     std::size_t chars_ = 0;
};


class Logger {
public:
     Logger(
          const std::string& appName
          , const boost::filesystem::path& logDir
          , OstreamPtr console = makeOstreamPtr( std::cerr )
     );

     /// Основной метод вывода в лог с указанием уровня логгирования
     LoggerRecord operator()( const Level );

     /// Вспомогательные методы вывода в лог для упрощения кода
     LoggerRecord debug() { return operator()( Debug ); }
     LoggerRecord info()  { return operator()( Info ); }
     LoggerRecord warn()  { return operator()( Warn ); }
     LoggerRecord error() { return operator()( Error ); }

     /// Возвращает кол-во @a LogRecord, сделанных за время жизни @a Logger
     std::uintmax_t totalRecords() const noexcept { return totalRecords_.value(); }

private:
     void prepareLogDirectory();
     void setFilteringStreams();
     void startLoggingInto( const boost::filesystem::path& path );

     Rotator rotator_;

     boost::atomic< std::uintmax_t > totalRecords_ = { 0 };

     /// Опциональный указатель на дополнительный (дублирующий) выходной поток.
     /// Предполагается, что это будет std::cerr, но использовать можно любой std::ostream.
     OstreamPtr console_;
     boost::filesystem::ofstream ofile_;
     Counter counter_;
     boost::iostreams::filtering_ostream olog_;

     boost::mutex mutex_;
};


} // namespace tiny_logger
} // namespace alexen
