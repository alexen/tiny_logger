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

     explicit Rotator( const std::string& appName, const boost::filesystem::path& logDir );

     const boost::filesystem::path& logDir() const noexcept { return logDir_; }

     boost::filesystem::path generateNextLogName() const;

private:
     const std::string appName_;
     const boost::filesystem::path logDir_;
};


} // namespace tiny_logger
} // namespace alexen
