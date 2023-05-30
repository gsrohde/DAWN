#include <iostream>
#include <string>

using std::cerr;
using std::cout;
using std::endl;
using std::string;

using Option_map = std::unordered_map<string, string>;
using Flag_map = std::unordered_map<string, bool>;


class Option_parser {
 public:
    Option_parser(int argC, char* argV[]) {

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
            if (!strcmp(argV[parmInd], "-?")) {
                usage();
                exit(2);
            }
            else if (!strncmp(argV[parmInd], "-v=", 3)) {
                const char* const parm = &argV[parmInd][3];

                if (!strcmp(parm, "never") || !strcmp(parm, "auto") || !strcmp(parm, "always")) {
                    parser_options["validation_scheme"] = string(parm);
                }
                else {
                    cerr << "Unknown -v= value: " << parm << endl;
                    exit(2);
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

    Option_map get_parser_options() {
        return parser_options;
    }

    string get_spec_file() {
        return specification_file;
    }

    string get_output_file() {
        return output_file;
    }

    void usage()
    {
        cout << "\nUsage:\n"
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


 private:
    // These defaults can be altered by command-line arguments:
    Option_map parser_options { {"validation_scheme", "always"} };

    string specification_file;
    string output_file{};
};
