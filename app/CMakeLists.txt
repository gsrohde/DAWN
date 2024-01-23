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

add_library(dawn_lib SHARED)
target_sources(dawn_lib PRIVATE DOMTreeErrorReporter.cpp)
target_sources(dawn_lib PRIVATE Result_xml_document.cpp)
target_sources(dawn_lib PRIVATE option_parser.cpp)
target_sources(dawn_lib PRIVATE run.cpp)
target_sources(dawn_lib PRIVATE simulation_definition.cpp)
target_link_libraries(dawn_lib PUBLIC BioCro XercesC::XercesC) # PUBLIC so tests can use Xerces as well
target_include_directories(dawn_lib PUBLIC ../biocro-lib/src) # PUBLIC so tests can use BioCro headers as well
target_include_directories(dawn_lib INTERFACE .)

# Set RPATH for installation prior to adding the executable:
include(GNUInstallDirs)
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
        TARGET dawn POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_SOURCE_DIR}/app/sample_input/
                ${CMAKE_CURRENT_BINARY_DIR}/sample_input
        BYPRODUCTS ${CMAKE_CURRENT_BINARY_DIR}/sample_input)

set_target_properties(dawn_lib PROPERTIES
                      PUBLIC_HEADER "app/simulation_definition.h;app/option_parser.h;app/Result_xml_document.h")
