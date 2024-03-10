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

# Add string options that are available only when CUSOMIZE_DOXYGEN is
# turned on.  (cmake_dependent_option handles only boolean options.)
if(CUSTOMIZE_DOXYGEN)
  set(DOXYGEN_HTML_COLORSTYLE_HUE 120 CACHE STRING "0 to 360: 0 = RED, 120 = GREEN, 240 = BLUE")
  set(DOXYGEN_HTML_COLORSTYLE_SAT 100 CACHE STRONG "0 = GRAY-SCALE, 255 = MOST VIVID; DEFAULT = 100")
else()
  # Unset these cache variables if customization is turned back off.
  unset(DOXYGEN_HTML_COLORSTYLE_HUE CACHE) # Doxygen default is 220 (between cyan and blue)
  unset(DOXYGEN_HTML_COLORSTYLE_SAT CACHE) # Doxygen default is 100
endif()

include(Doxygen)
Doxygen(docs . ../app)
