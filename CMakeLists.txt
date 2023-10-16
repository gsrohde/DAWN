cmake_minimum_required(VERSION 3.20.0)
project(DAWN)

message("Top level CMakeLists.txt")

enable_testing()

add_subdirectory(biocro-lib/src)
add_subdirectory(app)
add_subdirectory(tests)
