#ifndef RESULT_XML_DOCUMENT_H
#define RESULT_XML_DOCUMENT_H

/* Standard Library */
#include <string>

/* Xerces Library */
#include <xercesc/dom/DOM.hpp>

/* BioCro Library */
#include <framework/state_map.h> // for state_vector_map


class Result_xml_document
{
public:
   Result_xml_document(const state_vector_map& result);
    ~Result_xml_document();
    void print();
    void print(std::string);

    
private:
    void add_row(state_vector_map result, int row_number);
    
    xercesc::DOMDocument* doc;
};

#endif
