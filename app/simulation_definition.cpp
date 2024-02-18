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
#include <xercesc/dom/DOM.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>

/* DAWN app */
#include "simulation_definition.h"
#include "compilation_options.h"
#include "DOMTreeErrorReporter.h"
#include "StrX.h"
#include "xstr.h" // includes XX() macro

using namespace std; // invalid_argument, runtime_error, stod

using namespace xercesc; // DOMElement, DOMNodeList, XMLPlatformUtils,
                         // XMLException, OutOfMemoryException,
                         // DOMException, DOMImplementation,
                         // DOMDocument


void populate_parameter_mapping(DOMElement* parameters_element,
                                state_map& parameters)
{
    populate_mapping(parameters_element, parameters, true);
}

void populate_mapping(DOMElement* variable_containing_element, state_map& mapping,
                      bool is_parameters = false)
{
    DOMNodeList* children = variable_containing_element->getChildNodes();
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

            if ( XMLString::equals(current_element->getTagName(), XX("variable")))
            {
                // Read attributes of element "variable".
                const XMLCh* name
                    = current_element->getAttribute(XX("name"));
                string key = XMLString::transcode(name);

                const XMLCh* value
                    = current_element->getAttribute(XX("value"));
                string string_value = XMLString::transcode(value);

                if (is_parameters && key == "timestep") {
                    // timestep was specified in the parameters; issue
                    // a warning that the value will be ignored; only
                    // the value specified on the drivers element is
                    // used

                    cerr << "WARNING: "
                         << "The timestep value " << string_value << ", "
                         << "specified in the parameters, will be ignored.\n"
                         << "The value specified in the timestep attribute "
                         << "of the drivers element will be used instead."
                         << endl;
                }
                else if (mapping.count(key) > 0) {
                    string message {"Duplicate key (\""};
                    message += key;
                    message += "\") in element \"";
                    const XMLCh* vname = variable_containing_element->getTagName();
                    string variable_name
                        = XMLString::transcode(vname);
                    message += variable_name;
                    message += "\".";
                    throw runtime_error(message);
                }
                else {
                    mapping[key] = string_to_double(string_value);
                }

            } // if element is a "variable" element
            else {
                // shouldn't get here
            }
        } // if child node is an element
    } // for children of variable_containing_element
}

/**
 *  Constructor initializes Xerces-C++ libraries.
 *  The XML tags and attributes which we seek are defined.
 *  The Xerces-C++ DOM parser infrastructure is initialized.
 *
 *  @param in specification_file The name of the XML file giving the specifications for the simulation.
 */

Simulation_definition::Simulation_definition(Option_parser op)
    : Simulation_definition{op.get_spec_file(),
                            op.get_drivers_file(),
                            op.get_parser_options()}
{
}

Simulation_definition::Simulation_definition(string specification_file,
                                             Parser_options parser_options)
    : Simulation_definition{specification_file,
                            "",
                            parser_options}
{
}

/**
 *
 *  @param [in] specification_file The path to the XML file giving the specifications for the simulation.
 *  @param [in] drivers_file The path to the XML file specifying the drivers for the simulation.
 *  @param [in] parser_options A Parser_options object.
 */
