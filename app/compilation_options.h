#ifndef COMPILATION_OPTIONS_H
#define COMPILATION_OPTIONS_H


#define QUOTE(name) #name
#define STR(macro) QUOTE(macro)


#ifndef SIMULATION_INPUT_SCHEMA_URI
#define SIMULATION_INPUT_SCHEMA_URI https:\/\/raw.githubusercontent.com/gsrohde/DAWN/revise_schema_setting_mechanism/app/xml_schema/simulation_input.xsd
#endif

#define SIMULATION_INPUT_SCHEMA_URI_STRING STR(SIMULATION_INPUT_SCHEMA_URI)


#endif
