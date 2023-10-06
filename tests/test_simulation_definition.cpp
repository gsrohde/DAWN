#include <gtest/gtest.h>

#include <gmock/gmock.h>
using testing::HasSubstr;

#include <framework/biocro_simulation.h>
#include <simulation_definition.h>
#include <parser_options.h>

Parser_options default_options {}; // validating parser
Parser_options nonvalidating_parser {"never"};

// As a baseline, parsing minimal_system.xml shouldn't throw any
// exceptions, even with validation turned on.

TEST(SimulationDefinitionTest, MinimalSimulationDefinitionFile) {
    ASSERT_NO_THROW({
            Simulation_definition sim_def("test_input/minimal_system.xml",
                                          default_options);
        });
}

// Test that some poorly-formed input files trigger exceptions, even
// when schema validation is turned off.

TEST(SimulationDefinitionTest, DuplicateInitialStateDefinitions) {
    ASSERT_ANY_THROW({
            Simulation_definition sim_def("test_input/duplicate_initial_state_variable_definition.xml",
                                          nonvalidating_parser);
        });
}

TEST(SimulationDefinitionTest, DuplicateParameterDefinitions) {
    ASSERT_ANY_THROW({
            Simulation_definition sim_def("test_input/duplicate_parameter_definition.xml",
                                          nonvalidating_parser);
        });
}

TEST(SimulationDefinitionTest, DuplicateDriverVariables) {
    ASSERT_ANY_THROW({
            Simulation_definition sim_def("test_input/duplicate_driver_variables.xml",
                                          nonvalidating_parser);
        });
}

TEST(SimulationDefinitionTest, InconsistentDriverVariables) {
    EXPECT_THROW({
        try {
             Simulation_definition sim_def("test_input/inconsistent_driver_variables.xml",
                                           default_options);
        }
        catch( const std::runtime_error& e )
        {
            // and this tests that it has the correct message
            EXPECT_THAT(e.what(),
                        HasSubstr("The set of variables in the current row"
                                  " doesn't match that of the first row"));
            throw;
        }
    },
    std::runtime_error);
}

// Ensure a bad driver definition won't cause a segfault:
TEST(SimulationDefinitionTest, NoDriverVariablesWithValidation) {

    ASSERT_EXIT(({
        try {
            Simulation_definition sim_def("test_input/no_driver_variables.xml",
                                          default_options);

            biocro_simulation sim {
                sim_def.get_initial_state(),
                sim_def.get_parameters(),
                sim_def.get_drivers(),
                sim_def.get_direct_modules(),
                sim_def.get_differential_modules(),

                "homemade_euler",
                1,
                0.0001,
                0.0001,
                200
            };

            // The parser should throw an exception before we get here
            // and segfault.
            auto result = sim.run_simulation();
        }
        catch(std::exception) {
        }
        exit(0);
    }),
    ::testing::ExitedWithCode(0),
    ".*");
}

// Ensure a bad driver definition won't cause a segfault, even with validation
// turned off:
TEST(SimulationDefinitionTest, DISABLED_NoDriverVariablesWithoutValidation) {

    ASSERT_EXIT(({
        try {
            Simulation_definition sim_def("test_input/no_driver_variables.xml",
                                          nonvalidating_parser);

            biocro_simulation sim {
                sim_def.get_initial_state(),
                sim_def.get_parameters(),
                sim_def.get_drivers(),
                sim_def.get_direct_modules(),
                sim_def.get_differential_modules(),

                "homemade_euler",
                1,
                0.0001,
                0.0001,
                200
            };

            // The parser should throw an exception before we get here
            // and segfault.
            auto result = sim.run_simulation();
        }
        catch(std::exception) {
        }
        exit(0);
    }),
    ::testing::ExitedWithCode(0),
    ".*") << "Without schema validation, the parsing code fails to ward off a segmentation fault";
}
