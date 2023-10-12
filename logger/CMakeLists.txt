add_library("logger")
target_sources("logger"
     PRIVATE src/logger.cpp
     PUBLIC logger.h
)