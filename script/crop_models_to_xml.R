#!/usr/bin/env Rscript

start_time <- Sys.time()

USAGE_TEXT <- r"(
    %prog <crop> <year> <spec filename> [(-d|--drivers-file) <drivers filename>]
    %prog <crop> <spec filename> --no-drivers
    %prog (-h|--help|-m|--man)

(Option parameters may appear before, between, or after positional parameters.)
)"

MANUAL_TEXT <- r"(
General Information:

    This script provides a way to generate input files for various
    BioCro crop models from the data files in the BioCro R package.
    The BioCro R package is available on GitHub at
    https://github.com/biocro/biocro.

    When run without options, the script expects three arguments: the
    name of the crop model (either "willow" or
    "miscanthus_x_giganteus"), a specification of a year between 1995
    and 2020 (inclusive), and the name for the output file, which will
    be an XML file suitable for use as the simulation specification
    required by the DAWN executable.  For example,

        Rscript crop_models_to_xml.R willow 2005 biocro-system.willow.2005.xml

    By default, the driver information will be included in the
    simulation specification output file.  To write it to a separate
    drivers file, use the "-d" option with the name of the file to
    write the drivers to.  Here is a typical call:

        Rscript crop_models_to_xml.R willow 2005 biocro-system.willow.xml -d 2005_weather.xml

    Since often the same weather data--the same drivers--will be used
    with multiple crop models, and since, generally, generating the
    drivers element is the most time-consuming part of the script,
    there is an option, --no-drivers, to suppress the generation of
    driver data.  In this case, the year should be left out as well.
    For example,

        Rscript crop_models_to_xml.R --no-drivers miscanthus_x_giganteus biocro-system.miscanthus.xml

    This will result in a "driver-placeholder" element being used in
    place of a "drivers" element in the resulting simulation
    specification file.
  )"

main <- function() {

    library(optparse)

    option_error <- function(message) {
        cat(opt_parser@usage, "\n\n")
        cat(message, "\n")
    }

    ## The signature of callback functions such as this is partially
    ## predetermined, which is why we have three unused parameters.
    print_usage_manual <- function(option, opt, value, parser) {
        print_help(parser)
        cat(MANUAL_TEXT)
        return(TRUE)
    }

    is_valid_weather_year <- function(weather_year) {

        is.wholenumber <-
            function(x, tol = .Machine$double.eps^0.5)  abs(x - round(x)) < tol
        
        yr <- suppressWarnings(as.numeric(weather_year))

        return (!is.na(yr) && is.wholenumber(yr) &&
                yr >= 1995 && yr <= 2020)
   }

    is_valid_crop_name <- function(crop_name) {
        return(crop_name %in% c("willow", "miscanthus_x_giganteus"))
    }

    opt_parser <- OptionParser(usage = USAGE_TEXT)
    opt_parser <- add_option(opt_parser, c("-m", "--man"), help="Print complete usage information", callback = print_usage_manual)
    opt_parser <- add_option(opt_parser, c("-d", "--drivers-file"), help="File to write drivers to", action = "store")
    opt_parser <- add_option(opt_parser, c("--no-drivers"),
                             help="Suppress generation of drivers",
                             action = "store_true", default =  FALSE)

    arguments = parse_args(opt_parser, positional_arguments = TRUE)

    opt <- arguments$options
    pos_args <- arguments$args

    expected_number_of_positionals <- ifelse(is.null(opt$man),
                                             ifelse(opt$`no-drivers`, 2, 3),
                                             0)
    if (length(pos_args) != expected_number_of_positionals) {
        cat("\nUNEXPECTED NUMBER OF ARGUMENTS\n\n")
        print_help(opt_parser)
        return()
    }

    if (is.null(opt$man)) {
        SUPPRESS_DRIVERS <<- opt$`no-drivers`

        CROP_NAME <<- pos_args[[1]]

        if (!is_valid_crop_name(CROP_NAME)) {
            cat("\nINVALID CROP NAME\nValid crop names are 'willow' and 'miscanthus_x_giganteus'.\n\n")
            return()
        }

        if (SUPPRESS_DRIVERS) {
            OUTPUT_FILE_NAME <<- pos_args[[2]]
        } else {
            WEATHER_YEAR <<- pos_args[[2]]

            if (!is_valid_weather_year(WEATHER_YEAR)) {
                cat("\nINVALID WEATHER YEAR\nThe weather year must be between 1995 and 2020 (inclusive).\n\n")
                return()
            }
            OUTPUT_FILE_NAME <<- pos_args[[3]]
        }

        DRIVERS_FILE_NAME <<- opt$`drivers-file`

        generate_files()
    }
}

