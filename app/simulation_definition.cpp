#include <string>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <list>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

#include "simulation_definition.h"
#include "DOMTreeErrorReporter.hpp"
#include "StrX.h"

//using namespace xercesc;
using namespace std;

/**
 *  Constructor initializes xerces-C libraries.
 *  The XML tags and attributes which we seek are defined.
 *  The xerces-C DOM parser infrastructure is initialized.
 */

Simulation_definition::Simulation_definition(string specification_file)
    : specification_file{specification_file}
{

    // Initialize the XML4C2 system
    try
    {
        XMLPlatformUtils::Initialize();
    }
    catch(const XMLException& e )
    {
        cerr << "Error during Xerces-c Initialization.\n"
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
 *  attribute definitions. It als terminates use of the xerces-C
 *  framework.
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
        cerr << "Unknown exception encountered in TagNamesdtor" << endl;
    }

    // Terminate Xerces

    try
    {
        XMLPlatformUtils::Terminate();  // Terminate after release of memory
    }
    catch( XMLException& e )
    {
        char* message = XMLString::transcode( e.getMessage() );

        cerr << "XML ttolkit teardown error: " << message << endl;
        XMLString::release( &message );
    }
}

/**
 *  This function:
 *  - Tests the access and availability of the XML simulation specification file.
 *  - Configures the xerces-c DOM parser.
 *  - Reads, extracts, and stores the pertinent information from the XML file.
 *
 *  @param in specification_file The name of the XML file giving the specifications for the simulation.
 */

void Simulation_definition::read_spec_file()
        throw( std::runtime_error )
{
    // Test to see if the file is ok.

    struct stat fileStatus;

    errno = 0;
    if (stat(specification_file.c_str(), &fileStatus) == -1) // ==0 ok; ==-1 error
    {
        if ( errno == ENOENT )      // errno declared by include file errno.h
            throw ( std::runtime_error("Path file_name does not exist, or path is an empty string.") );
        else if ( errno == ENOTDIR )
            throw ( std::runtime_error("A component of the path is not a directory."));
        else if ( errno == ELOOP )
            throw ( std::runtime_error("Too many symbolic links encountered while traversing the path."));
        else if ( errno == EACCES )
            throw ( std::runtime_error("Permission denied."));
        else if ( errno == ENAMETOOLONG )
            throw ( std::runtime_error("File can not be read\n"));
    }

    // Configure DOM parser.

    parser->setValidationScheme( XercesDOMParser::Val_Always );
    parser->setDoNamespaces( true );
    parser->setDoSchema( true );
    parser->setLoadExternalDTD( false );
    parser->setValidationConstraintFatal(true);
    DOMTreeErrorReporter *errReporter = new DOMTreeErrorReporter();
    parser->setErrorHandler(errReporter);

    try
    {
        parser->parse( specification_file.c_str() );

        if (parser->getErrorCount() == 0)
            printf("XML file validated against the schema successfully\n");
        else
            printf("XML file doesn't conform to the schema\n");


        // no need to free this pointer - owned by the parent parser object
        DOMDocument* xmlDoc = parser->getDocument();

        // Get the top-level element: NAme is "root". No attributes for "root"

        DOMElement* elementRoot = xmlDoc->getDocumentElement();
        if ( !elementRoot ) throw(std::runtime_error( "empty XML document" ));

        // Parse XML file for tags of interest: "ApplicationSettings"
        // Look one level nested within "root". (child of root)

        DOMNodeList*     children = elementRoot->getChildNodes();
        const XMLSize_t nodeCount = children->getLength();

        // For all nodes, children of "" in the XML tree.

        for ( XMLSize_t i = 0; i < nodeCount; ++i )
        {
            DOMNode* currentNode = children->item(i);
            if ( currentNode->getNodeType() &&  // true is not NULL
                 currentNode->getNodeType() == DOMNode::ELEMENT_NODE ) // is element
            {
                // Found node which is an Element. Re-cast node as element
                DOMElement* currentElement
                    = dynamic_cast< DOMElement* >( currentNode );
                if ( XMLString::equals(currentElement->getTagName(), TAG_initial_values))
                {
                    // Already tested node as type element and of name "initial_values".
                    populate_mapping(currentElement, initial_state);
                }
                else if ( XMLString::equals(currentElement->getTagName(), TAG_parameters))
                {
                    // Already tested node as type element and of name "parameters".
                    populate_mapping(currentElement, parameters);
                }
                else if ( XMLString::equals(currentElement->getTagName(), TAG_drivers))
                {
                    populate_mapping(currentElement, drivers);
                }
                else if ( XMLString::equals(currentElement->getTagName(), TAG_direct_modules))
                {
                    set_module_list(currentElement, direct_modules);
                }
                else if ( XMLString::equals(currentElement->getTagName(), TAG_differential_modules))
                {
                    set_module_list(currentElement, differential_modules);
                }
                else {
                    cerr << "Unexpected child element of dynamical-system encountered: ";
                    cerr << XMLString::transcode(currentElement->getTagName()) << endl;
                }
            }
        }
    }
    catch( XMLException& e )
    {
        char* message = XMLString::transcode( e.getMessage() );
        ostringstream errBuf;
        errBuf << "Error parsing file: " << message << flush;
        XMLString::release( &message );
    }
}

