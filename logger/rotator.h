/// @file rotator.h
/// @brief
/// @copyright Copyright 2023 InfoTeCS Internet Trust

#pragma once

#include <boost/filesystem/path.hpp>


namespace alexen {
namespace tiny_logger {


inline std::string makeAppName( const char* const* argv )
{
     return boost::filesystem::path{ argv[ 0 ] }
          .filename()
          .replace_extension()
          .string();
}


class Rotator {
public:
     static constexpr auto logNamePattern = R"regex(\d{4}-\d{2}-\d{2}_.*_\d{9}.log)regex";
     static constexpr auto defaultMaxLogSize = 10u * 1024u * 1024u;
     static constexpr auto defaultMaxLogFiles = 25u;

     explicit Rotator(
          const std::string& appName
          , const boost::filesystem::path& logDir
          , std::size_t maxLogSize = Rotator::defaultMaxLogSize
          , unsigned maxLogFiles = Rotator::defaultMaxLogFiles
          );

     const boost::filesystem::path& logDir() const noexcept { return logDir_; }
     std::size_t maxLogSize() const noexcept { return maxLogSize_; }

     /// Генерирует имя нового лог-файла в установленном формате и возвращает путь до него
     ///
     /// @note Метод не создает никаких файлов, а просто генерирует имя с путем для последующего создания.
     ///
     boost::filesystem::path generateNextLogName() const;

     /// Возвращает путь до существующего лог-файла, который необходимо использовать,
     /// либо путь до нового лог-файла, сгенерированного методом @a generateNextLogName()
     ///
     /// @note Метод не производит никаких явных изменений на файловой системе (создание/изменение/удаление файлов).
     ///
     boost::filesystem::path getCurrentLogFile() const;

     /// Удаляет старые логи в директории @a logDir, если общее кол-во логов превышает @a maxLogFiles
     void rotateLogs();

private:
     const std::string appName_;
     const boost::filesystem::path logDir_;
     const std::size_t maxLogSize_;
     const unsigned maxLogFiles_;
};


} // namespace tiny_logger
} // namespace alexen
