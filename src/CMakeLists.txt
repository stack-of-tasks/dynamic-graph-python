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

#INCLUDE_DIRECTORIES(SYSTEM ${Boost_INCLUDE_DIRS})
LINK_DIRECTORIES(${Boost_LIBRARY_DIRS} ${PYTHON_LIBRARY_DIRS})

#
#
#  Python interpreter
#
#
SET(LIBRARY_NAME ${PROJECT_NAME})
ADD_LIBRARY(${LIBRARY_NAME}
  SHARED
  interpreter.cc)

target_include_directories(${LIBRARY_NAME}
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
#
#  Python bindings
#
#

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


#PKG_CONFIG_USE_DEPENDENCY(${PYTHON_MODULE} dynamic-graph)
TARGET_LINK_LIBRARIES(${PYTHON_MODULE} ${PYTHON_LIBRARY})
TARGET_LINK_LIBRARIES(${PYTHON_MODULE} dynamic-graph::dynamic-graph)

CONFIG_FILES(link-to-python.hh)
#INCLUDE_DIRECTORIES(SYSTEM ${PYTHON_INCLUDE_PATH})
#INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})

#
# Installation
#

SET(PYTHON_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/${PYTHON_SITELIB}/dynamic_graph)

INSTALL(TARGETS ${PYTHON_MODULE}
  EXPORT ${TARGETS_EXPORT_NAME}    
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
