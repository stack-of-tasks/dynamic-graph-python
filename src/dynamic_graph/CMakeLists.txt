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

TARGET_LINK_LIBRARIES(${PYTHON_MODULE} PUBLIC ${PROJECT_NAME} eigenpy::eigenpy)
TARGET_LINK_BOOST_PYTHON(${PYTHON_MODULE} PRIVATE)

# Remove prefix lib
SET_TARGET_PROPERTIES(${PYTHON_MODULE} PROPERTIES PREFIX "")

INSTALL(TARGETS ${PYTHON_MODULE}
  EXPORT ${TARGETS_EXPORT_NAME}
  DESTINATION ${PYTHON_SITELIB}/dynamic_graph)
