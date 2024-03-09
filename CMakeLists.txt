cmake_minimum_required(VERSION 3.20.0)
project(DAWN)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

message(VERBOSE "Processing top level CMakeLists.txt")

cmake_language(GET_MESSAGE_LOG_LEVEL MESSAGE_LOG_LEVEL)
set(LOG_LEVELS_FOR_INDENTATION VERBOSE;DEBUG;TRACE)
if(MESSAGE_LOG_LEVEL IN_LIST LOG_LEVELS_FOR_INDENTATION)
  list(APPEND CMAKE_MESSAGE_INDENT "  ")
endif()


set(CMAKE_WARN_DEPRECATED TRUE)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0")

option(INSTALL_BIOCRO_HEADERS "Install interface headers for the BioCro library" OFF)
option(BUILD_SHARED_LIBS "Build libraries as shared libraries by default rather than as static libraries" ON)
option(INSTALL_GTEST "Install Google Test" OFF)
option(CUSTOMIZE_DOXYGEN "Customize the Doxygen documentation" OFF)

# Set a default build type if none was specified
set(default_build_type "Release")
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE
      STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
    "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

enable_testing()
include(GNUInstallDirs)

add_subdirectory(biocro-lib)
add_subdirectory(app)
add_subdirectory(tests)
add_subdirectory(doc)

install(TARGETS BioCro)

if(INSTALL_BIOCRO_HEADERS)
  install(TARGETS BioCro
            FILE_SET biocro_lib_headers
              DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/BioCro
  )
endif()

install(TARGETS dawn_lib
          ARCHIVE
          PUBLIC_HEADER
            DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/dawn
        )

# Install the schema files
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_INSTALL_DATADIR}/dawn/
        DESTINATION ${CMAKE_INSTALL_DATADIR}/dawn)

install(TARGETS dawn)
