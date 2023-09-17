/* BioCro Library */
#include <framework/biocro_simulation.h>
#include <framework/module_factory.h>
#include <module_library/module_library.h>

/* DAWN app */
#include "option_parser.h"
#include "simulation_definition.h"
#include "print_result.h" // for now
#include "Result_xml_document.h"

int main(int argC, char* argV[]) {

    Option_parser op(argC, argV);

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

        Result_xml_document doc(result);
        doc.print(op.get_output_file());
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
