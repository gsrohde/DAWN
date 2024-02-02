# Installation and Compilation Instructions

## Get the source code

Clone _this_ repository using

    git clone --recurse-submodules https://github.com/gsrohde/DAWN.git

The `--recurse-submodules` option will initialize the `biocro-lib`
submodule and pull down its files into the `DAWN/biocro-lib`
subdirectory.  If you cloned the DAWN repository without using the
`--recurse-submodules` option, run

    git submodule update --init --recurse

to ensure the biocro-lib subdirectory gets populated.

## Build the BioCro library, the DAWN library and executable, and the
   tests using CMake.

1. Install CMake if you do not already have a copy.  See
https://cmake.org/download/.  Binary distributions are avaibable for
Windows, Linux, and macOS.  In addition, a macOS installation is
available from [Homebrew](https://brew.sh/).

2. If you haven't already installed it, consider installing the Xerces-C++ XML
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
`--out` option:

    ./main --out=harmonic_oscillator_result.xml sample_input/harmonic_oscillator_system.xml

To try this on a crop model, try using the input file `sample_input/biocro-system.miscanthus.2002.xml`:

    ./main --out=miscanthus_simulation_result.xml sample_input/biocro-system.miscanthus.2002.xml

### Using external drivers files

Since the drivers specification tends to comprise a large amount of
data, and since it may sometimes be desirable to run a simulation
using different driver sets with everything else kept the same, an
option has been added to specify the drivers in a file separate from
the other simulation specifications.  The command-line option to use
is called `--drivers`.

For example, the file `sample_input/biocro-system.miscanthus.xml` is
just like `biocro-system.miscanthus.2002.xml` except that the
`drivers` element has been replaced by a `driver-placeholder` element.
The driver data removed has been placed in a file called
`sample_input/2002_weather.xml`.  Using these two files, we can get
the same result as in the last example above by calling

    ./main --out=miscanthus_simulation_result.xml --drivers=sample_input/2002_weather.xml \
               sample_input/biocro-system.miscanthus.xml

With the ability to specify drivers independently of the main
specification file, it is now easy to switch out one weather data set
for another.  For example, if we want to use weather data from 2005,
we merely have to specify a different drivers file:

    ./main --out=miscanthus_simulation_result.xml --drivers=sample_input/2005_weather.xml \
               sample_input/biocro-system.miscanthus.xml

(Note that if the simultation specification file *does* specify a set
of drivers, using the `--drivers` option will override whatever driver
information is in the specification file so that it is effectively
ignored.  For example, using
`sample_input/biocro-system.miscanthus.2002.xml` in place of
`sample_input/biocro-system.miscanthus.xml` in the previous command so
that we have

    ./main --out=miscanthus_simulation_result.xml --drivers=sample_input/2005_weather.xml \
               sample_input/biocro-system.miscanthus.2002.xml

will yield the same result as before.)

## Using a separate XML validator

The diagnostic information provided by the app during validation of
the XML input file is usually somewhat less than ideal.  Better
diagnostic information is provided by `xmllint`, a command-line tool
that comes with most Linux and macOS installations.  For example, to
validate `sample_input/biocro-system.miscanthus.2002.xml` against the
schema file `sample_input/simulation_input.xsd`, we can run

    xmllint --noout --schema xml_schema/simulation_input.xsd sample_input/biocro-system.miscanthus.2002.xml

