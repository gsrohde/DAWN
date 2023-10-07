# This extension works for macOS
LIBRARY_FILE_EXTENSION = .so

BIOCRO_SOURCE_PATH = ../biocro-lib/src

BOOST_PATH = ../biocro-lib/inc

BIOCRO_LIB_NAME = BioCro

BIOCRO_LIB_FILE = $(BIOCRO_SOURCE_PATH)/$(BIOCRO_LIB_NAME)$(LIBRARY_FILE_EXTENSION)

# root directory for BioCro header files
BIOCRO_INCLUDES = -I $(BIOCRO_SOURCE_PATH) -I $(BOOST_PATH) -I $(APP_DIRECTORY)

APP_DIRECTORY = ../app

VPATH = $(APP_DIRECTORY)

SOURCES = $(wildcard *.cpp)
OBJECTS = $(SOURCES:.cpp=.o)
EXE = $(OBJECTS:.o=)


test_all : $(OBJECTS) $(BIOCRO_LIB_FILE) simulation_definition.o DOMTreeErrorReporter.o option_parser.o Result_xml_document.o
	clang++ -std=c++14 -o $@ $(BIOCRO_LIB_FILE) $(OBJECTS) \
             $(APP_DIRECTORY)/simulation_definition.o \
             $(APP_DIRECTORY)/DOMTreeErrorReporter.o \
             $(APP_DIRECTORY)/option_parser.o \
             $(APP_DIRECTORY)/Result_xml_document.o \
             -lgtest_main -lgtest -lxerces-c

$(EXE) : % : %.o $(BIOCRO_LIB) simulation_definition.o DOMTreeErrorReporter.o option_parser.o Result_xml_document.o
	clang++ -std=c++14 -o $@ $(BIOCRO_LIB_FILE) $< \
             $(APP_DIRECTORY)/simulation_definition.o \
             $(APP_DIRECTORY)/DOMTreeErrorReporter.o \
             $(APP_DIRECTORY)/option_parser.o \
             $(APP_DIRECTORY)/Result_xml_document.o \
             -lgtest_main -lgtest -lxerces-c


$(OBJECTS) : %.o : %.cpp
	clang++ -std=c++14 $(BIOCRO_INCLUDES) $< -o $@ -c -DVERBOSE=$(VERBOSE)

test_regression.o: run_app.h

clean:
	rm -f $(EXE) $(OBJECTS) test_all


simulation_definition.o DOMTreeErrorReporter.o option_parser.o Result_xml_document.o:
	$(MAKE) -C ../app $@
