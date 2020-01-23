# Copyright 2010-2020, Florent Lamiraux, Thomas Moulard, Olivier Stasse, Guilhem Saurel, JRL, CNRS/AIST, LAAS-CNRS

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

SET(PROJECT_NAMESPACE stack-of-tasks)
SET(PROJECT_NAME dynamic-graph-python)
SET(PROJECT_DESCRIPTION "Dynamic graph library Python bindings")
SET(PROJECT_URL "http://github.com/${PROJECT_NAMESPACE}/${PROJECT_NAME}")

SET(PROJECT_USE_CMAKE_EXPORT TRUE)
SET(CUSTOM_HEADER_DIR "dynamic-graph/python")
SET(CXX_DISABLE_WERROR TRUE)
SET(DOXYGEN_USE_MATHJAX YES)

INCLUDE(cmake/base.cmake)
INCLUDE(cmake/boost.cmake)
INCLUDE(cmake/eigen.cmake)
INCLUDE(cmake/python.cmake)
INCLUDE(cmake/test.cmake)
INCLUDE(cmake/sphinx.cmake)

SET(PKG_CONFIG_ADDITIONAL_VARIABLES plugindir ${PKG_CONFIG_ADDITIONAL_VARIABLES})

COMPUTE_PROJECT_ARGS(PROJECT_ARGS LANGUAGES CXX)
PROJECT(${PROJECT_NAME} ${PROJECT_ARGS})

FINDPYTHON()
INCLUDE_DIRECTORIES(SYSTEM ${PYTHON_INCLUDE_DIRS})

ADD_REQUIRED_DEPENDENCY(eigen3)
ADD_PROJECT_DEPENDENCY(dynamic-graph 3.8.1 REQUIRED)

# manually append to the .pc dependencies for now
# ref https://github.com/jrl-umi3218/jrl-cmakemodules/issues/320
_ADD_TO_LIST(_PKG_CONFIG_REQUIRES "dynamic-graph >= 3.8.1" ",")

PKG_CONFIG_APPEND_LIBS("dynamic-graph-python")

SET(BOOST_COMPONENTS filesystem system thread program_options unit_test_framework python)
SEARCH_FOR_BOOST()

ADD_SUBDIRECTORY(src)
ADD_SUBDIRECTORY(include/${CUSTOM_HEADER_DIR})
ADD_SUBDIRECTORY(doc)
ADD_SUBDIRECTORY(unitTesting)
