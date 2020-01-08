# Copyright 2010 - 2019, Florent Lamiraux, Thomas Moulard, Guilhem Saurel, JRL, CNRS/AIST, LAAS-CNRS

SET(PYTHON_MODULE wrap)

ADD_LIBRARY(${PYTHON_MODULE}
    MODULE
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

TARGET_LINK_LIBRARIES(${PYTHON_MODULE} ${PYTHON_LIBRARY})

# Remove prefix lib
SET_TARGET_PROPERTIES(${PYTHON_MODULE}
    PROPERTIES PREFIX "")

PKG_CONFIG_USE_DEPENDENCY(${PYTHON_MODULE} dynamic-graph)

INSTALL(TARGETS ${PYTHON_MODULE} DESTINATION ${PYTHON_SITELIB}/dynamic_graph)