# Copyright 2010-2020, Florent Lamiraux, Thomas Moulard, Olivier Stasse, Guilhem Saurel, JRL, CNRS/AIST, LAAS-CNRS

SET(PYTHON_MODULE wrap)

ADD_LIBRARY(${PYTHON_MODULE} MODULE
  debug-py.cc
  dynamic-graph-py.cc
  factory-py.cc
  pool-py.cc
  signal-base-py.cc
  signal-wrapper.cc
  )

TARGET_INCLUDE_DIRECTORIES(${PYTHON_MODULE} SYSTEM PUBLIC ${PYTHON_INCLUDE_DIRS})
TARGET_LINK_LIBRARIES(${PYTHON_MODULE} ${PYTHON_LIBRARY}
  dynamic-graph::dynamic-graph Boost::python ${PROJECT_NAME}
  eigenpy::eigenpy)

# Remove prefix lib
SET_TARGET_PROPERTIES(${PYTHON_MODULE} PROPERTIES PREFIX "")

INSTALL(TARGETS ${PYTHON_MODULE}
  EXPORT ${TARGETS_EXPORT_NAME}
  DESTINATION ${PYTHON_SITELIB}/dynamic_graph)
