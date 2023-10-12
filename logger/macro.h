/// @file macro.h
/// @brief
/// @copyright Copyright 2023 InfoTeCS Internet Trust

#pragma once

#include <string_view>
#include <c++/11/filesystem>
#include <boost/filesystem/path.hpp>

#include <logger/logger.h>


namespace alexen {
namespace tiny_logger {
namespace inner {

/// Использованы std::string_view (вместо boost::string_view) из-за того,
/// что у std::string_view лучше поддержка constexpr и ф-ция выполняется
/// в compile-time
///
constexpr std::string_view filename( std::string_view path )
{
     return path.substr( path.find_last_of( boost::filesystem::path::preferred_separator ) + 1 );
}


} // namespace inner
} // namespace tiny_logger
} // namespace alexen



#define LOG_PRIVATE( call ) \
     call() << '(' << alexen::tiny_logger::inner::filename( __FILE__ ) \
          << ':' << __LINE__ << ')' << ' '

#define LOG_DEBUG( logger )   LOG_PRIVATE( logger.debug )
#define LOG_INFO( logger )    LOG_PRIVATE( logger.info )
#define LOG_WARN( logger )    LOG_PRIVATE( logger.warn )
#define LOG_ERROR( logger )   LOG_PRIVATE( logger.error )
