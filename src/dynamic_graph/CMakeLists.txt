# Copyright 2010-2020, Florent Lamiraux, Thomas Moulard, Olivier Stasse, Guilhem Saurel, JRL, CNRS/AIST, LAAS-CNRS

SET(PYTHON_MODULE wrap)

ADD_LIBRARY(${PYTHON_MODULE} MODULE
  convert-dg-to-py.cc
  debug-py.cc
  dynamic-graph-py.cc
  entity-py.cc
  exception-python.cc
  factory-py.cc
  pool-py.cc
  python-compat.cc
  signal-base-py.cc
  signal-caster-py.cc
  signal-wrapper.cc
  )

TARGET_INCLUDE_DIRECTORIES(${PYTHON_MODULE} SYSTEM PUBLIC ${PYTHON_INCLUDE_DIRS})
TARGET_LINK_LIBRARIES(${PYTHON_MODULE} ${PYTHON_LIBRARY}
  dynamic-graph::dynamic-graph)

# Remove prefix lib
SET_TARGET_PROPERTIES(${PYTHON_MODULE} PROPERTIES PREFIX "")

INSTALL(TARGETS ${PYTHON_MODULE}
  EXPORT ${TARGETS_EXPORT_NAME}
  DESTINATION ${PYTHON_SITELIB}/dynamic_graph)
