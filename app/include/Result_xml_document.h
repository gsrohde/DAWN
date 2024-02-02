#ifndef RESULT_XML_DOCUMENT_H
#define RESULT_XML_DOCUMENT_H

/* Standard Library */
#include <string>
#include <unordered_map>
#include <vector>

/* Xerces Library */
#include <xercesc/dom/DOMDocument.hpp>

/* This is the only declaration we need from the BioCro library, and
   we make it directly rather than including state_map.h: */
using state_vector_map = std::unordered_map<std::string, std::vector<double>>;


class Result_xml_document
{
public:
   Result_xml_document(const state_vector_map& result);
    ~Result_xml_document();
    void print();
    void print(std::string);
    xercesc::DOMNodeList* get_elements_by_tag_name(std::string);
    
private:
    void add_row(state_vector_map result, int row_number);
    
    xercesc::DOMDocument* doc;
};

#endif
