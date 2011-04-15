# Copyright 2010, 2011, Florent Lamiraux, Thomas Moulard, JRL, CNRS/AIST
#
# This file is part of dynamic-graph-python.
# dynamic-graph-python is free software: you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public License
# as published by the Free Software Foundation, either version 3 of
# the License, or (at your option) any later version.
#
# dynamic-graph-python is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Lesser Public License for more details.  You should have
# received a copy of the GNU Lesser General Public License along with
# dynamic-graph-python. If not, see <http://www.gnu.org/licenses/>.

#
# Python
#
INCLUDE(../cmake/python.cmake)
FINDPYTHON()

INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})
# provide path to library libdynamic-graph.so
LINK_DIRECTORIES(${DYNAMIC_GRAPH_LIBRARY_DIRS})
ADD_DEFINITIONS(${DYNAMIC_GRAPH_CFLAGS})

INCLUDE(../cmake/python.cmake)
#
#
#  Python interpreter
#
#
SET(LIBRARY_NAME ${PROJECT_NAME})
ADD_LIBRARY(${LIBRARY_NAME}
  SHARED
  interpreter.cc)

TARGET_LINK_LIBRARIES(${LIBRARY_NAME}
  ${Boost_LIBRARIES}
  ${PYTHON_LIBRARY})
SET_TARGET_PROPERTIES(${LIBRARY_NAME} PROPERTIES SOVERSION ${PROJECT_VERSION})
PKG_CONFIG_USE_DEPENDENCY(${LIBRARY_NAME} dynamic-graph)

INSTALL(TARGETS ${LIBRARY_NAME}
  DESTINATION lib)

SET(EXECUTABLE_NAME dg-python)
ADD_EXECUTABLE(${EXECUTABLE_NAME} dg-python.cc)
TARGET_LINK_LIBRARIES(${EXECUTABLE_NAME}
  ${LIBRARY_NAME} ${Boost_LIBRARIES}
  )
INSTALL(TARGETS dg-python DESTINATION bin)

#
#
#  Python bindings
#
#

SET(PYTHON_MODULE wrap)

ADD_LIBRARY(${PYTHON_MODULE}
	MODULE
	exception-python.cc
	convert-dg-to-py.cc
	dynamic-graph-py.cc
	signal-base-py.cc
	entity-py.cc
	factory-py.cc
	pool-py.cc
	signal-caster-py.cc
)

# Remove prefix lib
SET_TARGET_PROPERTIES(${PYTHON_MODULE}
	PROPERTIES PREFIX "")

TARGET_LINK_LIBRARIES(${PYTHON_MODULE} ${DYNAMIC_GRAPH_LIBRARIES})

CONFIG_FILES(link-to-python.hh)
INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_PATH}
  ${CMAKE_CURRENT_BINARY_DIR})

#
# Installation
#

SET(PYTHON_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/${PYTHON_SITELIB}/dynamic_graph)

INSTALL(TARGETS ${PYTHON_MODULE}
	       DESTINATION
	       ${PYTHON_INSTALL_DIR})

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
  PYTHON_INSTALL_ON_SITE(dynamic_graph ${SOURCE} )
ENDFOREACH (SOURCE)



# --- ADD the wrap on the dg modules
EXEC_PROGRAM("${PKG_CONFIG_EXECUTABLE}"
             ARGS "--variable=plugindir" "dynamic-graph"
             OUTPUT_VARIABLE "DYNAMIC_GRAPH_PLUGIN_DIR")

# Tracer plugin
ADD_DEPENDENCIES(${LIBRARY_NAME} MKDIR_tracer)
ADD_CUSTOM_TARGET(MKDIR_tracer mkdir -p ../lib/dynamic_graph/tracer)
SET(TRACER_PLUGIN ${DYNAMIC_GRAPH_PLUGIN_DIR}/tracer.so)
DYNAMIC_GRAPH_PYTHON_MODULE("tracer" ${TRACER_PLUGIN} tracer/wrap)

# TracerRealTime plugin
ADD_DEPENDENCIES(${LIBRARY_NAME} MKDIR_TracerRealTime)
ADD_CUSTOM_TARGET(MKDIR_TracerRealTime mkdir -p ../lib/dynamic_graph/tracer_real_time)
SET(TRACERREALTIME_PLUGIN ${DYNAMIC_GRAPH_PLUGIN_DIR}/tracer-real-time.so)
DYNAMIC_GRAPH_PYTHON_MODULE("tracer_real_time" ${TRACERREALTIME_PLUGIN} tracer_real_time/wrap)


