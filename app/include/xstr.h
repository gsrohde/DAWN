#ifndef XSTR_H
#define XSTR_H

/*
 * The code in this file is a modification of code found in the file
 * xerces-c-3.2.4/samples/src/samples/src/CreateDOMDocument/CreateDOMDocument.hpp
 * in the archive file xerces-c-3.2.4.tar.gz downloaded from
 * https://downloads.apache.org/xerces/c/3/sources/.  The following
 * license notice appeared in that file.
 *
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


/* Xerces Library */
#include <xercesc/util/XMLString.hpp> // XMLString; includes XMLCh typedef

using xercesc::XMLString;
// (XMLCh is a global typedef.)

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
    XMLCh* fUnicodeForm;
};

// We use XX instead of X here so as not to conflict with the X()
// macro defined in the Boost library.
#define XX(str) XStr(str).unicodeForm()


#endif
