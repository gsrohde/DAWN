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
#include <set>

/* BioCro */
#include <framework/module_factory.h>
#include <framework/state_map.h>
#include <module_library/module_library.h>

/* DAWN app */
#include "solver.h"

using std::string;
using std::set;
using xercesc::DOMElement;
using xercesc::DOMNode;

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

    Solver solver() {
        return dynamical_system_solver;
    }

private:
    void read_spec_file();
    void check_spec_file_status();
    void configure_parser();
    void set_validation_scheme();
    void populate_mapping(DOMElement* current_element, state_map& mapping);
    void populate_mapping(DOMElement* current_element, state_vector_map& mapping);
    set<string> process_row(DOMElement* row, state_vector_map& mapping);
    void set_module_list(DOMElement* current_element, mc_vector& vec);
    void check_driver_variable_set(set<string> variable_set);
    void update_solver_specification(DOMNode* solver_spec_node);

    xercesc::XercesDOMParser *parser;

    string specification_file;
    Option_map parser_options;

    state_map initial_state;
    state_map parameters;
    state_vector_map drivers;
    mc_vector direct_modules;
    mc_vector differential_modules;

    Solver dynamical_system_solver {};

    static const char* default_schema_file;

    // Use for validating consistency of driver variables
    set<string> driver_variable_set;
};
#endif
