find_package(Doxygen)

if(DOXYGEN_FOUND)
  message("Using version ${DOXYGEN_VERSION} of Doxygen")
  message("Path to Doxygen executable is ${DOXYGEN_EXECUTABLE}")
else()
  message(WARNING
    "Doxygen not found.  You will need to having it installed and "
    "available in order to build the Doxygen documentation.")
endif()

# include(FetchContent)
# FetchContent_Declare(doxygen-awesome-css
#   GIT_REPOSITORY
#     https://github.com/jothepro/doxygen-awesome-css.git
#   GIT_TAG
#     v1.6.0
# )
# FetchContent_MakeAvailable(doxygen-awesome-css)

function(Doxygen target input)
  set(NAME "${target}")
  set(DOXYGEN_HTML_OUTPUT     ${PROJECT_BINARY_DIR}/${NAME})
  set(DOXYGEN_GENERATE_HTML         YES)
  set(DOXYGEN_GENERATE_TREEVIEW     YES)
  set(DOXYGEN_HAVE_DOT              YES)
  set(DOXYGEN_DOT_IMAGE_FORMAT      svg)
  set(DOXYGEN_DOT_TRANSPARENT       YES)
  set(DOXYGEN_EXTRACT_ALL           YES)
  set(DOXYGEN_SOURCE_BROWSER        YES)

  # set(DOXYGEN_HTML_EXTRA_STYLESHEET
  #     ${doxygen-awesome-css_SOURCE_DIR}/doxygen-awesome.css)

  if(DOXYGEN_FOUND)
    doxygen_add_docs(${NAME}
        ${PROJECT_SOURCE_DIR}/${input}
        COMMENT "Generate HTML documentation"
    )
  endif()

endfunction()
