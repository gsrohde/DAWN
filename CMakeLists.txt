cmake_minimum_required(VERSION 3.20.0)
project(DAWN)

message(DEBUG "Top level CMakeLists.txt")

enable_testing()

add_subdirectory(biocro-lib/src)
add_subdirectory(app)
add_subdirectory(tests)


include(GNUInstallDirs)
install(TARGETS dawn_lib
        ARCHIVE
        PUBLIC_HEADER
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/dawn
       )

install(TARGETS dawn)
