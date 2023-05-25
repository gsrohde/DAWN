#ifndef SIMULATION_DEFINITION_H
#define SIMULATION_DEFINITION_H
/**
 *  @file
 *  Class "Simulation_definition" provides the functions to read the XML data.
 *  @version 1.0
 */
#include <xercesc/dom/DOM.hpp>

#include <xercesc/parsers/XercesDOMParser.hpp>

/* Standard Library */
#include <string>

/* BioCro */
#include <framework/module_factory.h>
#include <framework/state_map.h>
#include <module_library/module_library.h>

using std::string;
using xercesc::DOMElement;

using Option_map = std::unordered_map<string, string>;
using Flag_map = std::unordered_map<string, bool>;

// Error codes

enum {
    ERROR_ARGS = 1,
    ERROR_XERCES_INIT,
    ERROR_PARSE,
    ERROR_EMPTY_DOCUMENT
};

class Simulation_definition
{
public:
    Simulation_definition(string specification_file, Option_map parser_options);
    ~Simulation_definition();

    state_map get_initial_state();
    state_map get_parameters();
    state_vector_map get_drivers();
    mc_vector get_direct_modules();
    mc_vector get_differential_modules();

private:
    void read_spec_file() throw(std::runtime_error);
    void check_spec_file_status();
    void configure_parser();
    void set_validation_scheme();
    void populate_mapping(DOMElement* current_element, state_map& mapping);
    void populate_mapping(DOMElement* current_element, state_vector_map& mapping);
    void process_row(DOMElement* row, state_vector_map& mapping);
    void set_module_list(DOMElement* current_element, mc_vector& vec);

    xercesc::XercesDOMParser *parser;

    string specification_file;
    Option_map parser_options;

    state_map initial_state;
    state_map parameters;
    state_vector_map drivers;
    mc_vector direct_modules;
    mc_vector differential_modules;

    const char* default_schema_file {"simulation_input.xsd"};

    // Internal class use only. Hold Xerces data in UTF-16 SMLCh type.

    XMLCh* TAG_initial_values;
    XMLCh* TAG_parameters;
    XMLCh* TAG_drivers;
    XMLCh* TAG_direct_modules;
    XMLCh* TAG_differential_modules;

    XMLCh* TAG_module;
    XMLCh* TAG_row;

    XMLCh* TAG_variable;
    XMLCh* ATTR_name;
    XMLCh* ATTR_value;
};
#endif
