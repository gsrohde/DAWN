#include <string> // for std::to_string
#include "Result_xml_document.h"

Result_xml_document::Result_xml_document(const state_vector_map& result) {
    try {
        XMLPlatformUtils::Initialize();  // Initialize Xerces infrastructure
    } catch( XMLException& e ) {
        char* message = XMLString::transcode( e.getMessage() );
        cerr << "XML toolkit initialization error: " << message << endl;
        XMLString::release( &message );
        // throw exception here to return ERROR_XERCES_INIT
    }

    DOMImplementation* impl =  DOMImplementationRegistry::getDOMImplementation(X("Core"));

    if (impl != NULL) {
        try {
            doc = impl->createDocument(
                                       nullptr,                    // root element namespace URI.
                                       X("simulation-result"),     // root element name
                                       nullptr);                   // document type object (DTD).
           
            auto some_column = result.begin();

            auto number_of_rows = some_column->second.size();

            for (int i = 0; i < number_of_rows; ++i) {
                add_row(result, i);
            }
           
        }
        catch (const OutOfMemoryException&) {
            cerr << "OutOfMemoryException" << endl;
            errorCode = 5;
        }
        catch (const DOMException& e) {
            cerr << "DOMException code is:  " << e.code << endl;
            errorCode = 2;
        }
        catch (...) {
            cerr << "An error occurred creating the document" << endl;
            errorCode = 3;
        }
    }  // (inpl != NULL)
    else {
        cerr << "Requested implementation is not supported" << endl;
        errorCode = 4;
    }
}

Result_xml_document::~Result_xml_document() {
    try {
        doc->release();
       
        XMLPlatformUtils::Terminate();  // Terminate after release of memory
    }
    catch( xercesc::XMLException& e ) {
        char* message = xercesc::XMLString::transcode( e.getMessage() );

        cerr << "XML ttolkit teardown error: " << message << endl;
        XMLString::release( &message );
    }
}

void Result_xml_document::print() {
    print("");
}

void Result_xml_document::print(string output_file) {
    XMLCh tempStr[3] = {chLatin_L, chLatin_S, chNull};
    DOMImplementation *impl          = DOMImplementationRegistry::getDOMImplementation(X("Core"));//tempStr);
    DOMLSSerializer   *theSerializer = ((DOMImplementationLS*)impl)->createLSSerializer();
    theSerializer->getDomConfig()->setParameter(XMLUni::fgDOMWRTFormatPrettyPrint, true);
    theSerializer->getDomConfig()->setParameter(XMLUni::fgDOMWRTXercesPrettyPrint, false);
    DOMLSOutput       *theOutputDesc = ((DOMImplementationLS*)impl)->createLSOutput();
    XMLFormatTarget *myFormTarget;

    // choose between file and stdout
    if (output_file.empty()) {
        myFormTarget=new StdOutFormatTarget();
    }
    else {
        myFormTarget=new LocalFileFormatTarget(output_file.c_str());
    }

    theOutputDesc->setByteStream(myFormTarget);
    theSerializer->write(doc, theOutputDesc);

    theOutputDesc->release();
    theSerializer->release();
}



void Result_xml_document::add_row(state_vector_map result, int i) {

    DOMElement* row = doc->createElement(X("row"));

    DOMElement* root_element = doc->getDocumentElement();
    root_element->appendChild(row);

    for (auto column : result) {
        auto key = column.first;
        auto value = column.second[i];
        auto variable = doc->createElement(X("variable"));
        row->appendChild(variable);
        variable->setAttribute(X("name"), X(key.c_str()));
        variable->setAttribute(X("value"), X(std::to_string(value).c_str()));
    }
}
