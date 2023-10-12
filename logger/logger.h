/// @file logger.h
/// @brief
/// @copyright Copyright 2023 InfoTeCS Internet Trust

#pragma once

#include <iosfwd>


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


class Logger {
public:
     /// Основной метод вывода в лог с указанием уровня логгирования
     LoggerRecord operator()( const Level );

     /// Вспомогательные методы вывода в лог для упрощения кода
     LoggerRecord debug() { return operator()( Debug ); }
     LoggerRecord info()  { return operator()( Info ); }
     LoggerRecord warn()  { return operator()( Warn ); }
     LoggerRecord error() { return operator()( Error ); }
};


} // namespace tiny_logger
} // namespace alexen
