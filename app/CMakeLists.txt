cmake_minimum_required(VERSION 3.20.0)
project(DAWN_app)

message(VERBOSE "Processcing app/CMakeLists.txt")
if(MESSAGE_LOG_LEVEL IN_LIST LOG_LEVELS_FOR_INDENTATION)
  list(APPEND CMAKE_MESSAGE_INDENT "  ")
endif()

set(CMAKE_CXX_STANDARD 11)

include(FetchContent)
FetchContent_Declare(
  XercesC
  GIT_REPOSITORY https://github.com/apache/xerces-c.git
  GIT_TAG v3.2.5
  FIND_PACKAGE_ARGS
)
FetchContent_MakeAvailable(XercesC)

add_library(dawn_lib STATIC)
target_sources(dawn_lib PRIVATE DOMTreeErrorReporter.cpp)
target_sources(dawn_lib PRIVATE Result_xml_document.cpp)
target_sources(dawn_lib PRIVATE option_parser.cpp)
target_sources(dawn_lib PRIVATE run.cpp)
target_sources(dawn_lib PRIVATE simulation_definition.cpp)
target_link_libraries(dawn_lib PUBLIC BioCro XercesC::XercesC) # public so tests can use Xerces as well
target_include_directories(dawn_lib PUBLIC ../biocro-lib/src) # public so tests can use BioCro headers as well
target_include_directories(dawn_lib INTERFACE .)

add_executable(dawn main.cpp)
target_link_libraries(dawn PRIVATE dawn_lib)

add_custom_command(
        TARGET dawn POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_SOURCE_DIR}/app/sample_input/
                ${CMAKE_CURRENT_BINARY_DIR}/sample_input
        BYPRODUCTS ${CMAKE_CURRENT_BINARY_DIR}/sample_input)

set_target_properties(dawn_lib PROPERTIES
                      PUBLIC_HEADER app/include/option_parser.h)# app/Result_xml_document.h)
set_target_properties(dawn_lib PROPERTIES
                      PUBLIC_HEADER simulation_definition.h)

install(TARGETS dawn_lib
         ARCHIVE
         PUBLIC_HEADER
         DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/dawn
        )
install(TARGETS dawn)
