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

#include "DOMTreeErrorReporter.h"

/* Standard Library */
#include <string>

/* Xerces Library */
#include <xercesc/sax/SAXParseException.hpp>

/* DAWN app */
#include "StrX.h"


void DOMTreeErrorReporter::warning(const SAXParseException& toCatch)
{
    fSawErrors = true;

    error_buffer << "WARNING";
    if (toCatch.getSystemId()[0] != 0) {
        error_buffer << " at file\n\"" << StrX { toCatch.getSystemId() }
            << "\",\nline " << toCatch.getLineNumber()
            << ", column " << toCatch.getColumnNumber() << ".";
    }
    error_buffer << "\n    " << StrX(toCatch.getMessage()) << std::endl;
}

void DOMTreeErrorReporter::error(const SAXParseException& toCatch)
{
    fSawErrors = true;

    error_buffer << "ERROR";
    if (toCatch.getSystemId()[0] != 0) {
        error_buffer << " at file\n\"" << StrX { toCatch.getSystemId() }
            << "\",\nline " << toCatch.getLineNumber()
            << ", column " << toCatch.getColumnNumber() << ".";
    }
    error_buffer << "\n    " << StrX(toCatch.getMessage()) << std::endl;
}

void DOMTreeErrorReporter::fatalError(const SAXParseException& toCatch)
{
    fSawErrors = true;

    error_buffer << "FATAL ERROR";
    if (toCatch.getSystemId()[0] != 0) {
        error_buffer << " at file\n\"" << StrX { toCatch.getSystemId() }
            << "\",\nline " << toCatch.getLineNumber()
            << ", column " << toCatch.getColumnNumber() << ".";
    }
    error_buffer << "\n    " << StrX(toCatch.getMessage()) << std::endl;
}

void DOMTreeErrorReporter::resetErrors()
{
    fSawErrors = false;
}

std::string DOMTreeErrorReporter::get_error_message() {
    std::string message { error_buffer.str() };

    // Clear the error buffer:
    error_buffer.str("");
    error_buffer.clear();

    return message;
}

