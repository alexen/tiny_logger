/// @file logger.h
/// @brief
/// @copyright Copyright 2023 InfoTeCS Internet Trust

#pragma once

#include <list>
#include <iosfwd>
#include <cstdint>
#include <iostream>

#include <boost/shared_ptr.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/core/addressof.hpp>
#include <boost/core/null_deleter.hpp>
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


class Logger {
public:
     explicit Logger(
          const boost::filesystem::path& logDir
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
     std::uintmax_t totalRecords() const noexcept { return totalRecords_; }

private:
     void createDirectories();
     void addDuplicatedStream( OstreamPtr );
     void addDestinationStream( OstreamPtr );
     boost::filesystem::path buildLogPath() const;
     OstreamPtr makeDestinationStream( const boost::filesystem::path& log );

     const boost::filesystem::path logDir_;
     std::uintmax_t totalRecords_ = 0;

     /// Хранилище нужно только для того, чтобы не допустить разрушения указателей
     /// и вызова деструкторов потоков. Используем std::list<> потому, что нам нужно
     /// только добавлять указатель в коллекцию и не нужно производить с этой коллекцией
     /// никаких операций.
     std::list< OstreamPtr > storage_;
     boost::iostreams::filtering_ostream olog_;
};


} // namespace tiny_logger
} // namespace alexen