void Simulation_definition::populate_mapping(DOMElement* currentElement, state_map& mapping) {
    DOMNodeList*     children = currentElement->getChildNodes();
    const XMLSize_t nodeCount = children->getLength();

    for ( XMLSize_t i = 0; i < nodeCount; ++i )
    {
        DOMNode* currentNode = children->item(i);
        if ( currentNode->getNodeType() &&  // true is not NULL
             currentNode->getNodeType() == DOMNode::ELEMENT_NODE ) // is element
        {
            // Found node which is an Element. Re-cast node as element
            DOMElement* currentElement
                = dynamic_cast< DOMElement* >( currentNode );

            if ( XMLString::equals(currentElement->getTagName(), TAG_variable))
            {
                // Read attributes of element "variable".
                const XMLCh* name
                    = currentElement->getAttribute(ATTR_name);
                string key = XMLString::transcode(name);

                const XMLCh* value
                    = currentElement->getAttribute(ATTR_value);
                string string_value = XMLString::transcode(value);
                try {
                    double variable_value = std::stod(string_value);
                    mapping[key] = variable_value;
                } catch( std::invalid_argument& e ) {
                    cout << e.what() << endl;
                    cout << "tried to convert \"" << string_value << "\", the value of " << key << ", to a double" << endl;
                    exit(1);
                }

            } // if element is a "variable" element
            else {
                // shouldn't get here
            }
        } // if child node is an element
    } // for children of currentElement
}

void Simulation_definition::populate_mapping(DOMElement* currentElement, state_vector_map& mapping) {
    DOMNodeList*     children = currentElement->getChildNodes();
    const XMLSize_t nodeCount = children->getLength();

    for ( XMLSize_t i = 0; i < nodeCount; ++i )
    {
        DOMNode* currentNode = children->item(i);
        if ( currentNode->getNodeType() &&  // true is not NULL
             currentNode->getNodeType() == DOMNode::ELEMENT_NODE ) // is element
        {
            // Found node which is an Element. Re-cast node as element
            DOMElement* currentElement
                = dynamic_cast< DOMElement* >( currentNode );

            if ( XMLString::equals(currentElement->getTagName(), TAG_row)) {
                process_row(currentElement, drivers);
            } else {
                cout << "shouldn't EVER get here!" << endl;
            }
        } // if child node is an element
    } // for children of currentElement
}

void Simulation_definition::process_row(DOMElement* row, state_vector_map& mapping) {
    DOMNodeList* children = row->getChildNodes();
    const XMLSize_t nodeCount = children->getLength();

    for ( XMLSize_t j = 0; j < nodeCount; ++j) {
        DOMNode* currentNode = children->item(j);
        if ( currentNode->getNodeType() &&  // true is not NULL
             currentNode->getNodeType() == DOMNode::ELEMENT_NODE ) // is element
        {
            // Found node which is an Element. Re-cast node as element
            DOMElement* currentElement
                = dynamic_cast< DOMElement* >( currentNode );

            if ( XMLString::equals(currentElement->getTagName(), TAG_variable))
            {
                // Read attributes of element "variable".
                const XMLCh* name
                    = currentElement->getAttribute(ATTR_name);
                string key = XMLString::transcode(name);
                const XMLCh* value
                    = currentElement->getAttribute(ATTR_value);
                string string_value = XMLString::transcode(value);
                try {
                    double variable_value = std::stod(string_value);
                    mapping[key].push_back(variable_value);
                } catch( std::invalid_argument& e ) {
                    cout << e.what() << endl;
                    cout << "tried to convert \"" << string_value << "\", the value of " << key << ", to a double" << endl;
                    exit(1);
                }
            } // if element is a "variable" element
            else {
                // shouldn't get here
            }

        }
    }
}

void Simulation_definition::set_module_list(DOMElement* currentElement, mc_vector& vec) {
    DOMNodeList*     children = currentElement->getChildNodes();
    const XMLSize_t nodeCount = children->getLength();

    for ( XMLSize_t i = 0; i < nodeCount; ++i )
    {
        DOMNode* currentNode = children->item(i);
        if ( currentNode->getNodeType() &&  // true is not NULL
             currentNode->getNodeType() == DOMNode::ELEMENT_NODE ) // is element
        {
            // Found node which is an Element. Re-cast node as element
            DOMElement* currentElement
                = dynamic_cast< DOMElement* >( currentNode );

            if ( XMLString::equals(currentElement->getTagName(), TAG_module))
            {
                // Read name attribute of module.
                const XMLCh* name
                    = currentElement->getAttribute(ATTR_name);
                string module_name = XMLString::transcode(name);
                vec.push_back(module_factory<standardBML::module_library>::retrieve(module_name));
            }
        }
    }
}
