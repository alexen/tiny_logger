/// @file rotator_test.cpp
/// @brief
/// @copyright Copyright 2023 InfoTeCS Internet Trust

#include <boost/regex.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/algorithm/string/predicate.hpp>

#include <set>

#include <logger/rotator.h>


BOOST_AUTO_TEST_SUITE( RotatorTest )

using alexen::tiny_logger::Rotator;

BOOST_AUTO_TEST_CASE( TestNextLogNameGeneration )
{
     const auto appName = "ApplicationName";
     const auto logDirPath = "./relative/log/path";
     Rotator rotator{ appName, logDirPath };

     const auto logName = rotator.generateNextLogName();

     BOOST_TEST( boost::algorithm::starts_with( logName.string(), logDirPath ) );
     BOOST_TEST( boost::algorithm::contains( logName.filename().string(), appName ) );
}
BOOST_AUTO_TEST_CASE( TestLogNameRegEx )
{
     const auto appName = "ApplicationName";
     const auto logDirPath = "./relative/log/path";
     Rotator rotator{ appName, logDirPath };

     const auto logName = rotator.generateNextLogName();

     boost::regex regex{ Rotator::logNamePattern };
     BOOST_TEST( boost::regex_match( logName.filename().string(), regex ),
          "\nRegex pattern: " << Rotator::logNamePattern <<
          "\nString: " << logName.string()
          );
}
BOOST_AUTO_TEST_CASE( TestGeneratedFileNameUniqueness )
{
     Rotator rotator{ "", "" };
     std::set< boost::filesystem::path > names;

     const auto total = 800u;
     for( auto i = 0; i < total; ++i )
     {
          BOOST_REQUIRE_NO_THROW( names.insert( rotator.generateNextLogName() ) );
     }
     BOOST_TEST( names.size() == total );
}
BOOST_AUTO_TEST_SUITE_END() /// RotatorTest