Simulation_definition::Simulation_definition(string specification_file,
                                             string drivers_file,
                                             Parser_options parser_options)
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
    if (use_external_drivers_file()) {
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
        cerr << "An OutOfMemoryException occurred while parsing the simulation specification." << endl;
        errors_occurred = true;
    }
    catch (const XMLException& e)
    {
        cerr << "An XMLException occurred while parsing the simulation specification.\n   Message: "
             << StrX(e.getMessage()) << endl;
        errors_occurred = true;
    }
    catch (const DOMException& e)
    {
        const unsigned int max_chars = 2047;
        XMLCh err_text[max_chars + 1];

        cerr << "\nA DOMException occurred while parsing the simulation specification file ('" << specification_file << "')\n"
             << "DOMException code is:  " << e.code << endl;

        if (DOMImplementation::loadDOMExceptionMsg(e.code, err_text, max_chars))
             cerr << "Message is: " << StrX(err_text) << endl;

        errors_occurred = true;
    }
    catch (...)
    {
        cerr << "An error occurred while parsing the simulation specification.\n " << endl;
        errors_occurred = true;
    }

    auto errs { parser->getErrorCount() };
    if (errs > 0) {
        string message { string("There ") +
                         (errs == 1 ? "was " : "were ") +
                         to_string(errs) +
                         (errs == 1 ? " error " : " errors ") +
                         "parsing the simulation specification file." };
        throw runtime_error(message);
    }
    else if (errors_occurred) {
        throw runtime_error( "There were errors parsing the simulation "
                             "specification file." );
    }

    // no need to free this pointer - owned by the parent parser object
    DOMDocument* xml_doc = parser->getDocument();

    // Get the dynamical-system element

    DOMNodeList* dyn_sys_list = xml_doc->getElementsByTagName(XX("dynamical-system"));
    if (dyn_sys_list->getLength() == 0) {
        // If schema validation is turned on, we shouldn't ever get here.
        throw runtime_error( "The dynamical-system element is missing from "
                             "simulation specification." );
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
            if ( XMLString::equals(current_element->getTagName(), XX("initial-state")))
            {
                // Already tested node as type element and of name
                // "initial_state".
                populate_mapping(current_element, initial_state);
            }
            else if ( XMLString::equals(current_element->getTagName(), XX("parameters")))
            {
                // Already tested node as type element and of name
                // "parameters".
                populate_parameter_mapping(current_element, parameters);
            }
            else if ( XMLString::equals(current_element->getTagName(), XX("drivers")))
            {
                if (!use_external_drivers_file()) {
                    populate_drivers(current_element);
                }
                // else the user specified an external drivers file to
                // override the drivers specified in this element
            }
            else if ( XMLString::equals(current_element->getTagName(), XX("driver-placeholder")))
            {
                if (!use_external_drivers_file()) {
                    throw runtime_error( "The spec file says drivers are "
                                         "defined externally but no drivers "
                                         "file was provided." );
                }
            }
            else if ( XMLString::equals(current_element->getTagName(), XX("duration")))
            {
                if (!use_external_drivers_file()) {
                    generate_drivers(current_element);
                }
                // else the user specified an external drivers file to
                // override the drivers implicitly specified by this element
            }
            else if ( XMLString::equals(current_element->getTagName(), XX("direct-modules")))
            {
                set_module_list(current_element, direct_modules);
            }
            else if ( XMLString::equals(current_element->getTagName(), XX("differential-modules")))
            {
                set_module_list(current_element, differential_modules);
            }
            else {
                cerr << "Unexpected child element of dynamical-system encountered: ";
                cerr << XMLString::transcode(current_element->getTagName()) << endl;
            }
        }
    }

    DOMNodeList* solver_spec_list = xml_doc->getElementsByTagName(XX("solver"));
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
        cerr << "An OutOfMemoryException occurred while parsing the drivers file." << endl;
        errors_occurred = true;
    }
    catch (const XMLException& e)
    {
        cerr << "An XMLException occurred while parsing the drivers file.\n   Message: "
             << StrX(e.getMessage()) << endl;
        errors_occurred = true;
    }
    catch (const DOMException& e)
    {
        const unsigned int max_chars = 2047;
        XMLCh err_text[max_chars + 1];

        cerr << "\nA DOMException occurred while parsing the drivers file ('" << drivers_file << "')\n"
             << "DOMException code is:  " << e.code << endl;

        if (DOMImplementation::loadDOMExceptionMsg(e.code, err_text, max_chars))
             cerr << "Message is: " << StrX(err_text) << endl;

        errors_occurred = true;
    }
    catch (...)
    {
        cerr << "An error occurred while parsing the drivers file.\n " << endl;
        errors_occurred = true;
    }

    if (errors_occurred || parser->getErrorCount() > 0) {
        throw runtime_error( "There were errors parsing the drivers file." );
    }

    // no need to free this pointer - owned by the parent parser object
    DOMDocument* xml_doc = parser->getDocument();

    // Get the drivers element

    DOMNodeList* drivers_list = xml_doc->getElementsByTagName(XX("drivers"));
    if (drivers_list->getLength() == 0) {
        // If schema validation is turned on, we shouldn't ever get here.
        throw runtime_error( "The drivers element is missing from simulation "
                             "specification." );
    }
    else if (drivers_list->getLength() > 1) {
        // If schema validation is turned on, we shouldn't ever get here.
        throw runtime_error( "The dynamical-system element must be unique." );
    }

    DOMElement* drivers_element = dynamic_cast< DOMElement* >(drivers_list->item(0));

    populate_drivers(drivers_element);
 }

