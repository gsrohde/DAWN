/* Standard Library */
#include <algorithm>
#include <sstream>
#include <string>

/* Xerces Library */
#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/IOException.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>
#include <xercesc/framework/StdOutFormatTarget.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>

/* DAWN app */
#include "Result_xml_document.h"
#include "StrX.h"
#include "xstr.h" // includes XX() macro

using namespace xercesc;

using std::cerr;
using std::endl;
using std::string;

// This makes for better formatting of variable values than does
// std::to_string.
string to_string(double const& value) {
    std::stringstream sstr {};
    sstr << value;
    return sstr.str();
}

Result_xml_document::Result_xml_document(const state_vector_map& result) {
    try {
        XMLPlatformUtils::Initialize();  // Initialize Xerces infrastructure
    }
    catch( XMLException& e ) {
        cerr << "Error during Xerces-C++ Initialization.\n"
             << "  Exception message:"
             << StrX(e.getMessage()) << endl;
        // throw exception here
    }

    DOMImplementation* impl = DOMImplementationRegistry::getDOMImplementation(XX("Core"));

    if (impl != NULL) {
        try {
            doc = impl->createDocument(
                            nullptr,                    // root element namespace URI.
                            XX("simulation-result"),    // root element name
                            nullptr);                   // document type object (DTD).
           
            auto some_column = result.begin();
            auto number_of_rows = some_column->second.size();
            for (int i = 0; i < number_of_rows; ++i) {
                add_row(result, i);
            }
        }
        catch (const OutOfMemoryException&) {
            cerr << "OutOfMemoryException" << endl;
            throw;
        }
        catch (const DOMException& e) {
            cerr << "DOMException code is:  " << e.code << endl;
            throw;
        }
        catch (...) {
            cerr << "An error occurred creating the document" << endl;
            throw;
        }
    } // (impl != NULL)
    else {
        throw std::runtime_error("Requested DOM implementation is not supported");
    }
}

Result_xml_document::~Result_xml_document() {
    try {
        doc->release();
       
        XMLPlatformUtils::Terminate(); // Terminate after release of memory
    }
    catch( xercesc::XMLException& e ) {
        cerr << "Error during Xerces-C++ Termination.\n"
             << "  Exception message:"
             << StrX(e.getMessage()) << endl;
    }
}

void Result_xml_document::add_row(state_vector_map result, int i) {

    DOMElement* row = doc->createElement(XX("row"));

    DOMElement* root_element = doc->getDocumentElement();
    root_element->appendChild(row);

    // Make a vector of sorted keys so we can print out the variables
    // in alphabetical order.
    std::vector<string> keys {};
    for (auto column : result) {
        keys.push_back(column.first);
    }
    std::sort(keys.begin(), keys.end());

    for (auto key : keys) {
        double value {result[key][i]};

        // Since ncalls is constant, don't make it a variable in each
        // row; instead, write it once, as the attribute of the
        // document element.
        if (key == "ncalls") {
            if (i == 0) {
                root_element->setAttribute(XX("ncalls"), XX(to_string(value).c_str()));
            }
            continue;
        }

        auto variable = doc->createElement(XX("variable"));
        row->appendChild(variable);
        row->setAttribute(XX("output-step-number"), XX(to_string(i).c_str()));
        variable->setAttribute(XX("name"), XX(key.c_str()));
        variable->setAttribute(XX("value"), XX(to_string(value).c_str()));
    }
}

void Result_xml_document::print() {
    print("");
}

void Result_xml_document::print(string output_file) {
    DOMImplementation *impl = DOMImplementationRegistry::getDOMImplementation(XX("LS"));
    DOMLSSerializer *theSerializer = ((DOMImplementationLS*)impl)->createLSSerializer();
    theSerializer->getDomConfig()->setParameter(XMLUni::fgDOMWRTFormatPrettyPrint, true);
    theSerializer->getDomConfig()->setParameter(XMLUni::fgDOMWRTXercesPrettyPrint, false);
    DOMLSOutput *theOutputDesc = ((DOMImplementationLS*)impl)->createLSOutput();
    XMLFormatTarget *myFormTarget;

    // choose between file and stdout
    if (output_file.empty()) {
        myFormTarget=new StdOutFormatTarget();
    }
    else {
        try {
            myFormTarget=new LocalFileFormatTarget(output_file.c_str());
        }
        catch (IOException& e) {
            // e.getMessage() doesn't persist after we close
            // XMLPlatformUtils, so we copy it first, converting it
            // into a C string first:
            throw std::runtime_error(StrX(e.getMessage()).localForm());
        }
    }

    theOutputDesc->setByteStream(myFormTarget);
    theSerializer->write(doc, theOutputDesc);

    theOutputDesc->release();
    theSerializer->release();
}

DOMNodeList* Result_xml_document::get_elements_by_tag_name(string element_name) {
    return doc->getElementsByTagName(XX(element_name.c_str()));
}
