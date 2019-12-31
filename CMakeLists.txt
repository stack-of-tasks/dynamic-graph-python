# Copyright 2010-2019, Florent Lamiraux, Thomas Moulard, Guilhem Saurel, JRL, CNRS/AIST, LAAS-CNRS

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

SET(PROJECT_NAMESPACE stack-of-tasks)
SET(PROJECT_NAME dynamic-graph-python)
SET(PROJECT_DESCRIPTION "Dynamic graph library Python bindings")
SET(PROJECT_URL "http://github.com/${PROJECT_NAMESPACE}/${PROJECT_NAME}")

# Export CMake Target
SET(PROJECT_USE_CMAKE_EXPORT TRUE)

# Make sure that every header is generated in dynamic-graph
SET(CUSTOM_HEADER_DIR "dynamic-graph/python")

# Do not fail on warning
SET(CXX_DISABLE_WERROR TRUE)

INCLUDE(cmake/base.cmake)

# Specify the project.
COMPUTE_PROJECT_ARGS(PROJECT_ARGS LANGUAGES CXX)
PROJECT(${PROJECT_NAME} ${PROJECT_ARGS})

INCLUDE(cmake/pthread.cmake)
include(cmake/sphinx.cmake)
include(cmake/test.cmake)
include(cmake/python.cmake)

add_project_dependency(Boost REQUIRED COMPONENTS python)
add_project_dependency(dynamic-graph 3.0.0 REQUIRED )

SET(DOXYGEN_USE_MATHJAX YES)

SET(CUSTOM_HEADER_DIR "dynamic-graph/python")

# Headers list.
SET(${PROJECT_NAME}_HEADERS
  include/dynamic-graph/python/interpreter.hh
  include/dynamic-graph/python/api.hh
  include/dynamic-graph/python/exception-python.hh
)

SET(PKG_CONFIG_ADDITIONAL_VARIABLES plugindir ${PKG_CONFIG_ADDITIONAL_VARIABLES})

FINDPYTHON()

# Trigger dependency to dynamic-graph.
ADD_PROJECT_DEPENDENCY(dynamic-graph 3.0.0 REQUIRED)

# Add dependency toward dynamic graph library in pkg-config file.
PKG_CONFIG_APPEND_LIBS("dynamic-graph-python")

ADD_SUBDIRECTORY(src)
ADD_SUBDIRECTORY(include/${CUSTOM_HEADER_DIR})
ADD_SUBDIRECTORY(doc)
ADD_SUBDIRECTORY(unitTesting)

get_cmake_property(_variableNames VARIABLES)
list (SORT _variableNames)
foreach (_variableName ${_variableNames})
  LIST(APPEND LOGGING_WATCHED_VARIABLES
    ${_variableName}
    )
endforeach()
