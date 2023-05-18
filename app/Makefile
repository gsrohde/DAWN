# This extension works for macOS
LIBRARY_FILE_EXTENSION = .so

BIOCRO_SOURCE_PATH = ../biocro-lib/src

BIOCRO_LIB_NAME = BioCro

BIOCRO_LIB_FILE = $(BIOCRO_SOURCE_PATH)/$(BIOCRO_LIB_NAME)$(LIBRARY_FILE_EXTENSION)

# root directory for BioCro header files
BIOCRO_INCLUDES = $(BIOCRO_SOURCE_PATH)

run: main
	./main

main: main.cpp xml_parser.cpp Result_xml_document.cpp $(BIOCRO_LIB_FILE) DOMTreeErrorReporter.cpp
	clang++ -std=c++11 -g -Wall -pedantic -I $(BIOCRO_INCLUDES) $(BIOCRO_LIB_FILE) -lxerces-c main.cpp xml_parser.cpp Result_xml_document.cpp DOMTreeErrorReporter.cpp -o main


# For macOS only:

ifeq ($(shell uname -s),Darwin)

debug: main-debug
	/Applications/Xcode.app/Contents/Developer/usr/bin/lldb main-debug

main-debug: main.cpp xml_parser.cpp Result_xml_document.cpp $(BIOCRO_LIB_FILE)
	clang++ -std=c++11 -g -O0 -Wall -pedantic -I $(BIOCRO_INCLUDES) $(BIOCRO_LIB_FILE) -lxerces-c main.cpp xml_parser.cpp Result_xml_document.cpp DOMTreeErrorReporter.cpp -o main-debug

endif
