# Installation, Compilation, and Usage Instructions

_This version of DAWN is built using CMake.  For the version that
requires Make only, check out the version tagged "cmake-less_version".
It is **strongly** recommended, however, that you upgrade to the
CMake-managed versions._

[Note: These instructions are biased towards the macOS operating
system, because that's what I use.  Your mileage may vary on other
platforms.]

## Get the DAWN source code

Clone _this_ repository using

    git clone --recurse-submodules https://github.com/gsrohde/DAWN.git

The `--recurse-submodules` option will initialize the `biocro-lib`
submodule and pull down its files into the `DAWN/biocro-lib`
subdirectory.  If you cloned the DAWN repository without using the
`--recurse-submodules` option, run

    git submodule update --init --recurse

to ensure the `biocro-lib` subdirectory gets populated.

## Build the BioCro library, the DAWN library and executable, and the tests

DAWN uses CMake to manage the build and testing processes.

1. Install CMake if you do not already have a copy.  See
https://cmake.org/download/.  Binary distributions are available for
Windows, Linux, and macOS.  In addition, a macOS installation is
available from [Homebrew](https://brew.sh/).

2. If you haven't already installed it, install the Xerces-C++ XML
parser.  Installation instructions are at
https://xerces.apache.org/xerces-c/install-3.html, but it may also be
installed on many platforms using a package manager.  (Homebrew calls
the package `xerces-c`.)

3. Create a build directory and then `cd` to it.  **The instructions
that follow assume the build directory is the current directory and
has the same parent directory as the `DAWN` root directory.**

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

The Doxygen documentation is not generated by default.  Instead, use
the `docs` target:

        cmake --build . --target docs

To view the documentation, open a Web browser to
`./doc/docs/index.html`.  (On many platforms, this may be done by
running `open doc/docs/index.html` from the build directory.)

Several CMake options are available for customizing the Doxygen
documentation generated.  See the _configuration options_ section.

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

Note: Some configuration options, such as those having to do with
customizing Doxygen output, are only available if other options are
turned on.  The Doxygen configuration options, for example, require
`CUSTOMIZE_DOXYGEN` to be turned on.  To ensure these options are
viewable when we use `-L` to list available options, we can use the
command

        cmake -S ../DAWN -B . -L -DCUSTOMIZE_DOXYGEN=ON

Here is more extensive information about the various options:

* BUILD_SHARED_LIBS (_default:_ ON)

    By default, both the BioCro and the DAWN libraries will be built
    as shared libraries.  Turn this option off to build them as static
    libraries.

    * BIOCROLIB_STATIC (dependent option; _default:_ ON)

        This option is only available if BUILD_SHARED_LIBS is turned
        OFF.

    * DAWNLIB_STATIC (dependent option; _default:_ ON)

        This option is only available if BUILD_SHARED_LIBS is turned
        OFF.

    These two sub-obtions are mainly useful if we want one library to
    be shared and the other to be static.  For example, if we want to
    build both `libBioCro.a` and `libdawn_lib.dylib`, we could use the
    configuration command

            cmake -S <DAWN source directory> -B <build directory> -DBUILD_SHARED_LIBS=OFF -DDAWNLIB_STATIC=OFF

* CMAKE_BUILD_TYPE (_default:_ Release)

    Set to "Debug" to build a debug version of the software.

* CMAKE_INSTALL_PREFIX (_default:_ system determined; typically /usr/local on UNIX-like systems)

    Set to a custom value if you don't want to or don't have
    permission to install to the default location.  As noted above,
    this variable may be set using the `--install-prefix` option
    rather than using `-DCMAKE_INSTALL_PREFIX=...`.  **Note that the
    prefix must be an absolute path to a directory!**

* INSTALL_BIOCRO_HEADERS (_default:_ OFF)

    Generally, everything needed to run BioCro simulations is
    available via the DAWN library functions.  But if, for some
    reason, you are writing code that needs to access BioCro classes
    and functions directly, turn this option on.

    Note that even when this option is OFF, the BioCro library itself
    is always installed.

* INSTALL_GTEST (_default:_ OFF)

    If GoogleTest was not installed on your system when you did a
    CMake build, turning this option on will install it for you when
    you do a CMake install.  Otherwise, this option has no effect.

    Note that it is not necessary to install GoogleTest when running
    `ctest` in the build directory; the tests will have been built
    using the GoogleTest library built by CMake.

* USE_LOCAL_SCHEMA_FILE (_default:_ ON)

    If this option is set to ON, both the _build_ location of the
    simulation specification schema file
    (`simulation-specification.xsd`) and the _install_ location will
    be placed in the search path for finding the document schema (in
    that order).  The first schema file found that parses correctly
    will be used in validating the simulation input and drivers files.

* USE_REMOTE_SCHEMA_URI (_default:_ OFF)

    If this option is set to ON, and if a suitable local schema file
    is not found, an attempt to use a version of the schema stored in
    the GitHub repository will be make.  (Currently, the version used
    will be the latest version stored on the `development` branch.)

* CUSTOMIZE_DOXYGEN (_default:_ OFF)

    Turning this option on will make the following Doxygen
    customization options available:

    * ADD_TREEVIEW (_default:_ ON)

        This setting controls the appearance of a tree-like index in a
        side-panel (see
        https://www.doxygen.nl/manual/config.html#cfg_generate_treeview).
        The default ON setting remains on even when CUSTOMIZE_DOXYGEN is
        turned on.

        Users of devices with limited horizonal screen space may want
        to turn treeview off.  To do this, set CUSTOMIZE_DOXYGEN to ON
        and then ADD_TREEVIEW to OFF.

    * DOCUMENT_PRIVATE_MEMBERS (_default:_ OFF)

        By default, only public class members are documented.  Turn
        this on to document private members as well.

    * USE_DOXYGEN_AWESOME_STYLESHEET (_default:_ OFF)

        Turning this on enables usage of the doxygen-awesome-css
        stylesheet from [jothepro](https://github.com/jothepro) to
        create a more modern-looking appearance for the Doxygen
        documenation as recommended by Rafał Świdziński in his _Modern
        CMake for C++_.  When this is turned on, the build will fetch
        the stylesheet from the GitHub repository and add it into the
        generated documention.

    * DOXYGEN_HTML_COLORSTYLE_HUE (_default:_ 120 (green) if
      CUSTOMIZE_DOXYGEN is on, 220 (cyan-blue) otherwise)

        This setting controls the color of the text in the generated
        documentation.  It should be a value between 0 and 360.  See
        https://www.doxygen.nl/manual/config.html#cfg_html_colorstyle_hue
        for details.

    * HTML_COLORSTYLE_SAT (_default:_ 100)

        This setting controls the purity (saturation) of the colors in
        the HTML Doxygen documentation output.  Values may range from
        0 (gray-scales only) to 255 (the most vivid colors).

    * Document_XercesC_Headers (_default:_ OFF)

        Include the XercesC header files among the files for Doxygen
        to document.  Since these files don't include Doxygen-style
        comments, this option is probably mainly useful as a way to
        show all Xerces header files in the dependency graph of file
        includes and as a convenient way to browse the source code in
        these header files.

## Using a separate XML validator

The diagnostic information provided by the app during validation of
the XML input file is usually somewhat less than ideal.  Better
diagnostic information is provided by `xmllint`, a command-line tool
that comes with most Linux and macOS installations.  For example, to
validate `sample_input/biocro-system.miscanthus.2002.xml` against the
schema file `xml_schema/simulation_input.xsd`, we can run

        xmllint --noout --schema xml_schema/simulation_input.xsd sample_input/biocro-system.miscanthus.2002.xml

## The format of the simulation specification and drivers files

The formal requirements of the XML specification and drivers files are
determined by the XML Schema files located in the `app/xml_schema`
directory.  See the many explanatory notes in those files for details.
Here we give a brief practical summary of the essentials.

### Informational requirements

In general, the simulation specification must specify details about
the dynamical system being solved and about the solver being used to
solve it.

Thus, the specification document will generally consist of a top-level
element called `simulation-specification` with child elements `solver`
and `dynamical-system` and so will have the following form:

```
<simulation-specification>

  <solver ... />

  <dynamical-system>
    ...
  </dynamical-system>

</simulation-specification>
```

The solver element is optional.  If omitted, a default value will be
used.  In fact, if there is no solver specification,
`dynamical-system` may be used as the top-level element.

#### Specifying a dynamical system



## The format of XML result files

Coming soon!

## Using the crop_models_to_xml.R script to generate input files

The fastest way to create input files for various BioCro crop models
is to use the `crop_models_to_xml.R` script in conjunction with a
working R installation and together with the BioCro R package.

The BioCro R package comes with a pre-determined set of crop models
for miscanthus (Miscanthus x giganteus) and willow (Salix spp.) (see
https://biocro.github.io/BioCro-documentation/latest/pkgdown/reference/crop_model_definitions.html).
It also comes with several years worth of weather data that may be
used as system drivers (see https://biocro.github.io/BioCro-documentation/latest/pkgdown/reference/cmi_weather_data.html).

To generate a simulation specification for one of these crops, run the
script as follows:

        Rscript crop_models_to_xml.R <crop name> <weather data year> <output file name>

where "crop name" is "willow" or "miscanthus_x_giganteus", the weather
data year is a year between 1995 and 2020 (inclusive), and "output
file name" is some suitable name of your choosing.  For example,

        Rscript crop_models_to_xml.R willow 2002 biocro-system.willow.2002.xml

should generate an input file for the willow crop model using weather
data from 2002.

### Separating out the drivers file

It is convenient to be able to use a crop model with weather data from
multiple years.  For this reason, DAWN provides the option to draw the
drivers information from a separate file (see _Using external drivers
files_ above).  Thus, the generation script also provides the option
to put the generated weather information in a separate file by using
the `-d` option.  For example, to make an input file for the willow
crop model, but with the 2002 weather data in a separate file, we
could alter the above run command to

        Rscript crop_models_to_xml.R willow 2002 biocro-system.willow.xml -d 2002_weather.xml

### Skipping drivers generation

Lastly, we don't always want to generate the weather data file
whenever we generate the input file for a crop model: the generation
of the weather data is by far the most time-consuming step of the
script.  To skip this step, use the `--no-drivers` option.

For example, suppose we have just used the previous command to
generate `biocro-system.willow.xml`, which contains the crop model
information for willow, and `2002_weather.xml`, which contains the
weather data for 2002.  Suppose we now want a file containing the crop
model information for miscanthus, but we don't want to go through the
time-consuming process of regenerating the 2002 weather data file.
Then we can simply run

        Rscript crop_models_to_xml.R miscanthus_x_giganteous biocro-system.miscanthus.xml --no-drivers

Here, we have left out the year parameter and have added the
`--no-drivers` option.  This command takes almost no time to complete
its run.
