cmake_minimum_required(VERSION 3.22)

project(tiny_logger
     DESCRIPTION "Small and simple logger"
     VERSION 1.0.0
     LANGUAGES CXX
     )

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

find_package(Boost REQUIRED
     COMPONENTS
          regex
          thread
          filesystem
          unit_test_framework
)

include(CTest)

add_subdirectory("logger")

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(
     ${PROJECT_NAME}
     PRIVATE
          logger
          Boost::regex
          Boost::thread
          Boost::filesystem
)