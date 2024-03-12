#ifndef OPTION_PARSER_H
#define OPTION_PARSER_H

#include <iostream>
#include <string>
#include <unordered_map>

#include "parser_options.h" // for Parser_options

using std::cerr;
using std::cout;
using std::endl;
using std::string;

using Option_map = std::unordered_map<string, string>;
using Flag_map = std::unordered_map<string, bool>;


class Option_parser {
 public:
    Option_parser(int argC, char* argV[]);

    Parser_options get_parser_options() {
        return parser_options;
    }

    string get_spec_file() {
        return specification_file;
    }

    string get_output_file() {
        return output_file;
    }

    string get_drivers_file() {
        return drivers_file;
    }

    void usage()
    {
        cout << "\nUsage:\n"
            "    main [options] <XML file> [options]\n"
            "    main -h\n"
            "    main -show_default_schema_uri\n\n"
            "This program reads the simulation specification from <XML file>, runs\n"
            "the simulation, and writes the result to standard output, or to the file\n"
            "specified by the --out option.\n\n"
            "Note that it is immaterial whether options are placed before or after\n"
            "the <XML file> argument.\n\n"
            "Options:\n"
            "    --drivers=xxx, -d xxx            Read the driver information from a separate XML file.\n"
            "    --validation_scheme=xxx, -v xxx  Validation scheme [always* | never | auto].\n"
            "    --out=xxx, -o xxx                Write to a file instead of stdout.\n"
            "    --help, -h                       Show this help.\n"
            "    --show_default_schema_uri, -s    Show the default location of the schema file used in\n"
            "                                     validating input.\n"
            "    --fail_fast, -f                  Fail and exit after the first schema validation error.\n"
            "    --keep_going, -k                 Keep going, even after a fatal parsing error.  Note\n"
            "                                     that the fail-fast option has no effect if this\n"
            "                                     option is used.\n\n"
            "  * = Default if not provided explicitly.\n\n"
            "(Options to long arguments may use a space in place of an '=' sign.  The space between a\n"
            "short option and its argument may be omitted.)\n\n" 
                                  << endl;
    }


 private:
    Parser_options parser_options {};

    string specification_file {};
    string output_file {};
    string drivers_file {};
};

#endif
