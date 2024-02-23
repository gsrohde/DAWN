#!/usr/bin/env Rscript

start_time <- Sys.time()

argument_list <- commandArgs(TRUE)

if (length(argument_list) != 3) {
    stop("Usage: Rscript crop_models_to_xml.R <crop name> <weather year> <output file name>")
}
CROP_NAME <- argument_list[[1]]
WEATHER_YEAR <- argument_list[[2]]
OUTPUT_FILE_NAME <- argument_list[[3]]


library(BioCro)
CROP <- eval(parse(text=CROP_NAME))

library(xml2)




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

drivers_element <- xml_add_child(dynamical_system_element, "drivers")
xml_set_attr(drivers_element, "timestep", timestep)

weather_data <- get_growing_season_climate(weather[[WEATHER_YEAR]])
dataframe_to_xml(drivers_element, weather_data)

direct_modules_element <- xml_add_child(dynamical_system_element, "direct-modules")

list_to_xml(direct_modules_element, CROP$direct_modules)

differential_modules_element <- xml_add_child(dynamical_system_element, "differential-modules")

list_to_xml(differential_modules_element, CROP$differential_modules)

write_xml(document_element, OUTPUT_FILE_NAME)

end_time <- Sys.time()

time_taken <- round(end_time - start_time)

print(time_taken)
