# Building and running these programs rely on these things being true:
#
# 1. The cgbml library is installed, with the added line in Makevars
# that copies the header files and makes them available in the
# package's "include" directory.
#
# 2. The doctest.h file has been downloaded and placed in this
# directory.
#
# 3. The Boost library is present in a system include directory
#

# This extension works for macOS
LIBRARY_FILE_EXTENSION = so

BIOCRO_LIB = ../biocro-lib/src/BioCro.$(LIBRARY_FILE_EXTENSION)

# directory containing BioCro library file
BIOCRO_DYLD_ADDITION = ../biocro-lib/src

# root directory for BioCro header files
BIOCRO_INCLUDES = ../biocro-lib/src

APP_DIRECTORY = ../app

VPATH = $(APP_DIRECTORY)


test_simulation_definition: test_simulation_definition.cpp $(BIOCRO_LIB) simulation_definition.o DOMTreeErrorReporter.o
	clang++ -std=c++14 -I $(BIOCRO_INCLUDES) -I $(APP_DIRECTORY) $(BIOCRO_LIB) \
            $(APP_DIRECTORY)/simulation_definition.o \
            $(APP_DIRECTORY)/DOMTreeErrorReporter.o \
            test_simulation_definition.cpp \
            -o test_simulation_definition -lgtest -lgtest_main -lxerces-c

simulation_definition.o DOMTreeErrorReporter.o:
	$(MAKE) -C ../app $@

run_test_biocro: test_biocro
	DYLD_LIBRARY_PATH=$(BIOCRO_DYLD_ADDITION) ./test_biocro --gtest_also_run_disabled_tests ‑‑gtest_repeat=1

test_biocro: test_biocro.cpp $(BIOCRO_LIB)
	clang++ -std=c++14 -I $(BIOCRO_INCLUDES) $(BIOCRO_LIB) test_biocro.cpp -o test_biocro -lgtest -lgtest_main
