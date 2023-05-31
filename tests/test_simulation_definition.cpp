#include <gtest/gtest.h>

#include <simulation_definition.h>

// As a baseline, parsing minimal_system.xml shouldn't throw any
// exceptions, even with validation turned on.

TEST(SimulationDefinitionTest, MinimalSimulationDefinitionFile) {
    ASSERT_NO_THROW({
            Simulation_definition sim_def("test_input/minimal_system.xml",
                                          { {"validation_scheme", "always"} });
        });
}

// Test that some poorly-formed input files trigger exceptions, even
// when schema validation is turned off.

TEST(SimulationDefinitionTest, DISABLED_DuplicateInitialStateDefinitions) {
    ASSERT_ANY_THROW({
            Simulation_definition sim_def("test_input/duplicate_initial_state_variable_definition.xml",
                                          { {"validation_scheme", "never"} });
        });
}

TEST(SimulationDefinitionTest, DISABLED_DuplicateParameterDefinitions) {
    ASSERT_ANY_THROW({
            Simulation_definition sim_def("test_input/duplicate_parameter_definition.xml",
                                          { {"validation_scheme", "never"} });
        });
}

TEST(SimulationDefinitionTest, DISABLED_DuplicateDriverVariables) {
    ASSERT_ANY_THROW({
            Simulation_definition sim_def("test_input/duplicate_driver_variables.xml",
                                          { {"validation_scheme", "never"} });
        });
}

TEST(SimulationDefinitionTest, InconsistentDriverVariables) {
    ASSERT_ANY_THROW({
            Simulation_definition sim_def("test_input/inconsistent_driver_variables.xml",
                                          { {"validation_scheme", "always"} });
        });
}
