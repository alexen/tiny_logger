set(THIS "logger")
add_library(${THIS})
target_sources(
     ${THIS}
     PRIVATE src/logger.cpp
     PUBLIC logger.h
)
target_link_libraries(
     ${THIS}
     PRIVATE
          Boost::filesystem
)