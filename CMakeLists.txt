cmake_minimum_required(VERSION 3.20.0)
project(DAWN)

message(VERBOSE "Processing top level CMakeLists.txt")

cmake_language(GET_MESSAGE_LOG_LEVEL MESSAGE_LOG_LEVEL)
set(LOG_LEVELS_FOR_INDENTATION VERBOSE;DEBUG;TRACE)
if(MESSAGE_LOG_LEVEL IN_LIST LOG_LEVELS_FOR_INDENTATION)
  list(APPEND CMAKE_MESSAGE_INDENT "  ")
endif()


set(CMAKE_WARN_DEPRECATED TRUE)
set(CMAKE_CXX_EXTENSIONS OFF)

enable_testing()

add_subdirectory(biocro-lib/src)
add_subdirectory(app)
add_subdirectory(tests)
