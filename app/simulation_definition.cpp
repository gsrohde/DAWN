/* Standard Library */
#include <string>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <list>
#include <set>

/* C Library */
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

/* Xerces Library */
#include <xercesc/util/OutOfMemoryException.hpp>

/* DAWN app */
#include "simulation_definition.h"
#include "compilation_options.h"
#include "DOMTreeErrorReporter.hpp"
#include "StrX.h"
#include "xstr.h" // includes X() macro

using namespace std; // invalid_argument, runtime_error, stod
using xercesc::DOMElement;

bool non_empty(const XMLCh* str); // see below

/**
 *  Constructor initializes Xerces-C++ libraries.
 *  The XML tags and attributes which we seek are defined.
 *  The Xerces-C++ DOM parser infrastructure is initialized.
 *
 *  @param in specification_file The name of the XML file giving the specifications for the simulation.
 */

Simulation_definition::Simulation_definition(string specification_file,
                                             string drivers_file,
                                             Option_map parser_options)
    : specification_file{specification_file},
      drivers_file{drivers_file},
      parser_options{parser_options}
{

    // Initialize the XML4C2 system
    try
    {
        XMLPlatformUtils::Initialize();
    }
    catch(const XMLException& e )
    {
        cerr << "Error during Xerces-C++ Initialization.\n"
             << "  Exception message:"
             << StrX(e.getMessage()) << endl;
        // throw exception here
    }

    parser = new XercesDOMParser;

    read_spec_file();
    if (drivers_file.length() > 0) {
        read_drivers_file();
    }
}

state_map Simulation_definition::get_initial_state() {
    return initial_state;
}
state_map Simulation_definition::get_parameters() {
    return parameters;
}
state_vector_map Simulation_definition::get_drivers() {
    return drivers;
}
mc_vector Simulation_definition::get_direct_modules() {
    return direct_modules;
}
mc_vector Simulation_definition::get_differential_modules() {
    return differential_modules;
}

/**
 *  Class destructor frees memory used to hold the XML tag and
 *  attribute definitions. It also deletes the parser and terminates
 *  use of the Xerces-C++ framework.
 */

Simulation_definition::~Simulation_definition()
{
    // Free memory

    delete parser;

    // Terminate Xerces

    try
    {
        XMLPlatformUtils::Terminate();  // Terminate after release of memory
    }
    catch( XMLException& e )
    {
        cerr << "Error during Xerces-C++ Termination.\n"
             << "  Exception message:"
             << StrX(e.getMessage()) << endl;
    }
}

/**
 *  This function:
 *  - Tests the access and availability of the XML simulation specification file.
 *  - Configures the Xerces-C++ DOM parser.
 *  - Reads, extracts, and stores the pertinent information from the XML file.
 */

