cmake_minimum_required(VERSION 3.20.0)
project(DAWN_app)

message(VERBOSE "Processing app/CMakeLists.txt")
if(MESSAGE_LOG_LEVEL IN_LIST LOG_LEVELS_FOR_INDENTATION)
  list(APPEND CMAKE_MESSAGE_INDENT "  ")
endif()

set(CMAKE_CXX_STANDARD 11)


set(XercesC_ROOT CACHE FILEPATH "Set for non-standard XercesC installations")
find_package(XercesC 3.2)
if (XercesC_FOUND)
    message("XercesC version " ${XercesC_VERSION} " was found.")
    message("The library file is located at " ${XercesC_LIBRARY})
else()
    message(FATAL_ERROR "XercesC was not found.")
endif()


add_library(dawn_lib)

target_sources(dawn_lib PRIVATE DOMTreeErrorReporter.cpp)
target_sources(dawn_lib PRIVATE Result_xml_document.cpp)
target_sources(dawn_lib PRIVATE option_parser.cpp)
target_sources(dawn_lib PRIVATE run.cpp)
target_sources(dawn_lib PRIVATE simulation_definition.cpp)

target_link_libraries(dawn_lib PUBLIC XercesC::XercesC) # PUBLIC so dawn app and tests can use Xerces as well
target_link_libraries(dawn_lib PUBLIC BioCro) # PUBLIC so tests (specifically, simulation_definitions_tests)
                                              # can use BioCro headers as well
target_include_directories(dawn_lib PUBLIC include) # PUBLIC so tests can use headers in include subdirectory as well

option(USE_REMOTE_SCHEMA_URI "Whether or not to fetch the schema from a public repository rather than using a local file" OFF)
option(USE_LOCAL_SCHEMA_FILE "Whether to look for the schema file locally" ON)
target_compile_definitions(dawn_lib PRIVATE
    $<$<BOOL:${USE_LOCAL_SCHEMA_FILE}>:USE_LOCAL_SCHEMA_FILE>
    $<$<BOOL:${USE_REMOTE_SCHEMA_URI}>:USE_REMOTE_SCHEMA_URI>
    INSTALL_SCHEMA_FILEPATH=${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATADIR}/dawn/simulation-specification.xsd
    BUILD_SCHEMA_FILEPATH=${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_DATADIR}/dawn/simulation-specification.xsd
)

# Copy the schema files to the build tree
add_custom_command(
        TARGET dawn_lib POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_SOURCE_DIR}/app/xml_schema
                ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_DATADIR}/dawn
        BYPRODUCTS ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_DATADIR}/dawn)


# Set RPATH for installation prior to adding the executable:
file(RELATIVE_PATH relativeRpath
    ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}
    ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}
)
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(EXECUTABLE_PATH "$ORIGIN")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(EXECUTABLE_PATH "@loader_path")
else() # e.g. Windows
    message("To set a non-standard installation prefix, use\n"
            "'cmake -B <build tree> -S <source tree> --install-prefix <non-standard installation prefix>'\n"
            "when running the configuration stage.")
endif()
list(APPEND CMAKE_INSTALL_RPATH
    # should work on macOS or UNIX if user overrides the default installation prefix at install time with
    #   cmake --install <build tree> --prefix <install prefix>
    ${EXECUTABLE_PATH}/${relativeRpath}

    # works if CMAKE_INSTALL_PREFIX is set during CMake config stage using
    #   cmake -B <build tree> -S <source tree> --install-prefix <non-standard installation prefix>
    ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}
)

add_executable(dawn main.cpp)
target_link_libraries(dawn PRIVATE dawn_lib)

add_custom_command(
        TARGET dawn_lib POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_SOURCE_DIR}/app/sample_input/
                ${CMAKE_CURRENT_BINARY_DIR}/sample_input
        BYPRODUCTS ${CMAKE_CURRENT_BINARY_DIR}/sample_input)

set_target_properties(dawn_lib PROPERTIES
                      PUBLIC_HEADER "app/include/solver.h;app/include/simulation_definition.h;app/include/run.h;app/include/parser_options.h;app/include/xstr.h;app/include/StrX.h;app/include/option_parser.h;app/include/Result_xml_document.h;app/include/DOMTreeErrorReporter.h")
