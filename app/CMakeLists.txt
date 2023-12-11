cmake_minimum_required(VERSION 3.20.0)
project(DAWN_app)

set(CMAKE_CXX_STANDARD 11)
find_package(XercesC REQUIRED)

if (XercesC_FOUND)
    message(DEBUG "XercesC was found.")
    message(DEBUG "It's headers are in ${XercesC_INCLUDE_DIRS}.")
endif()

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
                      PUBLIC_HEADER app/simulation_definition.h)
