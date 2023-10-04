/*
 * The code in this file is a modification of code found in the file
 * xerces-c-3.2.4/samples/src/DOMPrint/DOMTreeErrorReporter.cpp in the
 * archive file xerces-c-3.2.4.tar.gz downloaded from
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


/* Standard Library */
#include <iostream>

/* Xerces Library */
#include <xercesc/sax/SAXParseException.hpp>

/* DAWN app */
#include "DOMTreeErrorReporter.h"
#include "StrX.h"


void DOMTreeErrorReporter::warning(const SAXParseException& toCatch)
{
    fSawErrors = true;
    XERCES_STD_QUALIFIER cerr << "WARNING at file\n\"" << StrX(toCatch.getSystemId())
		 << "\",\nline " << toCatch.getLineNumber()
		 << ", column " << toCatch.getColumnNumber()
         << ".\nMessage: " << StrX(toCatch.getMessage()) << "\n" << XERCES_STD_QUALIFIER endl;
}

void DOMTreeErrorReporter::error(const SAXParseException& toCatch)
{
    fSawErrors = true;
    XERCES_STD_QUALIFIER cerr << "ERROR at file\n\"" << StrX(toCatch.getSystemId())
		 << "\",\nline " << toCatch.getLineNumber()
		 << ", column " << toCatch.getColumnNumber()
         << ".\nMessage: " << StrX(toCatch.getMessage()) << "\n" << XERCES_STD_QUALIFIER endl;
}

void DOMTreeErrorReporter::fatalError(const SAXParseException& toCatch)
{
    fSawErrors = true;
    XERCES_STD_QUALIFIER cerr << "FATAL ERROR at file\n\"" << StrX(toCatch.getSystemId())
		 << "\",\nline " << toCatch.getLineNumber()
		 << ", column " << toCatch.getColumnNumber()
         << ".\nMessage: " << StrX(toCatch.getMessage()) << "\n" << XERCES_STD_QUALIFIER endl;
}

void DOMTreeErrorReporter::resetErrors()
{
    fSawErrors = false;
}


