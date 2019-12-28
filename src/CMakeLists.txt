# Copyright 2010 - 2019, Florent Lamiraux, Thomas Moulard, Guilhem Saurel, JRL, CNRS/AIST, LAAS-CNRS
# License

#
#  TARGET: C++ Library dynamic-graph-python
#  Python interpreter
#
#

SET(LIBRARY_NAME ${PROJECT_NAME})
ADD_LIBRARY(${LIBRARY_NAME} SHARED interpreter.cc dynamic_graph/python-compat.cc)

target_include_directories(${LIBRARY_NAME}
  PRIVATE
  ${PYTHON_INCLUDE_DIRS}
  PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/../include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/>
  INTERFACE
  $<INSTALL_INTERFACE:include>
)

TARGET_LINK_LIBRARIES(${LIBRARY_NAME} ${PYTHON_LIBRARY})

IF(UNIX)
  TARGET_LINK_LIBRARIES(${LIBRARY_NAME} ${Boost_LIBRARIES})
  TARGET_LINK_LIBRARIES(${LIBRARY_NAME} ${Boost_PYTHON_LIBRARIES})
  TARGET_LINK_LIBRARIES(${LIBRARY_NAME} ${CMAKE_DL_LIBS})
ENDIF(UNIX)

SET_TARGET_PROPERTIES(${LIBRARY_NAME} PROPERTIES SOVERSION ${PROJECT_VERSION})
TARGET_LINK_LIBRARIES(${LIBRARY_NAME} dynamic-graph::dynamic-graph)

INSTALL(TARGETS ${LIBRARY_NAME}
  EXPORT ${TARGETS_EXPORT_NAME}
  PUBLIC_HEADER
  INCLUDES DESTINATION include
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )

#
#  TARGET: C++ library wrap
#  Python bindings
#
#

SET(PYTHON_MODULE wrap)

ADD_LIBRARY(${PYTHON_MODULE}
  MODULE
  ./dynamic_graph/convert-dg-to-py.cc
  ./dynamic_graph/debug-py.cc
  ./dynamic_graph/dynamic-graph-py.cc
  ./dynamic_graph/entity-py.cc
  ./dynamic_graph/exception-python.cc
  ./dynamic_graph/factory-py.cc
  ./dynamic_graph/pool-py.cc
  ./dynamic_graph/python-compat.cc
  ./dynamic_graph/signal-base-py.cc
  ./dynamic_graph/signal-caster-py.cc
  ./dynamic_graph/signal-wrapper.cc
)

target_include_directories(${PYTHON_MODULE}
  PRIVATE ${PYTHON_INCLUDE_DIR}
  )

# Remove prefix lib
SET_TARGET_PROPERTIES(${PYTHON_MODULE}
  PROPERTIES PREFIX "")

TARGET_LINK_LIBRARIES(${PYTHON_MODULE} ${PYTHON_LIBRARY})
TARGET_LINK_LIBRARIES(${PYTHON_MODULE} dynamic-graph::dynamic-graph)


# Installation
SET(PYTHON_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/${PYTHON_SITELIB}/dynamic_graph)
MESSAGE(STATUS "PYTHON_INSTALL_DIR: ${PYTHON_INSTALL_DIR}")

INSTALL(TARGETS ${PYTHON_MODULE}
  EXPORT ${TARGETS_EXPORT_NAME}
  DESTINATION
  ${PYTHON_INSTALL_DIR})


#
# Pure Python install
#
SET (PYTHON_SOURCES
    __init__.py
    attrpath.py
    entity.py
    signal_base.py
    script_shortcuts.py
    tools.py
    )

FOREACH (SOURCE ${PYTHON_SOURCES})
    PYTHON_INSTALL_ON_SITE(dynamic_graph ${SOURCE})
ENDFOREACH (SOURCE)

# --- ADD the wrap on the dg modules
#
# Target: Tracer plugin
#
IF(WIN32)
    SET(TRACER_PLUGIN ${DYNAMIC_GRAPH_PLUGINDIR}/tracer${CMAKE_STATIC_LIBRARY_SUFFIX})
ELSE(WIN32)
    SET(TRACER_PLUGIN ${DYNAMIC_GRAPH_PLUGINDIR}/tracer${CMAKE_SHARED_LIBRARY_SUFFIX})
ENDIF(WIN32)
DYNAMIC_GRAPH_PYTHON_MODULE("tracer" ${TRACER_PLUGIN} tracer-wrap)

target_include_directories(tracer-wrap
  PRIVATE ${PYTHON_INCLUDE_DIR}
)

#
# Target: TracerRealTime plugin
#
IF(WIN32)
    SET(TRACERREALTIME_PLUGIN ${DYNAMIC_GRAPH_PLUGINDIR}/tracer-real-time${CMAKE_STATIC_LIBRARY_SUFFIX})
ELSE(WIN32)
    SET(TRACERREALTIME_PLUGIN ${DYNAMIC_GRAPH_PLUGINDIR}/tracer-real-time${CMAKE_SHARED_LIBRARY_SUFFIX})
ENDIF(WIN32)
DYNAMIC_GRAPH_PYTHON_MODULE("tracer_real_time"
 ${TRACERREALTIME_PLUGIN}
 tracer_real_time-wrap)

target_include_directories(tracer_real_time-wrap
 PRIVATE ${PYTHON_INCLUDE_DIR}
 )
