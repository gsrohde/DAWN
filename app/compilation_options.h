#ifndef COMPILATION_OPTIONS_H
#define COMPILATION_OPTIONS_H


#define QUOTE(name) #name
#define STR(macro) QUOTE(macro)


#ifndef SIMULATION_INPUT_SCHEMA_URI
#define SIMULATION_INPUT_SCHEMA_URI https:\/\/gist.githubusercontent.com/gsrohde/3c1d3be487b94234de9ad1312be27491/raw
#endif

#define SIMULATION_INPUT_SCHEMA_URI_STRING STR(SIMULATION_INPUT_SCHEMA_URI)


#endif