void Simulation_definition::read_spec_file()
{
    check_file_status(specification_file);
    configure_parser();

    bool errors_occurred = false;
    try
    {
        parser->parse( specification_file.c_str() );
    }
    catch (const OutOfMemoryException&)
    {
        cerr << "OutOfMemoryException" << endl;
        errors_occurred = true;
    }
    catch (const XMLException& e)
    {
        cerr << "An error occurred during parsing\n   Message: "
             << StrX(e.getMessage()) << endl;
        errors_occurred = true;
    }
    catch (const DOMException& e)
    {
        const unsigned int max_chars = 2047;
        XMLCh err_text[max_chars + 1];

        cerr << "\nDOM Error during parsing: '" << specification_file << "'\n"
             << "DOMException code is:  " << e.code << endl;

        if (DOMImplementation::loadDOMExceptionMsg(e.code, err_text, max_chars))
             cerr << "Message is: " << StrX(err_text) << endl;

        errors_occurred = true;
    }
    catch (...)
    {
        cerr << "An error occurred during parsing\n " << endl;
        errors_occurred = true;
    }

    if (errors_occurred || parser->getErrorCount() > 0) {
        throw runtime_error( "There were errors parsing the simulation specification file." );
    }

    // no need to free this pointer - owned by the parent parser object
    DOMDocument* xml_doc = parser->getDocument();

    // Get the dynamical-system element

    DOMNodeList* dyn_sys_list = xml_doc->getElementsByTagName(X("dynamical-system"));
    if (dyn_sys_list->getLength() == 0) {
        // If schema validation is turned on, we shouldn't ever get here.
        throw runtime_error( "The dynamical-system element is missing from simulation specification." );
    }
    else if (dyn_sys_list->getLength() > 1) {
        // If schema validation is turned on, we shouldn't ever get here.
        throw runtime_error( "The dynamical-system element must be unique." );
    }

    DOMNode* dynamical_system_root = dyn_sys_list->item(0);

    DOMNodeList* children = dynamical_system_root->getChildNodes();
    const XMLSize_t node_count = children->getLength();

    // For all nodes, children of "" in the XML tree.

    for ( XMLSize_t i = 0; i < node_count; ++i )
    {
        DOMNode* current_node = children->item(i);
        if ( current_node->getNodeType() &&  // true is not NULL
             current_node->getNodeType() == DOMNode::ELEMENT_NODE ) // is element
        {
            // Found node which is an Element. Re-cast node as element
            DOMElement* current_element
                = dynamic_cast< DOMElement* >( current_node );
            if ( XMLString::equals(current_element->getTagName(), X("initial-state")))
            {
                // Already tested node as type element and of name "initial_state".
                populate_mapping(current_element, initial_state);
            }
            else if ( XMLString::equals(current_element->getTagName(), X("parameters")))
            {
                // Already tested node as type element and of name "parameters".
                populate_mapping(current_element, parameters);
            }
            else if ( XMLString::equals(current_element->getTagName(), X("drivers")))
            {
                // The drivers are populated by the read_drivers_file function.
            }
            else if ( XMLString::equals(current_element->getTagName(), X("driver-placeholder")))
            {
                // The drivers are populated by the read_drivers_file function.
            }
            else if ( XMLString::equals(current_element->getTagName(), X("direct-modules")))
            {
                set_module_list(current_element, direct_modules);
            }
            else if ( XMLString::equals(current_element->getTagName(), X("differential-modules")))
            {
                set_module_list(current_element, differential_modules);
            }
            else {
                cerr << "Unexpected child element of dynamical-system encountered: ";
                cerr << XMLString::transcode(current_element->getTagName()) << endl;
            }
        }
    }

    DOMNodeList* solver_spec_list = xml_doc->getElementsByTagName(X("solver-specification"));
    if (solver_spec_list->getLength() > 1) {
        // If schema validation is turned on, we shouldn't ever get here.
        throw runtime_error( "The dynamical-system element must be unique." );
    }
    else if (solver_spec_list->getLength() == 0) {
        // Nothing left to do here.  Fall back on solver defaults.
        return;
    }

    // The simulation specification file contained solver information.
    // Use it to update the defaults.
    DOMNode* solver_spec_node = solver_spec_list->item(0);
    update_solver_specification(solver_spec_node);
}

/**
 *  This function:
 *  - Tests the access and availability of the drivers file.
 *  - Configures the Xerces-C++ DOM parser.
 *  - Reads, extracts, and stores the pertinent information from the file.
 */

void Simulation_definition::read_drivers_file()
{
    check_file_status(drivers_file);
    configure_parser();

    bool errors_occurred = false;
    try
    {
        parser->parse( drivers_file.c_str() );
    }
    catch (const OutOfMemoryException&)
    {
        cerr << "OutOfMemoryException" << endl;
        errors_occurred = true;
    }
    catch (const XMLException& e)
    {
        cerr << "An error occurred during parsing\n   Message: "
             << StrX(e.getMessage()) << endl;
        errors_occurred = true;
    }
    catch (const DOMException& e)
    {
        const unsigned int max_chars = 2047;
        XMLCh err_text[max_chars + 1];

        cerr << "\nDOM Error during parsing: '" << drivers_file << "'\n"
             << "DOMException code is:  " << e.code << endl;

        if (DOMImplementation::loadDOMExceptionMsg(e.code, err_text, max_chars))
             cerr << "Message is: " << StrX(err_text) << endl;

        errors_occurred = true;
    }
    catch (...)
    {
        cerr << "An error occurred during parsing\n " << endl;
        errors_occurred = true;
    }

    if (errors_occurred || parser->getErrorCount() > 0) {
        throw runtime_error( "There were errors parsing the simulation specification file." );
    }

    // no need to free this pointer - owned by the parent parser object
    DOMDocument* xml_doc = parser->getDocument();

    // Get the drivers element

    DOMNodeList* drivers_list = xml_doc->getElementsByTagName(X("drivers"));
    if (drivers_list->getLength() == 0) {
        // If schema validation is turned on, we shouldn't ever get here.
        throw runtime_error( "The drivers element is missing from simulation specification." );
    }
    else if (drivers_list->getLength() > 1) {
        // If schema validation is turned on, we shouldn't ever get here.
        throw runtime_error( "The dynamical-system element must be unique." );
    }

    DOMElement* drivers_element = dynamic_cast< DOMElement* >(drivers_list->item(0));

    populate_mapping(drivers_element, drivers);
 }

