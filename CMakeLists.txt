# Copyright 2010, 2011, Florent Lamiraux, Thomas Moulard, JRL, CNRS/AIST

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

SET(PROJECT_NAMESPACE stack-of-tasks)
SET(PROJECT_NAME dynamic-graph-python)
SET(PROJECT_DESCRIPTION "Dynamic graph library Python bindings")
SET(PROJECT_URL "http://github.com/${PROJECT_NAMESPACE}/${PROJECT_NAME}")

INCLUDE(cmake/base.cmake)
INCLUDE(cmake/boost.cmake)
INCLUDE(cmake/eigen.cmake)
INCLUDE(cmake/pthread.cmake)
include(cmake/python.cmake)
include(cmake/test.cmake)
INCLUDE(cmake/sphinx.cmake)

SET(DOXYGEN_USE_MATHJAX YES)

SET(CUSTOM_HEADER_DIR "dynamic-graph/python")

SET(PKG_CONFIG_ADDITIONAL_VARIABLES plugindir ${PKG_CONFIG_ADDITIONAL_VARIABLES})

PROJECT(${PROJECT_NAME} CXX)

FINDPYTHON()

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
