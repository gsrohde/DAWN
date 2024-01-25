#ifndef COMPILATION_OPTIONS_H
#define COMPILATION_OPTIONS_H

#include <string>
#include <vector>

#define QUOTE(name) #name
#define STR(macro) QUOTE(macro)

inline std::vector<std::string> get_schema_uris() {

    std::vector<std::string> uri_list {};

#if defined(USE_LOCAL_SCHEMA_FILE)
    uri_list.insert(uri_list.end(), { STR(BUILD_SCHEMA_FILEPATH),
                                      STR(INSTALL_SCHEMA_FILEPATH) });
#endif

#if defined(USE_REMOTE_SCHEMA_URI)
    uri_list.push_back(
                       "https://raw.githubusercontent.com"
                       "/gsrohde/DAWN/develop/"
                       "app/xml_schema/simulation-specification.xsd"
                       );
#endif

    return uri_list;
}


#endif
