#include <framework/biocro_simulation.h>
#include <framework/module_factory.h>
#include <module_library/module_library.h>

#include "option_parser.h"
#include "simulation_definition.h"
#include "print_result.h" // for now
#include "Result_xml_document.h"

int main(int argC, char* argV[]) {

    Option_parser op(argC, argV);

    Simulation_definition sim_def(op.get_spec_file(),
                                  op.get_parser_options());

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

    auto result = sim.run_simulation();

    Result_xml_document doc(result);
    doc.print(op.get_output_file());
}
                           
