#include <gtest/gtest.h>

#include <simulation_definition.h>

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
