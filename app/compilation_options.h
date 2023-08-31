#ifndef COMPILATION_OPTIONS_H
#define COMPILATION_OPTIONS_H


#define QUOTE(name) #name
#define STR(macro) QUOTE(macro)


#ifndef SIMULATION_INPUT_SCHEMA_URI
#define SIMULATION_INPUT_SCHEMA_URI https:\/\/raw.githubusercontent.com/gsrohde/DAWN/develop/app/sample_input/simulation_input.xsd
#endif

#define SIMULATION_INPUT_SCHEMA_URI_STRING STR(SIMULATION_INPUT_SCHEMA_URI)


#endif
