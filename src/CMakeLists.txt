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
    signal-base-py.cc
    signal-caster-py.cc
    signal-wrapper.cc
    )

# Remove prefix lib
SET_TARGET_PROPERTIES(${PYTHON_MODULE}
    PROPERTIES PREFIX "")

PKG_CONFIG_USE_DEPENDENCY(${PYTHON_MODULE} dynamic-graph)

# TODO: that's ugly.
ADD_CUSTOM_TARGET(link_target ALL COMMAND ${CMAKE_COMMAND} -E create_symlink
  "${CMAKE_CURRENT_BINARY_DIR}/${PYTHON_MODULE}.so"
  "dynamic_graph/${PYTHON_MODULE}.so")

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})

#
# Installation
#

INSTALL(TARGETS ${PYTHON_MODULE} DESTINATION ${PYTHON_SITELIB}/dynamic_graph)

SET (PYTHON_SOURCES
    __init__.py
    attrpath.py
    entity.py
    signal_base.py
    matlab.py
    script_shortcuts.py
    tools.py
    )

FOREACH (SOURCE ${PYTHON_SOURCES})
    PYTHON_INSTALL_ON_SITE(dynamic_graph ${SOURCE})
ENDFOREACH (SOURCE)

# --- ADD the wrap on the dg modules
# Tracer plugin
IF(WIN32)
    SET(TRACER_PLUGIN ${DYNAMIC_GRAPH_PLUGINDIR}/tracer${CMAKE_STATIC_LIBRARY_SUFFIX})
ELSE(WIN32)
    SET(TRACER_PLUGIN ${DYNAMIC_GRAPH_PLUGINDIR}/tracer${CMAKE_SHARED_LIBRARY_SUFFIX})
ENDIF(WIN32)
DYNAMIC_GRAPH_PYTHON_MODULE("tracer" ${TRACER_PLUGIN} tracer-wrap)

# TracerRealTime plugin
IF(WIN32)
    SET(TRACERREALTIME_PLUGIN ${DYNAMIC_GRAPH_PLUGINDIR}/tracer-real-time${CMAKE_STATIC_LIBRARY_SUFFIX})
ELSE(WIN32)
    SET(TRACERREALTIME_PLUGIN ${DYNAMIC_GRAPH_PLUGINDIR}/tracer-real-time${CMAKE_SHARED_LIBRARY_SUFFIX})
ENDIF(WIN32)
DYNAMIC_GRAPH_PYTHON_MODULE("tracer_real_time" ${TRACERREALTIME_PLUGIN} tracer_real_time-wrap)