named_list_to_xml <- function(top, l) {
    for (name in names(l)) {
        variable <- xml_add_child(top, "variable")
        xml_set_attr(variable, "name", name)
        xml_set_attr(variable, "value", l[[name]])
    }
}

list_to_xml <- function(top, l) {
    for (item in l) {
        module <- xml_add_child(top, "module")
        xml_set_attr(module, "name", substring(item, 8)) # use substring to strip off the "BioCro:" prefix
    }
}

dataframe_to_xml <- function(top, df) {
    n <- nrow(df)
    names <- names(df)

    for (i in seq(1, n)) {
        row <- xml_add_child(top, "row")
        for (name in names) {
            variable <- xml_add_child(row, "variable")
            xml_set_attr(variable, "name", name)
            xml_set_attr(variable, "value", df[i, name])
        }
        if (!("time" %in% names)) {
            variable <- xml_add_child(row, "variable")
            xml_set_attr(variable, "name", "time")
            xml_set_attr(variable, "value", df[i, "doy"] + df[i, "hour"]/24.0)
        }
    }
}


generate_files <- function() {

    library(BioCro)

    CROP <- eval(parse(text=CROP_NAME))

    library(xml2)

    document_element <- xml_new_root("simulation-specification")

    solver_element <- xml_add_child(document_element, "solver")

    solver <- CROP$ode_solver
    xml_set_attr(solver_element, "name", solver$type)

                                        # The other solver attributes don't apply to Euler solvers.
    if (!(solver$type %in% c('boost_euler', 'homemade_euler'))) {
        xml_set_attr(solver_element, "output-step-size", solver$output_step_size)
        xml_set_attr(solver_element, "adaptive-relative-error-tolerance", solver$adaptive_rel_error_tol)
        xml_set_attr(solver_element, "adaptive-absolute-error-tolerance", solver$adaptive_abs_error_tol)
        xml_set_attr(solver_element, "adaptive-maximum-steps", solver$adaptive_max_steps)
    }

    dynamical_system_element <- xml_add_child(document_element, "dynamical-system")

    parameters_element <- xml_add_child(dynamical_system_element, "parameters")

    ## DAWN puts timestep as an attribute on the drivers rather than as a
    ## parameter:
    timestep <- CROP$parameters$timestep
    CROP$parameters$timestep <- NULL

    named_list_to_xml(parameters_element, CROP$parameters)

    initial_state_element <- xml_add_child(dynamical_system_element, "initial-state")

    named_list_to_xml(initial_state_element, CROP$initial_values)

    if (SUPPRESS_DRIVERS || !is.null(DRIVERS_FILE_NAME)) {
        driver_placeholder_element <- xml_add_child(dynamical_system_element, "driver-placeholder")
        xml_set_attr(driver_placeholder_element, "drivers-defined-externally", "true")
    }

    if (!SUPPRESS_DRIVERS) {
        if (is.null(DRIVERS_FILE_NAME)) {
            drivers_element <- xml_add_child(dynamical_system_element, "drivers")
        } else {
            drivers_element <- xml_new_root("drivers")
        }

        xml_set_attr(drivers_element, "timestep", timestep)

        weather_data <- get_growing_season_climate(weather[[WEATHER_YEAR]])
        dataframe_to_xml(drivers_element, weather_data)

        if (!is.null(DRIVERS_FILE_NAME)) {
            write_xml(drivers_element, DRIVERS_FILE_NAME)
        }
    }

    direct_modules_element <- xml_add_child(dynamical_system_element, "direct-modules")

    list_to_xml(direct_modules_element, CROP$direct_modules)

    differential_modules_element <- xml_add_child(dynamical_system_element, "differential-modules")

    list_to_xml(differential_modules_element, CROP$differential_modules)

    write_xml(document_element, OUTPUT_FILE_NAME)

    end_time <- Sys.time()

    time_taken <- round(end_time - start_time)

    print(time_taken)
}

invisible(main())
