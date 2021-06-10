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
CHECK_MINIMAL_CXX_STANDARD(14 ENFORCE)

# Project dependencies
FINDPYTHON()

ADD_PROJECT_DEPENDENCY(dynamic-graph REQUIRED)
ADD_PROJECT_DEPENDENCY(eigenpy REQUIRED)
SEARCH_FOR_BOOST_PYTHON(REQUIRED)
IF(BUILD_TESTING)
  FIND_PACKAGE(Boost REQUIRED COMPONENTS unit_test_framework)
ENDIF(BUILD_TESTING)

IF(Boost_VERSION GREATER 107299 OR Boost_VERSION_MACRO GREATER 107299)
  # Silence a warning about a deprecated use of boost bind by boost >= 1.73
  # without dropping support for boost < 1.73
  ADD_DEFINITIONS(-DBOOST_BIND_GLOBAL_PLACEHOLDERS)
ENDIF()

# Main Library
SET(${PROJECT_NAME}_HEADERS
  include/${CUSTOM_HEADER_DIR}/api.hh
  include/${CUSTOM_HEADER_DIR}/convert-dg-to-py.hh
  include/${CUSTOM_HEADER_DIR}/dynamic-graph-py.hh
  include/${CUSTOM_HEADER_DIR}/fwd.hh
  include/${CUSTOM_HEADER_DIR}/interpreter.hh
  include/${CUSTOM_HEADER_DIR}/module.hh
  include/${CUSTOM_HEADER_DIR}/python-compat.hh
  include/${CUSTOM_HEADER_DIR}/signal.hh
  include/${CUSTOM_HEADER_DIR}/signal-wrapper.hh
  )

SET(${PROJECT_NAME}_SOURCES
  src/interpreter.cc
  src/dynamic_graph/python-compat.cc
  src/dynamic_graph/entity-py.cc
  src/dynamic_graph/convert-dg-to-py.cc
  )

ADD_LIBRARY(${PROJECT_NAME} SHARED
  ${${PROJECT_NAME}_SOURCES} ${${PROJECT_NAME}_HEADERS})
TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} SYSTEM PUBLIC ${PYTHON_INCLUDE_DIR})
TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} PUBLIC $<INSTALL_INTERFACE:include>)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} PUBLIC ${PYTHON_LIBRARY}
  dynamic-graph::dynamic-graph)
TARGET_LINK_BOOST_PYTHON(${PROJECT_NAME} PRIVATE)

IF(SUFFIX_SO_VERSION)
  SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES SOVERSION ${PROJECT_VERSION})
ENDIF(SUFFIX_SO_VERSION)

TARGET_COMPILE_DEFINITIONS(${PROJECT_NAME} PRIVATE PYTHON_LIBRARY="${PYTHON_LIBRARY}")

INSTALL(TARGETS ${PROJECT_NAME} EXPORT ${TARGETS_EXPORT_NAME} DESTINATION lib)

ADD_SUBDIRECTORY(src)
IF(BUILD_TESTING)
  ADD_SUBDIRECTORY(tests)
ENDIF(BUILD_TESTING)

INSTALL(FILES package.xml DESTINATION share/${PROJECT_NAME})
