#include <iostream>
#include <string>
#include <unordered_map>

using std::cerr;
using std::cout;
using std::endl;
using std::string;

using Option_map = std::unordered_map<string, string>;
using Flag_map = std::unordered_map<string, bool>;


class Option_parser {
 public:
    Option_parser(int argC, char* argV[]);

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
            "    main [options] <XML file>\n"
            "    main -h\n"
            "    main -show_default_schema_uri\n\n"
            "This program reads the dynamical system specification <XML file>, \n"
            "runs the simulation, and writes the result to standard output.\n\n"
            "Options:\n"
            "    -v=xxx                    Validation scheme [always* | never | auto].\n"
            "    -wfile=xxx                Write to a file instead of stdout.\n"
            "    -h                        Show this help.\n"
            "    -show_default_schema_uri  Show the default location of the schema file used in validating input.\n\n"
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
