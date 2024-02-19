# Installation and Compilation Instructions

## Get the source code

Clone _this_ repository using

    git clone --recurse-submodules https://github.com/gsrohde/DAWN.git

The `--recurse-submodules` option will initialize the `biocro-lib`
submodule and pull down its files into the `DAWN/biocro-lib`
subdirectory.  If you cloned the DAWN repository without using the
`--recurse-submodules` option, run

    git submodule update --init --recurse

to ensure the `biocro-lib` subdirectory gets populated.

## Build the BioCro library, the DAWN library and executable, and the tests using CMake.

1. Install CMake if you do not already have a copy.  See
https://cmake.org/download/.  Binary distributions are avaibable for
Windows, Linux, and macOS.  In addition, a macOS installation is
available from [Homebrew](https://brew.sh/).

2. If you haven't already installed it, install the Xerces-C++ XML
parser.  Installation instructions are at
https://xerces.apache.org/xerces-c/install-3.html, but it may also be
installed on many platforms using a package manager.

3. Create a build directory.  The instructions that follow assume the
build directory is called `DAWN_build` and has the same parent
directory as the `DAWN` root directory.

4. CD to the build directory.

5. Run cmake, specifying the source directory with the -S and the
build directory with -B:

        cmake -S ../DAWN -B .

    This configures and generates build instructions to be used in the
    build step.

    If you wish to install the DAWN library and executable, and you
    wish to install to other than the standard location, use the
    `--install-prefix` option.  For example, to install the libraries
    under `/Users/yourname/local`, run

        cmake -S ../DAWN -B . --install-prefix /Users/yourname/local

    Note that the install prefix must be an absolute path to a
    directory.  We will discuss other available configuration options
    below.

6. Build the software with CMake:

        cmake --build .

7. Optional: Test the build by running ctest:

        ctest

8. Optional: Install the DAWN library and executable:

        cmake --install .

    If you set an installation prefix to install to a non-standard
    location such as `/Users/yourname/local` during the configuration
    step, consider adding the directory containing the DAWN executable
    (`/Users/yourname/local/bin`) to your path.

## Running the application.

The executable is called `dawn`.  In what follows, we assume that you
haven't necessarily installed this to your system.  Instead, we will
run the copy created in the `<build directory>/app` directory by the
build step.

Start by CD-ing to the `app` directory with

        cd app

(assuming you are in the DAWN_build directory to start with).

### Displaying usage inform

To see usage information, run

        ./dawn -h

(Note: If you installed dawn to a location in you path, you can just
run this with `dawn -h`.)

### Trying out the samples.

Several sample XML input files are provided in the directory
`DAWN_build/app/sample_input`.  For example,

        ./dawn sample_input/harmonic_oscillator_system.xml

will run a simulation of a harmonic oscillator system and write the
results to standard output.  To write to a file instead, use the
`--out` (or `-o`) option:

        ./dawn --out=harmonic_oscillator_result.xml sample_input/harmonic_oscillator_system.xml

To try this on a crop model, try using the input file `sample_input/biocro-system.miscanthus.2002.xml`:

        ./dawn --out=miscanthus_simulation_result.xml sample_input/biocro-system.miscanthus.2002.xml

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

        ./dawn --out=miscanthus_simulation_result.xml --drivers=sample_input/2002_weather.xml \
               sample_input/biocro-system.miscanthus.xml

With the ability to specify drivers independently of the main
specification file, it is now easy to switch out one weather data set
for another.  For example, if we want to use weather data from 2005,
we merely have to specify a different drivers file:

        ./dawn --out=miscanthus_simulation_result.xml --drivers=sample_input/2005_weather.xml \
               sample_input/biocro-system.miscanthus.xml

(Note that if the simulation specification file *does* specify a set
of drivers, using the `--drivers` option will override whatever driver
information is in the specification file so that it is effectively
ignored.  For example, using
`sample_input/biocro-system.miscanthus.2002.xml` in place of
`sample_input/biocro-system.miscanthus.xml` in the previous command so
that we have

        ./dawn --out=miscanthus_simulation_result.xml --drivers=sample_input/2005_weather.xml \
               sample_input/biocro-system.miscanthus.2002.xml

will still use the 2005 weather data, not the 2002 data contained in
`biocro-system.miscanthus.2002.xml`.)

## Generating the Doxygen documentation

Coming soon!

## CMake configuration options for DAWN

To see a list of the most useful user configuration options, run the
CMake configuration/generation command with the -L option:

        cmake -S ../DAWN -B . -L

(This assumes the build directory is the current directory and that
`../DAWN` is the relative path to the DAWN source directory.)

To see the help strings associated with each option, add the -H
option:

        cmake -S ../DAWN -B . -LH

(Adding the -A option will show advanced options, not documented
here.)

To set a configuration option from the command line, use the `-D`
option.  For example,

        cmake -S ../DAWN -B . -DCMAKE_BUILD_TYPE=Debug

will set the build type to "Debug".  Configuration variables may also
be set interactively if `ccmake` or `cmake-gui` are used.

Here is more extensive information about the various options:

* CMAKE_BUILD_TYPE default: Release

    Set to "Debug" to build a debug version of the software.

* BUILD_SHARED_LIBS default: OFF

* CMAKE_INSTALL_PREFIX default: system determined; typically /usr/local on UNIX-like systems

    Set to a custom value if you don't want to or don't have
    permission to install to the default location.  As noted above,
    this variable may be set using the `--install-prefix` option
    rather than using `-DCMAKE_INSTALL_PREFIX=...`.  **Note that the
    prefix must be an absolute path to a directory.!**

* INSTALL_BIOCRO_LIBRARY default: OFF

    If the BioCro library is built as a shared library, it must be
    installed so that libraries and executables that use it have
    access to it.  On the other hand, if it is built as a static
    library, it may, but need not be, installed; the DAWN library will
    include its code.

* INSTALL_GTEST default: OFF

    If GoogleTest was not installed on your system when you did a
    CMake build, turning this option on will install it for you when
    you do a CMake install.  Otherwise, this option has no effect.

    Note that it is not necessary to install GoogleTest when running
    `ctest` in the build directory; the tests will have been built
    using the GoogleTest library built by CMake.

* USE_LOCAL_SCHEMA_FILE default: ON

    If this option is set to ON, both the _build_ location of the
    simulation specification schema file
    (`simulation-specification.xsd`) and the _install_ location will
    be placed in the search path for finding the document schema.



## Using a separate XML validator

The diagnostic information provided by the app during validation of
the XML input file is usually somewhat less than ideal.  Better
diagnostic information is provided by `xmllint`, a command-line tool
that comes with most Linux and macOS installations.  For example, to
validate `sample_input/biocro-system.miscanthus.2002.xml` against the
schema file `xml_schema/simulation_input.xsd`, we can run

        xmllint --noout --schema xml_schema/simulation_input.xsd sample_input/biocro-system.miscanthus.2002.xml

## The format of the XML specification files

Coming soon!

## The format of XML result files

Coming soon!

## Using the xml_utilities.R script to generate input files

Coming soon!


