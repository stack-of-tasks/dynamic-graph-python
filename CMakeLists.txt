# Copyright 2010-2020, Florent Lamiraux, Thomas Moulard, Olivier Stasse, Guilhem Saurel, JRL, CNRS/AIST, LAAS-CNRS

CMAKE_MINIMUM_REQUIRED(VERSION 3.1)

# Project properties
SET(PROJECT_ORG stack-of-tasks)
SET(PROJECT_NAME dynamic-graph-python)
SET(PROJECT_DESCRIPTION "Dynamic graph library Python bindings")
SET(PROJECT_URL "https://github.com/${PROJECT_ORG}/${PROJECT_NAME}")

# Project options
OPTION(SUFFIX_SO_VERSION "Suffix library name with its version" ON)

# Project configuration
SET(PROJECT_USE_CMAKE_EXPORT TRUE)
SET(CUSTOM_HEADER_DIR "dynamic-graph/python")
SET(CXX_DISABLE_WERROR TRUE)
SET(DOXYGEN_USE_MATHJAX YES)

# JRL-cmakemodule setup
INCLUDE(cmake/base.cmake)
INCLUDE(cmake/boost.cmake)
INCLUDE(cmake/python.cmake)

# Project definition
COMPUTE_PROJECT_ARGS(PROJECT_ARGS LANGUAGES CXX)
PROJECT(${PROJECT_NAME} ${PROJECT_ARGS})

# Project dependencies
FINDPYTHON()

ADD_PROJECT_DEPENDENCY(dynamic-graph REQUIRED PKG_CONFIG_REQUIRES dynamic-graph)
SET(BOOST_COMPONENTS filesystem system thread program_options unit_test_framework python)
SEARCH_FOR_BOOST()


# Main Library
SET(${PROJECT_NAME}_HEADERS
  include/${CUSTOM_HEADER_DIR}/api.hh
  include/${CUSTOM_HEADER_DIR}/convert-dg-to-py.hh
  include/${CUSTOM_HEADER_DIR}/dynamic-graph-py.hh
  include/${CUSTOM_HEADER_DIR}/exception.hh
  include/${CUSTOM_HEADER_DIR}/exception-python.hh
  include/${CUSTOM_HEADER_DIR}/interpreter.hh
  include/${CUSTOM_HEADER_DIR}/python-compat.hh
  include/${CUSTOM_HEADER_DIR}/signal-wrapper.hh
  )

SET(${PROJECT_NAME}_SOURCES
  src/interpreter.cc
  src/dynamic_graph/python-compat.cc
  )

ADD_LIBRARY(${PROJECT_NAME} SHARED
  ${${PROJECT_NAME}_SOURCES} ${${PROJECT_NAME}_HEADERS})
TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} SYSTEM PUBLIC ${PYTHON_INCLUDE_DIRS})
TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} PUBLIC $<INSTALL_INTERFACE:include>)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${Boost_LIBRARIES}
  ${PYTHON_LIBRARY} ${Boost_PYTHON_LIBRARIES} dynamic-graph::dynamic-graph)

IF(SUFFIX_SO_VERSION)
  SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES SOVERSION ${PROJECT_VERSION})
ENDIF(SUFFIX_SO_VERSION)

TARGET_COMPILE_DEFINITIONS(${PROJECT_NAME} PRIVATE PYTHON_LIBRARY="${PYTHON_LIBRARY}")

INSTALL(TARGETS ${PROJECT_NAME} EXPORT ${TARGETS_EXPORT_NAME} DESTINATION lib)

ADD_SUBDIRECTORY(src)
ADD_SUBDIRECTORY(tests)

PKG_CONFIG_APPEND_LIBS(${PROJECT_NAME})
INSTALL(FILES package.xml DESTINATION share/${PROJECT_NAME})
