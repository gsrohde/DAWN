#ifndef COMPILATION_OPTIONS_H
#define COMPILATION_OPTIONS_H

#include <string>

using std::string;

#define QUOTE(name) #name
#define STR(macro) QUOTE(macro)

inline string get_schema_uri() {

#if defined(USE_LOCAL_SCHEMA_FILE) && (USE_LOCAL_SCHEMA_FILE == true) && defined(LOCAL_SCHEMA_FILEPATH)
    return STR(LOCAL_SCHEMA_FILEPATH);
#else
    #define SIMULATION_INPUT_SCHEMA_URI https:\/\/raw.githubusercontent.com/gsrohde/DAWN/revise_schema_setting_mechanism/app/xml_schema/simulation_input.xsd
    return STR(SIMULATION_INPUT_SCHEMA_URI);
#endif
}

#endif