void Simulation_definition::update_solver_specification(DOMNode* solver_spec_node) {
    DOMElement* solver_spec = dynamic_cast<DOMElement*>( solver_spec_node );

    const XMLCh* name = solver_spec->getAttribute(X("name"));
    dynamical_system_solver.name = XMLString::transcode(name);

    const XMLCh* step_size = solver_spec->getAttribute(X("output-step-size"));
    if (non_empty(step_size)) {
        dynamical_system_solver.step_size = stod(XMLString::transcode(step_size));
    }

    const XMLCh* relative_tolerance = solver_spec->getAttribute(X("adaptive-relative-error-tolerance"));
    if (non_empty(relative_tolerance)) {
        dynamical_system_solver.relative_tolerance = stod(XMLString::transcode(relative_tolerance));
    }

    const XMLCh* absolute_tolerance = solver_spec->getAttribute(X("adaptive-absolute-error-tolerance"));
    if (non_empty(absolute_tolerance)) {
        dynamical_system_solver.absolute_tolerance = stod(XMLString::transcode(absolute_tolerance));
    }

    const XMLCh* max_steps = solver_spec->getAttribute(X("adaptive-maximum-steps"));
    if (non_empty(max_steps)) {
        dynamical_system_solver.max_steps = stoi(XMLString::transcode(max_steps));
    }
}

void Simulation_definition::populate_mapping(DOMElement* current_element, state_map& mapping) {
    DOMNodeList*     children = current_element->getChildNodes();
    const XMLSize_t node_count = children->getLength();

    for ( XMLSize_t i = 0; i < node_count; ++i )
    {
        DOMNode* current_node = children->item(i);
        if ( current_node->getNodeType() &&  // true is not NULL
             current_node->getNodeType() == DOMNode::ELEMENT_NODE ) // is element
        {
            // Found node which is an Element. Re-cast node as element
            DOMElement* current_element
                = dynamic_cast< DOMElement* >( current_node );

            if ( XMLString::equals(current_element->getTagName(), X("variable")))
            {
                // Read attributes of element "variable".
                const XMLCh* name
                    = current_element->getAttribute(X("name"));
                string key = XMLString::transcode(name);

                const XMLCh* value
                    = current_element->getAttribute(X("value"));
                string string_value = XMLString::transcode(value);
                try {
                    double variable_value = stod(string_value);
                    mapping[key] = variable_value;
                }
                catch( invalid_argument& e ) {
                    cout << e.what() << endl;
                    cout << "tried to convert \"" << string_value << "\", the value of " << key << ", to a double" << endl;
                    exit(1);
                }

            } // if element is a "variable" element
            else {
                // shouldn't get here
            }
        } // if child node is an element
    } // for children of current_element
}

void Simulation_definition::populate_mapping(DOMElement* current_element, state_vector_map& mapping) {
    DOMNodeList* children = current_element->getChildNodes();
    const XMLSize_t node_count = children->getLength();

    int row_number{0};

    for ( XMLSize_t i = 0; i < node_count; ++i )
    {
        DOMNode* current_node = children->item(i);
        if ( current_node->getNodeType() &&  // true is not NULL
             current_node->getNodeType() == DOMNode::ELEMENT_NODE ) // is element
        {
            // Found node which is an Element. Re-cast node as element
            DOMElement* current_element
                = dynamic_cast< DOMElement* >( current_node );

            if ( XMLString::equals(current_element->getTagName(), X("row"))) {
                ++row_number;

                auto variable_set = process_row(current_element, drivers);

                if (row_number == 1) {
                    driver_variable_set = variable_set;
                }
                else {
                    try {
                        check_driver_variable_set(variable_set);
                    }
                    catch (runtime_error& e) {
                        throw runtime_error("Error in row " + to_string(row_number) + ": " + e.what());
                    }
                }
            } else {
                // schema validation should prevent getting here
                throw runtime_error("Unexpected child element of drivers found");
            }
        } // if child node is an element
    } // for children of current_element
}

