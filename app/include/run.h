/* Standard Library */
#include <memory>
#include <string>
#include <vector>

/* DAWN app */
#include "option_parser.h"
#include "Result_xml_document.h"


int run(int argC, char* argV[]);
std::unique_ptr<Result_xml_document> run(std::vector<std::string> command_line_args);
std::unique_ptr<Result_xml_document> run(Option_parser op);
