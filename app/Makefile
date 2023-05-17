# This extension works for macOS
LIBRARY_FILE_EXTENSION = so

BIOCRO_LIB = BioCro.$(LIBRARY_FILE_EXTENSION)

# root directory for BioCro header files
BIOCRO_INCLUDES = ../biocro-lib/src

run: main
	./main

debug: main-debug
	/Applications/Xcode.app/Contents/Developer/usr/bin/lldb main

main-debug: main.cpp xml_parser.cpp Result_xml_document.cpp $(BIOCRO_LIB)
	clang++ -std=c++11 -g -O0 -Wall -pedantic -I $(BIOCRO_INCLUDES) $(BIOCRO_LIB) -lxerces-c main.cpp xml_parser.cpp Result_xml_document.cpp -o main-debug -lgtest_main -lgtest


main: main.cpp xml_parser.cpp Result_xml_document.cpp $(BIOCRO_LIB) DOMTreeErrorReporter.cpp
	clang++ -std=c++11 -g -Wall -pedantic -I $(BIOCRO_INCLUDES) $(BIOCRO_LIB) -lxerces-c main.cpp xml_parser.cpp Result_xml_document.cpp DOMTreeErrorReporter.cpp -o main -lgtest_main -lgtest

