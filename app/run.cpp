#include "run.h"

/* BioCro Library */
#include <framework/biocro_simulation.h>
#include <framework/module_factory.h>
#include <module_library/module_library.h>

/* DAWN app */
#include "option_parser.h"
#include "simulation_definition.h"
#include "Result_xml_document.h"

/**
 * Run a BioCro simulation using the input file and the options
 * specified in the command_line_args argument.
 *
 * This is the primary entry point for applications using the DAWN
 * library instead of using the DAWN executable.
 *
 * @param[in] command_line_args
 * @parblock
 * A vector of strings corresponding to the command line options and
 * arguments that would be passed to the DAWN executable.  Run the
 * DAWN executable (`dawn`) with the `-h` option to see a list of
 * options.
 *
 * At a minimum, the vector should contain a string specifying the
 * path to an XML file specifying the simulation to be run.
 * @endparblock
 *
 * @returns A pointer to a Result_xml_document object representing the
 * result of running the simulation.
 */
std::unique_ptr<Result_xml_document> run(vector<string> command_line_args) {
    // cstrings will take the place of argV when calling the
    // Option_parser constructor.  Since the constructor expects the
    // command line options to start with the *second* element of the
    // array (argV[0] containing the name of the command itself), we
    // prepend command_line_args with a "dummy" element to represent
    // this first element.
    std::vector<const char*> cstrings{"dummy"};

    // Convert command_line_args to a C-style array so that it may be
    // passed to the Option_parser constructor.
    for (const auto& string : command_line_args) {
        cstrings.push_back(string.c_str());
    }

    // optind is a global variable used by getopt and getopt_long that
    // must be reset between test invocations:
    optind = 1;
    Option_parser op(cstrings.size(), const_cast<char**>(cstrings.data()));

    return run(op);
}

/**
 * Run a BioCro simulation based on the options stored in an
 * Option_parser object.
 *
 * This signature of the run function is probably most useful as the
 * backend to more user-friendly versions of the function.
 *
 * @param[in] op An Option_parser object.
 *
 * @returns A pointer to a Result_xml_document object representing the
 * result of running the simulation.
 */
std::unique_ptr<Result_xml_document> run(Option_parser op) {
    try {
        Simulation_definition sim_def(op);

        auto solver = sim_def.solver();

        biocro_simulation sim {
            sim_def.get_initial_state(),
            sim_def.get_parameters(),
            sim_def.get_drivers(),
            sim_def.get_direct_modules(),
            sim_def.get_differential_modules(),

            solver.name,
            solver.step_size,
            solver.relative_tolerance,
            solver.absolute_tolerance,
            solver.max_steps
        };

        auto result = sim.run_simulation();

        return std::unique_ptr<Result_xml_document>(new Result_xml_document{result});
    }
    catch(std::runtime_error& e) {
        cerr << "RUNTIME ERROR: " << e.what() << endl;
        exit(1);
    }
    catch(std::exception& e)   {
        cerr << "EXCEPTION: " << e.what() << endl;
        exit(1);
    }
    catch(...) {
        cerr << "UNKNOWN EXCEPTION" << endl;
        exit(1);
    }
}

/**
 * Run a BioCro simulation using the input file and the options
 * specified in the argV argument.
 *
 * This is the workhorse for the `dawn` executable: it expects to be
 * handed the arguments---argC and argV---passed to the `main`
 * function.
 *
 * @param[in] argC An integer; this should be equal to the number of items in argV.
 *
 * @param[in] argV
 * @parblock
 * A C-style array consisting of C-style strings representing
 * command-line arguments.  Run the DAWN executable (`dawn`) with the
 * `-h` option to see a list of options.
 *
 * At a minimum, argV should contain a string specifying the path to
 * an XML file specifying the simulation to be run.
 * @endparblock
 *
 * @returns 0
 */
int run(int argC, char* argV[]) {
    Option_parser op(argC, argV);
    auto doc = run(op);
    doc->print(op.get_output_file());
    return 0;
}
