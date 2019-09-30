# Copyright 2010, 2011, Florent Lamiraux, Thomas Moulard, JRL, CNRS/AIST

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

INCLUDE(cmake/base.cmake)
INCLUDE(cmake/boost.cmake)
INCLUDE(cmake/eigen.cmake)
INCLUDE(cmake/pthread.cmake)
INCLUDE(cmake/cpack.cmake)
include(cmake/header.cmake)
include(cmake/python.cmake)

SET(PROJECT_NAME dynamic-graph-python)
SET(PROJECT_DESCRIPTION "Dynamic graph library Python bindings")
SET(PROJECT_URL "http://github.com/stack-of-tasks/dynamic-graph-python")

SET(DOXYGEN_USE_MATHJAX YES)

SET(CUSTOM_HEADER_DIR "dynamic-graph/python")
# Headers list.
SET(${PROJECT_NAME}_HEADERS
  include/dynamic-graph/python/interpreter.hh
  include/dynamic-graph/python/api.hh
  include/dynamic-graph/python/exception-python.hh

)

SET(PKG_CONFIG_ADDITIONAL_VARIABLES plugindir ${PKG_CONFIG_ADDITIONAL_VARIABLES})

SETUP_PROJECT()

FINDPYTHON(2.7 EXACT REQUIRED)

# Trigger dependency to dynamic-graph.
ADD_REQUIRED_DEPENDENCY("dynamic-graph >= 3.0.0")

# Add dependency toward dynamic graph library in pkg-config file.
PKG_CONFIG_APPEND_LIBS("dynamic-graph-python")

# Search for Boost.
SET(BOOST_COMPONENTS python filesystem system thread program_options unit_test_framework)
SEARCH_FOR_BOOST()
SEARCH_FOR_EIGEN()

ADD_SUBDIRECTORY(src)
ADD_SUBDIRECTORY(include)
ADD_SUBDIRECTORY(doc)
ADD_SUBDIRECTORY(unitTesting)

SETUP_PROJECT_FINALIZE()

