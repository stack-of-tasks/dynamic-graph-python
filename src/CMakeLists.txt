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
INCLUDE(FindPythonLibs)
IF (NOT ${PYTHONLIBS_FOUND} STREQUAL TRUE)
   MESSAGE(FATAL_ERROR "Python has not been found.")
ENDIF (NOT ${PYTHONLIBS_FOUND} STREQUAL TRUE)

INCLUDE(FindPythonInterp)
IF (NOT ${PYTHONINTERP_FOUND} STREQUAL TRUE)
   MESSAGE(FATAL_ERROR "Python executable has not been found.")
ENDIF (NOT ${PYTHONINTERP_FOUND} STREQUAL TRUE)

INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})

# provide path to library libdynamic-graph.so
LINK_DIRECTORIES(${DYNAMIC_GRAPH_LIBRARY_DIRS})
ADD_DEFINITIONS(${DYNAMIC_GRAPH_CFLAGS})

#
#
#  Python interpreter
#
#
SET(LIBRARY_NAME ${PROJECT_NAME})
ADD_LIBRARY(${LIBRARY_NAME}
  SHARED
  interpreter.cc)

SET_TARGET_PROPERTIES(${LIBRARY_NAME} PROPERTIES SOVERSION ${PROJECT_VERSION})
PKG_CONFIG_USE_DEPENDENCY(${LIBRARY_NAME} dynamic-graph)

INSTALL(TARGETS ${LIBRARY_NAME}
  DESTINATION lib)

SET(EXECUTABLE_NAME dg-python)
ADD_EXECUTABLE(${EXECUTABLE_NAME} dg-python.cc)
TARGET_LINK_LIBRARIES(${EXECUTABLE_NAME}
  ${LIBRARY_NAME}
  ${Boost_LIBRARIES}
  ${PYTHON_LIBRARY})
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
	dynamic-graph-py.cc
	signal-base-py.cc
	entity-py.cc
	factory-py.cc
	signal-caster-py.cc
)

# Remove prefix lib
SET_TARGET_PROPERTIES(${PYTHON_MODULE}
	PROPERTIES PREFIX "")

TARGET_LINK_LIBRARIES(${PYTHON_MODULE} ${DYNAMIC_GRAPH_LIBRARIES})

INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_PATH})

#
# Installation
#

EXEC_PROGRAM(${PYTHON_EXECUTABLE} ARGS "-c \"from distutils import sysconfig; print sysconfig.get_python_lib(0,0,prefix='')\""
  OUTPUT_VARIABLE PYTHON_SITELIB)

MESSAGE(STATUS "PYTHON_SITELIB=${PYTHON_SITELIB}")

SET(PYTHON_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/${PYTHON_SITELIB}/dynamic_graph)

INSTALL(TARGETS ${PYTHON_MODULE}
	       DESTINATION
	       ${PYTHON_INSTALL_DIR})

SET (PYTHON_SOURCES
    dynamic_graph/__init__.py
    dynamic_graph/entity.py
    dynamic_graph/signal_base.py
)

SET (PYTHON_SOURCES_FULLPATH "")
FOREACH (SOURCE ${PYTHON_SOURCES})
    SET(PYTHON_SOURCES_FULLPATH
    	${PYTHON_SOURCES_FULLPATH}
	${CMAKE_SOURCE_DIR}/src/${SOURCE}
    )
ENDFOREACH (SOURCE)

INSTALL(FILES ${PYTHON_SOURCES_FULLPATH}
	DESTINATION ${PYTHON_INSTALL_DIR})
