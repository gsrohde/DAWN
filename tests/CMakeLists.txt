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

    message("GTest was found!")

endif()

add_executable(test_all
               test_regression.cpp
               test_simulation_definition.cpp)

target_include_directories(test_all PRIVATE ../app ../biocro-lib/inc)

if (NOT GTest_FOUND)
    target_link_libraries(test_all PRIVATE dawn_lib gtest_main)
else()
    target_link_libraries(test_all PRIVATE dawn_lib GTest::gtest_main)
endif()

add_custom_command(
        TARGET test_all POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_SOURCE_DIR}/tests/test_input/
                ${CMAKE_CURRENT_BINARY_DIR}/test_input)

include(GoogleTest)
gtest_discover_tests(test_all)
