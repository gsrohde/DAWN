#include <string>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <list>
#include <set>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

#include <xercesc/util/OutOfMemoryException.hpp>

#include "simulation_definition.h"
#include "compilation_options.h"
#include "DOMTreeErrorReporter.hpp"
#include "StrX.h"

using namespace std; // invalid_argument, runtime_error, stod
using xercesc::DOMElement;

/**
 *  Constructor initializes Xerces-C++ libraries.
 *  The XML tags and attributes which we seek are defined.
 *  The Xerces-C++ DOM parser infrastructure is initialized.
 *
 *  @param in specification_file The name of the XML file giving the specifications for the simulation.
 */

Simulation_definition::Simulation_definition(string specification_file, Option_map parser_options)
    : specification_file{specification_file},
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

    // Tags and attributes used in XML file.
    // Can't call transcode till after Xerces Initialize()
    TAG_initial_values = XMLString::transcode("initial-values");
    TAG_parameters = XMLString::transcode("parameters");
    TAG_drivers = XMLString::transcode("drivers");
    TAG_direct_modules = XMLString::transcode("direct-modules");
    TAG_differential_modules = XMLString::transcode("differential-modules");
    TAG_module = XMLString::transcode("module");
    TAG_row = XMLString::transcode("row");
    TAG_variable = XMLString::transcode("variable");
    ATTR_name = XMLString::transcode("name");
    ATTR_value = XMLString::transcode("value");

    parser = new XercesDOMParser;

    read_spec_file();
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

    try
    {
        XMLString::release( &TAG_initial_values );
        XMLString::release( &TAG_parameters );
        XMLString::release( &TAG_drivers );
        XMLString::release( &TAG_direct_modules );
        XMLString::release( &TAG_differential_modules );

        XMLString::release( &TAG_module );
        XMLString::release( &TAG_row );

        XMLString::release( &TAG_variable );
        XMLString::release( &ATTR_name );
        XMLString::release( &ATTR_value );
    }
    catch( ... )
    {
        cerr << "Unknown exception encountered when releasing XMLCh pointers" << endl;
    }

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
    check_spec_file_status();
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

    // Get the top-level element

    DOMElement* element_root = xml_doc->getDocumentElement();
    if ( !element_root ) {
        // The error count check should prevent us from ever getting here.
        throw runtime_error( "empty XML document" );
    }

    DOMNodeList* children = element_root->getChildNodes();
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
            if ( XMLString::equals(current_element->getTagName(), TAG_initial_values))
            {
                // Already tested node as type element and of name "initial_values".
                populate_mapping(current_element, initial_state);
            }
            else if ( XMLString::equals(current_element->getTagName(), TAG_parameters))
            {
                // Already tested node as type element and of name "parameters".
                populate_mapping(current_element, parameters);
            }
            else if ( XMLString::equals(current_element->getTagName(), TAG_drivers))
            {
                populate_mapping(current_element, drivers);
            }
            else if ( XMLString::equals(current_element->getTagName(), TAG_direct_modules))
            {
                set_module_list(current_element, direct_modules);
            }
            else if ( XMLString::equals(current_element->getTagName(), TAG_differential_modules))
            {
                set_module_list(current_element, differential_modules);
            }
            else {
                cerr << "Unexpected child element of dynamical-system encountered: ";
                cerr << XMLString::transcode(current_element->getTagName()) << endl;
            }
        }
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

            if ( XMLString::equals(current_element->getTagName(), TAG_variable))
            {
                // Read attributes of element "variable".
                const XMLCh* name
                    = current_element->getAttribute(ATTR_name);
                string key = XMLString::transcode(name);

                const XMLCh* value
                    = current_element->getAttribute(ATTR_value);
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

            if ( XMLString::equals(current_element->getTagName(), TAG_row)) {
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

            if ( XMLString::equals(current_element->getTagName(), TAG_variable))
            {
                // Read attributes of element "variable".
                const XMLCh* name
                    = current_element->getAttribute(ATTR_name);
                string key = XMLString::transcode(name);
                variable_set.insert(key);

                const XMLCh* value
                    = current_element->getAttribute(ATTR_value);
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

            if ( XMLString::equals(current_element->getTagName(), TAG_module))
            {
                // Read name attribute of module.
                const XMLCh* name
                    = current_element->getAttribute(ATTR_name);
                string module_name = XMLString::transcode(name);
                vec.push_back(module_factory<standardBML::module_library>::retrieve(module_name));
            }
        }
    }
}

void Simulation_definition::check_spec_file_status() {
    struct stat file_status;

    errno = 0;
    if (stat(specification_file.c_str(), &file_status) == -1) // ==0 ok; ==-1 error
    {
        if ( errno == ENOENT )      // errno declared by include file errno.h
            throw  runtime_error("Path file_name does not exist, or path is an empty string.");
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

// Use macro from compilation_options.h:
const char* Simulation_definition::default_schema_file = SIMULATION_INPUT_SCHEMA_URI_STRING;
