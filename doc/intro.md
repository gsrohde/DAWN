@mainpage

# An introduction to the DAWN software

The DAWN software provides an XML-based interface to the BioCro C++
library for running simulations of plant growth.  It consists of two main components:

* The DAWN application ("`runsim`") is a command-line program taking
  the name of an XML file specifying a simulation specification as a
  parameter.  When run, it produces an XML document showing the
  results of that simulation.

* The DAWN library ("`libdawn.dylib`" or "`libdawn.a`") is a library
  that provides a function for running such a simulation.  A program
  using the library may then extend this functionality in any way it
  chooses.  The library also provides classes that define object types
  for working with the inputs and output of this function.

## The `runsim` command-line application

The `runsim` command in its most basic form takes the name of an XML
file containing a simulation specification as its argument and returns
an XML document presenting the result of running that simulation.
Options exist for skipping schema validation of the input, for
specifying drivers in a separate file, and for wrting the result to a
file rather than standard output, among other things.  This is
explained in more detail in the README file for this repository and in
the command-line help given by running `runsim --help`.

## Using the DAWN library

Application programs may use the DAWN library to run simulations as
part of their processing.

In some cases, we may want to incorporate DAWN's ability to run
simulations into some other application rather than simply running the
`runsim` command-line app.  Here, we provide some guidelines as to how
this can be done.

## An example

The file
[regression_tests.cpp](https://github.com/gsrohde/use_dawn_lib/blob/master/regression_tests.cpp)
in the [use_dawn_lib](https://github.com/gsrohde/use_dawn_lib)
repository demonstrates the use of the DAWN library in another
application.  (This application is just a rewriting of
`tests/test_regression.cpp` in this repository as a free-standing
application.)