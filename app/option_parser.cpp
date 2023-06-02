#include "option_parser.h"
#include "compilation_options.h"

using std::cerr;
using std::endl;
using std::string;


Option_parser::Option_parser(int argC, char* argV[]) {

    // Check command line and extract arguments.
    if (argC < 2) {
        usage();
        exit(1);
    }


    // See if non validating dom parser configuration is requested.
    int parmInd;
    for (parmInd = 1; parmInd < argC; parmInd++) {
        // Break out on first parm not starting with a dash
        if (argV[parmInd][0] != '-') {
            break;
        }

        // Watch for special case help request
        if (!strcmp(argV[parmInd], "-h")) {
            usage();
            exit(0);
        }
        else if (!strncmp(argV[parmInd], "-show_default_schema_uri", 24)) {
            cout << "Default schema URI: ";
            if (!strcmp(SIMULATION_INPUT_SCHEMA_URI_STRING, "")) {
                cout << "(none)";
            }
            else {
                cout << "\"" << SIMULATION_INPUT_SCHEMA_URI_STRING << "\"";
            }
            cout << endl;
            exit(0);
        }
        else if (!strncmp(argV[parmInd], "-v=", 3)) {
            const char* const parm = &argV[parmInd][3];

            if (!strcmp(parm, "never") || !strcmp(parm, "auto") || !strcmp(parm, "always")) {
                parser_options["validation_scheme"] = string(parm);
            }
            else {
                cerr << "Unknown -v= value: " << parm << endl;
                exit(1);
            }
        }
        else if (!strncmp(argV[parmInd], "-wfile=", 7)) {
            output_file =  &(argV[parmInd][7]);
        }
        else {
            cerr << "Unknown option '" << argV[parmInd]
                 << "', ignoring it.\n" << endl;
        }
    }

    if (parmInd + 1 != argC) {
        usage();
        exit(1);
    }
    specification_file = argV[parmInd];
}
