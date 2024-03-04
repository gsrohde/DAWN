cmake_minimum_required(VERSION 3.20.0)
project(DAWN_documentation CXX)

# Add this here in case we are not building from the top level:
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../cmake")

message(VERBOSE "Processing doc/CMakeLists.txt")
if(MESSAGE_LOG_LEVEL IN_LIST LOG_LEVELS_FOR_INDENTATION)
  list(APPEND CMAKE_MESSAGE_INDENT "  ")
endif()


include(Doxygen)
Doxygen(docs ../app)