void Simulation_definition::update_solver_specification(DOMNode* solver_spec_node) {
    DOMElement* solver_spec = dynamic_cast<DOMElement*>( solver_spec_node );

    const XMLCh* name = solver_spec->getAttribute(XX("name"));
    dynamical_system_solver.name = XMLString::transcode(name);

    const XMLCh* step_size = solver_spec->getAttribute(XX("output-step-size"));
    if (non_empty(step_size)) {
        dynamical_system_solver.step_size =
            stod(XMLString::transcode(step_size));
    }

    const XMLCh* relative_tolerance =
        solver_spec->getAttribute(XX("adaptive-relative-error-tolerance"));
    if (non_empty(relative_tolerance)) {
        dynamical_system_solver.relative_tolerance =
            stod(XMLString::transcode(relative_tolerance));
    }

    const XMLCh* absolute_tolerance =
        solver_spec->getAttribute(XX("adaptive-absolute-error-tolerance"));
    if (non_empty(absolute_tolerance)) {
        dynamical_system_solver.absolute_tolerance =
            stod(XMLString::transcode(absolute_tolerance));
    }

    const XMLCh* max_steps =
        solver_spec->getAttribute(XX("adaptive-maximum-steps"));
    if (non_empty(max_steps)) {
        dynamical_system_solver.max_steps =
            stoi(XMLString::transcode(max_steps));
    }
}

void Simulation_definition::generate_drivers(DOMElement* current_element) {
    const XMLCh* timestep_value
        = current_element->getAttribute(XX("timestep"));
    string string_value = XMLString::transcode(timestep_value);
    const double timestep = string_to_double(string_value);
    parameters["timestep"] = timestep;

    const XMLCh* number_of_steps_value =
        current_element->getAttribute(XX("number-of-steps"));
    string_value = XMLString::transcode(number_of_steps_value);
    const int number_of_steps = string_to_int(string_value);

    vector<double> elapsed_times {};
    for (int i = 0; i <= number_of_steps; ++i) {
        elapsed_times.push_back(i * timestep);
    }
    drivers["elapsed_time"] = elapsed_times;
}

void Simulation_definition::populate_drivers(DOMElement* current_element) {

    // timestep "belongs" to the drivers, but the BioCro C++ code
    // requires it to be specified among the parameters:
    const XMLCh* value
        = current_element->getAttribute(XX("timestep"));
    string string_value = XMLString::transcode(value);
    parameters["timestep"] = string_to_double(string_value);

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

            if ( XMLString::equals(current_element->getTagName(), XX("row"))) {
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
                        throw runtime_error("Error in row " +
                                            to_string(row_number) +
                                            ": " + e.what());
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

            if ( XMLString::equals(current_element->getTagName(), XX("variable")))
            {
                // Read attributes of element "variable".
                const XMLCh* name
                    = current_element->getAttribute(XX("name"));
                string key = XMLString::transcode(name);

                if (variable_set.count(key) == 0) {
                    variable_set.insert(key);
                }
                else {
                    string message {"Duplicate key (\""};
                    message += key;
                    message += "\") in row ";
                    message += to_string((mapping.at(key)).size());
                    message += " of the drivers element.";

                    throw runtime_error(message);
                }

                const XMLCh* value
                    = current_element->getAttribute(XX("value"));
                string string_value = XMLString::transcode(value);
                mapping[key].push_back(string_to_double(string_value));

            } // if element is a "variable" element
            else {
                // schema validation should prevent getting here
                throw runtime_error("Unexpected child element of row found");
            }

        }
    }
    return variable_set;
}

