set(THIS "logger")
add_library(${THIS})
target_sources(
     ${THIS}
     PRIVATE
          src/logger.cpp
          src/rotator.cpp
     PUBLIC
          logger.h
          rotator.h
)
target_link_libraries(
     ${THIS}
     PRIVATE
          Boost::filesystem
)

if(BUILD_TESTING)
     set(THIS_UTEST ${THIS}-utest)
     add_executable(${THIS_UTEST}
          test/rotator_test.cpp
     )
     target_compile_definitions(
          ${THIS_UTEST}
          PRIVATE
               BOOST_TEST_MODULE ${THIS}
     )
     target_link_libraries(
          ${THIS_UTEST}
          PRIVATE
               ${THIS}
               Boost::regex
               Boost::unit_test_framework
     )
     add_test(${THIS} ${THIS_UTEST})
endif()