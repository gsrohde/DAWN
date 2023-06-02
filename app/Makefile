# This extension works for macOS
LIBRARY_FILE_EXTENSION = .so

BIOCRO_SOURCE_PATH = ../biocro-lib/src

BIOCRO_LIB_NAME = BioCro

BIOCRO_LIB_FILE = $(BIOCRO_SOURCE_PATH)/$(BIOCRO_LIB_NAME)$(LIBRARY_FILE_EXTENSION)

# root directory for BioCro header files
BIOCRO_INCLUDES = $(BIOCRO_SOURCE_PATH)


SOURCES = $(wildcard *.cpp)
OBJECTS = $(SOURCES:.cpp=.o)


OUTPUT_OPTION = -o $@
COMPILE.cpp = $(CXX11) $(CXX11STD) $(ALL_CPPFLAGS) $(ALL_CXXFLAGS) -c




.PHONY: clean main-debug

main:  $(OBJECTS)
	$(LINK) -o $@ $(OBJECTS) $(ALL_LIBS);

# To switch between building the production and debug versions of
# main, do "make clean" first.
main-debug: main

main-debug: CXX11FLAGS = -Wall -g -O0

$(OBJECTS) : %.o : %.cpp
	$(COMPILE.cpp) $(OUTPUT_OPTION) $<

# Additional object file dependencies (on header files).  This is
# simple enough that using automatically-generated dependency files is
# probably not warranted.
DOMTreeErrorReporter.o: DOMTreeErrorReporter.hpp StrX.h
Result_xml_document.cpp: Result_xml_document.h
main.o: option_parser.h simulation_definition.h print_result.h Result_XML_document.h
option_parser.o: option_parser.h
simulation_definition.o: simulation_definition.h compilation_options.h DOMTreeErrorReporter.hpp StrX.h



clean:
	rm -f main
	rm $(OBJECTS)
	rm -fr main.dSYM main-debug.dSYM




ADDL_CPPFLAGS = -I $(BIOCRO_INCLUDES)
CPPFLAGS = -I/usr/local/include # needed?
CXXFLAGS = $(CXX11FLAGS)
CXXPICFLAGS = $(CXX11PICFLAGS)
CXX11 = clang++ -mmacosx-version-min=10.13 # macOS-specific
CXX11FLAGS = -Wall -g -O2 # overridden to use -O0 by main-debug target
CXX11PICFLAGS = -fPIC # needed?
CXX11STD = -std=gnu++11
LDFLAGS = -L/usr/local/lib # needed?
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
