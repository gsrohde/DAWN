cmake_minimum_required(VERSION 3.20.0)
project(DAWN_tests)

set(CMAKE_CXX_STANDARD 14) # probably not needed if GTest is found

find_package(GTest)

if (NOT GTest_FOUND)
    message("GTest not found.  Will download and build.")

    include(FetchContent)
    FetchContent_Declare(
      googletest
      GIT_REPOSITORY https://github.com/google/googletest.git
      GIT_TAG release-1.11.0
    )
    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

else()

    message("GTest was found.  Using installed copy.")

endif()

add_executable(test_all
               test_regression.cpp
               test_simulation_definition.cpp)

target_include_directories(test_all PRIVATE ../app ../biocro-lib/inc)

target_link_libraries(test_all PRIVATE dawn_lib
                                       $<$<BOOL:${GTest_FOUND}>:GTest::>gtest_main
                                       $<$<BOOL:${GTest_FOUND}>:GTest::>gmock)

# Copy test input files.
add_custom_command(
        TARGET test_all POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_SOURCE_DIR}/tests/test_input/
                ${CMAKE_CURRENT_BINARY_DIR}/test_input
        DEPENDS ${CMAKE_SOURCE_DIR}/test_input)

## add_custom_command(
##     OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/test_input/duplicate_driver_variables.xml
##     COMMAND ${CMAKE_COMMAND} -E copy
##         ${CMAKE_SOURCE_DIR}/tests/test_input/duplicate_driver_variables.xml
##         ${CMAKE_CURRENT_BINARY_DIR}/test_input/duplicate_driver_variables.xml
##     DEPENDS ${CMAKE_SOURCE_DIR}/tests/test_input/duplicate_driver_variables.xml
##         )

## add_custom_target(tests DEPENDS test_all
## ${CMAKE_CURRENT_BINARY_DIR}/test_input/duplicate_driver_variables.xml)

include(GoogleTest)
gtest_discover_tests(test_all)
