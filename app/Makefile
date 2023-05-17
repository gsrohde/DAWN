# This extension works for macOS
LIBRARY_FILE_EXTENSION = so

BIOCRO_LIB = ../src/BioCro.$(LIBRARY_FILE_EXTENSION)

# directory containing BioCro library file
BIOCRO_DYLD_ADDITION = ../src

# root directory for BioCro header files
BIOCRO_INCLUDES = ../src

run: main
	DYLD_LIBRARY_PATH=$(BIOCRO_DYLD_ADDITION) ./main

debug: main-debug
	DYLD_LIBRARY_PATH=$(BIOCRO_DYLD_ADDITION) /Applications/Xcode.app/Contents/Developer/usr/bin/lldb main

main-debug: main.cpp xml_parser.cpp Result_xml_document.cpp $(BIOCRO_LIB)
	clang++ -std=c++11 -g -O0 -Wall -pedantic -I $(BIOCRO_INCLUDES) $(BIOCRO_LIB) -lxerces-c main.cpp xml_parser.cpp Result_xml_document.cpp -o main-debug -lgtest_main -lgtest


main: main.cpp xml_parser.cpp Result_xml_document.cpp $(BIOCRO_LIB) DOMTreeErrorReporter.cpp
	clang++ -std=c++11 -g -Wall -pedantic -I $(BIOCRO_INCLUDES) $(BIOCRO_LIB) -lxerces-c main.cpp xml_parser.cpp Result_xml_document.cpp DOMTreeErrorReporter.cpp -o main -lgtest_main -lgtest

