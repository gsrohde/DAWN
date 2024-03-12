#ifndef PARSER_OPTIONS_H
#define PARSER_OPTIONS_H

#include <xercesc/parsers/AbstractDOMParser.hpp> // XercesDOMParser;
                                                 // We don't need the
                                                 // full
                                                 // XercesDOMParser
                                                 // declaration in
                                                 // this file.

using xercesc::AbstractDOMParser;


class Parser_options {
    friend class Option_parser;

 public:
    Parser_options() {};

    Parser_options(std::string validation_scheme) {
        if (validation_scheme == "never") {
            this->validation_scheme = AbstractDOMParser::Val_Never;
        }
        else if (validation_scheme == "auto") {
            this->validation_scheme = AbstractDOMParser::Val_Auto;
        }
        else if (validation_scheme != "always") {
            throw std::runtime_error(validation_scheme + " is not a recognized validation scheme.");
        }
    }

    inline AbstractDOMParser::ValSchemes get_validation_scheme() {
        return validation_scheme;
    }
    inline bool keep_going() {
        return keep_going_option;
    }
    inline bool fast_fail() {
        return fast_fail_option;
    }
    
 private:
    AbstractDOMParser::ValSchemes validation_scheme { AbstractDOMParser::Val_Always };
    bool keep_going_option {false};
    bool fast_fail_option {false};
};

#endif
