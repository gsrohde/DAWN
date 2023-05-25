#include <framework/biocro_simulation.h>
#include <framework/module_factory.h>
#include <module_library/module_library.h>

#include "simulation_definition.h"
#include "print_result.h" // for now
#include "Result_xml_document.h"

void usage()
{
    XERCES_STD_QUALIFIER cout << "\nUsage:\n"
            "    main [options] <XML file>\n\n"
            "This program reads the dynamical system specification <XML file>, \n"
            "runs the simulation, and writes the result to standard output.\n"
            "Options:\n"
            "    -v=xxx      Validation scheme [always | never | auto*].\n"
            "    -wfile=xxx  Write to a file instead of stdout.\n"
            "    -?          Show this help.\n\n"
            "  * = Default if not provided explicitly.\n\n"
            "The parser has intrinsic support for the following encodings:\n"
            "    UTF-8, US-ASCII, ISO8859-1, UTF-16[BL]E, UCS-4[BL]E,\n"
            "    WINDOWS-1252, IBM1140, IBM037, IBM1047.\n"
          << endl;
}

int main(int argC, char* argV[]) {

    // Check command line and extract arguments.
    if (argC < 2)
    {
        usage();
        return 1;
    }

    string output_file{};

    // These defaults can be altered by command-line arguments:
    Option_map parser_options { {"validation_scheme", "always"} };

    // See if non validating dom parser configuration is requested.
    int parmInd;
    for (parmInd = 1; parmInd < argC; parmInd++)
    {
        // Break out on first parm not starting with a dash
        if (argV[parmInd][0] != '-')
            break;

        // Watch for special case help request
        if (!strcmp(argV[parmInd], "-?"))
        {
            usage();
            XMLPlatformUtils::Terminate();
            return 2;
        }
         else if (!strncmp(argV[parmInd], "-v=", 3))
        {
            const char* const parm = &argV[parmInd][3];

            if (!strcmp(parm, "never") || !strcmp(parm, "auto") || !strcmp(parm, "always")) {
                parser_options["validation_scheme"] = string(parm);
            }
            else
            {
                XERCES_STD_QUALIFIER cerr << "Unknown -v= value: " << parm << XERCES_STD_QUALIFIER endl;
                XMLPlatformUtils::Terminate();
                return 2;
            }
        }
        else if (!strncmp(argV[parmInd], "-wfile=", 7))
        {
            output_file =  &(argV[parmInd][7]);
        }
        else
        {
            cerr << "Unknown option '" << argV[parmInd]
                 << "', ignoring it.\n" << endl;
        }
    }

   if (parmInd + 1 != argC)
    {
        usage();
        return 1;
    }
    string specification_file = argV[parmInd];

    
    Simulation_definition sim_def(specification_file,
                                  parser_options);

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
    doc.print(output_file);
}
                           
