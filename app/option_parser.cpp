#include <getopt.h>

#include "option_parser.h"
#include "compilation_options.h"

using std::cerr;
using std::endl;
using std::string;


Option_parser::Option_parser(int argC, char* argV[]) {

    static struct option longopts[] = {
        { "help",                    no_argument,       NULL, 'h' },
        { "show_default_schema_uri", no_argument,       NULL, 's' },
        { "validation_scheme",       required_argument, NULL, 'v' },
        { "drivers",                 required_argument, NULL, 'd' },
        { "out",                     required_argument, NULL, 'o' },
        { NULL,                      0,                 NULL, 0   }
    };

    char ch {};
    while ((ch = getopt_long(argC, argV, "hsv:d:o:", longopts, NULL)) != -1) {
             switch (ch) {
             case 'h':
                 usage();
                 exit(0);
             case 's':
                 cout << "Default schema URI: ";
                 if (get_schema_uri() == "") {
                     cout << "(none)";
                 }
                 else {
                     cout << get_schema_uri();
                 }
                 cout << endl;
                 exit(0);
             case 'v':
                 if (!strcmp(optarg, "never")) {
                     cout << "setting validation scheme to never" << endl;
                     parser_options.validation_scheme = XercesDOMParser::Val_Never;
                 }
                 else if (!strcmp(optarg, "auto")) {
                     cout << "setting validation scheme to auto" << endl;
                     parser_options.validation_scheme = XercesDOMParser::Val_Auto;
                 }
                 else if (!strcmp(optarg, "always")) {
                     cout << "setting validation scheme to always" << endl;
                     parser_options.validation_scheme = XercesDOMParser::Val_Always;
                 }
                 else {
                     cerr << "Unknown validation schema (-v) value: " << optarg << endl;
                     exit(1);
                 }
                 break;
             case 'd':
                 drivers_file =  optarg;
                 break;
             case 'o':
                 output_file =  optarg;
                 break;
             default:
                 usage();
                 exit(1);
             }
     }

    if (optind + 1 != argC) {
        usage();
        exit(1);
    }
    specification_file = argV[optind];
}
