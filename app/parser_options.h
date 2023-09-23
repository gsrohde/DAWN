#ifndef PARSER_OPTIONS_H
#define PARSER_OPTIONS_H

#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/XercesDefs.hpp>

#include "option_parser.h"

XERCES_CPP_NAMESPACE_USE

class Parser_options {
    friend class Option_parser;

 public:
    inline XercesDOMParser::ValSchemes get_validation_scheme() {
        return validation_scheme;
    }
    
 private:
    XercesDOMParser::ValSchemes validation_scheme { XercesDOMParser::Val_Always };
};

#endif
