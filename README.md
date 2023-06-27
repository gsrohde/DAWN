# Installation and Compilation Instructions

## Get the source code

Clone _this_ repository using

    git clone --recurse-submodules https://github.com/gsrohde/DAWN.git

The `--recurse-submodules` option will initialize the `biocro-lib`
submodule and pull down its files into the `DAWN/biocro-lib`
subdirectory.

## Compile the BioCro library

1. CD into the `DAWN/biocro-lib/src` directory.

2. Run `make` in this directory.

## Compile the application

1. If you haven't already installed it, install the Xerces-C++ XML
parser.  Installation instructions are at
https://xerces.apache.org/xerces-c/install-3.html, but it may also be
installed on many platforms using a package manager.

2. CD into the `DAWN/app` directory.

3. Run `make main` in this directory.

## Run the application.

The executable is called `main`.  To see usage information, run
`./main -h` in the `DAWN/app` directory.

### Try out the samples.

Several sample XML input files are provided in the directory
`DAWN/app/sample_input`.  For example,

    ./main sample_input/harmonic_oscillator_system.xml

will run a simulation of a harmonic oscillator system and write the
results to standard output.  To write to a file instead, use the
`-wfile` option:

    ./main -wfile=harmonic_oscillator_result.xml sample_input/harmonic_oscillator_system.xml

To try this on a crop model, try using the input file `sample_input/biocro-system.miscanthus.2002.xml`:

    ./main -wfile=miscanthus_simulation_result.xml sample_input/biocro-system.miscanthus.2002.xml