set<string> Simulation_definition::process_row(DOMElement* row, state_vector_map& mapping) {
    DOMNodeList* children = row->getChildNodes();
    const XMLSize_t node_count = children->getLength();

    set<string> variable_set{};

    for ( XMLSize_t j = 0; j < node_count; ++j) {
        DOMNode* current_node = children->item(j);
        if ( current_node->getNodeType() &&  // true is not NULL
             current_node->getNodeType() == DOMNode::ELEMENT_NODE ) // is element
        {
            // Found node which is an Element. Re-cast node as element
            DOMElement* current_element
                = dynamic_cast< DOMElement* >( current_node );

            if ( XMLString::equals(current_element->getTagName(), X("variable")))
            {
                // Read attributes of element "variable".
                const XMLCh* name
                    = current_element->getAttribute(X("name"));
                string key = XMLString::transcode(name);
                variable_set.insert(key);

                const XMLCh* value
                    = current_element->getAttribute(X("value"));
                string string_value = XMLString::transcode(value);
                try {
                    double variable_value = stod(string_value);
                    mapping[key].push_back(variable_value);
                }
                catch( invalid_argument& e ) {
                    // schema validation should prevent getting here
                    cout << e.what() << endl;
                    cout << "tried to convert \"" << string_value << "\", the value of " << key << ", to a double" << endl;
                    throw;
                }
            } // if element is a "variable" element
            else {
                // schema validation should prevent getting here
                throw runtime_error("Unexpected child element of row found");
            }

        }
    }
    return variable_set;
}

void Simulation_definition::set_module_list(DOMElement* current_element, mc_vector& vec) {
    DOMNodeList*     children = current_element->getChildNodes();
    const XMLSize_t node_count = children->getLength();

    for ( XMLSize_t i = 0; i < node_count; ++i )
    {
        DOMNode* current_node = children->item(i);
        if ( current_node->getNodeType() &&  // true is not NULL
             current_node->getNodeType() == DOMNode::ELEMENT_NODE ) // is element
        {
            // Found node which is an Element. Re-cast node as element
            DOMElement* current_element
                = dynamic_cast< DOMElement* >( current_node );

            if ( XMLString::equals(current_element->getTagName(), X("module")))
            {
                // Read name attribute of module.
                const XMLCh* name
                    = current_element->getAttribute(X("name"));
                string module_name = XMLString::transcode(name);
                vec.push_back(module_factory<standardBML::module_library>::retrieve(module_name));
            }
        }
    }
}

void Simulation_definition::check_file_status(string filename) {
    struct stat file_status;

    errno = 0;
    if (stat(filename.c_str(), &file_status) == -1) // ==0 ok; ==-1 error
    {
        if ( errno == ENOENT )      // errno declared by include file errno.h
            throw  runtime_error(string("Path file_name \"") +
                                 filename +
                                 "\" does not exist, or path is an empty string.");
        else if ( errno == ENOTDIR )
            throw runtime_error("A component of the path is not a directory.");
        else if ( errno == ELOOP )
            throw runtime_error("Too many symbolic links encountered while traversing the path.");
        else if ( errno == EACCES )
            throw runtime_error("Permission denied.");
        else if ( errno == ENAMETOOLONG )
            throw runtime_error("File can not be read\n");
    }
}

void Simulation_definition::configure_parser() {
    set_validation_scheme();
    parser->setDoNamespaces( true );
    parser->setDoSchema( true );
    parser->setLoadExternalDTD( false );
    parser->setValidationConstraintFatal(true);
    DOMTreeErrorReporter* error_reporter = new DOMTreeErrorReporter();
    parser->setErrorHandler(error_reporter);

    // Possible errors show up when the parser is actually used, not
    // here, so don't test for exceptions.
    parser->setExternalNoNamespaceSchemaLocation(default_schema_file);
}

void Simulation_definition::set_validation_scheme() {
    auto setting = parser_options.at("validation_scheme");
    XercesDOMParser::ValSchemes scheme = setting == "always" ? XercesDOMParser::Val_Always
                                       : setting == "auto"   ? XercesDOMParser::Val_Auto
                                       : setting == "never"  ? XercesDOMParser::Val_Never
                                       :                       XercesDOMParser::Val_Always; // should never get here
    parser->setValidationScheme(scheme);
}

void Simulation_definition::check_driver_variable_set(set<string> variable_set) {
    if (variable_set != driver_variable_set) {
        throw runtime_error("The set of variables in the current row doesn't match that of the first row");
    }
}

bool non_empty(const XMLCh* str) {
    return XMLString::stringLen(str) > 0;
}

// Use macro from compilation_options.h:
const char* Simulation_definition::default_schema_file = SIMULATION_INPUT_SCHEMA_URI_STRING;
