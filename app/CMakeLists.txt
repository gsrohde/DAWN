set(CMAKE_CXX_STANDARD 11)
find_package(XercesC REQUIRED)

if (XercesC_FOUND)
    message(${XercesC_INCLUDE_DIRS})
    message("XercesC was found.")
endif()

add_library(dawn_lib STATIC)
target_sources(dawn_lib PRIVATE DOMTreeErrorReporter.cpp)
target_sources(dawn_lib PRIVATE Result_xml_document.cpp)
target_sources(dawn_lib PRIVATE option_parser.cpp)
target_sources(dawn_lib PRIVATE run.cpp)
target_sources(dawn_lib PRIVATE simulation_definition.cpp)

target_link_libraries(dawn_lib PRIVATE BioCro XercesC::XercesC)
message(${CMAKE_CURRENT_BINARY_DIR})
cmake_minimum_required(VERSION 3.20.0)
project(DAWN_app)

target_include_directories(dawn_lib SYSTEM PUBLIC ../biocro-lib/src XercesC::XercesC_INCLUDE_DIRS)

add_executable(dawn main.cpp)
target_link_libraries(dawn PRIVATE dawn_lib)

add_custom_command(
        TARGET dawn POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_SOURCE_DIR}/app/sample_input/
                ${CMAKE_CURRENT_BINARY_DIR}/sample_input
        BYPRODUCTS ${CMAKE_CURRENT_BINARY_DIR}/sample_input)

