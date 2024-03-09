cmake_minimum_required(VERSION 3.20.0)
project("DAWN documentation" CXX)

# Add this here in case we are not building from the top level:
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../cmake")

message(VERBOSE "Processing doc/CMakeLists.txt")
if(MESSAGE_LOG_LEVEL IN_LIST LOG_LEVELS_FOR_INDENTATION)
  list(APPEND CMAKE_MESSAGE_INDENT "  ")
endif()


include(CMakeDependentOption)
cmake_dependent_option(
  ADD_TREEVIEW
  "Add a tree-view menu to the Doxygen documentation"
  ON
  "CUSTOMIZE_DOXYGEN"
  ON
)
cmake_dependent_option(
  DOCUMENT_PRIVATE_MEMBERS
  "Document the private members of classes"
  OFF
  "CUSTOMIZE_DOXYGEN"
  OFF
)
cmake_dependent_option(
  USE_DOXYGEN_AWESOME_STYLESHEET
  "Use the Doxygen Awesome stylesheet from jothepro"
  OFF
  "CUSTOMIZE_DOXYGEN"
  OFF
)

include(Doxygen)
Doxygen(docs . ../app)
