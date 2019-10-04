# Copyright 2010 - 2019, Florent Lamiraux, Thomas Moulard, Guilhem Saurel, JRL, CNRS/AIST, LAAS-CNRS

ADD_SUBDIRECTORY(dynamic_graph)

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
