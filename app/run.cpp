/* BioCro Library */
#include <framework/biocro_simulation.h>
#include <framework/module_factory.h>
#include <module_library/module_library.h>

/* DAWN app */
#include "option_parser.h"
#include "simulation_definition.h"
#include "Result_xml_document.h"
#include "run.h"

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

int run(int argC, char* argV[]) {
    Option_parser op(argC, argV);
    auto doc = run(op);
    doc->print(op.get_output_file());
    return 0;
}