void Simulation_definition::set_module_list(DOMElement* module_element, mc_vector& vec) {
    DOMNodeList* children = module_element->getChildNodes();
    const XMLSize_t node_count = children->getLength();

    set<string> module_name_set {}; // for detecting duplicates

    for ( XMLSize_t i = 0; i < node_count; ++i )
    {
        DOMNode* current_node = children->item(i);
        if ( current_node->getNodeType() &&  // true is not NULL
             current_node->getNodeType() == DOMNode::ELEMENT_NODE ) // is element
        {
            // Found node which is an Element. Re-cast node as element
            DOMElement* current_element
                = dynamic_cast< DOMElement* >( current_node );

            if ( XMLString::equals(current_element->getTagName(), XX("module")))
            {
                // Read name attribute of module.
                const XMLCh* name
                    = current_element->getAttribute(XX("name"));
                string module_name = XMLString::transcode(name);

                if (module_name_set.count(module_name) == 0) {
                    module_name_set.insert(module_name);
                    vec.push_back(
                        module_factory<standardBML::module_library>::retrieve(module_name)
                    );
                }
                else {
                    string message {"Module \""};
                    message += module_name;
                    message += "\" appears more than once in the specified set of ";
                    const XMLCh* mod_type = module_element->getTagName();
                    string module_type
                        = XMLString::transcode(mod_type);
                    message += (module_type == "direct-modules") ? "direct" : "differential";
                    message += " modules.";

                    throw std::runtime_error(message);
                }
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
            throw runtime_error("Too many symbolic links encountered while "
                                "traversing the path.");
        else if ( errno == EACCES )
            throw runtime_error("Permission denied.");
        else if ( errno == ENAMETOOLONG )
            throw runtime_error("File can not be read\n");
    }
}

void Simulation_definition::configure_parser() {
    parser->setValidationScheme(parser_options.get_validation_scheme());
    parser->setDoNamespaces( true );
    parser->setDoSchema( true );
    parser->setLoadExternalDTD( false );
    parser->setValidationConstraintFatal(parser_options.fast_fail());
    parser->setExitOnFirstFatalError(!parser_options.keep_going());
    DOMTreeErrorReporter* error_reporter = new DOMTreeErrorReporter();
    parser->setErrorHandler(error_reporter);

    for (auto uri : get_schema_uris()) {
        try {
            parser->loadGrammar(uri.c_str(), Grammar::SchemaGrammarType, true);
            cerr << "Using schema at " << uri.c_str() << endl;
            break; // Use the first schema document successfully found and successfully parsed.
        }
        catch (std::runtime_error e) {
            std::cerr << "Error trying to load grammar: " << e.what() << endl;
        }
    }

    parser->useCachedGrammarInParse(true);
}

void Simulation_definition::check_driver_variable_set(set<string> variable_set) {
    if (variable_set != driver_variable_set) {
        throw runtime_error("The set of variables in the current row doesn't "
                            "match that of the first row");
    }
}

bool non_empty(const XMLCh* str) {
    return XMLString::stringLen(str) > 0;
}

double string_to_double(string string_value) {
    try {
        return stod(string_value);
    }
    catch( invalid_argument& e ) {
        // schema validation should prevent getting here
        cout << e.what() << endl;
        cout << "tried to convert \"" << string_value << " to a double" << endl;
        throw;
    }
}

int string_to_int(string string_value) {
    try {
        return stoi(string_value);
    }
    catch( invalid_argument& e ) {
        // schema validation should prevent getting here
        cout << e.what() << endl;
        cout << "tried to convert \"" << string_value << " to an int" << endl;
        throw;
    }
}
