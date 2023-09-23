# This extension works for macOS
LIBRARY_FILE_EXTENSION = .so

BIOCRO_SOURCE_PATH = ../biocro-lib/src

BOOST_PATH = ../biocro-lib/inc

BIOCRO_LIB_NAME = BioCro

BIOCRO_LIB_FILE = $(BIOCRO_SOURCE_PATH)/$(BIOCRO_LIB_NAME)$(LIBRARY_FILE_EXTENSION)

# root directory for BioCro header files
BIOCRO_INCLUDES = -I $(BIOCRO_SOURCE_PATH) -I $(BOOST_PATH)

# for installations that need it:
HOMEBREW_APPLE_SILICON_PREFIX = /opt/homebrew

SOURCES = $(wildcard *.cpp)
OBJECTS = $(SOURCES:.cpp=.o)


OUTPUT_OPTION = -o $@
COMPILE.cpp = $(CXX11) $(CXX11STD) $(ALL_CPPFLAGS) $(ALL_CXXFLAGS) -c


# Compilation options

# Override with "make <target> use_local_schema_file=true"
use_local_schema_file = false

VARIABLE_SETTINGS := -DUSE_LOCAL_SCHEMA_FILE=$(use_local_schema_file)

# If we *are* using a local schema file, we need to pass its path to the compiler
ifeq ($(use_local_schema_file),true)
    VARIABLE_SETTINGS += -DLOCAL_SCHEMA_FILEPATH=$(shell pwd)/xml_schema/simulation-specification.xsd
endif


# Rules

.PHONY: clean main-debug help

help: # Show make targets and options.
	@echo
	@echo "Targets:"
	@echo
	@echo "    make main: Build the app executable \"main\""
	@echo "    make main-debug: Build a version of the app executable \"main\" suitable for"
	@echo "                     use with a debugger"
	@echo "    make clean: Remove all build products"
	@echo
	@echo "    Note: Run \"make clean\" in between switching between building the debug and"
	@echo "          production versions of main."
	@echo
	@echo "Variable settings:"
	@echo
	@echo "    make use_local_schema_file=true -B main: Build a version of main that uses"
	@echo "                                             the local copy of the schema."
	@echo
	@echo "    Note: The -B option forces all object files to be recompiled, ensuring they"
	@echo "          get the new schema location setting."
	@echo

main:  $(OBJECTS)
	$(LINK) -o $@ $(OBJECTS) $(ALL_LIBS);

# To switch between building the production and debug versions of
# main, do "make clean" first.
main-debug: main

main-debug: CXX11FLAGS = -Wall -g -O0

$(OBJECTS) : %.o : %.cpp
	$(COMPILE.cpp) $(VARIABLE_SETTINGS) $(OUTPUT_OPTION) $<

# Additional object file dependencies (on header files).  This is
# simple enough that using automatically-generated dependency files is
# probably not warranted.
DOMTreeErrorReporter.o: DOMTreeErrorReporter.hpp StrX.h
Result_xml_document.o: Result_xml_document.h xstr.h
main.o: option_parser.h parser_options.h simulation_definition.h solver.h print_result.h Result_XML_document.h
option_parser.o: option_parser.h parser_options.h
simulation_definition.o: simulation_definition.h solver.h compilation_options.h DOMTreeErrorReporter.hpp StrX.h xstr.h



clean:
	rm -f main
	rm $(OBJECTS)
	rm -fr main.dSYM main-debug.dSYM




ADDL_CPPFLAGS = $(BIOCRO_INCLUDES)
CPPFLAGS = -I/usr/local/include -I$(HOMEBREW_APPLE_SILICON_PREFIX)/include
CXXFLAGS = $(CXX11FLAGS)
CXXPICFLAGS = $(CXX11PICFLAGS)
CXX11 = clang++ -mmacosx-version-min=10.13 # macOS-specific
CXX11FLAGS = -Wall -g -O2 # overridden to use -O0 by main-debug target
CXX11PICFLAGS = -fPIC # needed?
CXX11STD = -std=gnu++11
LDFLAGS = -L/usr/local/lib -L$(HOMEBREW_APPLE_SILICON_PREFIX)/lib
CXX11LD = $(CXX11) $(CXX11STD)
CXX11LDFLAGS =# -dynamiclib -Wl,-headerpad_max_install_names -undefined dynamic_lookup -single_module -multiply_defined suppress# $(CXX11FLAGS) $(CXX11PICFLAGS) # commented out for now
LINK = $(CXX11LD) $(CXX11LDFLAGS) $(LDFLAGS)
ALL_CPPFLAGS =  -DNDEBUG $(ADDL_CPPFLAGS) $(CPPFLAGS) 
ALL_CXXFLAGS =  $(CXXPICFLAGS) $(CXXFLAGS)
ALL_LIBS =  $(BIOCRO_LIB_FILE) -lxerces-c


# For macOS only:

ifeq ($(shell uname -s),Darwin)

.PHONY: debug

debug: main-debug
	/Applications/Xcode.app/Contents/Developer/usr/bin/lldb main-debug

endif
