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

enable_testing()
include(GNUInstallDirs)

add_subdirectory(biocro-lib/src)
add_subdirectory(app)
add_subdirectory(tests)
add_subdirectory(doc)

option(INSTALL_ALL_BIOCRO_HEADERS NO)

install(TARGETS BioCro)

if(INSTALL_ALL_BIOCRO_HEADERS)
  install(DIRECTORY "${CMAKE_SOURCE_DIR}/biocro-lib/src/" # source directory
          DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/BioCro
          FILES_MATCHING # install only matched files
          PATTERN "*.h" # select header files
  )
else()
  install(FILES "${CMAKE_SOURCE_DIR}/biocro-lib/src/framework/state_map.h"
          DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/BioCro/framework
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
