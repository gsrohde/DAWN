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

doc <- xml_new_root("simulation-specification")

dyn_sys <- xml_add_child(doc, "dynamical-system")

parameters <- xml_add_child(dyn_sys, "parameters")

named_list_to_xml(parameters, CROP$parameters)

initial_state <- xml_add_child(dyn_sys, "initial-state")

named_list_to_xml(initial_state, CROP$initial_values)

drivers <- xml_add_child(dyn_sys, "drivers")

weather_data <- get_growing_season_climate(weather[[WEATHER_YEAR]])
dataframe_to_xml(drivers, weather_data)

direct_modules <- xml_add_child(dyn_sys, "direct-modules")

list_to_xml(direct_modules, CROP$direct_modules)

differential_modules <- xml_add_child(dyn_sys, "differential-modules")

list_to_xml(differential_modules, CROP$differential_modules)

write_xml(doc, OUTPUT_FILE_NAME)

end_time <- Sys.time()

time_taken <- round(end_time - start_time)

print(time_taken)
