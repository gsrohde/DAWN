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
        { "fast_fail",               no_argument,       NULL, 'f' },
        { "keep_going",              no_argument,       NULL, 'k' },
        { "validation_scheme",       required_argument, NULL, 'v' },
        { "drivers",                 required_argument, NULL, 'd' },
        { "out",                     required_argument, NULL, 'o' },
        { NULL,                      0,                 NULL, 0   }
    };

    char ch {};
    while ((ch = getopt_long(argC, argV, "hsfkv:d:o:", longopts, NULL)) != -1) {
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
             case 'f':
                 parser_options.fast_fail_option = true;
                 break;
             case 'k':
                 parser_options.keep_going_option = true;
                 break;
             case 'v':
                 if (!strcmp(optarg, "never")) {
                     parser_options.validation_scheme = XercesDOMParser::Val_Never;
                 }
                 else if (!strcmp(optarg, "auto")) {
                     parser_options.validation_scheme = XercesDOMParser::Val_Auto;
                 }
                 else if (!strcmp(optarg, "always")) {
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
