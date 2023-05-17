#ifndef RESULT_XML_DOCUMENT_H
#define RESULT_XML_DOCUMENT_H

#include <iostream>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>
#include <xercesc/framework/StdOutFormatTarget.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <framework/state_map.h> // for state_vector_map

using namespace xercesc;
using std::cerr;
using std::endl;
using std::string;

class XStr
{
public :
    // -----------------------------------------------------------------------
    //  Constructors and Destructor
    // -----------------------------------------------------------------------
    XStr(const char* const toTranscode)
    {
        // Call the private transcoding method
        fUnicodeForm = XMLString::transcode(toTranscode);
    }

    ~XStr()
    {
        XMLString::release(&fUnicodeForm);
    }


    // -----------------------------------------------------------------------
    //  Getter methods
    // -----------------------------------------------------------------------
    const XMLCh* unicodeForm() const
    {
        return fUnicodeForm;
    }

private :
    // -----------------------------------------------------------------------
    //  Private data members
    //
    //  fUnicodeForm
    //      This is the Unicode XMLCh format of the string.
    // -----------------------------------------------------------------------
    XMLCh*   fUnicodeForm;
};

#define X(str) XStr(str).unicodeForm()

class Result_xml_document
{
public:
   Result_xml_document(const state_vector_map& result);
    ~Result_xml_document();
    void print();
  void print(string);

    
private:
    void add_row(state_vector_map result, int row_number);
    
    DOMDocument* doc;
    int errorCode = 0;
};

#endif
