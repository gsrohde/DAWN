#ifndef OPTION_PARSER_H
#define OPTION_PARSER_H

#include <iostream>
#include <string>
#include <unordered_map>

#include "parser_options.h"

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
            "    main [options] <XML file>\n"
            "    main -h\n"
            "    main -show_default_schema_uri\n\n"
            "This program reads the dynamical system specification <XML file>, runs\n"
            "the simulation, and writes the result to standard output.  Note that\n"
            "options may be placed after the <XML file> argument as well.\n\n"
            "Options:\n"
            "    --drivers=xxx, -d xxx            Read the driver information from a separate XML file.\n"
            "    --validation_scheme=xxx, -v xxx  Validation scheme [always* | never | auto].\n"
            "    --out=xxx, -o xxx                Write to a file instead of stdout.\n"
            "    --help, -h                       Show this help.\n"
            "    --show_default_schema_uri, -s    Show the default location of the schema file used in\n"
            "                                     validating input.\n\n"
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
